/*
 * Brian R Taylor
 * brian.taylor@bolderflight.com
 * 
 * Copyright (c) 2018 Bolder Flight Systems, 2026 Simon D. Levy
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Wire.h>

#include "BMI088.h"

static const uint8_t GYRO_DRDY_PIN = 4; 

static auto imu_ = Bmi088(Wire, 0x18, 0x68);

static volatile bool gyro_flag_;

static auto Okay(const int status) -> bool
{
    return status >= 0;
}

static void GyroDrdy()
{
    gyro_flag_ = true;
}

static void PrintFloat(const float x)
{
    Serial.print(x > 0 ? "+" : x < 0 ? "" : " ");
    Serial.print(x, 4);
}

void setup() 
{
    Serial.begin(115200);
    while(!Serial) {}

    if (!(
                Okay(imu_.begin())

                && Okay(imu_.setOdr(Bmi088::ODR_1000HZ))

                && Okay(imu_.setRange(Bmi088::ACCEL_RANGE_24G,
                        Bmi088::GYRO_RANGE_2000DPS))

                && imu_.pinModeDrdy(
                    Bmi088::PIN_MODE_PUSH_PULL, Bmi088::PIN_LEVEL_ACTIVE_HIGH)

                && imu_.mapDrdy(Bmi088::PIN_1)

         )) {

        while (true) {
            Serial.println("Unable to initialize BMI088");
            delay(500);
        }
    }

    pinMode(GYRO_DRDY_PIN, INPUT);
    attachInterrupt(GYRO_DRDY_PIN, GyroDrdy, RISING);  
}

void loop() 
{
    if (gyro_flag_) {

        imu_.readSensor();

        gyro_flag_ = false;

        Serial.print("accel: ");
        PrintFloat(imu_.getAccelX_mss());
        Serial.print(" m/s^2  ");
        PrintFloat(imu_.getAccelY_mss());
        Serial.print(" m/s^2  ");
        PrintFloat(imu_.getAccelZ_mss());
        Serial.print(" m/s^2  ");
        PrintFloat(imu_.getTemperature_C());
        Serial.print(" degC | ");
        Serial.print("gyro: ");
        PrintFloat(imu_.getGyroX_rads());
        Serial.print(" rad/s ");
        PrintFloat(imu_.getGyroY_rads());
        Serial.print(" rad/s ");
        PrintFloat(imu_.getGyroZ_rads());
        Serial.print(" rad/s\n");
     }

}
