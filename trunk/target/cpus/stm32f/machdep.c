/****************************************************************************
 *  $Id: proc.c 273 2011-03-03 21:03:19Z merdmann $
 *
 *  Mon Nov 26 06:50:00 2007
 *  Copyright  2007-2011 Michael Erdmann
 *  Email:Michael.Erdmann@snafu.de
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * Hardware dependencies implmented for the stm32f.
 */
#include "mkernel_config.h"
#include "kernel.h"

#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/scb.h>
#include <libopencmsis/core_cm3.h>

#include "typedefs.h"
#include "alarm.h"
#include "machdep.h"

#include "../../boards/disco/gfx.h"
#include "../../boards/disco/lcd-spi.h"

extern DATA POINTER _os_current_stack;

#define ref(x)  ((dword*)(x))

/* this counter instance is used for the system clock */
static AlarmBaseType System_Tick_Base = { (unsigned)0x7fffffff, 1, 100 };
t_counter System_Tick = { 0,0, &System_Tick_Base };

void msleep(uint32_t);
void sdram_init(void);
void ConfigurePendSV(dword);


/* milliseconds since boot */
static volatile uint32_t system_millis;         // simple 


static inline void _machdep_save_context(void) __attribute__((always_inline));
static inline void _machdep_restore_context(void) __attribute__((always_inline));


/**
 * @brief sleep a given number of milli seconds
 * @details This is a busy waiting for a given number of muliseconds. It uses
 * the systick counter set by the systick handler of the libcm3 package.
 * 
 * @param delay in miliseconds
 */
void msleep(uint32_t delay)
{
        uint32_t wake = system_millis + delay;
        while (wake > system_millis);
}

/**
 * This is the exception stack as it would have been created by an exception.
 */
typedef struct {
    dword r0;
    dword r1;
    dword r2;
    dword r3;
    dword r12;
    void  *lr;
    void  *pc;
    dword psr;
    dword s[16];
    dword fpscr;
    dword fill;     // highest address on stack
} t_exception;

/**
 * THis is the set of regísters which are not saved by the hardware but by
 * the procedure save/restore context on the stack.
 */
typedef struct {
    dword r4;      // lowest address
    dword r5;
    dword r6;
    dword r7;
    dword r8;
    dword r9;
    dword r10;
    dword r11;

    dword ret;
                    // *** exception stack frame 
    t_exception ex;
}  t_context;

/**
 * @brief Create the initial stack layout for the a task
 * @details Create an initial task context on the given stack and returns pointer to it.
 * 
 * @param POINTER  point to the top of the stack.
 * @param POINTER  points to the start address
 * @param pvParameters arguments passed to the task
 * @return  the place where the process context starts on the stack
 */
DATA POINTER _machdep_initialize_stack(DATA POINTER topOfStack, t_entry entry, TASK_ARGUMENT pvParameters ) {
    TASK_ARGUMENT args __attribute__((unused)) = pvParameters;
    t_context *f = (t_context*) (topOfStack-sizeof(t_context));

    f->ex.fill = 0x4711;
    f->ex.fpscr = 0;
    f->ex.psr = 0x01000000;                 // reset value of the PSR
    f->ex.pc = (POINTER)entry;              
    f->ex.lr = (void*)&_os_task_terminated;

    f->ex.r0 = 1;
    f->ex.r1 = 2;
    f->ex.r2 = 3;
    f->ex.r3 = 4;
    f->r4 = 5;
    f->r5 = 6;
    f->r6 = 7;
    f->r7 = 8;
    f->r8 = 9;
    f->r9= 10;
    f->r10 = 11;
    f->r11 = 12;
    f->ex.r12 = 13;
    f->ret = 0xFFFFFFED;   // return to thread mode 
    return (DATA POINTER)f;
}

/**
 * @brief enter multitaksing mode
 * @details The stm32 provides to stacks; this function set the msp to the kernel stack and 
 *          the psp to the next process to be scheduled.
 */
inline void _machdep_boot(void) {    
    __asm volatile (
    "    ldr r0, =_os_current_stack     \n"
    "    ldr r12,[r0,#0]                \n"
    "    msr psp,r12                    \n"
    "    ldr r0, =_os_kernel_stack      \n"
    "    ldr r12,[r0,#0]                \n"
    "    msr msp,r12                    \n"
    "    isb                            \n"
    "    cpsie i                        \n"  
    "    cpsie f                        \n"
    "    svc 0                          \n"       
    );
}

