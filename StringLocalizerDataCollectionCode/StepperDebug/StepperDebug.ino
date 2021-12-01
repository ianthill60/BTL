
#include <Stepper.h>
#include <AccelStepper.h>

const int stepsPerRevolution = 2048;
int Pos =0;
AccelStepper s1(AccelStepper::FULL4WIRE, 2,3,4,5);
AccelStepper s2(AccelStepper::FULL4WIRE, 6,7,8,9);
AccelStepper s3(AccelStepper::FULL4WIRE, 10,11,12,13);
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  s1.setCurrentPosition(0);
  s2.setCurrentPosition(0);
  s3.setCurrentPosition(0);
  s1.setMaxSpeed(800);
  s2.setMaxSpeed(800);
  s3.setMaxSpeed(800);
  s1.setAcceleration(100.0);
  s2.setAcceleration(100.0);
  s3.setAcceleration(100.0);
  s1.moveTo(Pos);
  s1.runToPosition();
  s2.moveTo(Pos);
  s2.runToPosition();
  s3.moveTo(Pos);
  s3.runToPosition();

}

void loop() {
//  int s1Pos = random(10, 2048*7);
//  int s2Pos = random(10, 2048*7);
//  int s3Pos = random(10, 2048*7);
//  Serial.print("s1");
//  Serial.print(s1Pos);
//  Serial.print("s2");
//  Serial.print(s2Pos);
//  Serial.print("s3");
//  Serial.print(s3Pos);
//  Serial.println();
}
