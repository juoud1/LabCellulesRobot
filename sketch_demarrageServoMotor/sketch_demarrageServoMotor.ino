

/*
 This sample code demonstrates how to use the SimpleKalmanFilter object. 
 Use a potentiometer in Analog input A0 as a source for the reference real value.
 Some random noise will be generated over this value and used as a measured value.
 The estimated value obtained from SimpleKalmanFilter should match the real
 reference value.

 SimpleKalmanFilter(e_mea, e_est, q);
 e_mea: Measurement Uncertainty 
 e_est: Estimation Uncertainty 
 q: Process Noise
 */
 
/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>
#include <SimpleKalmanFilter.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 36 
#define TFT_CS 38 
#define TFT_MOSI MOSI// 11
#define TFT_CLK SCK// 30 
#define TFT_RST 32 
#define TFT_MISO MISO //12

#define SERIAL_MONITOR    0
#define ADC               1
#define SERVO             2
#define GRAPHIC           3
#define NOTHING_TO_DO     10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
int counter;
  
Servo myservo1;  // create servo object to control a servo
Servo myservo2;
Servo myservo3;

//SimpleKalmanFilter simpleKalmanFilter(2, 2, 0.01);
SimpleKalmanFilter simpleKalmanFilter1(2, 2, 0.01);
SimpleKalmanFilter simpleKalmanFilter2(2, 2, 0.01);
SimpleKalmanFilter simpleKalmanFilter3(2, 2, 0.01);

int potpin1 = A1;  // analog pin used to connect the potentiometer
int potpin2 = A2;
int potpin3 = A3;
int val1, val2, val3;    // variable to read the value from the analog pin
float estimated_val1, estimated_val2, estimated_val3;
//static int continuer;

// Serial output refresh time
const long SERIAL_REFRESH_TIME = 100;
long refresh_time;

void plotter (int graph, int l_xp1, int l_yp1, int l_xp2, int l_yp2, int l_xp3, int l_yp3, int l_valp, int l_vals);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);//(115200);
  //Reset screen
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(LED_BUILTIN, LOW);
  delay(20);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(120);
  Serial.println("Bonjour, intégration servo et écran!"); 
   
  tft.begin(9600);//(40000000);

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);

  tft.fillScreen(ILI9341_OLIVE);
      
  myservo1.attach(6);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(7);
  myservo3.attach(8);
  
  //continuer = 1;
}