/**
 * @brief Save context 
 * @details Save the context and set the current stack 
 */
static inline void _machdep_save_context(void) {
    __asm (
    "   mrs r0, PSP                         \n"
    "   stmdb r0!, {r4-r11}                 \n" 
    "   msr psp,r0                          \n"
    "   ldr r12, =_os_current_stack         \n"
    "   str r0,[r12]                        \n"
    );
}

/**
 * @brief Restore the context from the curent tcb
 * @details [long description]
 */
static inline void _machdep_restore_context(void){
    __asm (
    "   ldr r0, =_os_current_stack         \n"
    "   ldr r0,[r0,#0]                     \n"
    "   ldmia r0!, {r4-r11}                \n" // load context of new thread
    "   msr PSP, r0                        \n"
    "   isb                                \n"
    );
}

/*
 * The following bioth routines working toghether. Calling _machdep_yield will 
 * force a schedule and a context switch. THe contents switch will be done in
 * handler mode by the sv_call_handfler;
 */
__attribute__ ((naked)) void _machdep_yield(void) {    
    _machdep_save_context();
    _os_schedule();
    
    __asm volatile ( "   svc 0           \n");  
}

/*
 * This function is called it initiates are context which to the newly scheduled
 * task.
 */
__attribute__ ((naked)) void sv_call_handler(void) {
    __asm volatile (   
    "   ldr r0, =_os_current_stack        \n"
    "   ldr r0,[r0,#0]                    \n"
    "   ldmia r0!, {r4-r11,lr}            \n"
    "   msr psp, r0                       \n"
    "   isb                               \n"
    "   bx lr                             \n"
    );
}

/*
 * Initialize the scheduler. Interrupts etc are expected to the setup
 * during the system initialization phase, but the interrupts are
 * turned off.
 */
void _machdep_initialize_timer(void) {
    systick_set_reload(168000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    systick_interrupt_enable();
    
    rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_168MHZ]);
    /* Enable GPIOD clock. */
    rcc_periph_clock_enable(RCC_GPIOG);

    /* TODO: not related to timers */
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13|GPIO14);	

//if( _oil_use_preemption ) {
//}
}

/**
 * @brief  System Clock Handler
 * @details This is the handler for the tick timer interrupt. Since T2 is not available
 * to scale down the clock rate to TICK_MS an additional in memory counter
 * is used to achieve the target clock rate.
 */
void sys_tick_handler(void) {
    _os_mode = KERNEL_MODE;
    _os_alarm_scheduler();
    //_os_schedule();
    _os_mode = USER_MODE;

    /* this is the preemption timer */
    if( ++system_millis % 1000 == 0 ) {
        SCB_ICSR |= SCB_ICSR_PENDSVSET;
    }

}

/**
 * @brief perform a context switch
 * @details [long description]
 * 
 * @return none
 */
__attribute__ ((naked)) void  pend_sv_handler() {
    _machdep_save_context();
    // select a new conext
    _os_mode = KERNEL_MODE;
    _os_schedule();
    _os_mode = USER_MODE;

    _machdep_restore_context();

    __asm volatile (
     "   ldr r0, .pend_sv_l1            \n"
     "   bx  r0                         \n"

     "   .align 2                       \n"
     ".pend_sv_l1:                      \n"
     "   .word -13                      \n"    );
}

/**
 * @brief Create a banner
 * @details This procedure create a banner
 */
void _machdep_banner() {
    sdram_init();
    lcd_spi_init();

    gfx_init(lcd_draw_pixel, 240, 320);
    
    gfx_fillScreen(LCD_GREY);
    gfx_setTextSize(1);
    gfx_setCursor(15, 30);
    gfx_puts("STM32F4-DISCO");

    lcd_show_frame();
}

/**
 * @brief begin a critical section
 * @details [long description]
 */
void _machdep_critical_begin(void) {
    __disable_irq();
}

/**
 * @brief critical section end
 * @details [long description]
 */
void _machdep_critical_end(void) {
    __enable_irq();
}

/*
 * Manage the watchdog 
 */
void _machdep_initialize_wdt(void) {
#if USE_WATCHDOG == 1 
#endif
}

/**
 * @brief clear the wdt timer
 * @details [long description]
 */
void _machdep_clear_wdt(void) {
#if USE_WATCHDOG == 1 
#endif
}

