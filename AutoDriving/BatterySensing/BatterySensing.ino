#include <LowPower.h>

#define Ignition A2

#define resetPin       17

#define R1          9.83
#define R2          4.61

int ignitionCount = 0;
int voltageData = 0;
float voltage = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, HIGH);

  while (ignitionCount < 1000) {
    voltageData = analogRead(Ignition);
    voltage = (float)voltageData * (5.0 / 1024.0);             // ADC -> Voltage
    voltage = voltage * ((R1 + R2) / R2);             // calcurate origin voltage

    if (voltage >= 7.6)
      ignitionCount++;
    else {
      if (ignitionCount > 0)
        ignitionCount--;
      else
        ;
    }
  }
  Serial.println("Arduino Power On");
}

void loop() {
  // put your main code here, to run repeatedly:
  voltageData = analogRead(Ignition);
  voltage = (float)voltageData * (5.0 / 1024.0);             // ADC -> Voltage
  voltage = voltage * ((R1 + R2) / R2);             // calcurate origin voltage

  Serial.print("Voltage: ");
  Serial.println(voltage);

  if (voltage < 7.6) {
    ignitionCount--;
  }
  else if (voltage >= 7.6 && ignitionCount < 1000) {
    ignitionCount++;
  }
  else
    ;

  if (ignitionCount < 0) {
    Serial.println("Low Battery Voltage or Ignition Off");
    delay(1000);
    digitalWrite(resetPin, LOW);
  }
  else
    ;
}
