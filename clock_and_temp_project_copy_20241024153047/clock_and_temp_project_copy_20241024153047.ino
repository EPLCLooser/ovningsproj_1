/*
* Name: clock and temp project
* Author: Lucas Norrflod
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
  #include <RTClib.h>
  #include <Wire.h>
  #include <Servo.h>
  #include "U8glib.h"
  // Init constants
  const int tempPin = A0;
  const int ServoPin = 8;
  const int redPin= A1;
  const int greenPin = A2;
  const int bluePin = A3;

  // Init global variables
  
  // construct objects
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
  Servo myServo;
  RTC_DS3231 rtc;

void setup() {
  // Attach objects
  myServo.attach(ServoPin);

  // init communication
  Serial.begin(9600);
  Wire.begin();
  // Init Hardware
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  u8g.setFont(u8g_font_unifont);
  u8g.firstPage(); 

  pinMode(tempPin,INPUT);
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
}

void loop() {
  oledWrite(String(getTime())); // Writes the time on the oled display
  servoWrite(measureTemp()); // Sets the servo to a degree depending on the temperature
  light(measureTemp()); // Show a color depending on the temperature
  delay(1000);
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  Serial.print("Time");
  Serial.println(" " +String(now.hour()) + " " + String(now.minute()) + " " + String(now.second()));
  return (" " +String(now.hour()) + " " + String(now.minute()) + " " + String(now.second()));
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temperature as float
*/
float measureTemp() {
  float temp = 0;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor

  int Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp = temp - 273.15;                                           //convert Kelvin to Celcius

  return temp;
}


/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text) {
  u8g.firstPage();
  do {

    u8g.drawStr(0, 22, text.c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
  Serial.println(value);
  Serial.println(map(value,20,30,0,180));
  myServo.write(map(value,20,30,0,180));
}



void light(int value) {
  int r = map(value, 20,30,0,255);
  int g;
  if (value<25){
    g = map(value, 20,25,0,255);
  }
  else{
    g = map(value, 25,30,255,0);
  }
  int b = map(value, 20,30,255,0);
  analogWrite(redPin, r);
  analogWrite(greenPin,  g);
  analogWrite(bluePin, b);
}