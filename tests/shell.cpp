#include <shell.h>

#include <fifo.h>
#include <string_view>
#include <static_map.h>
#include <iostream>
#include <sstream>

class TestExecutor{
public:
    void push (char c){
        output_.put(c);
    }
    void execute(std::size_t argc, std::array<std::string_view, 8> argv){
        for ( size_t i = 0; i < argc ; i++ ){
            for ( auto j: argv[i] ){
                push(j);
            }
            if( i != argc - 1 ){
                push(' ');
            }
        }
        push('\r');
        push('\n');
    }
    std::string str() const{
        return output_.str();
    }
private:
    std::ostringstream output_;
};

int main(){

    using namespace std::literals::string_view_literals;

    std::string_view input = 
        "\b12\b23\n"
        "123  123  123  \r"
        "  \r"sv;

    shell::Shell<TestExecutor,1024> sh;
    for ( auto i : input ){
        sh.exec(i);
    }

    std::string_view good = 
        "12\b \b23\r\n" "123\r\n"
        "\033[32m" "> " "\033[0m" "123  123  123  \r\n" "123 123 123\r\n"
        "\033[32m" "> " "\033[0m" "  \r\n"
        "\033[32m" "> " "\033[0m"sv;

    std::string tx = sh.str();

    if (tx.size()!=good.size()){
        std::cout<<"tx size error: " << tx.size() << " " << good.size() << std::endl;
        std::cout<<"output: \n"<<tx<<std::endl;
        std::cout<<"good: \n"<<good<<std::endl;
        return 1;
    }

    for (size_t i = 0; i < sizeof(input); i++){
        if(good[i]!=tx[i]){
            std::cout<<"bad data in tx: " << tx[i] << " not " << input[i] << std::endl;
        return 1;
        }
    }

    return 0;
}
