#include PLATFORM_HEADER

template <bool skipFirst>
void systickWaitInit(uint32_t ns){
    uint64_t ticks = ns;
    ticks *= SystemCoreClock;
    ticks /= 4000000000u;
    ticks = ticks/2 + ticks%2;
    if ( ticks > 0x1000000u ){
        ticks = 0x1000000u;
    }
    if ( ticks > 1){
        ticks--;
    }else{
        ticks = 1;
    }

    SysTick->CTRL = 0x00;
    if ( skipFirst )
        SysTick->LOAD = 1;
    else
        SysTick->LOAD = ticks;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
    if ( skipFirst )
        SysTick->LOAD = ticks;
}

template <typename Condition,typename Body>
void systickWaitUntil(const Condition &condition, const Body &body){
    while (((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0)&&condition())
        body();
}
static inline void systickWaitFinalize(void){
    SysTick->CTRL = 0x00;
}

template <typename Condition,typename Body>
void systickWait(uint32_t ns, const Condition &condition, const Body &body){
    systickWaitInit<false>(ns);
    systickWaitUntil(condition,body);
    systickWaitFinalize();
}
