#include <Wire.h>

#define BUFFER_SIZE 32  // Define buffer size for received string

char receivedString[BUFFER_SIZE];  // Buffer to store received string
boolean newData = false;           // Flag to indicate new data received

void setup() {
  Wire.begin(9);        // Initialize I2C communication with slave address 9
  Wire.onReceive(receiveEvent);  // Set up receive handler
  Serial.begin(9600);  
  pinMode(13, OUTPUT); // Set digital pin 13 as output
  pinMode(12, OUTPUT); // Set digital pin 12 as output
  pinMode(11, OUTPUT); // Set digital pin 11 as output
  pinMode(10, OUTPUT); // Set digital pin 10 as output
  pinMode(9, OUTPUT);  // Set digital pin 9  as output
  pinMode(8, OUTPUT);  // Set digital pin 8  as output
  pinMode(7, OUTPUT);  // Set digital pin 7  as output
  pinMode(6, OUTPUT); // Initialize serial communication
  Serial.println("Check");
}

void loop() {
  if (newData) {
    // Print received string
    Serial.print("Received data");
    Serial.println(receivedString);
    String str = String(receivedString);
    float operand2 = str.toFloat();
    Serial.println(operand2);
    float floatValue = operand2; // Change this to the desired float value within the range -12.00 to 12.00
    int mappedValue = map(floatValue * 100, -1200, 1200, 0, 255); // Map the float value to the range 0 to 255
    byte binaryValue = byte(mappedValue); // Convert the mapped integer value to a byte (8-bit) value
    delay(200);
  // Output the binary representation to digital pins 13 to 6
    digitalWrite(13, (binaryValue >> 7) & 1);
    digitalWrite(12, (binaryValue >> 6) & 1);
    digitalWrite(11, (binaryValue >> 5) & 1);
    digitalWrite(10, (binaryValue >> 4) & 1);
    digitalWrite(9, (binaryValue >> 3) & 1);
    digitalWrite(8, (binaryValue >> 2) & 1);
    digitalWrite(7, (binaryValue >> 1) & 1);
    digitalWrite(6, binaryValue & 1);
    Serial.println(binaryValue);
    
    // Reset flag
    newData = false;
  }
  
  delay(100);
  
}

void receiveEvent(int bytes) {
  int i = 0;
  while (Wire.available() && i < BUFFER_SIZE - 1) {
    receivedString[i] = Wire.read();  // Read data from master
    i++;
  }
  receivedString[i] = '\0';  // Null-terminate the string
  newData = true;  // Set flag to indicate new data received
}