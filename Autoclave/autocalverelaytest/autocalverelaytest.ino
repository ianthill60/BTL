//Ian Hill,Tanner Zachem - Autoclave Cycler Code for Brain Tool Laboratory @ Duke University
//Data Logging is handled externally, the program will output a 1x2 vector in the form TEMPERATURE,PRESSURE to the serial montior to be interpreted
//Declare constants for program as well as input and output pins
//----------------------------------------------------------------
//relay control
const int dRelay = 6; //drain | low = closed | high = open
const int pRelay = 7; //power | high = on | low = off
const int fRelay = 8; //fill | high = closed | low = open
const int rRelay = 9; //release steam | high = closed | low = open
//cycle parameters
const int cycleTemp = 110; //Temp to start a cycle at
const int solenoidSafteyTemp = 70; //Max temp at which it is safe to open the solenoids
const long duration = 1500000; //Duration of cycle in milliseconds (15 minutes)
const long solenoidActiveTime = 120000; //Duration of time for solenoids to be active for in milliseconds (2 minutes)
const int delayStep = 1000; //This value determines after how many 1ms delays do you want data printed
int numberCycles = 0; //number of cycles run
const int maxCycles = 2;
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
//Returns the average of N samples from the thermistor and uses the Steinhart-Hart Equation for conversion
//Return value is in degrees celcius and has been cross referenced with external measurments


//Setup Function
//Assign input and HIGHoutput pins
void setup() {
  Serial.begin(9600);
  pinMode(dRelay, OUTPUT); //Solenoid Relay is an output
  pinMode(pRelay, OUTPUT); //120V AC power to autoclave heating element is an output
  pinMode(fRelay, OUTPUT);
  pinMode(rRelay, OUTPUT);
  analogReference(EXTERNAL); //All analog measurments use the voltage applied to AREF pin (EXTERNAL) as the reference voltage. 3.3V is applied to AREF.
  digitalWrite(dRelay,LOW);
  digitalWrite(pRelay,LOW);
  digitalWrite(fRelay,HIGH);
  digitalWrite(rRelay,HIGH);
 
}
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
//Loop Function to be executed while active
//Note, for clarity all function calls are made seperatly so loop only contains a basic call to the main cycle Function
void loop() {
    Serial.print(getTemp());
//  digitalWrite(dRelay,HIGH);
// digitalWrite(rRelay,LOW);
// digitalWrite(fRelay,LOW);
// delay(15000);
// digitalWrite(fRelay,HIGH);
// digitalWrite(rRelay,LOW);
// delay(3000);
// digitalWrite(rRelay,HIGH);
}
