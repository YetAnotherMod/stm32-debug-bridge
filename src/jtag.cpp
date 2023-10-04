
#include <jtag.h>
#include <djm.h>

#include <global_resources.h>
#include <systick-wait.h>
#include <usb.h>

namespace jtag {
class Executor{
private:
    uint32_t timing = 10;
public:
    void execute_B(void){
        config::ledOn();
    }
    void execute_b(void){
        config::ledOff();
    }
    void execute_R(void){
        uint8_t c = global::jtagIn.read() ? '1' : '0';
        write(&c,1);
    }
    void execute_0(void){
        global::jtagOut.write(false, false, false);
    }
    void execute_1(void){
        global::jtagOut.write(true, false, false);
    }
    void execute_2(void){
        global::jtagOut.write(false, true, false);
    }
    void execute_3(void){
        global::jtagOut.write(true, true, false);
    }
    void execute_4(void){
        global::jtagOut.write(false, false, true);
    }
    void execute_5(void){
        global::jtagOut.write(true, false, true);
    }
    void execute_6(void){
        global::jtagOut.write(false, true, true);
    }
    void execute_7(void){
        global::jtagOut.write(true, true, true);
    }
    void execute_r(void){
        config::reset (false,false);
    }
    void execute_s(void){
        config::reset (false,true);
    }
    void execute_t(void){
        config::reset (true,false);
    }
    void execute_u(void){
        config::reset (true,true);
    }
    void execute_scanIo(uint16_t count, uint8_t* buffer){
        scan<true,true>(count,buffer);
    }
    void execute_scanI(uint16_t count, uint8_t* buffer){
        scan<true,false>(count,buffer);
    }
    void execute_scanO(uint16_t count, uint8_t* buffer){
        scan<false,true>(count,buffer);
    }
    void execute_move(uint16_t count, uint8_t* buffer){
        systickWaitInit<true>(timing);
        for ( uint32_t i = 0 ; i < count ; i++ ){
            bool tms = (buffer[i/8]&(1<<(i%8)))?true:false;
            systickWaitUntil([](){return true;},[](){});
            global::jtagOut.write(false, tms, false);
            systickWaitUntil([](){return true;},[](){});
            global::jtagOut.write<2>(true);
        }
        systickWaitUntil([](){return true;},[](){});
        global::jtagOut.write<2>(false);
        systickWaitFinalize();
    }
    void execute_waitTicks(uint16_t count){
        systickWaitInit<true>(timing);
        while ( count-- ){
            systickWaitUntil([](){return true;},[](){});
            global::jtagOut.write<2>(false);
            systickWaitUntil([](){return true;},[](){});
            global::jtagOut.write<2>(true);
        }
        systickWaitUntil([](){return true;},[](){});
        global::jtagOut.write<2>(false);
        systickWaitFinalize();
    }
    void execute_waitTime([[maybe_unused]] uint16_t count){}
    void execute_setSpeed([[maybe_unused]] uint16_t count){
        timing = count*10;
    }
    void write(const uint8_t *buf, uint16_t count){
        while ( count-- ){
            while(!global::jtagRx.pushSafe(*buf)){
                usb::regenerateRx();
            }
            buf++;
        }
    }
    template <bool read, bool write>
    void scan(uint16_t count, uint8_t* buffer){
        uint8_t r = 0;
        systickWaitInit<true>(timing);
        for ( uint32_t i = 0 ; i < count ; i++ ){
            if ( ( i%8 == 0 ) && ( i/8 > 0 ) ){
                buffer[i/8-1] = r;
                if (read)
                    r=0;
            }
            bool tdi = write?(buffer[i/8]&(1<<(i%8)))?true:false:false;
            bool tms = (i==count-1u);
            systickWaitUntil([](){return true;},[](){});
            global::jtagOut.write(tdi,tms,false);
            systickWaitUntil([](){return true;},[](){});
            global::jtagOut.write<2>(true);
            if (read)
                r |= global::jtagIn.read()?1<<i%8:0;
        }
        buffer[(count-1)/8] = r;
        systickWaitUntil([](){return true;},[](){});
        global::jtagOut.write(false,false,false);
        systickWaitUntil([](){return true;},[](){});
        global::jtagOut.write<2>(true);
        systickWaitUntil([](){return true;},[](){});
        systickWaitFinalize();
    }
};
static djm::Packets<Executor> executor;
void tick(void) {
    while ( !global::jtagTx.empty() )
        executor.exec(global::jtagTx.pop());
}
void reset (void){
    executor.reset();
}
} // namespace jtag
