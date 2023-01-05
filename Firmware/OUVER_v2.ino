/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#include <TFT_eSPI.h>
#include <SPI.h>
#include "U8g2_for_TFT_eSPI.h"


#include "esp32-hal-cpu.h"


#include "Button2.h"

#define BUTTON_1 35
#define BUTTON_2 0

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

int btnCick = false;

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
U8g2_for_TFT_eSPI u8f; 


BLEServer* pServer = NULL;
BLECharacteristic* cCharacteristic = NULL;
BLECharacteristic* sCharacteristic = NULL;
BLECharacteristic* tCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_1 "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_2 "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_3 "6E400004-B5A3-F393-E0A9-E50E24DCCA9E"

// Timer variables
int timerRunning = 0;
unsigned long lastCUpdateTime = 0;
unsigned long lastSUpdateTime = 0;
int timertime = 3000;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_BL, 1);
    lastCUpdateTime = millis();
 // u8f.setFontMode(0);                 // use u8g2 none transparent mode
  //u8f.setFontDirection(0);            // left to right (this is default)
 // u8f.setForegroundColor(TFT_WHITE);  // apply color

 // u8f.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8f.setCursor(0,20);                // start writing at this position
  u8f.print("Connected");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_BL, 1);
    lastCUpdateTime = millis();

 // u8f.setFontMode(0);                 // use u8g2 none transparent mode
 // u8f.setFontDirection(0);            // left to right (this is default)
 // u8f.setForegroundColor(TFT_WHITE);  // apply color

 // u8f.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8f.setCursor(0,20);                // start writing at this position
  u8f.print("Disconnected");
  }
};


class MyCallbacksC : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* cCharacteristic) {
    std::string value = cCharacteristic->getValue();

    if (value.length() > 0) {
      String valor = "";
      for (int i = 0; i < value.length(); i++) {
        valor = valor + value[i];
      }

      Serial.println(valor);  // Presenta valor.
      Serial.println(digitalRead(TFT_BL));
      digitalWrite(TFT_BL, 1);
      tft.fillRect(0, 0, tft.width(), tft.height() / 2, TFT_BLACK);
int string_length = valor.length();
if (string_length <= 26){
u8f.setCursor(0,20);
u8f.print(valor);
}
else if (string_length > 26 && string_length <= 52){
u8f.setCursor(0,20);
u8f.print(valor.substring(0,26));
u8f.setCursor(0,40);
u8f.print(valor.substring(26,string_length));
}
else if (string_length > 52 && string_length <= 78)
{
u8f.setCursor(0,20);
u8f.print(valor.substring(0,26));
u8f.setCursor(0,40);
u8f.print(valor.substring(26,52));
u8f.setCursor(0,60);
u8f.print(valor.substring(52,78));
}
else {
u8f.setCursor(0,20);
u8f.print(valor.substring(0,26));
u8f.setCursor(0,40);
u8f.print(valor.substring(26,52));
u8f.setCursor(0,60);
u8f.print(valor.substring(52,75)+ "...");
}

      lastCUpdateTime = millis();
    }
  }
};

class MyCallbacksS : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* sCharacteristic) {
    std::string value = sCharacteristic->getValue();

    if (value.length() > 0) {
      String valor = "";
      for (int i = 0; i < value.length(); i++) {
        valor = valor + value[i];
      }

      Serial.println(valor);  // Presenta valor.
      Serial.println(digitalRead(TFT_BL));
      digitalWrite(TFT_BL, 1);
      tft.fillRect(0, 70, tft.width(), 64, TFT_BLACK);
     int string_length = valor.length();
if (string_length <= 26){
u8f.setCursor(0,82);
u8f.print(valor);
}
else if (string_length > 26 && string_length <= 52){
u8f.setCursor(0,90);
u8f.print(valor.substring(0,26));
u8f.setCursor(0,110);
u8f.print(valor.substring(26,string_length));
}
else if (string_length > 52 && string_length <= 78)
{
u8f.setCursor(0,90);
u8f.print(valor.substring(0,26));
u8f.setCursor(0,110);
u8f.print(valor.substring(26,52));
u8f.setCursor(0,130);
u8f.print(valor.substring(52,78));
}
else {
u8f.setCursor(0,90);
u8f.print(valor.substring(0,26));
u8f.setCursor(0,110);
u8f.print(valor.substring(26,52));
u8f.setCursor(0,130);
u8f.print(valor.substring(52,75)+ "...");
}
      lastSUpdateTime = millis();
    }
  }
};

