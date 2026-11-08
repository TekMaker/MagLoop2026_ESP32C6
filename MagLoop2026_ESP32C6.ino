
/*
  MagLoopESP32C6
  does not use a driver, DRV8255 driver optional
  
  sleep - 18
  drive - 19
  drive3 1/8 step - 20
  drive2 1/4 step - 21
  drive1 1/2 step - 22 

*/

// #include "DRV8825.h" // DRV8825
// DRV8825 stepper;
//A4988 stepper;

// Motor Drive for Nema 4 wire motor with DRV8825 driver
#define stepPin 13
#define dirPin 12
#define M0 22// full step
#define M1 21 // 1/2 step
#define M2 20 // 1/4 step
#define SLP 18 // wake motor driver, saves heat on the board and power consumption
#define RST 19
#define T 10 // delay time
//#define enblPin 11 

int stepsPerRevolution = 200;
int countsPerHalfRev = stepsPerRevolution / 2;
int curPos = 0;
int stepSize = 1;

//////////////////////////////////////////////////////////////////////////////
///////  Digital encoder   ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define pinA 4
#define pinB 5
#define pinSW 6

// position monitoring
int currentPos = 0;
int requiredPos = 0;
int16_t bounceMsec = 150;
uint32_t msec = 0;

// keep the states of A and B inputs
int curStateA = 1;
int prevStateA = 1;
int curStateB = 1;
bool stateA = true;
bool stateB = false;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  /// DRV8825 driver only
  // pinMode(M0, OUTPUT);
  // pinMode(M1, OUTPUT);
  // pinMode(M2, OUTPUT);
  pinMode(SLP, OUTPUT);
  pinMode(RST, OUTPUT);

/// motor stepping control

  /// DRV8825 driver only
  // digitalWrite(M0 , LOW); // 1/2 step
  // digitalWrite(M1 , LOW); // 1/4 step
  // digitalWrite(M2 , LOW); // 1/8 step
  
  digitalWrite(RST, HIGH);
  digitalWrite(SLP, HIGH);// until needed to save power 
  
  Serial.println("magLoop ESP32 C6 tester started");

  // init motor
  // DriveMotor(stepsPerRevolution, HIGH);// CW
  // delay(2000);
  // DriveMotor(stepsPerRevolution, LOW); // CCW
  // delay(2000);  
  
  
  
// motor control vars
  pinMode(dirPin, OUTPUT); // direction
  pinMode(stepPin, OUTPUT);  // direction
  /// DRV8825 driver only
  digitalWrite(M0 , LOW); // 1/2 step
  digitalWrite(M1 , LOW); // 1/4 step
  digitalWrite(M2 , LOW); // 1/8 step
  
  digitalWrite(RST, LOW); // perform reset to DRV8825
  delay(1000);
  digitalWrite(RST, HIGH);// reset finished  
  //digitalWrite(SLP, HIGH);// until needed to save power 
  

  // initialize motor by taking end to end
  Serial.println("Init Motor");
  DriveMotor(stepsPerRevolution, HIGH); 
  delay(2000);
  DriveMotor(stepsPerRevolution, LOW); 
  delay(2000);    
}

// the loop function runs over and over again forever
void loop() {

// limit currentPos to acceptable range
  if (requiredPos < 0) {
    requiredPos = 0;
    currentPos = 0;
  }
  if (requiredPos > countsPerHalfRev) {
    requiredPos = countsPerHalfRev;
    currentPos = countsPerHalfRev;
  }

  getEncoderPosition();
  if (currentPos >= countsPerHalfRev) currentPos = countsPerHalfRev;
  if (currentPos <= 0) currentPos = 0;


  if (currentPos < requiredPos) {
    DriveMotorPulse(HIGH);
    currentPos += stepSize;
  }
  if (currentPos > requiredPos) {
    DriveMotorPulse(LOW);
    currentPos += stepSize;
  }
//   DriveMotor(stepsPerRevolution, HIGH);
//   delay(2000);
//   DriveMotor(stepsPerRevolution, LOW);
//   delay(2000);
 }


// drive motor number of steps, in direction
void DriveMotor(int _stepCount, bool _direction)
{
  digitalWrite(SLP, HIGH);  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print(" steps=");
  Serial.println(_stepCount);  
  digitalWrite(dirPin, _direction);
 
  for (int i = 0; i < _stepCount; i++) 
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
    if(_direction)
    {
      curPos = curPos + 1;
    }
    else{
      curPos = curPos - 1;
    }
    Serial.print("Pos");
    Serial.println (curPos);
    delay(T);
  }

  digitalWrite(SLP, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}

void getEncoderPosition() {
  bool a = digitalRead(pinA);
  bool b = digitalRead(pinB);

  // this is a stateful debounce to overcome bad contact in digital encoder
  if (stateA != a && millis() > (msec + bounceMsec)) {
    if (stateB != a) {
      currentPos--;
      Serial.println(currentPos);
      DriveMotorPulse(HIGH);
    } else {
      currentPos++;
      Serial.println(currentPos);
      DriveMotorPulse(LOW);
    }

    msec = millis();
  }

  stateA = a;
  stateB = b;
}


void DriveMotorPulse(bool _direction)
{
    digitalWrite(dirPin, _direction);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
}

// -- END OF FILE --