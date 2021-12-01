int a_s0 = 2;
int a_s1 = 3;
int a_s2 = 4;
int a_s3 = 5;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 pinMode(a_s0, OUTPUT); 
 pinMode(a_s1, OUTPUT); 
 pinMode(a_s2, OUTPUT); 
 pinMode(a_s3, OUTPUT); 
}


int readMux(int channel){
 int controlPin[] = {a_s0, a_s1, a_s2, a_s3};
 int muxChannel[7][4]={ //Change this reflect the number of antennas to loop through
   {0,0,0,0}, //channel 1
   {1,0,0,0}, //channel 2
   {0,1,0,0}, //channel 3
   {1,1,0,0}, //channel 4
   {0,0,1,0}, //channel 5
   {1,0,1,0}, //channel 6
   {0,1,1,0}, //channel 7
 };
 
 //loop through the 4 sig
 digitalWrite(controlPin[0], muxChannel[channel][0]);
 digitalWrite(controlPin[1], muxChannel[channel][1]);
 digitalWrite(controlPin[2], muxChannel[channel][2]);
 digitalWrite(controlPin[3], muxChannel[channel][3]);

}

void loop(){
  if (Serial.available() > 0) {
    int ant = Serial.read(); //Change Matlab to write correct number of antennas to serial port. 
    readMux(ant);
  }
}
