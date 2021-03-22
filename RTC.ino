#include <Wire.h>              // include Arduino Wire library (required for I2C devices)
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <RTClib.h>            // include Adafruit RTC library

#define TFT_RST   2      // TFT RST pin is connected to arduino pin 8
#define TFT_CS    3      // TFT CS  pin is connected to arduino pin 9
#define TFT_DC    4     // TFT DC  pin is connected to arduino pin 10

// initialize ST7735 TFT library
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
 
// initialize RTC library
RTC_DS3231 rtc;
DateTime   now;

#define button1   7   // button B1 is connected to Arduino pin 9
#define button2   6   // button B2 is connected to Arduino pin 8
int pulslar[128];
int original_log[128];
int toplam_puls;
float faktor=1;




void setup(void) {
  Serial.begin(9600);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  int tolam_puls = 0;

 for (int i = 0; i < 128; i++)
       {
        pulslar[i] = 0;           // alle Pulshöhentreffer auf 0 setzen
       }
 for (int i = 0; i < 128; i++)
       {
        original_log[i] = 0;           // alle Pulshöhentreffer auf 0 setzen
       }      

  rtc.begin();   // initialize RTC chip
 
  tft.initR(INITR_BLACKTAB);     // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);  // fill screen with black color
//  tft.drawFastHLine(0, 20,  tft.width(), ST7735_BLUE);    // draw horizontal blue line at position (0, 44)
  tft.drawFastHLine(0, 130, 63, ST7735_BLUE);    // draw horizontal blue line at position (0, 44)
  tft.drawFastHLine(0, 101, tft.width(), ST7735_BLUE);    // draw horizontal blue line at position (0, 102)
//  tft.drawFastVLine(66, 20, 20, ST7735_BLUE); 
  tft.drawFastVLine(64, 100, 59, ST7735_BLUE);
 
  tft.drawFastHLine(0, 100,  tft.width(), ST7735_BLUE);


 
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     // set text color to white and black background
  tft.setTextSize(1);                 // text size = 1
//  tft.setCursor(4, 2);                // move cursor to position (4, 10) pixel
//  tft.print("ARDUINO + ST7735 TFT");
//  tft.setCursor(28, 10);              // move cursor to position (28, 27) pixel
//  tft.print("+ DS3231 RTC");
//  tft.setCursor(14, 134);             // move cursor to position (28, 27) pixel
//  tft.print("CHIP TEMPERATURE:");
//  tft.setTextSize(1);                 // text size = 2
//  tft.setTextColor(ST7735_MAGENTA, ST7735_BLACK);     // set text color to magneta and black background
//  tft.setCursor(66, 103);              // move cursor to position (37, 112) pixel
//  tft.print("PULS:");
}
 
// a small function for button1 (B1) debounce
bool debounce ()
{
  byte count = 0;
  for(byte i = 0; i < 5; i++)
  {
    if ( !digitalRead(button1) )
      count++;
    delay(10);
  }
 
  if(count > 2)  return 1;
  else           return 0;
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
 
byte edit(byte parameter)
{
  static byte i = 0, y_pos,
              x_pos[5] = {4, 40, 100, 16, 52};
  char text[3];
  sprintf(text,"%02u", parameter);
 
  if(i < 3) {
    tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);    // set text color to green and black background
    y_pos = 52;
  }
  else {
    tft.setTextColor(ST7735_GREEN, ST7735_BLACK);     // set text color to yellow and black background
    y_pos = 107;
  }
 
  while( debounce() );   // call debounce function (wait for B1 to be released)
 
  while(true) {
    while( !digitalRead(button2) ) {  // while B2 is pressed
      parameter++;
      if(i == 0 && parameter > 31)    // if day > 31 ==> day = 1
        parameter = 1;
      if(i == 1 && parameter > 12)    // If month > 12 ==> month = 1
        parameter = 1;
      if(i == 2 && parameter > 99)    // If year > 99 ==> year = 0
        parameter = 0;
      if(i == 3 && parameter > 23)    // if hours > 23 ==> hours = 0
        parameter = 0;
      if(i == 4 && parameter > 59)    // if minutes > 59 ==> minutes = 0
        parameter = 0;
 
      sprintf(text,"%02u", parameter);
      tft.setCursor(x_pos[i], y_pos);
      tft.print(text);
      delay(200);       // wait 200ms
    }
 
    tft.fillRect(x_pos[i], y_pos, 22, 14, ST7735_BLACK);
    unsigned long previous_m = millis();
    while( (millis() - previous_m < 250) && digitalRead(button1) && digitalRead(button2)) ;
    tft.setCursor(x_pos[i], y_pos);
    tft.print(text);
    previous_m = millis();
    while( (millis() - previous_m < 250) && digitalRead(button1) && digitalRead(button2)) ;
 
    if(!digitalRead(button1))
    {                     // if button B1 is pressed
      i = (i + 1) % 5;    // increment 'i' for the next parameter
      return parameter;   // return parameter value and exit
    }
  }

}

