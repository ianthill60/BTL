
#include <AccelStepper.h>

const int stepsPerRevolution = 2048;
const int minNumRevs = 1000;
const int maxNumRevs = 5000;
int s1Pos;
int s2Pos;
int s3Pos;
AccelStepper s1(AccelStepper::FULL4WIRE, 2,3,4,5);
AccelStepper s2(AccelStepper::FULL4WIRE, 6,7,8,9);
AccelStepper s3(AccelStepper::FULL4WIRE, 10,11,12,13);


void setup() {
  Serial.begin(9600);
  // Configure each stepper
  s1.setCurrentPosition(0);
  s2.setCurrentPosition(0);
  s3.setCurrentPosition(0);
  s1.setMaxSpeed(800);
  s2.setMaxSpeed(800);
  s3.setMaxSpeed(800);
  s1.setAcceleration(100.0);
  s2.setAcceleration(100.0);
  s3.setAcceleration(100.0);
  randomSeed(analogRead(0));
}

void loop() {
  delay(10000);

  s1Pos = stepsPerRevolution*random(minNumRevs, maxNumRevs)/1000;
  s2Pos = stepsPerRevolution*random(minNumRevs, maxNumRevs)/1000;
  s3Pos = stepsPerRevolution*random(minNumRevs, maxNumRevs)/1000;
  
  s1.moveTo(-s1Pos);
  s2.moveTo(-s2Pos);
  s3.moveTo(-s3Pos);
  s1.runToPosition();
  s2.runToPosition();
  s3.runToPosition();

  Serial.print("s1");
  Serial.print(s1Pos);
  Serial.print("s2");
  Serial.print(s2Pos);
  Serial.print("s3");
  Serial.print(s3Pos);
  Serial.println();
  
  delay(122000);
  Serial.print("0");
  Serial.println();
  // Move back to original position:
  s1.moveTo(0);
  s2.moveTo(0);
  s3.moveTo(0);

  s1.runToPosition();
  s2.runToPosition();
  s3.runToPosition();

}
