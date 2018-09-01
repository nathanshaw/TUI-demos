import processing.serial.*;
Serial myPort;  // Create object from Serial class

void connectToArduino(int port) {
  for (int i = 0; i < Serial.list().length; i++){
    println(i, ":", Serial.list()[i]);
  }
  String portName = Serial.list()[port];
  myPort = new Serial(this, portName, 115200);
}