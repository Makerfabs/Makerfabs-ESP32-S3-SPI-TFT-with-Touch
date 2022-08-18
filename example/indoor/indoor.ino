//测试通过
#define LGFX_USE_V1

#include <LovyanGFX.hpp>
#include <vector>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "Adafruit_SGP30.h"
#include "DHT.h"

#include "FT6236.h"
const int i2c_touch_addr = TOUCH_I2C_ADD;
#define get_pos ft6236_pos

#define LCD_MOSI 13
#define LCD_MISO 12 // 12
#define LCD_SCK 14
#define LCD_CS 15
#define LCD_RST -1 // 26
#define LCD_DC 21  // 33
#define LCD_BL -1

#define I2C_SDA 38
#define I2C_SCL 39

#define SD_MOSI 2
#define SD_MISO 41
#define SD_SCK 42
#define SD_CS 1

#define DHTPIN 5 // Digital pin connected to the DHT sensor
#define BUZZPIN 7
#define LDO_PWR_EN_PIN 4 // AP2112K enable pin

#define BUZZER_ON digitalWrite(BUZZPIN, HIGH)
#define BUZZER_OFF digitalWrite(BUZZPIN, LOW)

#define LDO_1V8_ON digitalWrite(LDO_PWR_EN_PIN, HIGH)
#define LDO_1V8_OFF digitalWrite(LDO_PWR_EN_PIN, LOW)

#define DHTTYPE DHT11 // DHT11

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SGP30 sgp;

