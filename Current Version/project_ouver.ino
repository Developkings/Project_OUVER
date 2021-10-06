#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels




// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;


void setup() {
  Serial.begin(9600);


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
u8g2_for_adafruit_gfx.begin(display); 

  display.clearDisplay();   
}

void loop() {


  
String character = "";

    
if(Serial.available()>0) {
    //delay(60);
 character = Serial.readStringUntil('\n');
}
      
if (character != "") {
    Serial.println(character);

    int string_length = character.length();

    if (string_length <= 21){

    
  display.clearDisplay();                               // clear the graphcis buffer  
  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);      // apply Adafruit GFX color
  u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setCursor(0,10);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character);

  display.display();  
}


else if (string_length > 21 && string_length <= 42){

    display.clearDisplay();                               // clear the graphcis buffer  
  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);      // apply Adafruit GFX color
  u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setCursor(0,10);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(0,21));
    u8g2_for_adafruit_gfx.setCursor(0,21);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(21,string_length));

  display.display(); 
}


else if (string_length > 42 && string_length <= 63)
{
  display.clearDisplay(); 
    u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);      // apply Adafruit GFX color
  u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setCursor(0,10);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(0,21));
    u8g2_for_adafruit_gfx.setCursor(0,21);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(21,42));
      u8g2_for_adafruit_gfx.setCursor(0,32);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(42,63));

  display.display(); 
}


else {
  display.clearDisplay(); 
    u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);      // apply Adafruit GFX color
  u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setCursor(0,10);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(0,21));
    u8g2_for_adafruit_gfx.setCursor(0,21);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(21,42));
      u8g2_for_adafruit_gfx.setCursor(0,32);                // start writing at this position
  u8g2_for_adafruit_gfx.print(character.substring(42,60)+ "...");

  display.display(); 
}
}
}
