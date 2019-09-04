/**
   Supla.org NodeMCU WiFi minimal example
   Author: Programistyk - Kamil Kaminski <kamil@programistyk.pl>
   Modifications: sp9svh@kpgk.pl

   This example shows how to configure SuplaDevice for building for NodeMCU within Arduino IDE
   Działa ale do przerobienia
*/


#include <srpc.h>
#include <log.h>
#include <eh.h>
#include <proto.h>
#include <IEEE754tools.h>
// We define our own ethernet layer
#define SUPLADEVICE_CPP
#include <SuplaDevice.h>
#include <lck.h>

#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiServer.h>
#include <ESP8266WiFiGeneric.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiUdp.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme; //I2C
ADC_MODE(ADC_VCC);

#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET); // GPIO5 - SCL , GPIO4 -SDA

#define SEALEVELPRESSURE_HPA (1023.0)

// Adafruit_BME280 bme; // I2C

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO32_GLCD_HEIGHT 32
#define LOGO32_GLCD_WIDTH  32
static const unsigned char PROGMEM temp_glcd_bmp[] =
{ // temp_home
  0x00, 0x03, 0xc0, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x08, 0x50, 0x00, 
  0x00, 0x08, 0x50, 0x00, 0x00, 0x08, 0x50, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x09, 0x50, 0x00, 
  0x00, 0x08, 0xd0, 0x00, 0x00, 0x0b, 0x10, 0x00, 0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 
  0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 
  0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 
  0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 0x00, 0x0b, 0xd0, 0x00, 0x00, 0x17, 0xe8, 0x00, 
  0x00, 0x2f, 0xf4, 0x00, 0x00, 0x2f, 0x94, 0x00, 0x00, 0x2f, 0xb4, 0x00, 0x00, 0x2f, 0xf4, 0x00, 
  0x00, 0x07, 0xe0, 0x00, 0x00, 0x13, 0xc8, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x03, 0xc0, 0x00
};



