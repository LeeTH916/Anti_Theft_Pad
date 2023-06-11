import processing.serial.*;

Serial myPort;  // The serial port
int rows = 10;
int cols = 23;
int maxNumberOfSensors = rows*cols;     
float[] sensorValue = new float[maxNumberOfSensors];  // global variable for storing mapped sensor values
int rectSize = 2540/cols;
int averageMIN = 250;
int averageMAX = 600;
int first=0;
PImage TN_img;

void setup () { 
  size(2540, 1920);  // set up the window to whatever size you want
  TN_img = loadImage("Team_name.png");
  
  
  println(Serial.list());  // List all the available serial ports
  String portName = Serial.list()[2];
  myPort = new Serial(this, portName, 230400);
  myPort.clear();
  myPort.bufferUntil('\n');  // don't generate a serialEvent() until you get a newline (\n) byte
  background(255);    // set inital background
  smooth();  // turn on antialiasing
  rectMode(CORNER);
}


void draw () {
  // rotate the matrix so that the visualization aligns with the touchpad oriantation:
  pushMatrix();
  image(TN_img, 0, 0);
  
  // draw a rectangle for each sensor value and shade it grayscale
  for (int r=0; r<rows; r++) {
    for (int c=0; c<cols; c++) {
      fill(sensorValue[23 * r  + c]);
      rect(c * rectSize,r*rectSize+100, rectSize, rectSize);
    } // end for cols
  } // end for rows
  
  popMatrix();
}


void serialEvent (Serial myPort) {
  String inString = myPort.readStringUntil('\n');  // get the ASCII string

  if (inString != null) {  // if it's not empty
    inString = trim(inString);  // trim off any whitespace
    int incomingValues[] = int(split(inString, ","));  // convert to an array of ints

    if (incomingValues.length <= maxNumberOfSensors && incomingValues.length > 0) {
      for (int i = 0; i < incomingValues.length; i++) {
        // map the incoming values (0 to  1023) to an appropriate gray-scale range (0-255):
        sensorValue[i] = map(incomingValues[i], averageMIN, averageMAX, 0, 255);
        //println(sensorValue[i]);
      }
    }
  }
}
