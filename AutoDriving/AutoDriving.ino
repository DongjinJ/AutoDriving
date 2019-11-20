/* Servo Left/center/Right: 150/190/230 +-40 */

#define WheelGear 30
#define EncoderGear 81
#define TwoPiR 0.19823
#define Resolution 360
#define SamplingTime 0.1

#define Center 190

#define cdSensor A0
#define irSensor A1

/* Pin Number */
const byte Phase_A = 21;
const byte Phase_B = 20;
const byte Motor = 2;
const byte servo = 6;
const byte IN1 = 3;
const byte IN2 = 4;
const byte Left_LED = 53;
const byte Right_LED = 51;
const byte Head_LED = 49;
const byte Rear_LED = 47;

/* Initialize Function */
void Pin_Init();
void Interrupt_Init();
void Timer_Init();
void PWM_Init();

/* Motor Function */
void Forward();
void Backward();
void Servo_power(int value);
void Motor_power(int value);
void AEB_system();

/* ETC Function */
void Sensing();
void Actuating();

/* Interrupt variable */
String Order = "";

volatile int pulseCount = 0;
volatile float distance = 0;
volatile float velocity = 0;

/* Sensing Variable */
int brightness = 0;
int frontDistance = 0;

/* PI Motor control Varialble */
volatile float PI_Control = 0;
volatile float P_Control = 0;
volatile float I_Control = 0;
volatile float Kp = 25;
volatile float Ki = 11;
volatile float error = 0;

float refVelocity = 0;
int Steering = 0;
int power = 128;

class flag {
  public:
    volatile bool change;
    volatile bool AEB;
    volatile bool LED;

    flag();
    ~flag();
};
flag::flag() {
  change = false;
  AEB = false;
  LED = false;
}
flag::~flag() {

}
flag *state;
void setup() {
  // put your setup code here, to run once:
  Pin_Init();
  Interrupt_Init();
  Timer_Init();
  PWM_Init();

  state = new flag();

  Serial.begin(115200);
  Serial1.begin(9600);

  Order.reserve(200);


  Servo_power(189);
  refVelocity = 0.5;
  Forward();
}

void loop() {
  // put your main code here, to run repeatedly:
  Sensing();

  Actuating();
}

void Sensing() {
  // Serial.print("motor: ");
  // Serial.println(PI_Control);

  brightness = analogRead(cdSensor);
  frontDistance = analogRead(irSensor);

  if (brightness > 600) {
    state->LED = true;
    Serial1.println("LED signal ON");
  }
  else {
    state->LED = false;

  }


  // Serial.print("IR data: ");
  // Serial.println(frontDistance);
  if (frontDistance > 300) {
    state->AEB = true;
    Serial1.println("AEB signal ON");
  }
  else {
    state->AEB = false;

  }
}

void Actuating() {
  if (state->change) {
    state->change = false;
    int indexV = Order.indexOf("V");
    int indexS = Order.indexOf("S");
    String strV = Order.substring(indexV + 1, indexS);
    String strS = Order.substring(indexS + 1, Order.length());
    Serial.println("V:  " + strV + "\tS: " + strS);
    refVelocity = strV.toFloat();
    Steering = strS.toInt();
    Serial.print("Current velocity: ");
    Serial.print(velocity);
    Serial.print("\tGoal: ");
    Serial.print(refVelocity);
    Serial.println("m/s");
    Order = "";
  }

  if (state->LED)
    digitalWrite(Head_LED, HIGH);
  else
    digitalWrite(Head_LED, LOW);

  Servo_power(Steering);
}

void AEB_system() {
  digitalWrite(Rear_LED, HIGH);
  PI_Control = 0;
  I_Control = 0;
  if (velocity > 0 ) {
    Backward();
    Motor_power(200);
  }
  else {
    Forward();
    Motor_power(0);
  }
}

