#include <Arduino.h>

void setup()
{
    // Initialise USB Serial communication
    Serial.begin(115200);

    // Give USB time to initialise
    delay(2000);
}

void loop()
{
    // Print a message every second
    Serial.println("ESP32-S3 is running!");

    delay(1000);
}