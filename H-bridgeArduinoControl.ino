const int in1 = 10;                  // PWM output for IN1 (controls high-side P-MOSFET)
const int in2 = 9;                  // PWM output for IN2 (controls low-side N-MOSFET)
const int buttonPin = 2;             // Pin connected to the momentary button

int motorState = 0;                  // Tracks motor state: 0 = Stop, 1 = Forward, 2 = Backward
int lastButtonState = HIGH;          // Last button state for debouncing
int currentButtonState;              // Current debounced button state
unsigned long lastDebounceTime = 0;  // Tracks when the button state last changed
unsigned long debounceDelay = 50;    // Debounce delay in milliseconds

void setup() {
  pinMode(in1, OUTPUT);             // Configure IN1 as output
  pinMode(in2, OUTPUT);             // Configure IN2 as output
  pinMode(buttonPin, INPUT_PULLUP);  // Configure button pin with pull-up resistor

  // Initialize both pins to LOW (motor off)
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  Serial.begin(9600);
  Serial.println("Program started");
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Reset the debouncing timer if the button state changed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Check if button state is stable
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed:
    if (reading != currentButtonState) {
      currentButtonState = reading;

      // Only toggle the motor state if the new button state is LOW (pressed)
      if (currentButtonState == LOW) {
        motorState = (motorState + 1) % 3; // Cycle through states: 0 -> 1 -> 2 -> 0
        updateMotorState();               // Update motor based on the new state
        Serial.print("Motor State: ");
        Serial.println(motorState == 0 ? "Stopped" : (motorState == 1 ? "Forward" : "Backward"));
      }
    }
  }

  lastButtonState = reading;
  delay(10); // Small delay to stabilize readings
}

// Function to update the motor state based on the current motorState value
void updateMotorState() {
  switch (motorState) {
    case 0: // Stop the motor
      analogWrite(in1, 0); // C = 0V (P-MOSFET off)
      analogWrite(in2, 0); // D = 0V (N-MOSFET off)
      break;

    case 1: // Rotate motor forward (A = 0V, B = 9V)
      analogWrite(in1, 0);   // C = 0V (P-MOSFET off)
      analogWrite(in2, 255); // D = 5V (N-MOSFET on)
      break;

    case 2: // Rotate motor backward (A = 9V, B = 0V)
      analogWrite(in1, 255); // C = 5V (P-MOSFET on)
      analogWrite(in2, 0);   // D = 0V (N-MOSFET off)
      break;
  }
}
