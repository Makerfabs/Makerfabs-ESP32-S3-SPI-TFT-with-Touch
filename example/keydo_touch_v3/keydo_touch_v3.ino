#include <FT6236.h>
#include "SPI.h"
#include "SPI_9488.h"
#include "Button.h"

#include <vector>
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#define SD_MOSI 2
#define SD_MISO 41
#define SD_SCK 42
#define SD_CS 1

//touchscreen detail
#define I2C_SCL 39
#define I2C_SDA 38

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "USBHIDConsumerControl.h"


#define SYSTEM 1 //2 : google meet (no support to hang-up and accept calls.  1 : MS Teams
#define BUTTON_DELAY 150
#define BUTTON_COUNT_M 7

#define LCD_BRIGHTNESS 255 //LCD brightness between 0 to 255
#define LCD_IDLE_TIME 120000 //time to turn off the display without stopping the code
#define MOUSE_GIGGLER_TIME 50000 //time to send a small mouse movement to keep the pc and teams active


USBHIDConsumerControl ConsumerControl;
USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;
FT6236 ts = FT6236();
LGFX lcd;

SPIClass SD_SPI;

unsigned long startMillis; 
unsigned long currentMillis;
unsigned long lastTouchedMillis;


bool touch_checked = false;
bool lcd_off = false;
bool Mouse_giggler = true;
bool mic_muted = false;
bool speakerMute = false;
Button b[BUTTON_COUNT_M];

void setup(void)
{
    Serial.begin(9600);
    
    ConsumerControl.begin();
    Keyboard.begin();
    Mouse.begin();
    USB.begin();
    
    ts.begin(40, I2C_SDA, I2C_SCL);
    if (!ts.begin(40)) //40 in this case represents the sensitivity. Try higer or lower for better response. 
    {
        Serial.println("Unable to start the capacitive touchscreen.");
    }
    lcd.init();
    //lcd.setRotation();

    SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS, SD_SPI, 40000000))
    {
        Serial.println("Card Mount Failed");
        lcd.setCursor(10, 10);
        lcd.println("SD Card Failed");
        while (1)
            delay(1000);
    }
    else
    {
        Serial.println("Card Mount Successed");
    }

    Serial.println("SD init over.");
    print_img(SD, "/horizon.bmp", 320, 480, 0,0,0,0,0,0);
    delay(2000);
    main_page();
    
}

void loop(void)
{
  
}
void main_page()
{
    
    b[0].set(0,0 ,80,80, "", 0);
    b[1].set(230,0 ,80,80, "", 1);

    b[3].set(85,0,150,160, "", 3);
    b[2].set(0,170,310,190, "", 2);
    

    b[4].set(0,400 ,90,90, "", 4);
    b[5].set(110,400 ,100,90, "", 5);
    b[6].set(220,400 ,100,90, "", 6);
    
    print_img(SD, "/horizon_active.bmp", 320, 480, 0,0,0,410,70,0);
    
    while(1)
    {
      
      if (ts.touched())
      {
          // Retrieve a point
          TS_Point p = ts.getPoint();
  
          // Print coordinates to the serial output
          if(touch_checked==false){
            lastTouchedMillis = millis();
            if(lcd_off == true){
              lcd_off = false;
              lcd.setBrightness(LCD_BRIGHTNESS);
            }else {
                //check the area
                //Serial.print("X Coordinate: ");
                //Serial.println(p.x);
                //Serial.print("Y Coordinate: ");
                //Serial.println(p.y);
                for (int i = 0; i < BUTTON_COUNT_M; i++)
                {
                    int button_value = UNABLE;
                    if ((button_value = b[i].checkTouch(p.x, p.y)) != UNABLE)
                    {
                        command(button_value);
                        delay(BUTTON_DELAY);  
                    }
                }
            }
            touch_checked = true;          
          }
      }else{
          touch_checked = false;
      }
      delay(50);//Debouncing. To avoid returning the same touch multiple times you can play with this delay.
      
      //here should action routine
      currentMillis = millis();
      // you can insert a small annimations function to make sure that the device is still functionning
      
      //time for idel
      if((currentMillis - lastTouchedMillis >= LCD_IDLE_TIME) && lcd_off!=true)
      {
        turn_off();
      }
      //mouse giggler
      if((currentMillis - startMillis >= MOUSE_GIGGLER_TIME) && Mouse_giggler)
      {
        startMillis = currentMillis;
        Mouse.move(5, 0, 0);
        delay(50);
        Mouse.move(-5, 0, 0);
      }
      
  }
}

