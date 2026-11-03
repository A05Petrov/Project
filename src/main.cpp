
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// TFT pins
#define TFT_CS   10
#define TFT_DC   13
#define TFT_RST  14

#define TFT_MOSI 11
#define TFT_SCLK 12

// Joystick pins
#define JOYSTICK_X  4
#define JOYSTICK_Y  5
#define JOYSTICK_SW 6

// Create display object
Adafruit_ST7789 tft = Adafruit_ST7789(
    TFT_CS, TFT_DC, TFT_RST
);

// Menu options
const char* menuItems[] = {
    "Pressure (hPa)",
    "Pressure (mbar)",
    "Temperature (C)"
};

const int menuSize = 3;

int selectedItem = 0;

// Screen state
bool onMenu = true;

// Joystick thresholds
const int UP_THRESHOLD = 1000;
const int DOWN_THRESHOLD = 3000;

// Button state
bool lastButtonState = HIGH;

// Timing
unsigned long lastMoveTime = 0;
unsigned long lastButtonTime = 0;

const unsigned long MOVE_DELAY = 250;
const unsigned long BUTTON_DELAY = 250;


// Draw main menu
void drawMenu()
{
    tft.fillScreen(ST77XX_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_CYAN);

    tft.setCursor(45, 25);
    tft.println("WEATHER");

    tft.setCursor(45, 50);
    tft.println("STATION");

    for (int i = 0; i < menuSize; i++)
    {
        int y = 110 + (i * 55);

        if (i == selectedItem)
        {
            tft.fillRect(15, y - 5, 290, 40, ST77XX_BLUE);
        }

        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(25, y + 5);
        tft.println(menuItems[i]);
    }

    tft.setTextSize(1);
    tft.setCursor(45, 290);
    tft.println("Move joystick to navigate");
}


// Draw selected measurement page
void drawMeasurementPage()
{
    tft.fillScreen(ST77XX_BLACK);

    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(2);

    tft.setCursor(20, 25);
    tft.println(menuItems[selectedItem]);

    tft.drawFastHLine(15, 65, 290, ST77XX_WHITE);

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);

    tft.setCursor(35, 120);

    // Temporary example values
    if (selectedItem == 0)
    {
        tft.println("1013 hPa");
    }
    else if (selectedItem == 1)
    {
        tft.println("1013 mbar");
    }
    else if (selectedItem == 2)
    {
        tft.println("22.5 C");
    }

    tft.setTextSize(1);
    tft.setCursor(45, 200);
    tft.println("Press to return to menu");
}


// Read joystick movement
void readJoystick()
{
    static bool joystickReady = true;

    int yValue = analogRead(JOYSTICK_Y);

    // Wait for joystick to return to its centre
    if (yValue > 1200 && yValue < 2800)
    {
        joystickReady = true;
        return;
    }

    // Only accept one movement until joystick is released
    if (joystickReady &&
        millis() - lastMoveTime >= 300)
    {
        if (yValue < 1000)
        {
            selectedItem--;

            if (selectedItem < 0)
            {
                selectedItem = menuSize - 1;
            }

            drawMenu();
            joystickReady = false;
            lastMoveTime = millis();
        }
        else if (yValue > 3000)
        {
            selectedItem++;

            if (selectedItem >= menuSize)
            {
                selectedItem = 0;
            }

            drawMenu();
            joystickReady = false;
            lastMoveTime = millis();
        }
    }
}

// Read joystick button
void readButton()
{
    bool buttonState = digitalRead(JOYSTICK_SW);

    if (buttonState == LOW &&
        lastButtonState == HIGH &&
        millis() - lastButtonTime >= BUTTON_DELAY)
    {
        if (onMenu)
        {
            onMenu = false;
            drawMeasurementPage();
        }
        else
        {
            onMenu = true;
            drawMenu();
        }

        lastButtonTime = millis();
    }

    lastButtonState = buttonState;
}


void setup()
{
    Serial.begin(115200);
    delay(2000);

    pinMode(JOYSTICK_SW, INPUT_PULLUP);

    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

    tft.init(240, 320);
    tft.setRotation(1);

    drawMenu();

    Serial.println("Weather Station Started");
}


void loop()
{
    if (onMenu)
    {
        readJoystick();
    }

    readButton();
}
