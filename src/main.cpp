#include <Arduino.h>
#include <Wire.h>

#define I2C_SDA 8
#define I2C_SCL 9

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println("BMP280 I2C Scanner");

    Wire.begin(I2C_SDA, I2C_SCL);

    Serial.println("Scanning...");
}

void loop()
{
    byte error;
    int devicesFound = 0;

    for (byte address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address: 0x");
            Serial.println(address, HEX);
            devicesFound++;
        }
    }

    if (devicesFound == 0)
    {
        Serial.println("No I2C devices found.");
    }

    Serial.println("Scan complete.\n");

    delay(3000);
}