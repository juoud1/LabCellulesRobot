/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;
Servo myservo3;

int potpin1 = A1;  // analog pin used to connect the potentiometer
int potpin2 = A2;
int potpin3 = A3;
int val1,val2,val3;    // variable to read the value from the analog pin

void setup() {
  myservo1.attach(6);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(7);
  myservo3.attach(8);
}

void loop() {
  val1 = analogRead(potpin1);            // reads the value of the potentiometer (value between 0 and 1023)
  val1 = map(val1, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  myservo1.write(val1);                  // sets the servo position according to the scaled value
  val2 = analogRead(potpin2);
  val2 = map(val2, 0, 1023, 180, 0);
  myservo2.write(val2);
  val3 = analogRead(potpin3);
  val3 = map(val3, 0, 1023, 0, 180);
  myservo3.write(val3);
  delay(15);                           // waits for the servo to get there
}
