
namespace commands{
template <typename backend>
class CommandExecutor : public backend {
public:
    template<size_t L>
    void execute(size_t argc, const std::array<std::string_view, L> &argv){
        uint8_t x = 0;
        uint8_t d[]="0123456789abcdef";
        for ( size_t i = 0; i < argc ; i++){
            for (uint8_t j:argv[i]){
                x += j;
                backend::push(j);
            }
            if ( i!=argc-1)
                backend::push(' ');
        }
        backend::push(' ');
        backend::push(d[x/16]);
        backend::push(d[x%16]);
        backend::push('\r');
        backend::push('\n');
    }
private:
};
}

