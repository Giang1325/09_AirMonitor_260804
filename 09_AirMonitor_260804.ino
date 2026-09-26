#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Wire.h>
#include <Adafruit_BME680.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
#include "PMS.h"
#include <time.h>

#define WLAN_SSID       "YOUR_WIFI_SSID"
#define WLAN_PASS       "YOUR_WIFI_PASSWORD"
#define IO_USERNAME     "YOUR_ADAFRUIT_IO_USERNAME"
#define IO_KEY          "YOUR_ADAFRUIT_IO_KEY"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish temp_f =
Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish hum_f =
Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish pressure_f =
Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/pressure");
Adafruit_MQTT_Publish pm1_f =
Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/pm10");
Adafruit_MQTT_Publish pm25_f =
Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/pm25");
Adafruit_MQTT_Publish pm10_f =
Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/pm100");

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; //chuan thoi gian
const int daylightOffset_sec = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BME680 bme;
RTC_DS3231 rtc;

PMS pms(Serial2);
PMS::DATA data;

unsigned long prevMqttMillis = 0;
unsigned long prevDisplayMillis = 0;
const long mqttInterval = 15000;
const long displayInterval = 3000;
int displayPage = 0;
int currentPM25 = 0;
int currentPM10 = 0;
int currentPM1 = 0;

void MQTT_connect();

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n===== DANG KHOI DONG =====");
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    Wire.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("Kiem tra lai man hinh OLED!");
        while (1);
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0,0);
    display.println("He thong dang chay ... ");
    display.display();

    Serial.print("Dang ket noi WiFi");
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Da ket noi WIFI!");
    if (!bme.begin())
    {
        Serial.println("Kiem tra lai cam bien BME680!");
        while (1);
    }
    if (!rtc.begin())
    {
        Serial.println("Kiem tra lai RTC!");
        while (1);
    }
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    Serial.print("Dang dong bo thoi gian");
    while (!getLocalTime(&timeinfo))
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println(" OK");
    rtc.adjust(DateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
    ));
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    display.clearDisplay();
    display.display();
}

void loop()
{
    MQTT_connect();
    unsigned long currentMillis = millis();
    if (pms.read(data))
    {
        currentPM1  = data.PM_AE_UG_1_0;
        currentPM25 = data.PM_AE_UG_2_5;
        currentPM10 = data.PM_AE_UG_10_0;
    }
    if (currentMillis - prevDisplayMillis >= displayInterval)
    {
        prevDisplayMillis = currentMillis;
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        DateTime now = rtc.now();
        if(displayPage==0)
        {
            if(bme.performReading())
            {
                display.setCursor(0,0);
                display.println("AIR QUALITY");
                display.setCursor(0,12);

                if(now.hour()<10) display.print("0");
                display.print(now.hour());
                display.print(":");
                if(now.minute()<10) display.print("0");
                display.print(now.minute());
                display.print(" ");
                if(now.day()<10) display.print("0");
                display.print(now.day());
                display.print("/");
                if(now.month()<10) display.print("0");
                display.print(now.month());
                display.print("/");
                display.print(now.year());

                display.setCursor(0,24);
                display.print("T : ");
                display.print(bme.temperature,1);
                display.println(" C");

                display.setCursor(0,36);
                display.print("H : ");
                display.print(bme.humidity,0);
                display.println(" %");

                display.setCursor(0,48);
                display.print("P : ");
                display.print(bme.pressure / 100.0, 1);
                display.print(" hPa");

            displayPage=1;
            }
        }
        else
        {
            display.setCursor(0,0);
            display.println("AIR QUALITY");
            display.setCursor(0,12);

            if(now.hour()<10) display.print("0");
            display.print(now.hour());
            display.print(":");
            if(now.minute()<10) display.print("0");
            display.print(now.minute());
            display.print(" ");
            if(now.day()<10) display.print("0");
            display.print(now.day());
            display.print("/");
            if(now.month()<10) display.print("0");
            display.print(now.month());
            display.print("/");
            display.print(now.year());

            display.setCursor(0,24);
            display.print("PM1.0 : ");
            display.print(currentPM1);
            display.println(" ug/m3");

            display.setCursor(0,36);
            display.print("PM2.5 : ");
            display.print(currentPM25);
            display.println(" ug/m3");

            display.setCursor(0,48);
            display.print("PM10  : ");
            display.print(currentPM10);
            display.println(" ug/m3");

            displayPage=0;
        }

        display.display();
    }
    if(currentMillis-prevMqttMillis>=mqttInterval)
    {
        prevMqttMillis=currentMillis;
        if(bme.performReading())
        {
            String temp_str = String(bme.temperature,1);
            String hum_str  = String(bme.humidity,0);
            String pressure_str = String(bme.pressure / 100.0, 1);

            String pm1_str  = String(currentPM1);
            String pm25_str = String(currentPM25);
            String pm10_str = String(currentPM10);

            temp_f.publish(temp_str.c_str());
            hum_f.publish(hum_str.c_str());
            pressure_f.publish(pressure_str.c_str());
            pm1_f.publish(pm1_str.c_str());
            pm25_f.publish(pm25_str.c_str());
            pm10_f.publish(pm10_str.c_str());

            Serial.println("Da gui du lieu len Adafruit IO");
        }
    }
}
void MQTT_connect()
{
    if(mqtt.connected())
        return;

    Serial.print("Dang ket noi MQTT ... ");
    while(mqtt.connect()!=0)
    {
        mqtt.disconnect();
        delay(5000);
        Serial.print(".");
    }
    Serial.println("MQTT Connected!");
}