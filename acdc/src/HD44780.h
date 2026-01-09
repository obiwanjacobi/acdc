#pragma once
#include "../lib/HD44780_DriverIO.h"
#include "../lib/HD44780_Driver.h"
#include "../lib/HD44780_Controller.h"
#include "../lib/HD44780_View.h"
#include "../lib/BitArrayWriter.h"
#include "../lib/Twi.h"
#include "../lib/atl/TextWriter.h"

/// this is specific to the i2c-lcd board
static const uint8_t RegSel_Index = 0;
static const uint8_t ReadWrite_Index = 1;
static const uint8_t Enable_Index = 2;
static const uint8_t LED_Index = 3;
static const uint8_t Data04_Index = 4;
static const uint8_t Data15_Index = 5;
static const uint8_t Data26_Index = 6;
static const uint8_t Data37_Index = 7;

// I2C IO expanders on HD44780 LCDs can have addresses ranging from: 0x20-0x27 and 0x38-0x3F.
// Also (typically) the A0-A2 pins on the PCB are pulled high, so address ranges start at the end (0x27/0x3F)

// clang-format off
typedef TextWriter<
    HD44780_View<
        HD44780_Controller<
            HD44780_Driver<
                HD44780_DriverBitArray<
                    I2C_BitArrayWriter<I2cT, 0x3F>,
                    RegSel_Index, Enable_Index, Data04_Index, Data15_Index, Data26_Index, Data37_Index,
                    BitArray<uint8_t>
                >,
                HD44780_ProfileTweaked
            >,
            HD44780_ProfileTweaked
        >, 
        2, 16
    >
> LCD;
// typedef TextWriter<
//     HD44780_View<
//         HD44780_Controller<
//             HD44780_Driver<
//                 HD44780_DriverPins<DigitalOutputPin<PortPins::D2>, DigitalOutputPin<PortPins::D3>, 
//                     DigitalOutputPin<PortPins::D4>, DigitalOutputPin<PortPins::D5>,
//                     DigitalOutputPin<PortPins::D6>, DigitalOutputPin<PortPins::D7>
//                 >
//             >
//         >,
//         2, 16
//     >
// > LCD;
// clang-format on
