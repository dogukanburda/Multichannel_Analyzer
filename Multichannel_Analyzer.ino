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

// Defining SPI Pins for TFT LCD
#define TFT_CS        53
#define TFT_RST        9
#define TFT_DC         8


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
// Initialize RTC module and configure the days
RTC_DS3231 rtc;
DateTime   now;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char dow_matrix[7][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};


char _buffer[11];
float p = 3.1415926;
int pulslar[128]; // For drawing on LCD
int original_log[128];    // For keeping the original logs of pulses
int toplam_puls;          // To display the number of pulses occurred in the region of interest (ROI)
float faktor=1;         // Multiplying factor for the histogram to not exceed the screen

unsigned long oncekizaman = 0; 

byte x_pos[7] = {8, 8, 6, 4, 4, 8, 4}; // 29,29,23,11,17,29,17
static byte previous_dow = 8;

 

void setup(void) {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //pinMode(2, INPUT);
  pinMode(2, INPUT); // comparator çıkışı pin 2
  pinMode(3, OUTPUT);  //reset pulse'ı pin 3

  int tolam_puls = 0;
  for (int i = 0; i < 128; i++)
       {
        pulslar[i] = 0;
       }
  for (int i = 0; i < 128; i++)
       {
        original_log[i] = 0;
       }
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.fillScreen(ST77XX_BLACK);
  tft.fillScreen(ST77XX_BLACK);
  // Drawing the outline of sections for the display
  tft.drawFastHLine(0, 130, 63, ST7735_BLUE);
  tft.drawFastHLine(0, 101, tft.width(), ST7735_BLUE);
  tft.drawFastVLine(64, 100, 59, ST7735_BLUE);
  tft.drawFastHLine(0, 100,  tft.width(), ST7735_BLUE);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     // set text color to white and black background
  tft.setTextSize(1);                               // text size = 1
  //Outline for dividers' value and toplam puls
  tft.setTextSize(1);       
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     
  tft.setCursor(4,132);   // For the first Divider
  tft.print("CH1:" );
  tft.setCursor(4,141);   // For the second Divider
  tft.print("CH2:" );
  tft.setCursor(4,150);    // For the Total pulses between dividers
  tft.print("puls:");
  
}

void loop() {
  unsigned long ilkzaman = millis();
  int val = digitalRead(2);
  if (val == 1) {
    int deger = analogRead(A5);   // genliği okuduğum pin A5
    digitalWrite(3,HIGH);
    delayMicroseconds(1);
    digitalWrite(3,LOW);

  Serial.println(deger);
  int toplam_puls = 0;
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

  int Index = map(deger,0,1023,0,127);
  original_log[Index] = original_log[Index] + 1;    // Reads from analog pin and appends to the right channel accordingly

  bool flag=1;
  pulslar[Index] = pulslar[Index] + 1;
  
  for(int t=0;t<127&&flag==1;t++){                  // This code is for checking if the pulses exceeded display screen
    if(pulslar[t]>100/faktor){                      // Needs to be optimized.
      faktor=faktor/2;
      flag=0;
      }
      }
  
  unsigned char div0 = map(analogRead(A3),0,1023,0,127); //first divider index between 0-127
  unsigned char div1 = map(analogRead(A1),0,1023,0,127); //second divider index between 0-127

  for (int i = 0; i < 128; i++){
    tft.drawLine(i,100,i,100-pulslar[i],ST7735_RED);
    tft.drawLine(i,100-int(faktor*pulslar[i]),i,0,ST7735_BLACK);
    //tft.drawLine(div0,100,div0,0,ST7735_WHITE);
    //tft.drawLine(div1,100,div1,0,ST7735_WHITE);
    }

  
  tft.drawLine(div0,100,div0,0,ST7735_WHITE);
  tft.drawLine(div1,100,div1,0,ST7735_WHITE);
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
  
  for (int i = div0+1; i < div1; i++)
     {
     tft.drawLine(i,100,i,100-pulslar[i],ST7735_RED);
     tft.drawLine(i,100-int(faktor*pulslar[i]),i,0,ST7735_BLACK);
     }
  tft.drawLine(div0,100,div0,0,ST7735_WHITE);


  tft.setCursor(23,132);
  tft.print(div0);
  tft.print("   ");
  tft.setCursor(23,141);
  tft.print(div1);
  tft.print("   ");

  tft.setCursor(35,150);
  tft.print(toplam_puls);
  tft.print("  ");
  unsigned long ikincizaman = millis();
  unsigned long timer=ikincizaman-ilkzaman;
  Serial.println(timer);
}
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
