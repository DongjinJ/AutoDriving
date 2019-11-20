const byte Phase_A = 2;
const byte Phase_B = 3;

volatile int pulseCount = 0;

void count() {
  if (digitalRead(Phase_B))
    pulseCount++;
  else
    pulseCount--;

}   // Phase A

void setup() {
  // put your setup code here, to run once:
  pinMode(Phase_A, INPUT_PULLUP);     // Encoder Phase A
  pinMode(Phase_B, INPUT_PULLUP);     // Encoder Phase B

  attachInterrupt(digitalPinToInterrupt(Phase_A), count, FALLING);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Pulse Count: ");
  Serial.println(pulseCount);
  delay(1000);
}
