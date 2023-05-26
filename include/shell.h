#pragma once

#include <utility>
#include <cstdint>
#include <string_view>

namespace shell{

template <std::size_t lineLen, typename CE>
class Shell:private CE{
public:
    Shell():len(0),line_(){}
    template<typename fifoIn, typename fifoOut> 
        requires requires(fifoIn &i, fifoOut &o, uint8_t &x){
            i.empty();
            x = i.pop();
            o.push(x);
        }
    void exec(fifoIn &input, fifoOut &output){
        using std::string_view;
        using namespace std::literals;
        while(!input.empty()){
            char c = input.pop();
            switch ( c ){
                case '\b':
                    if ( len > 0 ){
                        --len;
                        output.push('\b');
                        output.push(' ');
                        output.push('\b');
                    }
                    break;
                case '\n':
                case '\r':
                    {
                        static const string_view separators = " \t\n\f\r\v"sv;
                        std::string_view line(line_,len); 
                        size_t ind = line.find_first_not_of(separators);
                        string_view command;
                        string_view param;
                        output.push('\r');
                        output.push('\n');
                        if (ind!=std::string_view::npos){
                            line.remove_prefix(ind);
                        }else{
                            return;
                        }
                        ind = line.find_last_not_of(separators);
                        if (ind!=string_view::npos){
                            line.remove_suffix(line.size()-ind-1);
                        }
                        ind = line.find_first_of(separators);
                        if (ind==string_view::npos){
                            command = line;
                        }else{
                            command = line.substr(0,ind);
                            ind = line.find_first_not_of(separators,ind);
                            line.remove_prefix(ind);
                            param = line;
                        }
                        CE::execute(command, param, output);
                        output.push('>');
                        output.push(' ');
                        len = 0;
                        break;
                    }
                default:
                    line_[len++] = c;
                    output.push(c);
                    break;
            }
        }
    }
private:
    std::size_t len;
    char line_[lineLen];
};

} // namespace shell
