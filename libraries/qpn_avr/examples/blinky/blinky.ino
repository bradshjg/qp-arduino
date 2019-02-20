/*$file${.::blinky.ino} ####################################################*/
/*
* Model: blinky.qm
* File:  ${.::blinky.ino}
*
* This code has been generated by QM 4.4.0 (https://www.state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*$endhead${.::blinky.ino} #################################################*/
#include "qpn.h"     // QP-nano framework
#include "Arduino.h" // Arduino API

//============================================================================
// declare all AO classes...
/*$declare${AOs::Blinky} ###################################################*/
/*${AOs::Blinky} ...........................................................*/
typedef struct Blinky {
/* protected: */
    QActive super;
} Blinky;

/* protected: */
static QState Blinky_initial(Blinky * const me);
static QState Blinky_off(Blinky * const me);
static QState Blinky_on(Blinky * const me);
/*$enddecl${AOs::Blinky} ###################################################*/
//...

// AO instances and event queue buffers for them...
Blinky AO_Blinky;
static QEvt l_blinkyQSto[10]; // Event queue storage for Blinky
//...

//============================================================================
// QF_active[] array defines all active object control blocks ----------------
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,           (QEvt *)0,        0U                  },
    { (QActive *)&AO_Blinky,  l_blinkyQSto,     Q_DIM(l_blinkyQSto) }
};

//============================================================================
// various constants for the application...
enum {
    BSP_TICKS_PER_SEC = 100, // number of system clock ticks in one second
    LED_L = 13               // the pin number of the on-board LED (L)
};

//............................................................................
void setup() {
    // initialize the QF-nano framework
    QF_init(Q_DIM(QF_active));

    // initialize all AOs...
    QActive_ctor(&AO_Blinky.super, Q_STATE_CAST(&Blinky_initial));

    // initialize the hardware used in this sketch...
    pinMode(LED_L, OUTPUT); // set the LED-L pin to output
}

//............................................................................
void loop() {
    QF_run(); // run the QF-nano framework
}

//============================================================================
// interrupts...
ISR(TIMER2_COMPA_vect) {
    QF_tickXISR(0); // process time events for tick rate 0
}

//============================================================================
// QF callbacks...
void QF_onStartup(void) {
    // set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking...
    TCCR2A = (1U << WGM21) | (0U << WGM20);
    TCCR2B = (1U << CS22 ) | (1U << CS21) | (1U << CS20); // 1/2^10
    ASSR  &= ~(1U << AS2);
    TIMSK2 = (1U << OCIE2A); // enable TIMER2 compare Interrupt
    TCNT2  = 0U;

    // set the output-compare register based on the desired tick frequency
    OCR2A  = (F_CPU / BSP_TICKS_PER_SEC / 1024U) - 1U;
}
//............................................................................
void QV_onIdle(void) {   // called with interrupts DISABLED
    // Put the CPU and peripherals to the low-power mode. You might
    // need to customize the clock management for your application,
    // see the datasheet for your particular AVR MCU.
    SMCR = (0 << SM0) | (1 << SE); // idle mode, adjust to your project
    QV_CPU_SLEEP();  // atomically go to sleep and enable interrupts
}
//............................................................................
void Q_onAssert(char const Q_ROM * const file, int line) {
    // implement the error-handling policy for your application!!!
    QF_INT_DISABLE(); // disable all interrupts
    QF_RESET();  // reset the CPU
}

//============================================================================
// define all AO classes (state machine)...
/*$skip${QP_VERSION} #######################################################*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 640U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpn version 6.4.0 or higher required
#endif
/*$endskip${QP_VERSION} ####################################################*/
/*$define${AOs::Blinky} ####################################################*/
/*${AOs::Blinky} ...........................................................*/
/*${AOs::Blinky::SM} .......................................................*/
static QState Blinky_initial(Blinky * const me) {
    /*${AOs::Blinky::SM::initial} */
        QActive_armX((QActive *)me, 0U,
                     BSP_TICKS_PER_SEC/2U, BSP_TICKS_PER_SEC/2U);
    return Q_TRAN(&Blinky_off);
}
/*${AOs::Blinky::SM::off} ..................................................*/
static QState Blinky_off(Blinky * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Blinky::SM::off} */
        case Q_ENTRY_SIG: {
            digitalWrite(LED_L, LOW);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Blinky::SM::off::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Blinky_on);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::Blinky::SM::on} ...................................................*/
static QState Blinky_on(Blinky * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Blinky::SM::on} */
        case Q_ENTRY_SIG: {
            digitalWrite(LED_L, HIGH);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Blinky::SM::on::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Blinky_off);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Blinky} ####################################################*/
//...