void loop() {
  int x1 = 0, x1_old = 0, 
    y1 = 0, y1_old = 0,
    x2 = 0, x2_old = 0, 
    y2 =0, y2_old = 0,
    x3=0, x3_old = 0,
    y3 = 0, y3_old = 0;

  int xp1 = 10, yp1 = 60, 
    xp2 = 10, yp2 = 10, 
    xp3 = 200, yp3 = 60,

    xp4 = 10, yp4 = 120, 
    xp5 = 10, yp5 =70, 
    xp6 = 200, yp6 = 120,

    xp7 = 10, yp7 = 180, 
    xp8 = 10, yp8 = 130, 
    xp9 = 200, yp9 = 180;

  int val1p, val2p, val3p;
  int val1s, val2s, val3s;
  
  tft.setRotation(1);
  //tft.setCursor(10, 205);
  tft.setTextColor(ILI9341_NAVY);  
  tft.setTextSize(2);
  //tft.println("Bjr Jodo, Abby, Honorine\n Evan, Malicka, Zara\n");
    
  static int token = NOTHING_TO_DO;
  
  static unsigned long previous_Millis_SERIAL_MONITOR = 0;
  static unsigned long previous_Millis_ADC = 0;
  static unsigned long previous_Millis_SERVO = 0;
  static unsigned long previous_Millis_GRAPHIC = 0;

  const long interval_SERIAL_MONITOR = 60;
  const long interval_ADC = 5;
  const long interval_SERVO = 20;
  const long interval_GRAPHIC = 20;

  static unsigned long currentMillis = millis();

  if ((currentMillis - previous_Millis_SERIAL_MONITOR) >= interval_SERIAL_MONITOR ) token = SERIAL_MONITOR;
  if ((currentMillis - previous_Millis_ADC) >= interval_ADC )  token = ADC;
  if ((currentMillis - previous_Millis_SERVO) >= interval_SERVO )  token = SERVO;
  if ((currentMillis - previous_Millis_GRAPHIC) >= interval_GRAPHIC )  token = GRAPHIC;
  
  switch (token){
    case SERIAL_MONITOR:
    {
      Serial.print("min:");
      Serial.print(0);
      Serial.print(" s1:");
      Serial.print((int)val1s);
      Serial.print(" s2:");
      Serial.print((int)val2s);
      Serial.print(" s3:");
      Serial.print((int)val3s);
      Serial.print(" max:");
      Serial.print((int)180);
      previous_Millis_SERIAL_MONITOR = millis();
      token = GRAPHIC; //NOTHING_TO_DO;
    }break;
    case ADC:
    {
      val1p = analogRead(potpin1); 
      val2p = analogRead(potpin2); 
      val3p = analogRead(potpin3); 
      previous_Millis_ADC = millis();
      token = GRAPHIC;//NOTHING_TO_DO;
    }break;
    case SERVO:
    {
      val1p = analogRead(potpin1); 
      val2p = analogRead(potpin2); 
      val3p = analogRead(potpin3);
      
      val1s = map(val1p, 0, 1023, 0, 180);
      val2s = map(val2p, 0, 1023, 180, 0);
      val3s = map(val3p, 0, 1023, 270, 90);
      myservo1.write(val1s);                  // sets the servo position according to the scaled value
      myservo2.write(val2s);
      myservo3.write(val3s);
      previous_Millis_SERVO = millis();
      token = GRAPHIC; //NOTHING_TO_DO;
    }break;
    case GRAPHIC:
    {
      plotter (1, xp1, yp1, xp2, yp2, xp3, yp3, val1p, val1s);
      plotter (2, xp4, yp4, xp5, yp5, xp6, yp6, val2p, val2s);
      plotter (3, xp7, yp7, xp8, yp8, xp9, yp9, val3p, val3s);
      previous_Millis_GRAPHIC = millis();
      token = GRAPHIC; //NOTHING_TO_DO;
    }break;
    case NOTHING_TO_DO:
    { 
      token = GRAPHIC;
    }break;
    default:
    {
      token = GRAPHIC;
    }break;
  }
  delay(100);
}

void plotter (int graph, int l_xp1, int l_yp1, int l_xp2, int l_yp2, int l_xp3, int l_yp3, int l_valp, int l_vals){
  static int x1_old[4] = {0, 0, 0, 0};
  static int y1_old[4] = {0, 0, 0, 0};
  int l_x1, l_y1;
  
  tft.drawLine(l_xp2, l_yp2, l_xp1, l_yp1, ILI9341_WHITE);
  tft.drawLine(l_xp1, l_yp1, l_xp3, l_yp3, ILI9341_WHITE);
  l_x1 = map(l_valp, 0, 1023, l_xp1, l_xp3);
  l_y1 = map(l_vals, 0, 180, l_yp1, l_yp2);
  tft.fillCircle(x1_old[graph], y1_old[graph], 3, ILI9341_OLIVE );
  tft.fillCircle(l_x1, l_y1, 3, ILI9341_ORANGE );
  
  x1_old[graph] = l_x1;
  y1_old[graph] = l_y1;
}

void getDummyButterfly(void){
    tft.fillTriangle(175, 75, 175, 125, 225, 125, ILI9341_CYAN);
    tft.fillTriangle(175, 180, 175, 130, 225, 130, ILI9341_ORANGE);
    tft.fillTriangle(285, 75, 285, 125, 235, 125, ILI9341_PINK);
    tft.fillTriangle(285, 180, 285, 130, 235, 130, ILI9341_LIGHTGREY);
    tft.fillCircle(230, 100, 18, ILI9341_WHITE);
    tft.fillRect(227, 118, 9, 40, ILI9341_WHITE);
  }
