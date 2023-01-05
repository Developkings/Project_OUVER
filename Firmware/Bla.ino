



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

/*
 Typewriting Demo on LCD 1602
 
 
 Created 17 April 2017
 @Gorontalo, Indonesia
 by ZulNs
 */

#include <MultitapKeypad.h>
 
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "esp32-hal-cpu.h"
//const uint16_t BACKLIGHT_PERIODS = 10000;
//const uint16_t MODE_FLASHING_PERIODS = 1500;

const byte ROW0 = 26;
const byte ROW1 = 25;
const byte ROW2 = 33;
const byte ROW3 = 32;
const byte COL0 = 13;
const byte COL1 = 12;
const byte COL2 = 14;
const byte COL3 = 27;

/*const byte LCD_RS = A4;
const byte LCD_EN = A5;
const byte LCD_D4 = A0;
const byte LCD_D5 = A1;
const byte LCD_D6 = A2;
const byte LCD_D7 = A3;
const byte BACKLIGHT = 3;
const byte BEEP = 2;

const byte UPPER_A = 0;
const byte UPPER_B = 1;
const byte LOWER_A = 2;
const byte LOWER_B = 3;
const byte NUMBER_1 = 4;
const byte NUMBER_2 = 5;
const byte CR = 6;*/

const byte CHR_BOUND = 3;
const byte BACKSPACE = 8;
const byte CLEARSCREEN = 12;
const byte CARRIAGE_RETURN = 13;
const uint8_t CAPSLOCK_ON = 17;
const uint8_t CAPSLOCK_OFF = 18;
const uint8_t NUMLOCK_ON = 19;
const uint8_t NUMLOCK_OFF = 20;
const byte SP1 = 24;
const byte SP2 = 25;
const byte SP3 = 26;
const byte SP4 = 27;

const char SYMBOL[] PROGMEM = {
  ',', '.', ';', ':', '!',
  '?', '\'', '"','-', '+',
  '*', '/', '=', '%', '^',
  '(', ')', '#', '@', '$',
  '[', ']', '{', '}', CHR_BOUND
};
const char ALPHABET[] PROGMEM = {
  'A', 'B', 'C', CHR_BOUND, CHR_BOUND,
  'D', 'E', 'F', CHR_BOUND, CHR_BOUND,
  'G', 'H', 'I', CHR_BOUND, CHR_BOUND,
  'J', 'K', 'L', CHR_BOUND, CHR_BOUND,
  'M', 'N', 'O', CHR_BOUND, CHR_BOUND,
  'P', 'Q', 'R', 'S',       CHR_BOUND,
  'T', 'U', 'V', CHR_BOUND, CHR_BOUND,
  'W', 'X', 'Y', 'Z',       CHR_BOUND
};
/*byte letter_A[8] = {
  0b11111,
  0b11001,
  0b10110,
  0b10110,
  0b10000,
  0b10110,
  0b10110,
  0b11111
};
byte letter_B[8] = {
  0b11111,
  0b10001,
  0b10110,
  0b10001,
  0b10110,
  0b10110,
  0b10001,
  0b11111
};
byte letter_a[8] = {
  0b11111,
  0b11111,
  0b11000,
  0b11110,
  0b11000,
  0b10110,
  0b11000,
  0b11111
};
byte letter_b[8] = {
  0b11111,
  0b10111,
  0b10111,
  0b10001,
  0b10110,
  0b10110,
  0b10001,
  0b11111
};
byte number_1[8] = {
  0b11111,
  0b11011,
  0b10011,
  0b11011,
  0b11011,
  0b11011,
  0b10001,
  0b11111
};
byte number_2[8] = {
  0b11111,
  0b11001,
  0b10110,
  0b11110,
  0b11101,
  0b11011,
  0b10000,
  0b11111
};
byte carriage_return[8] = {
  0b00000,
  0b00000,
  0b00101,
  0b01001,
  0b11111,
  0b01000,
  0b00100,
  0b00000
};
*/
char modeBuf[] = "  ";
byte cursorPos = 0;
boolean alphaMode = true;
boolean upperCaseMode = true;
boolean isEndOfDisplay = false;
boolean autoOffBacklight = false;
boolean isModeFlashing = false;


// creates lcd as LiquidCrystal object

// creates kpd as MultitapKeypad object
// for matrix 4 x 3 keypad
// MultitapKeypad kpd( ROW0, ROW1, ROW2, ROW3, COL0, COL1, COL2 );
// for matrix 4 x 4 keypad
MultitapKeypad kpd( ROW0, ROW1, ROW2, ROW3, COL0, COL1, COL2, COL3 );
// creates key as Key object
Key key;


BLEServer* pServer = NULL;
BLECharacteristic* cCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "6E400005-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_1 "6E400006-B5A3-F393-E0A9-E50E24DCCA9E"


class MyServerCallbacks: public BLEServerCallbacks {
  
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    
       cCharacteristic->setValue("HI THERE");
        cCharacteristic->notify();
         
Serial.println("Connected");
Serial.println("Sending notify...");
};

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
   delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("Disconnected");
        oldDeviceConnected = deviceConnected;
}        
};