class MyCallbacksT : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* tCharacteristic) {
    std::string value = tCharacteristic->getValue();

    if (value.length() > 0) {
      String valor = "";
      for (int i = 0; i < value.length(); i++) {
        valor = valor + value[i];
      }

      timertime = valor.toInt();
    }
  }
};

void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void button_init() {
  btn1.setLongClickHandler([](Button2& b) {
    btnCick = false;
    int r = digitalRead(TFT_BL);
    digitalWrite(TFT_BL, 1);
    tft.fillScreen(TFT_BLACK);
     u8f.setCursor(0,20);                // start writing at this position
  u8f.print("Press again to wake up");
    
    espDelay(6000);
    digitalWrite(TFT_BL, !r);

    tft.writecommand(TFT_DISPOFF);
    tft.writecommand(TFT_SLPIN);
    //After using light sleep, you need to disable timer wake, because here use external IO port to wake up
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
    delay(200);
    esp_deep_sleep_start();
  });
  btn1.setPressedHandler([](Button2& b) {
    Serial.println("btn1 pressed");
    btnCick = true;
  });

  btn2.setPressedHandler([](Button2& b) {
    btnCick = false;
    Serial.println("btn2 press");
     tft.fillRect(0, 0, tft.width(), tft.height() / 2, TFT_BLACK);
           u8f.setCursor(0,20);                // start writing at this position
  u8f.print("I'M ALIVE");
      lastCUpdateTime = millis();
  });
}

void button_loop() {
  btn1.loop();
  btn2.loop();
}

void setup() {
  Serial.begin(115200);
  setCpuFrequencyMhz(160);

  button_init();


  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

   u8f.begin(tft);

 // tft.setTextSize(2);
   u8f.setFontMode(0);                 // use u8g2 none transparent mode
  u8f.setFontDirection(0);            // left to right (this is default)
  u8f.setForegroundColor(TFT_WHITE);  // apply color

  u8f.setFont(u8g2_font_9x18B_mf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8f.setCursor(0,20);                // start writing at this position
  u8f.print("Ouver");

  // Create the BLE Device
  BLEDevice::init("OUVER");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());


  // Create the BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  cCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_1,
    //BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  //|
                                       // BLECharacteristic::PROPERTY_NOTIFY |
                                       // BLECharacteristic::PROPERTY_INDICATE
  );
  // Create a BLE Characteristic
  sCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_2,
    // BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  //  |
                                       // BLECharacteristic::PROPERTY_NOTIFY |
                                       // BLECharacteristic::PROPERTY_INDICATE
  );

  tCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_3,
    // BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  //  |
                                       // BLECharacteristic::PROPERTY_NOTIFY |
                                       // BLECharacteristic::PROPERTY_INDICATE
  );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  cCharacteristic->addDescriptor(new BLE2902());
  sCharacteristic->addDescriptor(new BLE2902());
  tCharacteristic->addDescriptor(new BLE2902());

  cCharacteristic->setCallbacks(new MyCallbacksC());
  sCharacteristic->setCallbacks(new MyCallbacksS());
  tCharacteristic->setCallbacks(new MyCallbacksT());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  if (btnCick) {
    //do something
  }
  button_loop();

  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }


  // Clear the screen if the input characteristic has not been updated in the past 2 seconds
  if (lastCUpdateTime != 0) {
    if (millis() - lastCUpdateTime > timertime) {
      tft.fillRect(0, 0, tft.width(), tft.height() / 2, TFT_BLACK);
      lastCUpdateTime = 0;
      Serial.println("deleting c");
      Serial.println(digitalRead(TFT_BL));
      if ((lastCUpdateTime == 0) && (lastSUpdateTime == 0)) {
        digitalWrite(TFT_BL, 0);
      }
    }
  }
  // Clear the second line of the screen if the text characteristic has not been updated in the past 2 seconds
  if (lastSUpdateTime != 0) {
    if (millis() - lastSUpdateTime > timertime) {
      tft.fillRect(0, tft.height() / 2, tft.width(), tft.height() / 2, TFT_BLACK);
      lastSUpdateTime = 0;
      Serial.println("deleting s");
      Serial.println(digitalRead(TFT_BL));
      if ((lastCUpdateTime == 0) && (lastSUpdateTime == 0)) {
        digitalWrite(TFT_BL, 0);
      }
    }
  }
}