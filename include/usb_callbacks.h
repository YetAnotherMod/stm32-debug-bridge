#pragma once

#include <usb_std.h>

namespace usb{
void controlRxHandler();
void controlTxHandler();
void controlSetupHandler();

void uartRxHandler();
void uartTxHandler();
void uartInterruptHandler();

void shellRxHandler();
void shellTxHandler();
void shellInterruptHandler();

void jtagRxHandler();
void jtagTxHandler();
void jtagInterruptHandler();
}