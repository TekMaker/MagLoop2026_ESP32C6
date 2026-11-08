#include <ESP8266WiFi.h>
//#include <WiFi.h>
// This Arduino example demonstrates bidirectional operation of a
// 28BYJ-48, using a VMA401 - ULN2003 interface board to drive the stepper.
// The 28BYJ-48 motor is a 4-phase, 8-beat motor, geared down by
// a factor of 68. One bipolar winding is on motor pins 1 & 3 and
// the other on motor pins 2 & 4. The step angle is 5.625/64 and the
// operating Frequency is 100pps. Current draw is 92mA.
////////////////////////////////////////////////

//declare variables for the motor pins
uint8_t motorPin1 = 5;                                                               // Blue - 28BYJ48 pin 1
uint8_t motorPin2 = 6;                                                               // Pink - 28BYJ48 pin 2
uint8_t motorPin3 = 7;                                                               // Yellow - 28BYJ48 pin 3
uint8_t motorPin4 = 8;                                                               // Orange - 28BYJ48 pin 4
                                                                                     // Red - 28BYJ48 pin 5 (VCC)
int motorSpeed = 3600;                                                               // 800 - 4800 defaultvariable to set stepper speed, ***higher is slower***
int lookup[8] = { B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001 };  // used for stepping the motor

//////////////////////////////////////////////////////////////////////////////
// Digital encoder
//////////////////////////////////////////////////////////////////////////////
int8_t pinA = 2;
int8_t pinB = 3;
int8_t pinSW = 4;

int16_t currentPos = 0;
int16_t requiredPos = 0;
int16_t bounceMsec = 5;
uint16_t limit = 2136;
uint16_t stepSize = 8;
uint16_t countsPerHalfRev = 264;

bool stateA = true;
bool stateB = false;
uint32_t msec = 0;

// WiFi credentials
const char* ssid = "VM1118113";
const char* password = "Kd3rrxmm9Cjw";


void get_network_info() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[*] Network information for ");
    Serial.println(ssid);
    Serial.println("[+] BSSID : " + WiFi.BSSIDstr());
    Serial.print("[+] Gateway IP : ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("[+] Subnet Mask : ");
    Serial.println(WiFi.subnetMask());
    Serial.println((String) "[+] RSSI : " + WiFi.RSSI() + " dB");
    Serial.print("[+] ESP32 IP : ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  // setup digital encoder
  pinMode(pinSW, INPUT_PULLUP);  // saves need for external pullup resistors
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);

  //declare the motor pins as outputs
  // pinMode(motorPin1, OUTPUT);
  // pinMode(motorPin2, OUTPUT);
  // pinMode(motorPin3, OUTPUT);
  // pinMode(motorPin4, OUTPUT);
  
 // initMotor();  //do this to run motor from end to end once to calibrate without using end stops as it won't hurt it.

  Serial.println("Motor Ready!");
  
  Serial.begin(115200);
//  delay(1000);

  //WiFi.persistent(false);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  get_network_info();
}

void loop() {

  while(true)
  {
    Serial.println("Connected");
    delay(1000);
  }

  // getEncoderPosition();

  // if (requiredPos < currentPos) {
  //   clockwise();
  // }
  // if (requiredPos > currentPos) {
  //   anticlockwise();
  // }
}

//////////////////////////////////////////////////////////////////////////////
// manage the digital encoder values
//////////////////////////////////////////////////////////////////////////////
void getEncoderPosition() {
  bool a = digitalRead(pinA);
  bool b = digitalRead(pinB);


  if (!digitalRead(pinSW)) {
    if (stepSize == 200) {
      stepSize = 8;
      Serial.println("Step 8");
      delay(500);
    } else {
      if (stepSize == 8) {
        stepSize = 40;
        Serial.println("Step 40");
        delay(500);
      }
    }
    return;
  }

  if (stateA != a && millis() > msec + bounceMsec) {
    if (stateB != a) {
      requiredPos += stepSize;
      if (requiredPos >= limit) {
        requiredPos = limit;
        delay(100);
      }
    } else {
       requiredPos -= stepSize;
      if (requiredPos < 0) {
        requiredPos = 0;
        delay(100);
      }
    }
    // limit requiredPos
    Serial.println(requiredPos);
    msec = millis();

    stateA = a;
    stateB = b;
  }
}


// run the motor from end to end and back. It is a stepper so it will not harm it and no limit switches are needed
// if it does not do 180 degrees then the value of countsPerHalfRev is wrong, refer to documentation for your specific motor
// you can reverse antoclocwise and clockwise statements to ensure the cap is closed
void initMotor() {

  Serial.println("Init ACW");
  for (int i = 0; i <= countsPerHalfRev; i++) {
    anticlockwise();
  }

  Serial.println("Init CW");
  for (int i = countsPerHalfRev; i > -5; i--) {
    clockwise();
  }
  currentPos = 0;
}

// close the cap to prevent dust getting on vanes
void shutdown() {
  for (int i = 0; i < countsPerHalfRev; i++) {
    anticlockwise();
  }
}

//////////////////////////////////////////////////////////////////////////////
// move the motor
//////////////////////////////////////////////////////////////////////////////
// set pins to ULN2003 high in sequence from 1 to 4
// delay "motorSpeed" between each pin setting (to determine speed)
void anticlockwise() {
  for (int i = 0; i < 8; i++) {
    setOutput(i);
    currentPos++;
    delayMicroseconds(motorSpeed);
  }
}

void clockwise() {
  for (int i = 7; i >= 0; i--) {
    setOutput(i);
    currentPos--;
    delayMicroseconds(motorSpeed);
  }
}

// drive the outputs sequentially as desired
void setOutput(int out) {
  digitalWrite(motorPin1, bitRead(lookup[out], 0));
  digitalWrite(motorPin2, bitRead(lookup[out], 1));
  digitalWrite(motorPin3, bitRead(lookup[out], 2));
  digitalWrite(motorPin4, bitRead(lookup[out], 3));
}

