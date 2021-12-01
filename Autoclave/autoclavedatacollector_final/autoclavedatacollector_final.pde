//Ian Hill, Tanner Zachem - Autoclave Cycler Data Aquisition Code for Brain Tool Laboratory @ Duke University
//This file will read the serial port at baud rate 9600 to collect the 1x2 vector (TEMPERATURE,PRESSURE) and log into a csv file

import processing.serial.*;
Serial myPort; //Serial port to read data
Table data; //Create a table to hold values
String fileName; //need to declare filename string to save CSV
int numReadings = 100; //number of readings before writing to CSV
long readingCounter = 0; //number of readings taken
void setup(){
//Setup Serial Communicaiton
data = new Table();
print(Serial.list());
String portName = Serial.list()[0]; //This will be changed when arduino is connected and handshake is established. Create handshake by priting the serial list to determine apporpriate port
myPort = new Serial(this, "COM3", 9600); //Instance of myPort, read data from here
//Setup data table
data.addColumn("id"); //Create a unique id for each data entry starting at 0
//Add Timestamps to data
data.addColumn("year");
data.addColumn("month");
data.addColumn("day");
data.addColumn("hour");
data.addColumn("minute");
data.addColumn("second");
data.addColumn("millisecond");
//add column for each data entry
data.addColumn("Temperature");
}
void serialEvent(Serial myPort){
try{
  String val = myPort.readStringUntil('\n'); //obtain value from serial port
  //make sure value is not null
  if(val==null){return;}
  else if(val!=null){
    val=trim(val); //remove whitespace if any exists
    println(val); //use to debug, comment out is desired
    //create array of values to add to csv
    float sensorVal = float(val);
    //add new sensor readings to data table
    TableRow newRow = data.addRow();
    newRow.setInt("id",data.lastRowIndex()); //add unique id to col 1
    //add time stamp
    newRow.setInt("year",year());
    newRow.setInt("month",month());
    newRow.setInt("day",day());
    newRow.setInt("hour",hour());
    newRow.setInt("second",second());
    newRow.setInt("millisecond",millis());
    //add in temp & pressure readings
    newRow.setFloat("Temperature",sensorVal);
    readingCounter++; //update so the program knows when to save the file
    //check to see if it is time to save the file
    if(readingCounter % numReadings == 0){
      fileName = data.lastRowIndex()+"10292020data.csv"; //create filename indexed by YYYY/MM/DD last data index
      saveTable(data,fileName);
    } 
  }
}
catch(RuntimeException e){e.printStackTrace();}}

//data visualization is handeled externally, but void draw is a required part of a processing file so it will be left blank
void draw(){
}
