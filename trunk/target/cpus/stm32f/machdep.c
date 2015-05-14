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

/** Process Manager
 * This is the process manager of mkernel.
 * The scheduler is called either by the tick handler which is called by
 * a low priority interrupt for by the YIELD method.
 * Additionally to the scheduler the module contains also the task API.
 *
 */
#include "mkernel_config.h"
#include "kernel.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "typedefs.h"
#include "alarm.h"
#include "machdep.h"

extern DATA POINTER _os_current_stack;

/* this counter instance is used for the system clock */
static AlarmBaseType System_Tick_Base = { 4294967295, 1, 100 };
t_counter System_Tick = { 0,0, &System_Tick_Base };

extern void System_Tick_Handler(void);          // comminig frm the alarm module

/* milliseconds since boot */
static volatile uint32_t system_millis;         // simple 


void msleep(uint32_t delay)
{
        uint32_t wake = system_millis + delay;
        while (wake > system_millis);
}


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
    dword r0;
    dword r1;
    dword r2;
    dword r3;
    dword r12;
    dword lr;
    dword pc;
    dword psr;
    dword s[16];
    dword fpscr;
    dword fill;     // highest address on stack
}  t_frame;


DATA POINTER _machdep_initialize_stack(DATA POINTER topOfStack, DATA POINTER addr, TASK_ARGUMENT pvParameters ) {
    TASK_ARGUMENT args __attribute__((unused)) = pvParameters;
    t_frame *f = (t_frame*) (topOfStack-sizeof(t_frame));

    f->fill = 0x4711;
    f->fpscr = 0;
    f->psr = 0;
    f->pc = (dword)addr;
    f->lr = (dword)&_os_task_terminated;

    f->r0 = 1;
    f->r1 = 2;
    f->r2 = 3;
    f->r3 = 4;
    f->r4 = 5;
    f->r5 = 6;
    f->r6 = 7;
    f->r7 = 8;
    f->r8 = 9;
    f->r9= 10;
    f->r10 = 11;
    f->r11 = 12;
    f->r12 = 13;
    f->ret = 0xfffffffd;
    return (DATA POINTER)f;
}

/*
 * The CPU supports two stacks, in thread mode. THis function switchs to process stack 
 * pointer which is stored in the variable _os_current_stack and it restores the registers
 * which have been pushed to the stack.
*/
__attribute__ ((naked)) void _machdep_restore_context() {
   asm( 
        "ldr r0, =_os_current_stack \n\t"
        "ldr sp,[r0,#0]             \n\t"

        "pop {r4-r11}               \n\t"
        "pop {pc}                   \n\t" 
    );    
}

/*
 * Save all application registets on the stack and set the _os_current_stack 
 * value.
 */
__attribute__ ((naked)) void _machdep_save_context() {
    asm(
        "push {r0-r12}              \n\t"

        "msr  apsr,r0               \n\t"
        "push {r0}                  \n\t"

        "push {lr}                  \n\t"

        "ldr r0, =_os_current_stack \n\t"
        "str sp,[r0,#0]             \n\t"
    );
}



__attribute__ ((naked)) void _machdep_yield(void) {    
  asm( 
        "ldr r0, =_os_current_stack \n\t"
        "ldr sp,[r0,#0]             \n\t"
        "isb                        \n\t"

        "pop {r4-r11}               \n\t"
        "pop {pc}                   \n\t" 
    );       
}



/*  
 * The stm32 provides to stacks; this function switches to this thread mode.
 */
__attribute__ ((naked)) void _machdep_boot(void) {    
   asm(
        "ldr r0, =_os_kernel_stack      \n\t"
        "ldr sp,[r0,#0]                 \n\t"
        "mov r0,#0b011                  \n\t"
        "msr control, r0                \n\t"
        "isb                            \n\t"
        "bx lr                          \n\t"
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
    //rcc_periph_clock_enable(RCC_GPIOG);

    /* TODO: not related to timers */
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13|GPIO14);	

//if( _oil_use_preemption ) {
//}
}

/*
 * This is the handler for the tick timer interrupt. Since T2 is not available
 * to scale down the clock rate to TICK_MS an additional in memory counter
 * is used to achieve the target clock rate.
 *
 */
void sys_tick_handler(void) {
    system_millis++;

#ifdef FOO
    _machdep_save_context();

    _os_mode = KERNEL_MODE;

    _os_alarm_scheduler();
    _os_schedule();

    _os_mode = USER_MODE;

    _machdep_restore_context();
    /* you will never get here */
#endif
}

/*
 * Manage crical section.
 */
void _machdep_critical_begin(void) {
    void __disable_irq();
} 

void _machdep_critical_end(void) {
    void __enable_irq();
}

/*
 * Manage the watchdog 
 */
void _machdep_initialize_wdt(void) {
#if USE_WATCHDOG == 1 
#endif
}

void _machdep_clear_wdt(void) {
#if USE_WATCHDOG == 1 
#endif
}

/*
 * Output some trace information on the two led's of the discovery board.
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