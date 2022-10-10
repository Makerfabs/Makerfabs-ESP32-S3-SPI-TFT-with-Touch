
#include "SPI.h"
#include "SPI_9488.h"

#define COLOR_BACKGROUND TFT_WHITE
#define COLOR_TEXT TFT_BLACK

LGFX lcd;

void setup()
{
    Serial.begin(115200);
    Serial.println("Keyboard begin");

    lcd_init();
}

void loop()
{
    for (int i = 0; i < 255; i++)
    {
        lcd.setBrightness(i);
        delay(10);
    }
    for (int i = 0; i < 255; i++)
    {
        lcd.setBrightness(255 - i);
        delay(10);
    }
}

// Hardware init
void lcd_init()
{
    // lcd init
    lcd.init();
    lcd.fillScreen(COLOR_BACKGROUND);
    lcd.setTextColor(COLOR_TEXT);
    lcd.setTextSize(2);
    lcd.setCursor(10, 10);
    lcd.print("Makerfabs ESP32-S3");
    lcd.setCursor(10, 26);
    lcd.print("SPI lcd with Touch");
    lcd.setCursor(10, 42);
    lcd.print("Back Light Demo");

    lcd.fillRect(0, 100, 320, 80, TFT_YELLOW);
    lcd.fillRect(0, 200, 320, 80, TFT_BLUE);
    lcd.fillRect(0, 300, 320, 80, TFT_RED);
    lcd.fillRect(0, 400, 320, 80, TFT_GREEN);
}
