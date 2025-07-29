#include <Arduino.h>
#include <IRremote.h>
#include <Wire.h>

float receivedValue;
int RECV_PIN=2;
String expression;
String inter;
float floatValue;
int mappedValue;
byte binaryValue;

IRrecv irrecv(RECV_PIN);

decode_results results;


String b="a70";
String enter;

  const int MAX_SIZE = 50; 
  const int MAX_EXPRESSION_LENGTH = 50; // Maximum length of the expression

  template<typename T>
  class Stack {
  private:
      T data[MAX_SIZE]; // Array to store stack elements
      int top; // Index of the top element

  public:
      Stack() : top(-1) {} // Constructor to initialize an empty stack

      bool isEmpty() {
          return top == -1; // Stack is empty if top is -1
      }

      bool isFull() {
          return top == MAX_SIZE - 1; // Stack is full if top is equal to the maximum size
      }

      void push(T element) {
          if (isFull()) {
              Serial.println("Stack Overflow! Cannot push element.");
              return;
          }
          data[++top] = element; // Increment top and insert element into the stack
      }

      T pop() {
          if (isEmpty()) {
              Serial.println("Stack Underflow! Cannot pop element.");
              return T(); // Return default value of type T
          }
          return data[top--]; // Decrement top and return the element
      }

      T peek() {
          if (isEmpty()) {
              Serial.println("Stack is empty! No top element.");
              return T(); // Return default value of type T
          }
          return data[top]; // Return the top element without removing it
      }
  };

  Stack<float> operators; // Stack for operators (now for float numbers)
  char postfix[MAX_EXPRESSION_LENGTH]; // Array to store postfix expression
  int postfixIndex = 0; // Index for postfix expression array

  // Precedence function with caret operator
  int precedence(char operate) {
      if (operate == '^') // Higher precedence for caret operator
          return 4;
      else if (operate == '*' || operate == '/')
          return 3;
      else if (operate == '+' || operate == '-')
          return 2;
      else if (operate == '(')
          return 1;
      else
          return 0;
  }

  // Function to check if character is an operand
  bool isOperand(char x) {
      return (x >= '0' && x <= '9') || x == '.'; // Allow digits and decimal point
  }

  // Function to convert infix to postfix
  void InfixtoPostfix(String s) {
      int length = s.length();
      postfixIndex = 0; // Reset postfix index

      for (int i = 0; i < length; i++) {
          char c = s[i];
          if (isOperand(c)) {
              postfix[postfixIndex++] = c;
              // Check for multi-digit numbers or floats
              while (i + 1 < length && (isOperand(s[i + 1]) || s[i + 1] == '.')) {
                  postfix[postfixIndex++] = s[++i];
              }
              postfix[postfixIndex++] = ' '; // Add space after each operand
          } else if (c == '(') {
              operators.push(c);
          } else if (c == ')') {
              while (!operators.isEmpty() && operators.peek() != '(') {
                  postfix[postfixIndex++] = operators.pop();
                  postfix[postfixIndex++] = ' '; // Add space after each operator
              }
              operators.pop(); // Pop '(' from stack
          } else { // Operator
              while (!operators.isEmpty() && precedence(operators.peek()) >= precedence(c)) {
                  postfix[postfixIndex++] = operators.pop();
                  postfix[postfixIndex++] = ' '; // Add space after each operator
              }
              operators.push(c);
          }
      }

      // Pop remaining operators from stack
      while (!operators.isEmpty()) {
          postfix[postfixIndex++] = operators.pop();
          postfix[postfixIndex++] = ' '; // Add space after each operator
      }
  }

  // Function to evaluate the postfix expression
  float evaluate() {
    char buffer[10];
    bool dataReceived = false;
      Stack<float> operands; // Stack for operands (now for float numbers)

      char* endPtr; // Pointer for strtod function

      for (int i = 0; i < postfixIndex; i++) {
          char c = postfix[i];
          if (isOperand(c)) {
              // Convert multi-digit numbers or floats to float
              float num = atof(&postfix[i]);
              operands.push(num);
              while (isOperand(postfix[i]) || postfix[i] == '.') {
                  i++; // Skip the rest of the number
              }
              i--; // Adjust index
          } else if (c != ' ') { // Skip spaces
              // Operator
              float operand2 = operands.pop();
              float operand1 = operands.pop();
              switch (c) {
                  case '+':
                      Serial.println("plus");
                      //Serial.println("plus");
                      Serial.println(operand1);
                      Serial.println(operand2);
                      floatValue = operand1; // Change this to the desired float value within the range -12.00 to 12.00
                      mappedValue = map(floatValue * 100, -1200, 1200, 0, 255); // Map the float value to the range 0 to 255
                      binaryValue = byte(mappedValue); // Convert the mapped integer value to a byte (8-bit) value
                      buffer[10];
                      dtostrf(operand2, 6, 2, buffer);
                      Serial.println(buffer);
                      Wire.beginTransmission(9);  // Start communication with slave at address 9
                      Wire.write(buffer);        // Send data to slave
                      Wire.endTransmission();
                      Wire.beginTransmission(7);
                      Wire.write("1");
                      Wire.endTransmission();
                      digitalWrite(3,LOW);
                      digitalWrite(4,LOW);
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
                      delay(200);
                      //Wire.requestFrom(SLAVE_ADDRESS, 2); // Request 2 bytes of data from slave Arduino
                      dataReceived = false;
                      if (!dataReceived) { // Check if data has not been received yet
                        float receivedValue = 0.0;

                        Wire.requestFrom(7, sizeof(float)); // Request bytes of data from slave Arduino

                        if (Wire.available()) { // Check if data is available
                          byte buffer[sizeof(float)];
                          for (int i = 0; i < sizeof(float); i++) {
                            buffer[i] = Wire.read(); // Read bytes into buffer
                          }
                          memcpy(&receivedValue, buffer, sizeof(float)); // Convert bytes back to float

                          Serial.print("Received Value: ");
                          Serial.println(receivedValue);
                          operands.push(receivedValue);
      
                          dataReceived = true; // Set flag to indicate that data has been received

                        }

                      }
                      break;
                  case '-':
                      Serial.println("sub");
                      //Serial.println("plus");
                      Serial.println(operand1);
                      Serial.println(operand2);
                      floatValue = operand1; // Change this to the desired float value within the range -12.00 to 12.00
                      mappedValue = map(floatValue * 100, -1200, 1200, 0, 255); // Map the float value to the range 0 to 255
                      binaryValue = byte(mappedValue); // Convert the mapped integer value to a byte (8-bit) value
                      buffer[10];
                      dtostrf(operand2, 6, 2, buffer);
                      Serial.println(buffer);
                      Wire.beginTransmission(9);  // Start communication with slave at address 9
                      Wire.write(buffer);        // Send data to slave
                      Wire.endTransmission();
                      Wire.beginTransmission(7);
                      Wire.write("1");
                      Wire.endTransmission();
                      digitalWrite(3,LOW);
                      digitalWrite(4,HIGH);
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
                      delay(200);
                      //Wire.requestFrom(SLAVE_ADDRESS, 2); // Request 2 bytes of data from slave Arduino
                      dataReceived = false;
                      if (!dataReceived) { // Check if data has not been received yet
                        float receivedValue = 0.0;

                        Wire.requestFrom(7, sizeof(float)); // Request bytes of data from slave Arduino

                        if (Wire.available()) { // Check if data is available
                          byte buffer[sizeof(float)];
                          for (int i = 0; i < sizeof(float); i++) {
                            buffer[i] = Wire.read(); // Read bytes into buffer
                          }
                          memcpy(&receivedValue, buffer, sizeof(float)); // Convert bytes back to float

                          Serial.print("Received Value: ");
                          Serial.println(receivedValue);
                          operands.push(receivedValue);
      
                          dataReceived = true; // Set flag to indicate that data has been received

                        }

                      }
                      break;
                  case '*':
                      Serial.println("mul");
                      //Serial.println("plus");
                      Serial.println(operand1);
                      Serial.println(operand2);
                      floatValue = operand1; // Change this to the desired float value within the range -12.00 to 12.00
                      mappedValue = map(floatValue * 100, -1200, 1200, 0, 255); // Map the float value to the range 0 to 255
                      binaryValue = byte(mappedValue); // Convert the mapped integer value to a byte (8-bit) value
                      buffer[10];
                      dtostrf(operand2, 6, 2, buffer);
                      Serial.println(buffer);
                      Wire.beginTransmission(9);  // Start communication with slave at address 9
                      Wire.write(buffer);        // Send data to slave
                      Wire.endTransmission();
                      Wire.beginTransmission(7);
                      Wire.write("1");
                      Wire.endTransmission();
                      digitalWrite(3,HIGH);
                      digitalWrite(4,LOW);
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
                      delay(200);
                      //Wire.requestFrom(SLAVE_ADDRESS, 2); // Request 2 bytes of data from slave Arduino
                      dataReceived = false;
                      if (!dataReceived) { // Check if data has not been received yet
                        float receivedValue = 0.0;

                        Wire.requestFrom(7, sizeof(float)); // Request bytes of data from slave Arduino

                        if (Wire.available()) { // Check if data is available
                          byte buffer[sizeof(float)];
                          for (int i = 0; i < sizeof(float); i++) {
                            buffer[i] = Wire.read(); // Read bytes into buffer
                          }
                          memcpy(&receivedValue, buffer, sizeof(float)); // Convert bytes back to float

                          Serial.print("Received Value: ");
                          Serial.println(receivedValue);
                          operands.push(receivedValue);
      
                          dataReceived = true; // Set flag to indicate that data has been received

                        }

                      }
                      break;
                  case '/':
                      Serial.println("nth root");
                      Serial.println(operand1);
                      Serial.println(operand2);
                      floatValue = operand1; // Change this to the desired float value within the range -12.00 to 12.00
                      mappedValue = map(floatValue * 100, -1200, 1200, 0, 255); // Map the float value to the range 0 to 255
                      binaryValue = byte(mappedValue); // Convert the mapped integer value to a byte (8-bit) value
                      buffer[10];
                      dtostrf(operand2, 6, 2, buffer);
                      Serial.println(buffer);
                      Wire.beginTransmission(9);  // Start communication with slave at address 9
                      Wire.write(buffer);        // Send data to slave
                      Wire.endTransmission();
                      Wire.beginTransmission(7);
                      Wire.write("1");
                      Wire.endTransmission();
                      digitalWrite(3,HIGH);
                      digitalWrite(4,HIGH);
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
                      delay(200);
                      //Wire.requestFrom(SLAVE_ADDRESS, 2); // Request 2 bytes of data from slave Arduino
                      dataReceived = false;
                      if (!dataReceived) { // Check if data has not been received yet
                        float receivedValue = 0.0;

                        Wire.requestFrom(7, sizeof(float)); // Request bytes of data from slave Arduino

                        if (Wire.available()) { // Check if data is available
                          byte buffer[sizeof(float)];
                          for (int i = 0; i < sizeof(float); i++) {
                            buffer[i] = Wire.read(); // Read bytes into buffer
                          }
                          memcpy(&receivedValue, buffer, sizeof(float)); // Convert bytes back to float

                          Serial.print("Received Value: ");
                          Serial.println(receivedValue);
                          operands.push(receivedValue);
      
                          dataReceived = true; // Set flag to indicate that data has been received

                        }

                      }
                      break;
  //                 case '^':
  //                     Serial.println(operand1);
  //                     Serial.println(operand2);
  //                     floatValue = operand1; // Change this to the desired float value within the range -12.00 to 12.00
  //                     mappedValue = map(floatValue * 100, -1200, 1200, 0, 255); // Map the float value to the range 0 to 255
  //                     binaryValue = byte(mappedValue); // Convert the mapped integer value to a byte (8-bit) value
  //                     buffer[10];
  //                     dtostrf(operand2, 6, 2, buffer);
  //                     Serial.println(buffer);
  //                     Wire.beginTransmission(9);  // Start communication with slave at address 9
  //                     Wire.write(buffer);        // Send data to slave
  //                     Wire.endTransmission();
  //                     Wire.beginTransmission(7);
  //                     Wire.write("1");
  //                     Wire.endTransmission();
  //                     digitalWrite(3,LOW);
  //                     digitalWrite(4,LOW);
  //                     delay(200);

                      
  // // Output the binary representation to digital pins 13 to 6
  //                     digitalWrite(13, (binaryValue >> 7) & 1);
  //                     digitalWrite(12, (binaryValue >> 6) & 1);
  //                     digitalWrite(11, (binaryValue >> 5) & 1);
  //                     digitalWrite(10, (binaryValue >> 4) & 1);
  //                     digitalWrite(9, (binaryValue >> 3) & 1);
  //                     digitalWrite(8, (binaryValue >> 2) & 1);
  //                     digitalWrite(7, (binaryValue >> 1) & 1);
  //                     digitalWrite(6, binaryValue & 1);
  //                     Serial.println(binaryValue);
  //                     delay(200);
  //                     //Wire.requestFrom(SLAVE_ADDRESS, 2); // Request 2 bytes of data from slave Arduino
  //                     dataReceived = false;
  //                     if (!dataReceived) { // Check if data has not been received yet
  //                       float receivedValue = 0.0;

  //                       Wire.requestFrom(7, sizeof(float)); // Request bytes of data from slave Arduino

  //                       if (Wire.available()) { // Check if data is available
  //                         byte buffer[sizeof(float)];
  //                         for (int i = 0; i < sizeof(float); i++) {
  //                           buffer[i] = Wire.read(); // Read bytes into buffer
  //                         }
  //                         memcpy(&receivedValue, buffer, sizeof(float)); // Convert bytes back to float

  //                         Serial.print("Received Value: ");
  //                         Serial.println(receivedValue);
  //                         operands.push(receivedValue);
      
  //                         dataReceived = true; // Set flag to indicate that data has been received

  //                       }

  //                     }
  //                     break;
                  default:
                      // Handle unsupported operators
                      Serial.println("Unsupported operator!");
                      return 0.0;
              }
          }
      }
      return operands.pop();
  }


  void setup()
{
  String package;
  char buffer[10]; 
  Wire.begin();
  pinMode(13, OUTPUT); // Set digital pin 13 as output
  pinMode(12, OUTPUT); // Set digital pin 12 as output
  pinMode(11, OUTPUT); // Set digital pin 11 as output
  pinMode(10, OUTPUT); // Set digital pin 10 as output
  pinMode(9, OUTPUT);  // Set digital pin 9  as output
  pinMode(8, OUTPUT);  // Set digital pin 8  as output
  pinMode(7, OUTPUT);  // Set digital pin 7  as output
  pinMode(6, OUTPUT);  // Set digital pin 6  as output
  Serial.begin(9600);
  Serial.println("Enabling IRin");

  irrecv.enableIRIn();
  Serial.println("Enabled IRin");
  while(enter != "2ce9"){
  if (enter != "2ce9"){
  if(irrecv.decode(&results)){
    String a = String(results.value, HEX);
    if (a=="2ce9"){
      
    }

    if(a.length()<=5 && (b=="a70")) 
    {
      Serial.println(a);
      inter = "";
      if (a != "a70"){
          if (a == "10"){
            inter = "1";
          }
          if (a == "810"){
            inter = "2";
          }
          if (a == "410"){
            inter = "3";
          }
          if (a == "c10"){
            inter = "4";
          }
          if (a == "210"){
            inter = "5";
          }
          if (a == "a10"){
            inter = "6";
          }
          if (a == "610"){
            inter = "7";
          }
          if (a == "e10"){
            inter = "8";
          }
          if (a == "110"){
            inter = "9";
          }
          if (a == "910"){
            inter = "0";
          }
          if (a == "fd0"){
            inter = ".";
          }
          if (a == "7123"){
            inter = "(";
          }
          if (a == "1f58"){
            inter = ")";
          }
          if (a == "1ee9"){
            inter = "-";
          }
          if (a == "4ce9"){
            inter = "*";
          }
          if (a == "5ee9"){
            inter = "+";
          }
          if (a == "ce9"){
            inter = "/";
          }
          if (a == "e90"){
            if (expression.length()> 0){
              expression.remove(expression.length() - 1);
          }
          }
          expression = expression + inter;
          Serial.println(expression);
          enter = a;
      }
      
      
    }
    if(a.length()<=5) 
    {
      b=a;
    }
    irrecv.resume();
    //Serial.println("check");
    
  } 
  
  }
  
}


Serial.println("Expression complete");
Serial.println(expression);

Serial.println(expression);

          InfixtoPostfix(expression);

          // Output the postfix expressionression
          Serial.print("Postfix expression: ");
          for (int i = 0; i < postfixIndex; i++) {
              Serial.print(postfix[i]);
              Serial.print(" ");
          }
          Serial.println();

          // Evaluate the postfix expression
          float result = evaluate();
          Serial.print("Result: ");
          Serial.println(result);
}


void loop() {
     
  }