void setup() {
  Serial.begin(115200);  
 
setCpuFrequencyMhz(160);

   // Create the BLE Device
  BLEDevice::init("Bla");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());


  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  cCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_1,
                      BLECharacteristic::PROPERTY_NOTIFY
                                                              );
  // Create a BLE Characteristic
               

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
 cCharacteristic->addDescriptor(new BLE2902());


  
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}


////////////////////////////////////////////// LOOP //////////////////////////////////////////////////////////



void loop() {
  char chr;
  key = kpd.getKey();
  switch ( key.state ) {
  case KEY_DOWN:
  case MULTI_TAP:
    switch ( key.code ) {
    case KEY_ASTERISK:
      chr = BACKSPACE;
      break;
    case KEY_NUMBER_SIGN:
      if ( alphaMode ) {
        upperCaseMode = !upperCaseMode;
        chr = upperCaseMode ? CAPSLOCK_ON : CAPSLOCK_OFF;
      }
     
      break;
    case KEY_0:
      chr = alphaMode ? ' ' : '0';
      break;
    case KEY_1:
      chr = alphaMode ? getSymbol( key.tapCounter ) : '1';
      break;
    case KEY_A:
    chr =  SP1; 
     break;
    case KEY_B:
     chr =  SP2; 
      break;
    case KEY_C:
     chr =  SP3; 
      break;
    case KEY_D:
     chr =  SP4; 
      break;
    default:
      chr = alphaMode ? getAlphabet( key.character, key.tapCounter ) : key.character;
    }
    if ( !upperCaseMode && chr >= 'A' )
      chr += 32; // makes lower case
    if ( key.state == MULTI_TAP && alphaMode && key.character >= '1' && key.character <= '9' )
      printToLcd( BACKSPACE );
    printToLcd( chr );
    break;
  case LONG_TAP:
    switch ( key.code ) {
    case KEY_ASTERISK:
      chr = CLEARSCREEN;
      break;
    case KEY_NUMBER_SIGN:
        chr = CARRIAGE_RETURN;
      break;
    default:
      chr = key.character;
    }
    if ( chr < ' ' || alphaMode && chr >= '0' && chr <= '9' ) {
      if ( chr >= '0' || chr == NUMLOCK_OFF )
        printToLcd( BACKSPACE );
      printToLcd( chr );
    }
    break;
  case MULTI_KEY_DOWN:
    break;
  case KEY_UP:
String adf = "a"; 
 }
 }

void printToLcd( char chr ) {
  switch ( chr ) {
    case BACKSPACE:
 
      Serial.println("BACKSPACE");
cCharacteristic->setValue("BACKSPACE");
        cCharacteristic->notify();  
  
      break;
    case CLEARSCREEN:
   Serial.println("CLEARSCREEN");
cCharacteristic->setValue("CLEARSCREEN");
        cCharacteristic->notify();     


      break;
    case CARRIAGE_RETURN:
      Serial.println("ENTER");
cCharacteristic->setValue("ENTER");
        cCharacteristic->notify();  

        
      break;


          case SP1:
      Serial.println("SP1");
cCharacteristic->setValue("SP1");
        cCharacteristic->notify();  

        
      break;


                case SP2:
      Serial.println("SP2");
cCharacteristic->setValue("SP2");
        cCharacteristic->notify();  

        
      break;


                case SP3:
      Serial.println("SP3");
cCharacteristic->setValue("SP3");
        cCharacteristic->notify();  

        
      break;


                      case SP4:
      Serial.println("SP4");
cCharacteristic->setValue("SP4");
        cCharacteristic->notify();  

        
      break;

    case CAPSLOCK_ON:
    case CAPSLOCK_OFF:
    case NUMLOCK_ON:
    case NUMLOCK_OFF:
      if ( chr == NUMLOCK_ON ) {
  Serial.println("123");
cCharacteristic->setValue("123");
        cCharacteristic->notify();    
      }
      else {
        if ( upperCaseMode ) {
  Serial.println("ABC");
cCharacteristic->setValue("ABC");
        cCharacteristic->notify();  

          
        }
        else {
  Serial.println("abc");
cCharacteristic->setValue("abc");
        cCharacteristic->notify();  
        
                }
      }
      break;
    default:

  Serial.println(chr);

  String myString = String(chr);  
cCharacteristic->setValue(myString.c_str());
        cCharacteristic->notify();  

        

  }
}

byte getSymbol( byte ctr ) {
  byte sym = pgm_read_byte_near( SYMBOL + ctr );
  if ( sym == CHR_BOUND ) {
    sym = pgm_read_byte_near( SYMBOL );
    kpd.resetTapCounter();
  }
  return sym;
}

byte getAlphabet( byte chr, byte ctr ) {
  chr = ( chr - '2' ) * 5;
  byte alpha = pgm_read_byte_near( ALPHABET + chr + ctr );
  if ( alpha == CHR_BOUND ) {
    alpha = pgm_read_byte_near( ALPHABET + chr );
    kpd.resetTapCounter();
  }
  return alpha;
}

