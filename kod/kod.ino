/*
* Name: övningsprojekt
* Author: Victor Huke
* Date: 2025-10-8
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display, 
* Further, it measures temprature and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include <U8glib.h>
#include <Servo.h>
#include <LedControl.h>

// Setting up classes.
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myServo;
LedControl lc = LedControl(11, 13, 10, 1);


void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  myServo.attach(9);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8); 
  lc.clearDisplay(0);    

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  float temp = getTemp();
  String timeNow = getTime();
  oledWrite(timeNow,temp);
  servoWrite(temp);
  Serial.print("Tid: ");
  Serial.print(timeNow);
  Serial.print("  Temp: ");
  Serial.println(temp);
  
  int hour = rtc.now().hour();
  int minute = rtc.now().minute();
  int second = rtc.now().second();

  lc.setDigit(0, 7, hour / 10, false); 
  lc.setDigit(0, 6, hour % 10, false);   
  lc.setDigit(0, 5, minute / 10, false);  
  lc.setDigit(0, 4, minute % 10, false);  
  lc.setDigit(0, 3, second / 10, false);  
  lc.setDigit(0, 2, second % 10, false);

  Serial.print("Tid: ");
  Serial.print(getTime());
  Serial.print("  Temp: ");
  Serial.println(temp);

  delay(1000);
}



/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

/*
* This function takes temprature from ds3231 and returns as a float
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
  return rtc.getTemperature();
}
/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text, float temp) {
  u8g.setFont(u8g_font_unifont);
  u8g.firstPage();
  do {
    u8g.drawStr(0, 30, text.c_str());
    String tempStr = "Temp: " + String(temp, 1) + "C";  
    u8g.drawStr(0, 50, tempStr.c_str());
  } while (u8g.nextPage());
}

/*
* This function write a temperature value to the servo
*Parameters: - value: temperature value to write to servo
*Returns: void
*/
void servoWrite(float value) {
  // Mappa temp (0°C–40°C) till servo (0°–180°)
  int angle = map((int)value, 0, 40, 0, 180);
  angle = constrain(angle, 0, 180);  // begränsa till giltig vinkel
  myServo.write(angle);
}