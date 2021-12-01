//Ian Hill,Tanner Zachem - Autoclave Cycler Code for Brain Tool Laboratory @ Duke University
//Data Logging is handled externally, the program will output a 1x2 vector in the form TEMPERATURE,PRESSURE to the serial montior to be interpreted
//Declare constants for program as well as input and output pins
//----------------------------------------------------------------
//relay control

const int dRelay = 6; //drain | high = open | low = closed
const int pRelay = 7; //power | high = on | low = off
const int fRelay = 8; //fill | high = closed | low = open
const int rRelay = 9; //release steam | high = closed | low = open

//cycle parameters
const int fillTime = 15000; //fill time 15 seconds
const int numFills = 8; //run through 8 fills for each cycle
const int cycleTemp = 110; //Temp to start counting a cycle at
const int solenoidSafteyTemp = 100; //Max temp at which it is safe to open the solenoids
const long duration = 900000; //Duration of cycle in milliseconds (15 minutes)
const long drainTime = 30000; //Duration of time for draining in milliseconds (30 seconds)
const int delayStep = 1000; //This value determines after how many 1ms delays do you want data printed
int numberCycles = 0; //number of cycles run
const int maxCycles = 6;
int dangerFlag = 1; //logic for if it surpassed 140 C
//thermistor setup
#define THERMISTORPIN A1 //read input from pin 1
#define THERMISTORNOMINAL 10000 //from datasheet, for calibration
#define TEMPERATURENOMINAL 25 //from datasheet, for calibration
#define NUMSAMPLES 10 //number of measurments to sample together
#define BCOEFFICIENT 3950 //from datasheet, for calibration
#define SERIESRESISTOR  10000 //resistor placed in series 10k used to minimzie self heating
int samples[NUMSAMPLES]; //array to hold samples
//pressure calculation Setup

//option for robust serial outputs for debugging, must be false for data collection
bool robust = false;
//----------------------------------------------------------------
//Cycle Helper functions
//----------------------------------------------------------------
//Thermistor Read Function
//Takes 1 second
//Returns the average of N samples from the thermistor and uses the Steinhart-Hart Equation for conversion
//Return value is in degrees celcius and has been cross referenced with external measurments
float getTemp() {
  uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTORPIN);
    delay(100);
  }
  // average all the samples
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;
  //map to an analog signal
  average = 1023 / average - 1;
  //voltage division
  average = SERIESRESISTOR / average;
  //apply S-H equation, equation steps are noted inline
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;// convert to C from kelvin
  return (steinhart);
}

//fill Function
//open fill and release solenids for amount of time determined by the variables numFills and fillTime 
//This is only called if system is below saftey temp
void fill() {
  digitalWrite(pRelay, LOW); //power off
  for (int num = 1; num < numFills; num++) {
    digitalWrite(fRelay,LOW); //open fill solenoid 15 seconds
    delay(15000); 
    digitalWrite(fRelay,HIGH); //close fill solenoid
    digitalWrite(rRelay,LOW); //open bleed solenoid 3 seconds to equalize pressure
    delay(3000);
    digitalWrite(rRelay,HIGH); //close bleed solenoid
    Serial.println(getTemp()); //print for data collection
  }
  digitalWrite(fRelay, HIGH); //close fill solenoid
  digitalWrite(rRelay, HIGH); //close bleed solenoid
  return;
}

//Pre Cycle function
//Gets the autoclave up to temp before a cycle begins
void preCycle() {
  float temp;
  while (getTemp() < cycleTemp) {
     digitalWrite(pRelay, HIGH); //Give the autovlave heating element power
     temp = getTemp();
     if (robust == true) {
       Serial.print(" Warming up, Current Temp: ");
       Serial.println(temp);//print temp for debugging
     }
     else {
       Serial.println(temp);//print temp to serial monitor for data logger
     }
   }
  return;
}

//At Temp function
//The autovclave is at temp, hold at this temp which is regulated by a steam release valve
void atTemp() {
  digitalWrite(pRelay, HIGH); //leave power for autovclave on for duraiton of cycle, steam valve will regulate temp
  for (int time = 0; time < duration / delayStep; time++) { 
    float temp = getTemp();
    Serial.println(temp);//print temp for data collection
    if(temp > 140){
      digitalWrite(pRelay, LOW);
      dangerFlag = 0;
      Serial.println("MAX TEMP REACHED, CUTTING POWER");
      }
    if (robust == true && time % 10 == 0) {
      Serial.println("In Cycle: " + time);
    }
  }
  return;
}

