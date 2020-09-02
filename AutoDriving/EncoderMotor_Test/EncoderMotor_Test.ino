#define Motor       2
#define Dir         3
#define Phase_B     20
#define Phase_A     21

#define SamplingTime 0.09                   // Sec 단위 (속도 계산 주기)

volatile int pulseCount = 0;
volatile float rpm = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(Phase_A, INPUT_PULLUP);     // Encoder Phase A
  pinMode(Phase_B, INPUT_PULLUP);     // Encoder Phase B

  pinMode(Motor, OUTPUT);             // Motor PWM

  pinMode(Dir, OUTPUT);               // Dir

  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);       // INT0 Falling Edge

  EICRA &= ~(1 << ISC21);
  EICRA |= (1 << ISC20);       // INT2 Falling Edge

  EIFR |= (1 << INTF0);         // Clear INT0 Flag
  EIFR |= (1 << INTF2);         // Clear INT2 Flag

  EIMSK |= (1 << INT0);         // Enable INT0
  EIMSK |= (1 << INT2);         // Enable INT2

  sei();

  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);
  TCCR1A &= ~(1 << WGM11);
  TCCR1A &= ~(1 << WGM10);          // CTC Mode

  TCCR1B &= ~(1 << CS12);
  TCCR1B |= (1 << CS11);
  TCCR1B |= (1 << CS10);            // CLK/64

  OCR1A = (16000000 / 64) * SamplingTime - 1;                // Sampling Time을 Tick으로 계산 (Sec 단위)
  TCNT1 = 0x0000;

  TIMSK1 |= (1 << OCIE1A);          // interrupt Enable
  TIFR1 |= (1 << OCF1A);            // Clear Flag
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(Dir, HIGH);
  analogWrite(Motor, 200);
  Serial.print("Pulse: ");
  Serial.println(pulseCount);
  Serial.print("RPM: ");
  Serial.println(rpm);
  delay(100);
}

ISR(TIMER1_COMPA_vect) {
  rpm = (float)pulseCount/(float)13 / 50;
  rpm = rpm / SamplingTime * 60;
  pulseCount = 0;
}

ISR(INT0_vect) {
  if (!digitalRead(Phase_B))
    pulseCount++;
  else
    pulseCount--;

}   // Phase A