/**
 * @brief Output trace information
 * @details [long description]
 * 
 * @param code [description]
 */
void _machdep_trace(unsigned code ) {
    switch(code) {
        case TRACE_ALIVE:
            gpio_toggle(GPIOG, GPIO13);
            break;
        case TRACE_PANIC_UNDERFLOW:
            break;
    }

}

/*
 * int print_hex(int value)
 *
 * Very simple routine for printing out hex constants.
 */
static void print_hex(unsigned long v) {
    int ndx = 0;
    char buf[20];
 
    buf[ndx++]='0'; buf[ndx++]='x';

    do {
        char  c = v & 0xf;
        buf[ndx++] = (c > 9) ? '7' + c : '0' + c;
        v = (v >> 4) & 0x0fffffff;
    } while (v != 0);

    buf[ndx++] = ' ';
    buf[ndx++] = '\000';

    gfx_puts(buf);
}


static int row = 18;
static int col = 5;
#define nl { row = row + 18; gfx_setCursor(col,row); }

/**
 * @brief [brief description]
 * @details [long description]
 */
void  hard_fault_handler(void) {
    dword *ex = (dword*)0;

    // here we are calculating the stack pointer. If you add any local defintions
    // this might change
    __asm__ __volatile__ (
        "mrs %0, msp         \n\t"
        :"=r"(ex)
        :
        :"r0"
    );
    ex = &ex[10]; 
    // end getting the stack pointer

    volatile unsigned long hfsr = SCB_HFSR ;
    volatile unsigned long cfsr = SCB_CFSR ;

    volatile dword psr = (dword)((t_exception*)ex)->psr;
    volatile dword pc  = (dword)((t_exception*)ex)->pc;   
    volatile dword lr  = (dword)((t_exception*)ex)->lr;

    gfx_fillScreen(LCD_GREY);
    gfx_setTextSize(1); nl;
    
    gfx_puts("hard fault exception"); nl;
    gfx_puts("hfsr:"); print_hex(hfsr); nl;

    if( cfsr & SCB_HFSR_FORCED ) {
        gfx_puts("forced exception"); nl;
        if(cfsr & 0xFFFF0000) {
            int i;
            unsigned long ufsr = cfsr >> 16;
            gfx_puts("ufsr:"); print_hex(ufsr); nl;

            for(i=0; i<16; ++i ) {
                if( (ufsr & (1<<i)) == 0 )
                    continue;

                switch(i) {
                    case 0:  gfx_puts("UNDEF "); 
                        break;
                    case 1:  gfx_puts("INVST "); 
                        break;
                    case 2:  gfx_puts("INVPC ");
                        break;
                    case 3:  gfx_puts("NOCP ");
                        break;

                    case 8:  gfx_puts("UNALLIGN");
                        break;
                    case 9:  gfx_puts("DIVBYZERO ");
                        break;
                } 
            }
            nl;
        }
    }

    gfx_puts("psr :"); print_hex(psr); nl;
    gfx_puts("pc  :"); print_hex(pc); nl;
    gfx_puts("lr  :"); print_hex(lr); nl;
    gfx_puts("ccr :"); print_hex(SCB_CCR); nl;
    lcd_show_frame();

    while(1);
}

/**
 * @brief [brief description]
 * @details [long description]
 */
void  nmi_handler() {
}

/* Taken from
 * http://stackoverflow.com/questions/5745880/simulating-ldrex-strex-load-store-exclusive-in-cortex-m0
*/

#define unlikely(x) __builtin_expect((dword)(x),0)

static inline dword atomic_LL(volatile void *addr) {
    dword dest;

    __asm__ __volatile__("ldrex %0, [%1]" : "=r" (dest) : "r" (addr));
    
    return dest;
}

static inline dword atomic_SC(volatile void *addr, dword value) {
    dword dest;

    __asm__ __volatile__("strex %0, %2, [%1]" :
          "=&r" (dest) : "r" (addr), "r" (value) : "memory");

    return dest;
}

/**
 * atomic Compare And Swap
 * @param addr Address
 * @param expected Expected value in *addr
 * @param store Value to be stored, if (*addr == expected).
 * @return 0  ok, 1 failure.
 */
inline BOOL _machdep_cas_byte(volatile void *addr, dword expected, dword store) {
    
    if (unlikely(atomic_LL(addr) != expected))
        return FALSE;

    return atomic_SC(addr, store) ? FALSE : TRUE;
}