static const unsigned char PROGMEM pressure_glcd_bmp[] =
{
  // 'pressure'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 
  0x01, 0xc1, 0xc1, 0xf8, 0x03, 0xc3, 0xc1, 0xf8, 0x03, 0x83, 0x81, 0xf8, 0x03, 0x03, 0x00, 0x18, 
  0x07, 0x07, 0x00, 0x18, 0x07, 0x07, 0x00, 0xf8, 0x07, 0x07, 0x00, 0xf8, 0x07, 0x07, 0x00, 0xf8, 
  0x07, 0x07, 0x00, 0x18, 0x03, 0x03, 0x00, 0x18, 0x03, 0x83, 0x80, 0x18, 0x03, 0x83, 0x81, 0xf8, 
  0x03, 0x83, 0x81, 0xf8, 0x03, 0x83, 0x80, 0x18, 0x01, 0x81, 0x80, 0x18, 0x01, 0x81, 0x80, 0x18, 
  0x03, 0x83, 0x80, 0xf8, 0x03, 0x83, 0x80, 0xf8, 0x03, 0x83, 0x80, 0xf8, 0x13, 0x93, 0x80, 0x18, 
  0x1f, 0x1f, 0x00, 0x18, 0x1f, 0x1f, 0x01, 0xf8, 0x1e, 0x1e, 0x01, 0xf8, 0x1f, 0x1f, 0x01, 0xf8, 
  0x1f, 0x9f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char PROGMEM humidity_glcd_bmp[] =
{

  0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 
  0x00, 0xfc, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x01, 0xce, 0x00, 0x00, 0x03, 0xcf, 0x00, 0x00, 
  0x03, 0x87, 0x00, 0x00, 0x07, 0x87, 0x80, 0x00, 0x07, 0x03, 0x80, 0x00, 0x0e, 0x01, 0xc0, 0x00, 
  0x1e, 0x01, 0xe0, 0x00, 0x1c, 0x00, 0xe0, 0x00, 0x38, 0x00, 0x70, 0x00, 0x78, 0x00, 0x78, 0x00, 
  0x70, 0x80, 0x38, 0x00, 0x61, 0x48, 0x18, 0x00, 0xe2, 0x48, 0x1c, 0x00, 0xe3, 0x50, 0x1c, 0x00, 
  0xe1, 0xa0, 0x1c, 0x00, 0xe0, 0x2a, 0x1c, 0x00, 0xe0, 0x52, 0x1c, 0x00, 0xe0, 0x92, 0x1c, 0x00, 
  0x60, 0x8c, 0x18, 0x00, 0x70, 0x00, 0x38, 0x00, 0x38, 0x00, 0x70, 0x00, 0x3c, 0x00, 0xf0, 0x00, 
  0x1e, 0x01, 0xe0, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0xfc, 0x00, 0x00

  
};
static const unsigned char PROGMEM logo32_glcd_bmp[] =
{
  // 'logo SUPLA'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x00,
  0x0e, 0x0e, 0x00, 0x00, 0x0c, 0x06, 0x00, 0x00, 0x1c, 0x03, 0x00, 0x00, 0x1c, 0x03, 0x00, 0x00,
  0x1c, 0x03, 0x00, 0x00, 0x0c, 0x07, 0x00, 0x00, 0x0e, 0x0f, 0x80, 0x00, 0x07, 0xfc, 0xe0, 0x00,
  0x03, 0xf8, 0x30, 0x00, 0x00, 0xf0, 0x0d, 0xe0, 0x00, 0x10, 0x07, 0x30, 0x00, 0x18, 0x02, 0x10,
  0x00, 0x18, 0x06, 0x18, 0x00, 0x08, 0x02, 0x10, 0x00, 0x08, 0x03, 0xf0, 0x00, 0x0c, 0x07, 0xc0,
  0x00, 0x04, 0x0c, 0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00,
  0x00, 0x03, 0xc0, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x0c, 0x20, 0x00, 0x00, 0x0c, 0x20, 0x00,
  0x00, 0x04, 0x40, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char PROGMEM ASL_glcd_bmp[] =
{
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 
  0x00, 0x00, 0x60, 0x00, 0x00, 0x08, 0x21, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x62, 0x00, 
  0x00, 0x1f, 0xf8, 0x00, 0x00, 0x3f, 0x0c, 0x00, 0x00, 0x61, 0x84, 0x00, 0x00, 0xc0, 0xc4, 0xe0, 
  0x01, 0xc0, 0xf4, 0x00, 0x03, 0x00, 0x1c, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x06, 0x00, 0x0c, 0x00, 
  0x06, 0x10, 0xcd, 0x00, 0x03, 0x10, 0x99, 0x00, 0x01, 0x84, 0x30, 0x00, 0x00, 0x25, 0x00, 0x00, 
  0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
  };

char* Message[] = {"This is string 1",
                   "Already initialized",
                   "Cb not assigned",
                   "Invalid GUID",
                   "Unknown server address",
                   "Unknow location ID",
                   "Initialized",
                   "Channel limit exceeded",
                   "Disconnected",
                   "Register in progress",
                   "Iterate fail",
                   "Protocol version error",
                   "Bad credentials",
                   "Temporarily unawaliable",
                   "Location conflict",
                   "Channel conflict",
                   "Registered and ready",
                   "Device is diasbled",
                   "Location is disabled",
                   "Device limit execeeded"
                  };



#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
WiFiClient client;

// Setup Supla connection
const char* ssid     = "*****";
const char* password = "*****";

char str[10];
char StatCommStr[25];
int StatCommInt;
byte Icon;
byte FiveSek;





// BME280 Sensor read implementation
void 
get_temperature_and_humidity (int channelNumber, double *temp, double *humidity){
  
    *temp = bme.readTemperature();
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    *humidity = bme.readHumidity();
    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    if ( isnan(*temp) || isnan(*humidity) ) {
      *temp = -275;
      *humidity = -1;
    }

  
  }
double get_distance(int channelNumber, double distance) {
   
 distance = bme.readPressure() / 100.0F;
   return  distance; 
}

void timer0_ISR (void) {
  FiveSek++;
  if (FiveSek==7) {
    FiveSek=0;
    Icon++;
    if (Icon==7) Icon=1;
  }
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec 
}


void setup() {
  // Init Serial port
  Serial.begin(115200);

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();
 
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
    }
SuplaDevice.addDS18B20Thermometer(); //kPa
SuplaDevice.addDHT22();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
 
  Icon = 0;

  // Clear the buffer.
  display.clearDisplay();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setRotation(2);
  display.setCursor(50, 25);
  display.print("SUPLA");
  drawbitmap(logo32_glcd_bmp, LOGO32_GLCD_HEIGHT, LOGO32_GLCD_WIDTH);
  display.display();





  // ﻿Replace the falowing GUID
char GUID[SUPLA_GUID_SIZE] = {0x60,0xC9,0x23,0x0A,0x61,0x24,0x06,0x24,0x62,0x9f,0x94,0x75,0xFF,0x0F,0x5E,0x0A};

  // ﻿with GUID that you can retrieve from https://www.supla.org/arduino/get-guid

  // Ethernet MAC address
  uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  /*
     Having your device already registered at cloud.supla.org,
     you want to change CHANNEL sequence or remove any of them,
     then you must also remove the device itself from cloud.supla.org.
     Otherwise you will get "Channel conflict!" error.
  */


  // CHANNEL0 - Temp 1
    // SuplaDevice.addBME280();  // to jest kopia SuplaDevice.addDHT22() pod inną nazwą
   
    SuplaDevice.addDHT22(); // ten objekt jest tylko po to aby w aplikacji wyświetlić temp i wilgotność 
    SuplaDevice.addDistanceSensor(); // ten obiekt wyświetla nam ciśnienie atm. niestety bez jednostek (hPA)
  

  
  SuplaDevice.setStatusFuncImpl(&status_func);

  SuplaDevice.begin(GUID,              // Global Unique Identifier
                    mac,               // Ethernet MAC address
                    "*****.supla.org",  // SUPLA server address
                    ****,                 // Location ID
                    "****");               // Location Password

}


// ***********************************************************************************************************
void loop() {
  SuplaDevice.iterate();
  DisplayTemp();

}

// ***********************************************************************************************************

void DisplayTemp() {

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setRotation(2);
  display.setCursor(45, 25);  
 
  switch (Icon) {
      case 1:
         display.setTextSize(3);
          display.setCursor(45, 25);  
        display.print(bme.readTemperature(), 0);
        display.print((char)247);
        display.println("C");
        drawbitmap(temp_glcd_bmp, LOGO32_GLCD_HEIGHT, LOGO32_GLCD_WIDTH);
        
        break;
      case 2:
       display.setTextSize(3);
        display.setCursor(45, 25);  
        display.print(bme.readHumidity(), 0);
        display.println("%");

        drawbitmap(humidity_glcd_bmp, LOGO32_GLCD_HEIGHT, LOGO32_GLCD_WIDTH);
        break;
      case 3:
         display.setTextSize(2);
          display.setCursor(40, 25);  
        display.print(bme.readPressure() / 100.0F, 0);
        display.println(" hPa");
        drawbitmap(pressure_glcd_bmp, LOGO32_GLCD_HEIGHT, LOGO32_GLCD_WIDTH);
        break;
      case 4:
        display.setTextSize(2);
         display.setCursor(25, 25);  
        //display.print(bme.readAltitude(SEALEVELPRESSURE_HPA), 0);
        //display.println("m npm");
        //display.print(WiFi.RSSI());
        display.print(ESP.getVcc()/1000.0);
        //display.println(" dbm");
        display.println(" V");
        drawbitmap(ASL_glcd_bmp, LOGO32_GLCD_HEIGHT, LOGO32_GLCD_WIDTH);
        break;
       case 5:
        display.setTextSize(2);
         display.setCursor(25, 25);  
        display.print(WiFi.RSSI());
        //display.print(ESP.getVcc()/1000.0);
        display.println(" dbm");
        drawbitmap(ASL_glcd_bmp, LOGO32_GLCD_HEIGHT, LOGO32_GLCD_WIDTH);
        break;
         case 6:
        display.setTextSize(1);
         display.setCursor(25, 25);  
        display.print(WiFi.localIP());
        display.println("          IP address");
        drawbitmap(ASL_glcd_bmp, LOGO32_GLCD_HEIGHT, LOGO32_GLCD_WIDTH);
        break;
  };
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 55);
  display.print(Message[StatCommInt - 1]);
  display.display();
  display.display();

}; //DisplayTemp

void drawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];

  display.drawBitmap(0, 0, bitmap, w, h, WHITE);
} //drawbitmap


// Supla.org ethernet layer
int supla_arduino_tcp_read(void *buf, int count) {
  _supla_int_t size = client.available();

  if ( size > 0 ) {
    if ( size > count ) size = count;
    return client.read((uint8_t *)buf, size);
  };

  return -1;
};

int supla_arduino_tcp_write(void *buf, int count) {
  return client.write((const uint8_t *)buf, count);
};

bool supla_arduino_svr_connect(const char *server, int port) {
  return client.connect(server, 2015);
}

bool supla_arduino_svr_connected(void) {
  return client.connected();
}

void supla_arduino_svr_disconnect(void) {
  client.stop();
}

void supla_arduino_eth_setup(uint8_t mac[6], IPAddress *ip) {

  // Serial.println("WiFi init");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //    Serial.print(".");
  }

  //Serial.print("\nlocalIP: ");
  //Serial.println(WiFi.localIP());
  //Serial.print("subnetMask: ");
  //Serial.println(WiFi.subnetMask());
  //Serial.print("gatewayIP: ");
  //Serial.println(WiFi.gatewayIP());
}

SuplaDeviceCallbacks supla_arduino_get_callbacks(void) {
  SuplaDeviceCallbacks cb;

  cb.tcp_read = &supla_arduino_tcp_read;
  cb.tcp_write = &supla_arduino_tcp_write;
  cb.eth_setup = &supla_arduino_eth_setup;
  cb.svr_connected = &supla_arduino_svr_connected;
  cb.svr_connect = &supla_arduino_svr_connect;
  cb.svr_disconnect = &supla_arduino_svr_disconnect;
  cb.get_temperature_and_humidity = get_temperature_and_humidity;
  cb.get_temperature = NULL;
  cb.get_rgbw_value = NULL;
  cb.set_rgbw_value = NULL;
  cb.get_distance= get_distance;

  return cb;
}



void status_func(int status, const char *msg) {
  Serial.print("Status : ");
  Serial.print(status);
  StatCommInt = status;
  Serial.print(" - "); 
  Serial.println(Message[StatCommInt - 1]);
  display.fillRect(0, 55, 128, 65, BLACK);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(2);
  display.setCursor(0, 55);
  display.print(Message[StatCommInt - 1]);
  display.display();

}
