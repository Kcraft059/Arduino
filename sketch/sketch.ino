#define TIME_INTERVAL 5000

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(TIME_INTERVAL);
  digitalWrite(LED_BUILTIN, LOW);
  delay(TIME_INTERVAL);
}