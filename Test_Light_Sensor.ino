#include <Servo.h>

#define servoPin 2

//shift register
int latchPin = 12;
int clockPin = 13;
int dataPin = 11;

Servo servoMotor;

int mode = 0;
byte leds = 0;

int redLightsOnOff = 0;
int lock_unlock = 0;

int light_sensor;
int already_off = 0;

char command;

void setup()
{
  //red LEDs
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  //servomotor
  servoMotor.attach(servoPin);

  //yellow LEDs
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  Serial.begin(9600);

  leds = 0;

  updateShiftRegister();
  delay(200);

  // reset servo-motor
  servoMotor.write(180);
  delay(1000);
}

void loop()
{
  //read light sensor value
  light_sensor = analogRead(0);
  Serial.println(light_sensor);

  if(light_sensor > 151 && already_off == 0) {
    if(mode == 0) {
      lock_animation();
      already_off = 1;
      redLightsOnOff = 0;
    }
  } else if(light_sensor < 151 && lock_unlock == 1) {
    redLightsAnimation();
    already_off = 0;
  }

  if(Serial.available() > 0) {
    command = Serial.read();

    if(command == '1') { // left signal
      while(1) {
        for(int i = 3; i < 11; i++) {
          digitalWrite(i, HIGH);
          delay(100);
        }

        for(int i = 3; i < 11; i++) {
          digitalWrite(i, LOW);
        }

        if(Serial.available() > 0) {
          char c = Serial.read();
          if(c == '5') {
            for(int i = 3; i < 11; i++) {
              digitalWrite(i, LOW);
            }
            break;
          }
        }
        delay(500);
      }
    }

    if(command == '2') { //right signal
      while(1) {
        for(int i = 10; i >= 3; i--) {
          digitalWrite(i, HIGH);
          delay(100);
        }

        for(int i = 10; i >= 3; i--) {
          digitalWrite(i, LOW);
        }

        if(Serial.available() > 0) {
          char c = Serial.read();
          if(c == '5') {
            for(int i = 10; i >= 3; i--) {
              digitalWrite(i, LOW);
          }
            break;
          }
        }
        delay(500);
      }
    }

    if(command == '3') { // unlock
      if(lock_unlock == 1) {
        Serial.println("Already unlocked!");
      } else {
        do_unlock();
        lock_unlock = 1;
      }
    }

    if(command == '4') { //lock
      if(lock_unlock == 0) {
        Serial.println("Already locked!");
      } else {
        do_lock();
        lock_unlock = 0;
      }
    }

    if(command == '6') { //avarii
      while(1) {
        avarii();

        if(Serial.available() > 0) {
          char c = Serial.read();
          if(c == '5') {
            for(int i = 10; i >= 3; i--) {
              digitalWrite(i, LOW);
          }
            break;
          }
        }
        delay(500);
      }
    }

    if(command == '7') {
      if(redLightsOnOff == 0) {
        redLightsAnimation();
        mode = 1;
        already_off = 0;
      } else if(redLightsOnOff == 1) {
        lock_animation();
        already_off = 1;
        mode = 0;
      }
    }

  }
}

void avarii() {
  for(int i = 6, j = 7; i >= 0, j < 11; i--, j++) {
    digitalWrite(i, HIGH);
    digitalWrite(j, HIGH);
    delay(100);
  }

  for(int i = 6, j = 7; i >= 0, j < 11; i--, j++) {
    digitalWrite(i, LOW);
    digitalWrite(j, LOW);
  }
}

void do_lock() {
  //TODO
  avarii();
  delay(500);
  if(redLightsOnOff == 1) {
    lock_animation();
  }
  redLightsOnOff = 0;
  servoMotor.write(180);
  delay(200);
}

void do_unlock() {
  //if car unlocked and night let red LEDs ON
  avarii();
  delay(350);
  avarii();
  if(light_sensor < 151) {
    redLightsAnimation();
  } else {
    leds = 0;
    updateShiftRegister();
    delay(200);
  }
  servoMotor.write(0);
  delay(200);
}

void redLightsAnimation() {
  if(redLightsOnOff == 0) {
    leds = 0;
    updateShiftRegister();
    delay(500);

    for (int i = 0; i < 8; i += 2) {
      bitSet(leds, i);
      updateShiftRegister();
        delay(100);
    }

    for (int i = 1; i < 8; i += 2) {
      bitSet(leds, i);
      updateShiftRegister();
        delay(100);
    }

    turning_off_animation(100);

    leds = 0;
    updateShiftRegister();
    delay(200);

    for (int i = 7; i >= 0; i--) {
      bitSet(leds, i);
      updateShiftRegister();
      delay(100);
    }

    redLightsOnOff = 1;
  }
}

void turning_off_animation(int s) {
    leds = 254;
    updateShiftRegister();
    delay(s);

    leds = 252;
    updateShiftRegister();
    delay(s);

    leds = 248;
    updateShiftRegister();
    delay(s);

    leds = 240;
    updateShiftRegister();
    delay(s);

    leds = 224;
    updateShiftRegister();
    delay(s);

    leds = 192;
    updateShiftRegister();
    delay(s);

    leds = 128;
    updateShiftRegister();
    delay(s);

    leds = 0;
    updateShiftRegister();
    delay(s);

    redLightsOnOff = 0;
}

void lock_animation() {
    leds = 231;
    updateShiftRegister();
    delay(200);

    leds = 195;
    updateShiftRegister();
    delay(200);

    leds = 129;
    updateShiftRegister();
    delay(200);

    leds = 0;
    updateShiftRegister();
    delay(200);

    redLightsOnOff = 0;
}

void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}
