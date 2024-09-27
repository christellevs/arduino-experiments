/*
  Blink
  Turns an LED on for one second, then off for one second, repeatedly.
*/

void setup() {
  // Initialize the built-in LED pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED on (HIGH is the voltage level)
  delay(5000);                       // Wait for one second
  digitalWrite(LED_BUILTIN, LOW);    // Turn the LED off by making the voltage LOW
  delay(5000);                       // Wait for one second
}
