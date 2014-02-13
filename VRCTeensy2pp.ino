#include <Bounce.h>

#define MIN 0
#define MAX 1023
#define RANGE 1024.0

#define THROTTLE  A1
#define STEERING  A0
#define BUTTON_A1 12
#define BUTTON_A2 9
#define BUTTON_B1 10
#define BUTTON_B2 11
#define BUTTON_C1 8

#define HAT_UP    2
#define HAT_DOWN  5
#define HAT_LEFT  4
#define HAT_RIGHT 6
#define HAT_CLICK 3

#define LED 13
#define NUM_READS 5
#define MINCALIB  9999
#define MAXCALIB -9999

#define VERBOSE false
#define HAT_AS_MOUSE false
#define HAT_AS_ADJUSTER true

#define CALC_MINMAX false
#define MINX 0
#define MAXX 1023
#define MINY 0
#define MAXY 1023

int minX = MINCALIB;
int maxX = MAXCALIB;
int minY = MINCALIB;
int maxY = MAXCALIB;

int xValue;
int yValue;

bool calibrated = false;
bool reported = false;

Bounce button_a1 = Bounce(BUTTON_A1, 1);
Bounce button_a2 = Bounce(BUTTON_A2, 1);
Bounce button_b1 = Bounce(BUTTON_B1, 1);
Bounce button_b2 = Bounce(BUTTON_B2, 1);
Bounce button_c1 = Bounce(BUTTON_C1, 1);

Bounce button_up    = Bounce(HAT_UP, 1);
Bounce button_down  = Bounce(HAT_DOWN, 1);
Bounce button_left  = Bounce(HAT_LEFT, 1);
Bounce button_right = Bounce(HAT_RIGHT, 1);
Bounce button_click = Bounce(HAT_CLICK, 1);

int serPrint = 10;

void setup() {
  if (VERBOSE) Serial.begin(9600);

  analogReadResolution(10);
  analogReadAveraging(0);
  // analogReference(EXTERNAL);

  Joystick.useManualSend(true);

  pinMode(BUTTON_A1, INPUT_PULLUP);
  pinMode(BUTTON_A2, INPUT_PULLUP);
  pinMode(BUTTON_B1, INPUT_PULLUP);
  pinMode(BUTTON_B2, INPUT_PULLUP);
  pinMode(BUTTON_C1, INPUT_PULLUP);

  pinMode(HAT_UP, INPUT_PULLUP);
  pinMode(HAT_DOWN, INPUT_PULLUP);
  pinMode(HAT_RIGHT, INPUT_PULLUP);
  pinMode(HAT_LEFT, INPUT_PULLUP);
  pinMode(HAT_CLICK, INPUT_PULLUP);

  pinMode(LED, OUTPUT);


  if (CALC_MINMAX) {
    if (VERBOSE) Serial.println("Waiting for calibration...");

    while (!calibrated) {
      xValue = analogRead(STEERING);
      yValue = analogRead(THROTTLE);

      if (xValue < minX) minX = xValue;
      if (xValue > maxX) maxX = xValue;
      if (yValue < minY) minY = yValue;
      if (yValue > maxY) maxY = yValue;

      if (VERBOSE) {
        Serial.print("X: ");
        Serial.print(xValue);
        Serial.print(" minX: ");
        Serial.print(minX);
        Serial.print(" maxX: ");
        Serial.print(maxX);
        Serial.print(" Y: ");
        Serial.print(yValue);
        Serial.print(" minY: ");
        Serial.print(minY);
        Serial.print(" maxY: ");
        Serial.println(maxY);
      }

      Joystick.X(xValue);
      Joystick.Y(yValue);
      Joystick.send_now();

      button_c1.update();

      if (button_c1.fallingEdge()) {
        if (VERBOSE) Serial.println("Calibration completed.");
        blink(10);
        calibrated = true;
      }
    }
  }
}

