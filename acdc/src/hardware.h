#pragma once
#include "../lib/Twi.h"
#include "../lib/Port.h"
#include "../lib/DigitalOutputPin.h"
#include "../lib/VL53L0X.h"
#include "../lib/INA219.h"
#include "../lib/PCA9685.h"
#include "../lib/PCA9685_PwmOutputPin.h"
#include "../lib/TB6612FNG_Controller.h"
#include "../lib/TB6612FNG_Driver.h"
#include "MotorController.h"

typedef TwiReceive<TwiTransmit<Twi>> I2cT;
typedef INA219<I2cT, 0x40> Ina219T_0;
typedef INA219<I2cT, 0x41> Ina219T_1;
typedef INA219<I2cT, 0x44> Ina219T_2;
typedef INA219<I2cT, 0x45> Ina219T_3;
typedef PCA9685<I2cT, 0x46> PwmModuleT;

// typedef VL53L0X<I2cT, DigitalOutputPin<PortPins::D6>, 0x50> VL53L0XT_0;
// typedef VL53L0X<I2cT, DigitalOutputPin<PortPins::D7>, 0x51> VL53L0XT_1;

// clang-format off
template <const PCA9685_Pins PwmPinId, const PortPins In1PinId, const PortPins In2PinId>
class MotorControllerT : public MotorController<
    TB6612FNG_Controller<TB6612FNG_Driver<
        PCA9685_PwmOutputPin<PwmModuleT, PwmPinId>, 
        DigitalOutputPin<In1PinId>, 
        DigitalOutputPin<In2PinId>
    >>
> {};
// clang-format on

typedef MotorControllerT<PCA9685_Pins::Pin0, PortPins::D2, PortPins::D3> MotorControllerT_0;
typedef MotorControllerT<PCA9685_Pins::Pin1, PortPins::D4, PortPins::D5> MotorControllerT_1;
typedef MotorControllerT<PCA9685_Pins::Pin2, PortPins::D6, PortPins::D7> MotorControllerT_2;
typedef MotorControllerT<PCA9685_Pins::Pin3, PortPins::B0, PortPins::B1> MotorControllerT_3;
