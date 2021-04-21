/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  The 1.14" TFT breakout
  ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
  ----> https://www.adafruit.com/product/4313
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <RTClib.h>            // include Adafruit RTC library

// These pins will also work for the 1.8" TFT shield.
#define TFT_CS        53
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8
#define CLK 34
#define DT 36
#define SW 38

int counter = 0;
int currentStateCLK;
int lastStateCLK;
int currentdiv=0;
String currentDir ="";
unsigned long lastButtonPress = 0;
 



// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
RTC_DS3231 rtc;
DateTime   now;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char dow_matrix[7][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};


char _buffer[11];
float p = 3.1415926;
int pulslar[128];
int original_log[128];
int toplam_puls;
float faktor=1;
byte x_pos[7] = {8, 8, 6, 4, 4, 8, 4}; // 29,29,23,11,17,29,17
static byte previous_dow = 8;
int div0 = 0;
int div1 = 0;
 


void setup(void) {
  Serial.begin(9600);
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(CLK);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  
  int tolam_puls = 0;
 for (int i = 0; i < 128; i++)
       {
        pulslar[i] = 0;           // alle Pulshöhentreffer auf 0 setzen
       }
 for (int i = 0; i < 128; i++)
       {
        original_log[i] = 0;           // alle Pulshöhentreffer auf 0 setzen
       }   
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
 
  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  delay(500);
//  testdrawrects(ST77XX_GREEN);
//  delay(500);
  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  delay(500);
  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  delay(500);
  tft.fillScreen(ST77XX_BLACK);

  tft.drawFastHLine(0, 130, 63, ST7735_BLUE);
  tft.drawFastHLine(0, 101, tft.width(), ST7735_BLUE);
  tft.drawFastVLine(64, 100, 59, ST7735_BLUE);
  tft.drawFastHLine(0, 100,  tft.width(), ST7735_BLUE);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     // set text color to white and black background
  tft.setTextSize(1);                 // text size = 1
  
}

void loop() {
  counter =0;
 int toplam_puls = 0;
  int btnState = digitalRead(SW);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 20) {
      Serial.println("Button pressed!,currentdiv =");
      if (currentdiv==0){
        currentdiv=1;
      }
      else if (currentdiv==1){
        currentdiv=0;
      }
      Serial.println(currentdiv);
      
    }

    // Remember last button press event
    lastButtonPress = millis();
  }
 currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
  
    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(DT) != currentStateCLK) {
      counter =-1;
      currentDir ="CCW";
    } else {
      // Encoder is rotating CW so increment
      counter =1;
      currentDir ="CW";
    }
  
    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
  }
  // Remember last CLK state
  lastStateCLK = currentStateCLK;

 now = rtc.now();  // read current time and date from the RTC chip
  tft.setCursor(x_pos[previous_dow], 103);
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);     // set text color to cyan and black background
  tft.print( dow_matrix[now.dayOfTheWeek()] );
  
 
  // print date
  sprintf( _buffer, "%02u-%02u-%04u", now.day(), now.month(), now.year() );
  tft.setCursor(4, 112);
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);     // set text color to yellow and black background
  tft.print(_buffer);
  
  // print time
  sprintf( _buffer, "%02u:%02u:%02u", now.hour(), now.minute(), now.second() );
  tft.setCursor(4, 121);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);     // set text color to green and black background
  tft.print(_buffer);
  
 

 int Index = map(analogRead(A3),0,1023,0,127);
  original_log[Index] = original_log[Index] + 1;


  bool flag=1;

  
        
         pulslar[Index] = pulslar[Index] + 1;
        
        for(int t=0;t<127&&flag==1;t++){
            if(pulslar[t]>100/faktor){
              faktor=faktor/2;
                 flag=0;    
          }
        
        }
  //int div0 = map(analogRead(A0),0,1023,0,127);
  //int div1 = map(analogRead(A1),0,1023,0,127);

  if (counter!=0 && currentdiv==0) div0=div0+counter;
  if (counter!=0 && currentdiv==1) div1=div1+counter;
  if (div1>127) div1=127;
  if (div0>127) div0=127;

for (int i = 0; i < 128; i++)
       {
       tft.drawLine(i,100,i,100-pulslar[i],ST7735_RED);
       tft.drawLine(i,100-int(faktor*pulslar[i]),i,0,ST7735_BLACK);
       tft.drawLine(div0,100,div0,0,ST7735_BLUE);
       tft.drawLine(div1,100,div1,0,ST7735_BLUE);

       }
 

  
//  tft.drawFastVLine(div0,0,101, ST7735_BLUE);
//  tft.drawFastVLine(div1,0,101, ST7735_BLUE);
//  tft.drawLine(div0,100,div0,0,ST7735_RED);
//  tft.drawLine(div1,100,div1,0,ST7735_RED);
  if( div0 < div1){
    for (int i = div0; i < div1; i++){
      toplam_puls = toplam_puls + original_log[i];
    }

    
  }
  else {
    for (int i = div1; i < div0; i++){
      toplam_puls = toplam_puls + original_log[i];
  }
  }
for (int i = div0; i < div1; i++)
       {
       tft.drawLine(i,100,i,100-pulslar[i],ST7735_RED);
       tft.drawLine(i,100-int(faktor*pulslar[i]),i,0,ST7735_BLACK);

       }
        
  tft.setTextSize(1);       
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     
  tft.setCursor(4,132);  
  tft.print("div0:" );
  tft.setCursor(34,132);       
  tft.print(div0);
  //tft.print("   ");
  tft.setCursor(4,141);  
  tft.print("div1:" );
  tft.setCursor(34,141);
  
  tft.print(div1);
  //tft.print("   ");      
      
  tft.setCursor(4,150); 
  tft.print("puls:");
  tft.setCursor(35,150);
  tft.print(toplam_puls);
  tft.print("  ");
}




void RTC_display()
{
  char _buffer[11];
  char dow_matrix[7][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
  byte x_pos[7] = {8, 8, 6, 4, 4, 8, 4}; // 29,29,23,11,17,29,17
  static byte previous_dow = 8;
 
  // print day of the week
  if( previous_dow != now.dayOfTheWeek() )
  {
    previous_dow = now.dayOfTheWeek();
//    tft.fillRect(11, 55, 108, 14, ST7735_BLACK);     // draw rectangle (erase day from the display)
    tft.setCursor(x_pos[previous_dow], 103);
    tft.setTextColor(ST7735_CYAN, ST7735_BLACK);     // set text color to cyan and black background
    tft.print( dow_matrix[now.dayOfTheWeek()] );
  }
 
  // print date
  sprintf( _buffer, "%02u-%02u-%04u", now.day(), now.month(), now.year() );
  tft.setCursor(4, 112);
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);     // set text color to yellow and black background
  tft.print(_buffer);
  
  // print time
  sprintf( _buffer, "%02u:%02u:%02u", now.hour(), now.minute(), now.second() );
  tft.setCursor(4, 121);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);     // set text color to green and black background
  tft.print(_buffer);
}
void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}