ISR(TIMER1_COMPA_vect) {
  distance = (float)EncoderGear / (float)Resolution * pulseCount / (float) WheelGear * TwoPiR;
  velocity = distance / 0.1;
  pulseCount = 0;

  if (!state->AEB) {
    error = refVelocity - velocity;
    P_Control = Kp * error;
    I_Control += Ki * error * SamplingTime;
    PI_Control = P_Control + I_Control;
    if (velocity > 0)
      ;
    else
      I_Control += 50;
    if (PI_Control > 255)
      PI_Control = 255;
    else if (PI_Control < 0)
      PI_Control = 0;


    Motor_power(PI_Control);
    digitalWrite(Rear_LED, LOW);
  }
  else
    AEB_system();
}   // 0.1초마다 동작




ISR(INT0_vect) {
  if (!digitalRead(Phase_B))
    pulseCount++;
  else
    pulseCount--;

}   // Phase A

void Pin_Init() {
  /* Input */
  pinMode(Phase_A, INPUT);     // Encoder Phase A
  pinMode(Phase_B, INPUT);     // Encoder Phase B

  /* Output */
  pinMode(Motor, OUTPUT);             // Motor PWM
  pinMode(servo, OUTPUT);             // Servo PWM
  pinMode(IN1, OUTPUT);               // IN 1
  pinMode(IN2, OUTPUT);               // IN 2
  pinMode(Left_LED, OUTPUT);          // 왼쪽 방향 지시등
  pinMode(Right_LED, OUTPUT);         // 오른쪽 방향 지시등
  pinMode(Head_LED, OUTPUT);          // 헤드 램프
  pinMode(Rear_LED, OUTPUT);          // 브레이크 등
}

void Interrupt_Init() {
  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);       // INT0 Falling Edge

  EIFR |= (1 << INTF0);         // Clear INT0 Flag
  EIMSK |= (1 << INT0);         // Enable INT0
  sei();
}

void Timer_Init() {
  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);
  TCCR1A &= ~(1 << WGM11);
  TCCR1A &= ~(1 << WGM10);          // CTC Mode

  TCCR1B &= ~(1 << CS12);
  TCCR1B |= (1 << CS11);
  TCCR1B |= (1 << CS10);            // CLK/64

  OCR1A = 24999;                    // 10Hz = 0.1 sec
  TCNT1 = 0x0000;

  TIMSK1 |= (1 << OCIE1A);          // interrupt Enable
  TIFR1 |= (1 << OCF1A);            // Clear Flag
}

void PWM_Init() {
  TCCR3B &= ~(1 << WGM33);
  TCCR3B |= (1 << WGM32);
  TCCR3A &= ~(1 << WGM31);
  TCCR3A |= (1 << WGM30);           // Fast PWM, 8-bit

  TCCR3A |= (1 << COM3B1);
  TCCR3A &= ~(1 << COM3B0);         // OC3A  Clear on compare match

  TCCR3B &= ~(1 << CS32);
  TCCR3B |= (1 << CS31);
  TCCR3B |= (1 << CS30);            // CLK/1

  TCNT3 = 0;

  TCCR4B &= ~(1 << WGM43);
  TCCR4B &= ~(1 << WGM42);
  TCCR4A |= (1 << WGM41);
  TCCR4A &= ~(1 << WGM40);           // Fast PWM, 8-bit

  TCCR4A |= (1 << COM4A1);
  TCCR4A &= ~(1 << COM4A0);         // OC4A Clear on compare match

  TCCR4B &= ~(1 << CS42);
  TCCR4B |= (1 << CS41);
  TCCR4B |= (1 << CS40);            // CLK/1

  TCNT4 = 0;
}

void Forward() {
  PORTE &= ~(1 << PORTE5);          // IN1 LOW
  PORTG |= (1 << PORTG5);           // IN2 HIGH
}

void Backward() {
  PORTE |= (1 << PORTE5);           // IN1 HIGH
  PORTG &= ~(1 << PORTG5);          // IN2 LOW
}

void Servo_power(int value) {
  OCR4A = value;
}

void Motor_power(int value) {
  // OCR3A = value;
  OCR3B = value;
}

void serialEvent1() {
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    Order += inChar;
    delay(10);
  }
  state->change = true;
}