class LGFX : public lgfx::LGFX_Device
{
    // lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    // コンストラクタを作成し、ここで各種設定を行います。
    // クラス名を変更した場合はコンストラクタも同じ名前を指定してください。
    LGFX(void)
    {
        {                                      // バス制御の設定を行います。
            auto cfg = _bus_instance.config(); // バス設定用の構造体を取得します。

            // SPIバスの設定
            cfg.spi_host = SPI3_HOST;  // 使用するSPIを選択  (VSPI_HOST or HSPI_HOST)
            cfg.spi_mode = 0;          // SPI通信モードを設定 (0 ~ 3)
            cfg.freq_write = 40000000; // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
            cfg.freq_read = 16000000;  // 受信時のSPIクロック
            cfg.spi_3wire = true;      // 受信をMOSIピンで行う場合はtrueを設定
            cfg.use_lock = true;       // トランザクションロックを使用する場合はtrueを設定
            cfg.dma_channel = 1;       // Set the DMA channel (1 or 2. 0=disable)   使用するDMAチャンネルを設定 (0=DMA不使用)
            cfg.pin_sclk = LCD_SCK;    // SPIのSCLKピン番号を設定
            cfg.pin_mosi = LCD_MOSI;   // SPIのMOSIピン番号を設定
            cfg.pin_miso = LCD_MISO;   // SPIのMISOピン番号を設定 (-1 = disable)
            cfg.pin_dc = LCD_DC;       // SPIのD/Cピン番号を設定  (-1 = disable)
                                       // SDカードと共通のSPIバスを使う場合、MISOは省略せず必ず設定してください。

            _bus_instance.config(cfg);              // 設定値をバスに反映します。
            _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
        }

        {                                        // 表示パネル制御の設定を行います。
            auto cfg = _panel_instance.config(); // 表示パネル設定用の構造体を取得します。

            cfg.pin_cs = LCD_CS;   // CSが接続されているピン番号   (-1 = disable)
            cfg.pin_rst = LCD_RST; // RSTが接続されているピン番号  (-1 = disable)
            cfg.pin_busy = -1;     // BUSYが接続されているピン番号 (-1 = disable)

            // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

            cfg.memory_width = 320;   // ドライバICがサポートしている最大の幅
            cfg.memory_height = 480;  // ドライバICがサポートしている最大の高さ
            cfg.panel_width = 320;    // 実際に表示可能な幅
            cfg.panel_height = 480;   // 実際に表示可能な高さ
            cfg.offset_x = 0;         // パネルのX方向オフセット量
            cfg.offset_y = 0;         // パネルのY方向オフセット量
            cfg.offset_rotation = 0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
            cfg.dummy_read_pixel = 8; // ピクセル読出し前のダミーリードのビット数
            cfg.dummy_read_bits = 1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
            cfg.readable = true;      // データ読出しが可能な場合 trueに設定
            cfg.invert = false;       // パネルの明暗が反転してしまう場合 trueに設定
            cfg.rgb_order = false;    // パネルの赤と青が入れ替わってしまう場合 trueに設定
            cfg.dlen_16bit = false;   // データ長を16bit単位で送信するパネルの場合 trueに設定
            cfg.bus_shared = true;    // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance); // 使用するパネルをセットします。
    }
};

LGFX lcd;
SPIClass SD_SPI;

// Global value
int eco2 = 0;
int tvoc = 0;
int counter = 0;
float humidity = 0;
float temperature = 0;

void setup(void)
{
    Serial.begin(115200);

    lcd.init();
    lcd.fillScreen(TFT_WHITE);

    lcd.setTextSize(3);

    sd_init();

    Wire.begin(I2C_SDA, I2C_SCL);
    touch_init();
    sensor_init();

    touch_check();

    lcd.setRotation(1);
    print_img(SD, "/logo.bmp", 480, 320);
    delay(1000);
}

void loop(void)
{
    co2_level_measure();
    display();
    delay(2000);
}

void sd_init()
{
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
}

void touch_init()
{
    byte error, address;

    Wire.beginTransmission(i2c_touch_addr);
    error = Wire.endTransmission();

    if (error == 0)
    {
        Serial.print("I2C device found at address 0x");
        Serial.print(i2c_touch_addr, HEX);
        Serial.println("  !");
    }
    else if (error == 4)
    {
        Serial.print("Unknown error at address 0x");
        Serial.println(i2c_touch_addr, HEX);
        lcd.setCursor(10, 40);
        lcd.println("Touch Failed");
    }
}

void touch_check()
{
    lcd.setRotation(3);
    lcd.fillRect(160, 80, 160, 160, TFT_BLACK);
    lcd.setCursor(170, 100);
    lcd.println(" TOUCH");
    lcd.setCursor(170, 130);
    lcd.println("  TO");
    lcd.setCursor(170, 160);
    lcd.println("CONTINUE");

    int pos[2] = {0, 0};

    while (1)
    {
        get_pos(pos);

        if (pos[0] > 80 && pos[0] < 240 && pos[1] > 160 && pos[1] < 320)
            break;
        delay(100);
    }
}

void sensor_init()
{
    if (!sgp.begin())
    {

        Serial.println("SGP320 not found");
        lcd.setCursor(10, 70);
        lcd.println("SGP320 not found");
    }

    dht.begin();
}

// Display image from file
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

// Display
void display()
{
    String temp;

    lcd.setRotation(3);
    lcd.fillScreen(TFT_WHITE);
    lcd.setTextColor(TFT_BLACK);

    temp = "";
    temp = temp + "humidity " + humidity;
    lcd.setCursor(10, 10);
    lcd.println(temp);

    temp = "";
    temp = temp + "temperature " + temperature;
    lcd.setCursor(10, 40);
    lcd.println(temp);

    temp = "";
    temp = temp + "tvoc " + tvoc;
    lcd.setCursor(10, 70);
    lcd.println(temp);

    temp = "";
    temp = temp + "eco2 " + eco2;
    lcd.setCursor(10, 100);
    lcd.println(temp);
}

// Sensor
void co2_level_measure()
{
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    Serial.print("humidity=");
    Serial.println(humidity);
    Serial.print("temperature=");
    Serial.println(temperature);

    sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

    if (!sgp.IAQmeasure())
    {
        Serial.println("Measurement failed");
        return;
    }
    Serial.print("TVOC ");
    Serial.print(sgp.TVOC);
    Serial.print(" ppb\t");
    Serial.print("eCO2 ");
    Serial.print(sgp.eCO2);
    Serial.println(" ppm");
    eco2 = sgp.eCO2;
    tvoc = sgp.TVOC;

    if (!sgp.IAQmeasureRaw())
    {
        Serial.println("Raw Measurement failed");
        return;
    }
    Serial.print("Raw H2 ");
    Serial.print(sgp.rawH2);
    Serial.print(" \t");
    Serial.print("Raw Ethanol ");
    Serial.print(sgp.rawEthanol);
    Serial.println("");
    counter++;
    delay(500);
    if (counter == 30)
    {
        counter = 0;

        uint16_t TVOC_base, eCO2_base;
        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
            Serial.println("Failed to get baseline readings");
            return;
        }
        Serial.print("****Baseline values: eCO2: 0x");
        Serial.print(eCO2_base, HEX);
        Serial.print(" & TVOC: 0x");
        Serial.println(TVOC_base, HEX);
    }
}

uint32_t getAbsoluteHumidity(float temperature, float humidity)
{
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                // [mg/m^3]
    return absoluteHumidityScaled;
}