void blink(int count) {
  for(int i = 0; i < count; i++){
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}

float xPerc;
float xConverted;
float yPerc;
float yConverted;
int xCenter;
int yCenter;
int mouseX = 0;
int mouseY = 0;
int update_interval = 0;
elapsedMicros sinceLastUpdate;

void loop() {
  if (sinceLastUpdate > update_interval) {
    sinceLastUpdate = 0;
    xValue = analogRead(STEERING);
    yValue = analogRead(THROTTLE);

    if (VERBOSE) {
      Serial.print(xValue);
      Serial.print(" ");
      Serial.println(yValue);
    }

    if (CALC_MINMAX) {
      xCenter = constrain(map(xValue, minX, maxX, MIN, MAX), MIN, MAX);
      yCenter = constrain(map(yValue, minY, maxY, MIN, MAX), MIN, MAX);
    } else {
      xCenter = xValue;
      yCenter = yValue;
    }


    Joystick.X(xCenter);
    Joystick.Y(yCenter);
    Joystick.send_now();

    button_a1.update();
    button_a2.update();
    button_b1.update();
    button_b2.update();
    button_c1.update();

    button_up.update();
    button_down.update();
    button_left.update();
    button_right.update();
    button_click.update();

    if (button_a1.fallingEdge()) {
      if (VERBOSE) Serial.println("[a1] e-");
      Keyboard.set_key1(KEY_E);
      Keyboard.set_key2(KEY_MINUS);
      Keyboard.send_now();
      Keyboard.set_key1(0);
      Keyboard.set_key2(0);
      Keyboard.send_now();
    }

    if (button_b2.fallingEdge()) {
      if (VERBOSE) Serial.println("[a2] e=");
      Keyboard.set_key1(KEY_E);
      Keyboard.set_key2(KEY_EQUAL);
      Keyboard.send_now();
      Keyboard.set_key1(0);
      Keyboard.set_key2(0);
      Keyboard.send_now();
    }

    if (button_b1.fallingEdge()) {
      if (VERBOSE) Serial.println("[b1] -");
      Keyboard.set_key1(KEY_MINUS);
      Keyboard.send_now();
      Keyboard.set_key1(0);
      Keyboard.send_now();
    }

    if (button_a2.fallingEdge()) {
      if (VERBOSE) Serial.println("[b2] =");
      Keyboard.set_key1(KEY_EQUAL);
      Keyboard.send_now();
      Keyboard.set_key1(0);
      Keyboard.send_now();
    }

    if (button_c1.fallingEdge()) {
      if (VERBOSE) Serial.println("[c1][space]");
      Keyboard.set_key1(KEY_SPACE);
      Keyboard.send_now();
      Keyboard.set_key1(0);
      Keyboard.send_now();
    }


    if (HAT_AS_MOUSE) {
      mouseX = 0;
      mouseY = 0;

      if (!digitalRead(HAT_UP)) {
        if (VERBOSE) Serial.println("[up]");
        mouseY = -1;
      }

      if (!digitalRead(HAT_DOWN)) {
        if (VERBOSE) Serial.println("[down]");
        mouseY = 1;
      }

      if (!digitalRead(HAT_LEFT)) {
        if (VERBOSE) Serial.println("[left]");
        mouseX = -1;
      }

      if (!digitalRead(HAT_RIGHT)) {
        if (VERBOSE) Serial.println("[right]");
        mouseX = 1;
      }

      if (!digitalRead(HAT_CLICK)) {
        if (VERBOSE) Serial.println("[click]");
        Mouse.click();
      }

      if (mouseX != 0 || mouseY != 0) {
        Mouse.move(mouseX, mouseY);
      }
    }

    if (HAT_AS_ADJUSTER) {
      if (button_up.fallingEdge()) {
        update_interval += 250;

        if (VERBOSE) {
          Serial.print("updating every ");
          Serial.print(update_interval);
          Serial.println("ms");
        }
      }

      if (button_down.fallingEdge()) {
        update_interval -= 250;

        if (VERBOSE) {
          Serial.print("updating every ");
          Serial.print(update_interval);
          Serial.println("ms");
        }
      }

      if (update_interval < 0) update_interval = 0;
    }
  }
}
