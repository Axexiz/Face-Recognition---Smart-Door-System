#include <rgb_lcd.h>
#include <Servo.h>

Servo myservo;
rgb_lcd lcd;

const int trigPin = 13;
const int echoPin = 12;
const int trigPin2 = 11;
const int echoPin2 = 10;
const int LEDlampGreen = 7;
const int LEDlampRed = 6;
const int ButtonINPUT = 8;

#define soundbuzzer 5
#define T_C 262
#define T_D 294
#define T_E 330
#define T_F 349
#define T_G 392
#define T_A 440
#define T_B 493

long duration;
bool status = false; // Initialize status as disabled
bool previousButtonState = false;
String true_STATEMENT = "Activated";
int distance, sound = 500, true_COUNT = true_STATEMENT.length(), data;
bool disableButton = false; // Flag to disable button functionality temporarily

bool dataChanged = false; // Flag to track if data has changed

void setup() {
  Serial.begin(9600);
  myservo.attach(9);
  myservo.write(0);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setRGB(0, 0, 0);
  pinMode(ButtonINPUT, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(LEDlampRed, OUTPUT);
  pinMode(LEDlampGreen, OUTPUT);
  pinMode(soundbuzzer, OUTPUT);
}

//NOTE THAT STATUS TELLS YOU WHETHER THE SECURITY MECHANISM IS ENABLED OR DISABLED.
//IF STATUS = TRUE, ANY TRESPASSERS THAT TRIED TO BREAK THROUGH THE DOOR WILL SOUND AN ALARM
//IF STATUS = FALSE, SECUIRTY MECHANISM WILL BE DISABLED. 


void loop() {
  // Check for serial data
  while (Serial.available()) {
    data = Serial.read(); //reads for any incoming data
    if (data == '1' && !status && !dataChanged) {//data of 1 represents any UNKNOWN/UNAUTHORIZED personnel. If the security mechanism is disabled and the camera senses an UNKNOWN personnel, it will close the door shut and 
      activateStatus(); // Function to enable the status
      dataChanged = true;
    } else if (data == '0' && status && !dataChanged) {//data of 0 represents the AUTHORIZED personnel, in which case, is me.
      disableStatus(); // Function to disable the status
      dataChanged = true;
    }
  }

  // Check button state to toggle the status
  bool currentButtonState = digitalRead(ButtonINPUT);
  if (currentButtonState != previousButtonState) {
    delay(50);  // Debounce delay
    if (currentButtonState == LOW) {
      toggleStatus(); // Function to toggle the status
    }
  }

  previousButtonState = currentButtonState;

  // Reset dataChanged flag if there's no change in data
  if (Serial.available() == 0) { //This is used to check if there is no incoming byte or any different incoming byte compared to the previous one
    dataChanged = false;         //EXAMPLE: if the byte recieved is 90 and the previous byte was also 90, Serial.available = 0; 
  }                              //If in the current loop, no byte has been transmited to the audrino, it will show Serial.available = 0; as well

  if (status) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    duration = pulseIn(echoPin, HIGH);
    distance = (duration * 0.034) / 2;
    delay(100);

    if (distance < 25) {
      digitalWrite(LEDlampGreen, LOW);
    } else {
      digitalWrite(LEDlampRed, LOW);
      digitalWrite(LEDlampGreen, HIGH);
    }

    if (distance > 25 || distance <= 0) {
      noTone(soundbuzzer);
    } else {
      digitalWrite(LEDlampGreen, LOW);
      for (int i = 450; i < 1000; i++) {
        tone(soundbuzzer, i);
        analogWrite(LEDlampRed, i);
        delay(2);
      }

      for (int i = 1000; i > 450; i--) {
        tone(soundbuzzer, i);
        analogWrite(LEDlampRed, i);
        delay(2);
      }
    }
  }
}

void activateStatus() {
  if (!status) { // Check if the status is already activated
    status = true;
    lcd.clear();
    lcd.setColor(RED);
    lcd.setCursor(0, 0);
    lcd.print("LOCKED");
    digitalWrite(LEDlampGreen, HIGH);
    digitalWrite(LEDlampRed, LOW);

    // Play the desired buzzer sound once
    tone(soundbuzzer, T_D);
    delay(100);
    tone(soundbuzzer, T_F);
    delay(100);
    tone(soundbuzzer, T_A);
    delay(250);
    noTone(soundbuzzer);

    // Disable button functionality temporarily
    disableButton = true;
    delay(500); // Wait for a short period to avoid accidental button presses
    disableButton = false;

    //SERVO MOTOR CLOSED
    myservo.write(0);

  }
}

void disableStatus() {
  if (status) { // Check if the status is already disabled
    status = false;
    lcd.clear();
    lcd.setColor(GREEN);
    lcd.setCursor(0, 0);
    lcd.print("UNLOCKED");
    digitalWrite(LEDlampRed, HIGH);
    digitalWrite(LEDlampGreen, LOW);

    // Play the desired buzzer sound once
    tone(soundbuzzer, T_A);
    delay(100);
    tone(soundbuzzer, T_F);
    delay(100);
    tone(soundbuzzer, T_D);
    delay(250);
    noTone(soundbuzzer);

    // Disable button functionality temporarily
    disableButton = true;
    delay(500); // Wait for a short period to avoid accidental button presses
    disableButton = false;

    //SERVO MOTOR OPENED
    for(int i=0;i<=90;i++)
    {
      myservo.write(i);
      delay(20);
    }

    delay(500);

    while (true) {
      // bool newDataReceived = false; // Flag to indicate if new data has been received
        data = Serial.read(); // Reads for any incoming data
        if (data == '1' && !status && !dataChanged) {
          activateStatus(); // Function to enable the status
          dataChanged = true;
          break;
          // newDataReceived = true; // New data received, set the flag to true
        } else if (data == '0' && status && !dataChanged) {
          disableStatus(); // Function to disable the status
          dataChanged = true;
          // newDataReceived = true; // New data received, set the flag to true
        }
      
        // Reset dataChanged flag if there's no change in data
        if (Serial.available() == 0) {
          dataChanged = false;
        }


      bool currentButtonState = digitalRead(ButtonINPUT);
      if (currentButtonState == HIGH) {
        delay(500);
        toggleStatus();
        break; // Break out of the loop and set status back to true
      }

      digitalWrite(trigPin2, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin2, HIGH);
      delayMicroseconds(10);

      duration = pulseIn(echoPin2, HIGH);
      distance = (duration * 0.034) / 2;
      delay(100);
      if (distance < 25) {
        //SERVO MOTOR CLOSED AGAIN
        for(int i=90;i>=0;i--)
        {
          myservo.write(i);
          delay(20);
        }
        toggleStatus();
        delay(200);
        break;
      }
    }
  }
}

void toggleStatus() {
  if (status) {
    disableStatus(); // Function to disable the status
  } else {
    activateStatus(); // Function to enable the status
  }
}