void loop() {
  int toplam_puls = 0;
  if( !digitalRead(button1) )  // if B1 is pressed
  if( debounce() )             // call debounce function (make sure B1 is pressed)
  {
    while( debounce() );  // call debounce function (wait for B1 to be released)
 
    byte day    = edit( now.day() );          // edit date
    byte month  = edit( now.month() );        // edit month
    byte year   = edit( now.year() - 2000 );  // edit year
    byte hour   = edit( now.hour() );         // edit hours
    byte minute = edit( now.minute() );       // edit minutes
 
    // write time & date data to the RTC chip
    rtc.adjust(DateTime(2000 + year, month, day, hour, minute, 0));
 
    while(debounce());  // call debounce function (wait for button B1 to be released)
  }
 
  now = rtc.now();  // read current time and date from the RTC chip
  RTC_display();    // diaplay time & calendar
//  tft.drawFastVLine(map(analogRead(A0),0,1023,0,127),0,100, ST7735_BLACK); 
 int Index = map(analogRead(A7),0,1023,0,127);
  original_log[Index] = original_log[Index] + 1;


  bool flag=1;

  
        
         pulslar[Index] = pulslar[Index] + 1;
        
        for(int t=0;t<127&&flag==1;t++){
            if(pulslar[t]>100/faktor){
              faktor=faktor/2;
                 flag=0;    
          }
        
        }

for (int i = 0; i < 128; i++)
       {
       tft.drawLine(i,100,i,100-pulslar[i],ST7735_RED);
       tft.drawLine(i,100-int(faktor*pulslar[i]),i,0,ST7735_BLACK);

       }
 
  int div0 = map(analogRead(A0),0,1023,0,127);
  int div1 = map(analogRead(A1),0,1023,0,127);
  
  tft.drawFastVLine(div0,0,101, ST7735_BLUE);
  tft.drawFastVLine(div1,0,101, ST7735_BLUE);
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
        
// for (int i = 2; i<10; i++){
//    a=a+i;
//    } 
//  tft.print(a);
 
  tft.setTextSize(1);       
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     
  tft.setCursor(4,132);  
  tft.print("A0:" );
  tft.setCursor(23,132); 
  if(div0 >100 || div1 >100){
    tft.fillRect(23, 141, 40, 150, ST7735_BLACK);       
  }        
  tft.print(div0);
  tft.setCursor(4,141);  
  tft.print("A1:" );
  tft.setCursor(23,141);
  
  tft.print(div1);      
  tft.setCursor(4,150); 
  tft.print("puls:");
  tft.setCursor(35,150);
  tft.print(toplam_puls);
  delay(500);
  
  
  
  
  Serial.println("Gelen Puls'un mapi \n");
  Serial.println(Index);
//  Serial.println(Index);




  

//  // read chip temperature
//  Wire.beginTransmission(0x68); // start I2C protocol with DS3231 address
//  Wire.write(0x11);             // send register address (temperature MSB)
//  Wire.endTransmission(false);  // I2C restart
//  Wire.requestFrom(0x68, 2);    // request 2 bytes from DS3231 and release I2C bus at end of reading
//  byte t_msb = Wire.read();     // read temperature MSB
//  byte t_lsb = Wire.read();     // read temperature LSB
// 
//  // print chip temperature
//  char _buffer[6];
//  uint16_t chip_temp = (uint16_t)t_msb << 2 | t_lsb >> 6;
// 
//  if(t_msb & 0x80) {
//    chip_temp |= 0xFC00;
//    sprintf(_buffer, "-%02u.%02u", abs((int)chip_temp * 25) / 100, abs((int)chip_temp * 25) % 100);
//  }
//  else
//    sprintf(_buffer, " %02u.%02u", (chip_temp * 25) / 100, (chip_temp * 25) % 100);
// 
//  tft.setCursor(11, 146);
//  tft.setTextColor(ST7735_RED, ST7735_BLACK); // set text color to red and black background
//  tft.print(_buffer);
//  tft.drawCircle(90, 148, 2, ST7735_RED);      // print degree symbol ( ° )
//  tft.setCursor(95, 146);
//  tft.print("C");
// 
//  delay(100);       // wait 100ms

}