void command(int cmd)
{
   switch (cmd)
    {
    case 0:
        keep_wake();
        break;
    case 1:
        turn_off();
        break;
    case 2:
        mic_mute();
        break;
    case 3:
        speakers_mute();
        break;
    case 4:
        call_accept();
        break;
    case 5:
        video();
        break;
    case 6:
        hang_up();
        break;
    defualt:
        break;
    }
    delay(100);
  
}
void mic_mute()
{
  if(SYSTEM == 1)
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press('m');
    Keyboard.releaseAll(); 
    
    if(mic_muted)
    {
      print_img(SD, "/horizon.bmp", 320, 320, 0,160,0,100,0,0);
      mic_muted = false;
    }else{
      print_img(SD, "/horizon_active.bmp", 320, 320, 0,160,0,100,0,0);
      mic_muted = true;
    }
    
  }else if(SYSTEM == 2)
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('d');
    Keyboard.releaseAll(); 
  }
}

void call_accept()
{
  if(SYSTEM == 1)
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press('s');
    Keyboard.releaseAll(); 
  }else if(SYSTEM == 2)
  {
    
  }
  
}

void hang_up()
{
  if(SYSTEM == 1)
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press('h');
    Keyboard.releaseAll();
  }else if(SYSTEM == 2)
  {
    
  }
   
}
void video()
{
  if(SYSTEM == 1)
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press('o');
    Keyboard.releaseAll();     
  }else if(SYSTEM == 2)
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('e');
    Keyboard.releaseAll();   
  }

}
void speakers_mute() 
{
  ConsumerControl.press(CONSUMER_CONTROL_MUTE); // Speakers off
  ConsumerControl.release();
  if(speakerMute)
  {
    speakerMute= false;
    print_img(SD, "/horizon.bmp", 320, 480, 71,0,71,380,250,0);
  }else
  {
    speakerMute= true;
    print_img(SD, "/horizon_active.bmp", 320, 480, 71,0,71,380,250,0);
  }
}

void turn_off()
{
  lcd.setBrightness(0);
  lcd_off = true;
}

void keep_wake()
{
  if(Mouse_giggler)
  {
    Mouse_giggler = false;
    print_img(SD, "/horizon.bmp", 320, 480, 0,0,0,410,70,0);
    //lcd.fillRect(0, 0, 2, 80, TFT_RED);
  }else
  {
    Mouse_giggler = true;
    print_img(SD, "/horizon_active.bmp", 320, 480, 0,0,0,410,70,0);
    //lcd.fillRect(0, 0, 2, 80, TFT_GREEN);
  }
}

/*
int print_img(fs::FS &fs, String filename, int x, int y)
{

    File f = fs.open(filename, "r");
    if (!f)
    {
        Serial.println("Failed to open file for reading");
        f.close();
        return 0;
    }

    f.seek(54);
    int X = x;
    int Y = y;
    uint8_t RGB[3 * X];
    for (int row = 0; row < Y; row++)
    {
        f.seek(54 + 3 * X * row);
        f.read(RGB, 3 * X);

        lcd.pushImage(0, row, X, 1, (lgfx::rgb888_t *)RGB);
    }

    f.close();
    return 0;
}
*/

// Display image from file vertical flip
int print_img(fs::FS &fs, String filename, int x, int y,int pos_x, int pos_y, int start_x_px, int start_y_px, int end_x_px, int end_y_px)
{

    File f = fs.open(filename, "r");
    if (!f)
    {
        Serial.println("Failed to open file for reading");
        f.close();
        return 0;
    }

    f.seek(54);
    //
    int endLine;
    int lineWidth;
    if(end_y_px != 0){
      endLine = end_y_px;
    }else {
      endLine = y;
    }
    int startLine = 0;
    if(start_y_px != 0){
      startLine = start_y_px;
    }

    int lineWidth_start=0;
    if(start_x_px !=0)
    {
      lineWidth_start = start_x_px;
    }
    int lineWidth_end=x;
    if(end_x_px != 0)
    {
      lineWidth_end = end_x_px;
    }
    lineWidth = lineWidth_end - lineWidth_start;
    
    int Y = y;
    int X = x;
    
    uint8_t RGB[3 * X];
    for (int row = 0; row < (endLine - startLine); row++)
    {
        f.seek((54 + 3 * X * (endLine-row-1))+(3*lineWidth_start));
        f.read(RGB, 3 * lineWidth);
        lcd.pushImage(pos_x, pos_y+row-1, lineWidth, 1, (lgfx::rgb888_t *)RGB);
    }

    f.close();
    return 0;
}
