/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor1;
VL53L0X sensor2;

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 7
#define SHT_LOX2 6

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  // all reset
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and resetting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // sensor1.setTimeout(500);
  // sensor2.setTimeout(500);

  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor1 !");
    while (1)
    {
    }
  }
  sensor1.setAddress(LOX1_ADDRESS);

  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  if (!sensor2.init())
  {
    Serial.println("Failed to detect and initialize sensor2 !");
    while (1)
    {
    }
  }
  sensor2.setAddress(LOX2_ADDRESS);

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor1.startContinuous();
  sensor2.startContinuous();
}

void loop()
{
  Serial.print("1: ");
  Serial.print(sensor1.readRangeContinuousMillimeters());
  if (sensor1.timeoutOccurred())
  {
    Serial.print(" TIMEOUT 1");
  }

  Serial.print(" - 2: ");
  Serial.print(sensor2.readRangeContinuousMillimeters());
  if (sensor2.timeoutOccurred())
  {
    Serial.print(" TIMEOUT 2");
  }

  Serial.println();
}
