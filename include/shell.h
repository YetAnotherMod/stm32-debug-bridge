#pragma once

#include <utility>
#include <cstdint>
#include <string_view>
#include <array>

namespace shell{

namespace color{
enum class index{
    red,
    green,
    yellow,
    blue,
    purple,
    magenta,
    white,
    normal
};
constexpr std::string_view get(index ind){
    using namespace std::literals::string_view_literals;
    switch (ind){
        case index::red:
            return "\033[31m"sv;
        case index::green:
            return "\033[32m"sv;
        case index::yellow:
            return "\033[33m"sv;
        case index::blue:
            return "\033[34m"sv;
        case index::purple:
            return "\033[35m"sv;
        case index::magenta:
            return "\033[36m"sv;
        case index::white:
            return "\033[37m"sv;
        case index::normal:
            return "\033[0m"sv;
    };
    return "\033[0m"sv;
}
};

namespace endl{
enum class index{
    _r,
    _n,
    _r_n,
    _n_r
};
constexpr std::string_view get(index ind){
    using namespace std::literals::string_view_literals;
    switch(ind){
        case index::_r:
            return "\r"sv;
        case index::_n:
            return "\n"sv;
        case index::_r_n:
            return "\r\n"sv;
        case index::_n_r:
            return "\n\r"sv;
    }
    return "\r\n"sv;
}
};

enum class res{
    ok,
    err,
    errPar,
    errTokNum,
    errClFull,
    errCplt
};

namespace internal{

static inline bool paste ( char * dst, std::size_t len, std::size_t capacity, std::string_view text, std::size_t pos ){
    if ( len + text.size() > capacity ){
        return false;
    }
    if ( pos > len ){
        return false;
    }
    for ( size_t i = 0 ; i < text.size() ; i++ ){
        dst[len+text.size()-i-1] = dst[len-i-1];
    }
    return true;
}
};

template <
    typename CE,
    std::size_t lineLen = 60,
    std::size_t tokNum = 8,
    std::size_t promptLen = 4,
    color::index promptColor = color::index::green,
    bool useComplete = false,
    bool useQuoting = false,
    bool useEchoOff = false,
    size_t historyLen = 0,
    bool useEscSeq = true,
    bool useCarriageReturn = true,
    bool useCtrlC = false,
    bool promptOnInit = true,
    endl::index endLine = endl::index::_r_n
>
    requires requires(CE ce, char c, std::array<std::string_view, tokNum> l){
        ce.execute(tokNum, l);
        ce.push(c);
    }
class Shell:public CE{
public:
    enum class seq{
        bracket,
        home,
        end,
        del
    };
    enum class echo {
        once,
        on,
        off
    };
    struct escAnsi {
        static constexpr char nul = 0x00;
        static constexpr char soh = 0x01;
        static constexpr char stx = 0x02;
        static constexpr char etx = 0x03;
        static constexpr char eot = 0x04;
        static constexpr char neq = 0x05;
        static constexpr char ack = 0x06;
        static constexpr char bel = 0x07;
        static constexpr char bs  = 0x08;
        static constexpr char ht  = 0x09;
        static constexpr char lf  = 0x0a;
        static constexpr char vt  = 0x0b;
        static constexpr char ff  = 0x0c;
        static constexpr char cr  = 0x0d;
        static constexpr char so  = 0x0e;
        static constexpr char si  = 0x0f;
        static constexpr char dle = 0x10;
        static constexpr char dc1 = 0x11;
        static constexpr char dc2 = 0x12;
        static constexpr char dc3 = 0x13;
        static constexpr char dc4 = 0x14;
        static constexpr char nak = 0x15;
        static constexpr char syn = 0x16;
        static constexpr char etb = 0x17;
        static constexpr char can = 0x18;
        static constexpr char em  = 0x19;
        static constexpr char sub = 0x1a;
        static constexpr char esc = 0x1b;
        static constexpr char fs  = 0x1c;
        static constexpr char gs  = 0x1d;
        static constexpr char rs  = 0x1e;
        static constexpr char us  = 0x1f;

        static constexpr char del = 0x7f;
    };

