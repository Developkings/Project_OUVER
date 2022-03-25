#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;


void setup() {
Serial.begin(9600);
if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
Serial.println(F("SSD1306 allocation failed"));
for(;;);
}
// Show initial display buffer contents on the screen --
// the library initializes this with an Adafruit splash screen.
u8g2_for_adafruit_gfx.begin(display);

//set side of the display. 0 right 2 left
//display.setRotation(0);

display.clearDisplay();
display.display();


u8g2_for_adafruit_gfx.setFontMode(1);
u8g2_for_adafruit_gfx.setFontDirection(0);
u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);
u8g2_for_adafruit_gfx.setCursor(0,10);
u8g2_for_adafruit_gfx.print("OUVER PROJECT ON!!!");
display.display();
 delay(2000);
 display.clearDisplay();
display.display();
}
void loop() {
String character = "";
while(Serial.available()) {
character = Serial.readStringUntil('\n');
}
if (character != "") {
Serial.println(character);
int string_length = character.length();
if (string_length <= 21){
display.clearDisplay();
u8g2_for_adafruit_gfx.setFontMode(1);
u8g2_for_adafruit_gfx.setFontDirection(0);
u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);
u8g2_for_adafruit_gfx.setCursor(0,10);
u8g2_for_adafruit_gfx.print(character);
display.display();
}
else if (string_length > 21 && string_length <= 42){
display.clearDisplay();
u8g2_for_adafruit_gfx.setFontMode(1);
u8g2_for_adafruit_gfx.setFontDirection(0);
u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);
u8g2_for_adafruit_gfx.setCursor(0,10);
u8g2_for_adafruit_gfx.print(character.substring(0,21));
u8g2_for_adafruit_gfx.setCursor(0,21);
u8g2_for_adafruit_gfx.print(character.substring(21,string_length));
display.display();
}
else if (string_length > 42 && string_length <= 63)
{
display.clearDisplay();
u8g2_for_adafruit_gfx.setFontMode(1);
u8g2_for_adafruit_gfx.setFontDirection(0);
u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);
u8g2_for_adafruit_gfx.setCursor(0,10);
u8g2_for_adafruit_gfx.print(character.substring(0,21));
u8g2_for_adafruit_gfx.setCursor(0,21);
u8g2_for_adafruit_gfx.print(character.substring(21,42));
u8g2_for_adafruit_gfx.setCursor(0,32);
u8g2_for_adafruit_gfx.print(character.substring(42,63));
display.display();
}
else {
display.clearDisplay();
u8g2_for_adafruit_gfx.setFontMode(1);
u8g2_for_adafruit_gfx.setFontDirection(0);
u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_12_mf);
u8g2_for_adafruit_gfx.setCursor(0,10);
u8g2_for_adafruit_gfx.print(character.substring(0,21));
u8g2_for_adafruit_gfx.setCursor(0,21);
u8g2_for_adafruit_gfx.print(character.substring(21,42));
u8g2_for_adafruit_gfx.setCursor(0,32);
u8g2_for_adafruit_gfx.print(character.substring(42,60)+ "...");
display.display();
}}}
