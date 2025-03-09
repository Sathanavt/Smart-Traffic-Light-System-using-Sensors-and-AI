#include <TM1637Display.h>

// Pin definitions for the TM1637 displays for each lane
#define CLK1 2  // CLK pin for TM1637 Display 1
#define DIO1 3  // DIO pin for TM1637 Display 1
#define CLK2 4  // CLK pin for TM1637 Display 2
#define DIO2 5  // DIO pin for TM1637 Display 2
#define CLK3 0  // CLK pin for TM1637 Display 3
#define DIO3 1   // DIO pin for TM1637 Display 3

// Initialize TM1637 displays for each lane
TM1637Display display1(CLK1, DIO1);
TM1637Display display2(CLK2, DIO2);
TM1637Display display3(CLK3, DIO3);

// LED pin definitions for each lane
int lane1Red = 13, lane1Yellow = 12, lane1Green = 14;
int lane2Red = 27, lane2Yellow = 26, lane2Green = 25;
int lane3Red = 33, lane3Yellow = 32, lane3Green = 15;

// IR sensor pin definitions for each lane
int irSensor1_1 = 21, irSensor1_2 = 22, irSensor1_3 = 23;
int irSensor2_1 = 16, irSensor2_2 = 17, irSensor2_3 = 18;
int irSensor3_1 = 34, irSensor3_2 = 35, irSensor3_3 = 36;

void setup() {
  // Set all LED pins as OUTPUT
  pinMode(lane1Red, OUTPUT);
  pinMode(lane1Yellow, OUTPUT);
  pinMode(lane1Green, OUTPUT);

  pinMode(lane2Red, OUTPUT);
  pinMode(lane2Yellow, OUTPUT);
  pinMode(lane2Green, OUTPUT);

  pinMode(lane3Red, OUTPUT);
  pinMode(lane3Yellow, OUTPUT);
  pinMode(lane3Green, OUTPUT);

  // Set all IR sensor pins as INPUT
  pinMode(irSensor1_1, INPUT);
  pinMode(irSensor1_2, INPUT);
  pinMode(irSensor1_3, INPUT);

  pinMode(irSensor2_1, INPUT);
  pinMode(irSensor2_2, INPUT);
  pinMode(irSensor2_3, INPUT);

  pinMode(irSensor3_1, INPUT);
  pinMode(irSensor3_2, INPUT);
  pinMode(irSensor3_3, INPUT);

  // Initialize displays and set brightness
  display1.setBrightness(0x0f); // Max brightness
  display2.setBrightness(0x0f);
  display3.setBrightness(0x0f);
}

void countdownDisplay(TM1637Display &display, int seconds) {
  display.showNumberDec(seconds, true); // Show countdown on display
}

void laneSequence(int &currentLane, TM1637Display &display, int redPin, int yellowPin, int greenPin) {
  for (int time = 13; time >= 0; time--) {
    // Display the countdown
    countdownDisplay(display, time);

    // Control the LEDs based on the time
    if (time > 3) { // Green light for first 10 seconds
      digitalWrite(greenPin, HIGH);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);
    } else if (time > 0) { // Yellow light for last 3 seconds
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(redPin, LOW);
    } else { // Red light when time reaches 0
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, HIGH);
    }

    delay(1000); // 1 second delay for each countdown step
  }
}

bool checkAllSensors(int sensor1, int sensor2, int sensor3) {
  return digitalRead(sensor1) == LOW && digitalRead(sensor2) == LOW && digitalRead(sensor3) == LOW;
}

void loop() {
  static int currentLane = 1;
  static bool emergencyFlag = false;

  // Check if all IR sensors in a lane are triggered
  if (checkAllSensors(irSensor1_1, irSensor1_2, irSensor1_3)) {
    currentLane = 1;
    emergencyFlag = true;
  } else if (checkAllSensors(irSensor2_1, irSensor2_2, irSensor2_3)) {
    currentLane = 2;
    emergencyFlag = true;
  } else if (checkAllSensors(irSensor3_1, irSensor3_2, irSensor3_3)) {
    currentLane = 3;
    emergencyFlag = true;
  }

  // Execute the sequence for the current lane
  if (currentLane == 1) {
    laneSequence(currentLane, display1, lane1Red, lane1Yellow, lane1Green);
    digitalWrite(lane2Red, HIGH);
    digitalWrite(lane3Red, HIGH);
    currentLane = 2;
  } 
  else if (currentLane == 2)
   {
    laneSequence(currentLane, display2, lane2Red, lane2Yellow, lane2Green);
    digitalWrite(lane1Red, HIGH);
    digitalWrite(lane3Red, HIGH);
    currentLane = 3;
  } 
  else if (currentLane == 3) 
  {
    laneSequence(currentLane, display3, lane3Red, lane3Yellow, lane3Green);
    digitalWrite(lane1Red, HIGH);
    digitalWrite(lane2Red, HIGH);
    currentLane = 1;

  }

  // If all sensors in any lane are detected, wait until the green LED timing ends
  if (emergencyFlag) {
    delay(3000); // Keep current lane green for an additional 10 seconds before switching to detected lane
    emergencyFlag = false;
  }
}