    static size_t tokenize(std::string_view line, std::array<std::string_view, tokNum> &toks)
        requires (useQuoting == false){
        enum stat{
            empty,
            token

        }status = empty;
        size_t tokInd = 0;
        size_t tokBegin = 0;
        using namespace std::literals;
        using std::string_view;
        static const string_view separators = " \t\n\f\r\v"sv;
        for ( size_t i = 0 ; i < line.size() ; ++i ){
            if ( separators.find(line[i])!=string_view::npos ){
                if (status == token){
                    if ( tokInd == tokNum ) throw res::errTokNum;
                    toks[tokInd++] = line.substr(tokBegin, i-tokBegin);
                    status = empty;
                }
            }else{
                if (status == empty){
                    tokBegin = i;
                    status = token;
                }
            }
        }

        if ( status == token ){
            if ( tokInd == tokNum ) throw res::errTokNum;
            toks[tokInd++] = line.substr(tokBegin);
            status = empty;
        }

        return tokInd;
    }
static size_t tokenize(std::string_view line, std::array<char, lineLen> &buf, std::array<std::string_view, tokNum> &toks)
        requires (useQuoting == true){
        enum stat{
            empty,
            token,
            singleQuotes,
            doubleQuotes,
            tokenEsc,
            singleQuotesEsc,
            doubleQuotesEsc

        }status = empty;
        size_t tokInd = 0;
        size_t tokBegin = 0;
        size_t bufInd = 0;
        for ( size_t i = 0 ; i < line.size() ; ++i ){
            switch ( line[i] ){
                case '\0':
                    break;
                case '\t':
                case ' ':{
                        switch ( status ){
                            case empty:
                                break;
                            case token:
                                toks[tokInd++] = std::string_view(buf+tokBegin,bufInd-tokBegin);
                                status = empty; 
                                break;
                            case tokenEsc:
                                status = token;
                                [[fallthrough]];
                            case singleQuotes:
                            case doubleQuotes:
                                buf[bufInd++] = line[i];
                                break;
                            case singleQuotesEsc:
                                status = singleQuotes;
                                buf[bufInd++] = line[i];
                                break;
                            case doubleQuotesEsc:
                                status = doubleQuotes;
                                buf[bufInd++] = line[i];
                                break;
                        }
                    }
                    break;
                case '\\':{
                        switch ( status ){
                            case empty:
                                if ( tokInd == tokNum ) throw res::errTokNum;
                                tokBegin = bufInd;
                                [[fallthrough]];
                            case token:
                                status = tokenEsc;
                                break;
                            case singleQuotes:
                                status = singleQuotesEsc;
                                break;
                            case doubleQuotes:
                                status = doubleQuotesEsc;
                                break;
                            case tokenEsc:
                                status = token;
                                buf[tokInd++] = '\\';
                                break;
                            case singleQuotesEsc:
                                status = singleQuotes;
                                buf[tokInd++] = '\\';
                                break;
                            case doubleQuotesEsc:
                                status = doubleQuotes;
                                buf[tokInd++] = '\\';
                                break;
                        }
                    }
                    break;
                case '\'':{
                        switch ( status ){
                            case empty:
                                if ( tokInd == tokNum ) throw res::errTokNum;
                                status = singleQuotes;
                                break;
                            case token:
                                status = singleQuotes;
                                break;
                            case doubleQuotesEsc:
                                buf[bufInd++] = '\'';
                                status = doubleQuotes;
                                break;
                            case singleQuotes:
                                status = token;
                                break;
                            case doubleQuotes:
                            case tokenEsc:
                            case singleQuotesEsc:
                                buf[bufInd++] = '\'';
                                status = singleQuotes;
                                break;
                        }
                    }
                    break;
                case '\"':{
                        switch ( status ){
                            case empty:
                                if ( tokInd == tokNum ) throw res::errTokNum;
                                status = doubleQuotes;
                                break;
                            case token:
                                status = doubleQuotes;
                                break;
                            case singleQuotesEsc:
                                buf[bufInd++] = '\"';
                                break;
                            case doubleQuotes:
                                status = token;
                                break;
                            case singleQuotes:
                            case tokenEsc:
                            case doubleQuotesEsc:
                                buf[bufInd++] = '\"';
                                break;
                        }
                    }
                    break;
                default:{
                        switch ( status ) {
                            case empty:
                                if ( tokInd == tokNum ) throw res::errTokNum;
                                status = token;
                                tokBegin = bufInd;
                                [[fallthrough]];
                            case token:
                            case singleQuotes:
                            case doubleQuotes:
                                buf[bufInd++] = line[i];
                                break;
                            case tokenEsc:
                            case singleQuotesEsc:
                            case doubleQuotesEsc:
                                throw res::err;
                                break;
                        }
                    }
                    break;
            }
        }
        return tokInd;
    }


    size_t tokenize () requires (useQuoting == true){
        return tokenize(std::string_view(line_[historyLen].data(),len),line_[historyLen+1],tokens);
    }
    size_t tokenize () requires (useQuoting == false){
        return tokenize(std::string_view(line_[historyLen].data(),len),tokens);
    }

    template<typename... Args>
    constexpr Shell(Args... args):CE(args...),len(0),line_(){}
    void exec(char c){
        using std::string_view;
        using namespace std::literals;
        switch ( c ){
            case '\b':
                if ( len > 0 ){
                    --len;
                    CE::push('\b');
                    CE::push(' ');
                    CE::push('\b');
                }
                break;
            case '\n':
            case '\r':
                {
                    for ( auto i: endl_ )
                        CE::push(i);
                    std::size_t num = tokenize();
                    if ( num > 0 )
                        CE::execute(num,tokens);
                    for ( auto i: colorPmt_ )
                        CE::push(i);
                    for ( auto i: prompt_ )
                        CE::push(i);
                    for ( auto i: colorDef_ )
                        CE::push(i);
                    len = 0;
                    break;
                }
            default:
                line_[historyLen][len++] = c;
                CE::push(c);
                break;
        }
    }
private:
    std::size_t len;
    std::array<std::array<char,lineLen>,historyLen+(useQuoting?2:1)> line_;
    std::array<std::string_view, tokNum> tokens;
    static constexpr std::string_view colorPmt_ = color::get(promptColor);
    static constexpr std::string_view colorDef_ = color::get(color::index::normal);
    static constexpr std::string_view endl_ = endl::get(endLine);
    static constexpr std::string_view prompt_ = [](){using namespace std::literals::string_view_literals; return "> "sv;}();
};

} // namespace shell