//Cool Down Function
//allow the autoclave to cool down passivley
void coolDown() {
  digitalWrite(pRelay, LOW); //turn power off
  int flag = 1; //logic for if to use bleed solenoid
  while (getTemp() > solenoidSafteyTemp) {
    if(flag == 1){
      for(int i = 0; i<10; i++){
        if(robust==true){
          Serial.println("bleeding");
        }
        digitalWrite(rRelay,LOW);//open bleed solenoid - pressure bursts on and off
        delay(200);
        digitalWrite(rRelay,HIGH);
        delay(200);
        }
      flag = 2; //first cycle is through, dont open bleed soleoid again unless this function is called again
    }
    if(robust==true){
      Serial.println("Cooling");
    }
    digitalWrite(rRelay,LOW);//open bleed solenoid for entire cooling session
    Serial.println(getTemp()); //print for data collection
  }
  digitalWrite(rRelay,HIGH);//close bleed solenoid after cool
  return;
}
//drain Function
//open drain and release solenids for amount of time determined by the variable drainTime (1 minute)
//This is only called if system is below saftey temp
void drain() {
  for (int time = 0; time < drainTime / delayStep; time++) {
    digitalWrite(pRelay, LOW); //power off
    digitalWrite(rRelay, LOW);//open bleed solenoid
    digitalWrite(dRelay, HIGH); //open drain solenoid
    Serial.println(getTemp()); //print for data collection
    //delay for timer
    delay(delayStep);
  }
  digitalWrite(dRelay, LOW); //close drain solenoids
  return;
}

//----------------------------------------------------------------
//Main Cycle Function, will cause the autoclave to run a cycle of duration dur as determiend in line 11 and a solenoid peroid of drainTime as determiend in line 12
//This function control the phases of the cycle and calls on children function to execute each phase
void cycleControl() {
  while(dangerFlag = 1){
    //get the device up to the temp required for a cycle to run
    if (getTemp() < solenoidSafteyTemp) {
      fill();
      if (getTemp() < cycleTemp) {
        preCycle();
        //the autoclave is at temp, proceed with a cycle
        atTemp();
        //finished the cycle, alllow autoclave to cool to room temp passivley
        coolDown();
        //autoclave is cool enough for solenoids to activate
        drain();
      }
    }
  }
}

//Setup Function
//Assign input and HIGHoutput pins
void setup() {
  pinMode(dRelay, OUTPUT); //drain Relay 
  pinMode(pRelay, OUTPUT); //120V AC power to autoclave heating element is an output
  pinMode(fRelay, OUTPUT); //fill Relay
  pinMode(rRelay, OUTPUT); //release Relay
  analogReference(EXTERNAL); //All analog measurments use the voltage applied to AREF pin (EXTERNAL) as the reference voltage. 3.3V is applied to AREF.
  //For saftey, immediately close and turn off power

  digitalWrite(pRelay,LOW);
  digitalWrite(dRelay,LOW);
  digitalWrite(fRelay,HIGH);
  digitalWrite(rRelay,HIGH);
  
  Serial.begin(9600); //Standard Baud Rate of 9600 has been selected
  //Robust Output if desired
  if (robust == true) {
    Serial.println("------------------------");
    Serial.println("Autoclave Cycler - BTL");
    Serial.println("Setup Complete");
    Serial.print("Current Temp in Autoclave is: ");
    Serial.print(getTemp());
    Serial.println(" degrees Celcius");
    Serial.println("Continuting To Cycle");
    Serial.println("------------------------");
  }
}

//Loop Function to be executed while active
//Note, for clarity all function calls are made seperatly so loop only contains a basic call to the main cycle Function
void loop() {
  if (robust == true) {
    Serial.print("In Loop, Calling Cycle with duration: ");
    Serial.print(duration);
    Serial.print(" and solenoid timer: ");
    Serial.println(drainTime);
  }
  if (numberCycles >= maxCycles) {
    Serial.println("Cycles Complete");
  }
  else {
    cycleControl();
    numberCycles++;
  }
}
