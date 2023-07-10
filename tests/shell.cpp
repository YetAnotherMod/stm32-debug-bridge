#include <shell.h>

#include <fifo.h>
#include <string_view>
#include <static_map.h>
#include <iostream>

class TestExecutor{
public:
    template <typename fifoOut>
        requires requires(fifoOut &o, uint8_t x){
            o.push(x);
        }
    void execute(std::string_view command, std::string_view param, fifoOut &output){
        for ( auto i:command ){
            output.push(i);
        }
        if( !param.empty()){
            output.push(' ');
            for ( auto i:param ){
                output.push(i);
            }
        }
        output.push('\r');
        output.push('\n');
    }
private:
};

int main(){
    fifo::Fifo<uint8_t,1024> rx, tx;

    const static uint8_t input[] = {
        '\b','1','2','\b','2','3','\n',
        '1','2','3', ' ', ' ', '1','2','3', ' ', ' ', '1','2','3', ' ', ' ', '\r',
        ' ', ' ', '\r'
    };

    for (auto i:input){
        rx.push(i);
    }

    shell::Shell<1024,TestExecutor> sh;
    sh.exec(rx,tx);

    const static uint8_t good[] = {
        '1','2','\b',' ','\b','2','3','\r','\n','1','2','3','\r','\n',
        '>',' ','1','2','3', ' ', ' ', '1','2','3', ' ', ' ', '1','2','3', ' ', ' ', '\r', '\n',
        '1','2','3', ' ', '1','2','3', ' ', ' ', '1','2','3', '\r', '\n',
        '>', ' ', ' ', ' ', '\r', '\n',
        '>',' '
    };

    if (tx.size()!=sizeof(good)){
        std::cout<<"tx size error: " << tx.size() << " " << sizeof(good) << std::endl;
        return 1;
    }

    for (auto i:good){
        auto x = tx.pop();
        if(x!=i){
            std::cout<<"bad data in tx: " << x << " not " << i << std::endl;
        return 1;
        }
    }

    return 0;
}
