/* Servo Left/center/Right: 150/190/230 +-40 */
/*  include */
#include <LowPower.h>

/* Configuration */
#define ENABLE  1
#define DISABLE 0

#define L298N               0
#define MD10C               1

#define DEBUG               ENABLE

#define MOTOR_DRIVER_SPEC   MD10C

/*  define  */
#define WheelGear 30
#define EncoderGear 81
#define TwoPiR 0.19823
#define Resolution 360

#define Servo_MAXus     (1880U) * 2
#define Servo_MINus     (520U) * 2

#define cdSensor A0
#define irSensor A1

#define SamplingTime 0.09                   // Sec 단위 (속도 계산 주기)

/*    Command    */
#define Command_Servo          0
#define Command_Velocity       1
#define Command_Transmission   2
#define Command_Battery        3
#define Command_CarState       4

/* Pin Number */
#define Motor       2
#if (MOTOR_DRIVER_SPEC == L298N)
#define IN1         3
#define IN2         4
#elif (MOTOR_DRIVER_SPEC == MD10C)
#define Dir         3
#else
#error "Check Motor Driver Spec!!!"
#endif
#define servo       6
#define Ignition    7
#define WakeUp      19
#define Phase_B     20
#define Phase_A     21
#define Rear_LED    47
#define Head_LED    49
#define Right_LED   51
#define Left_LED    53


/* Initialize Function */
void Pin_Init();
void Interrupt_Init();
void Timer_Init();
void PWM_Init();
void Watchdog_Init();

/* Motor Function */
void Forward();
void Backward();
void Servo_power(int angle);
void Motor_power(int value);
void AEB_system();
void PI_Controller();

/* ETC Function */
void Sensing();
void Actuating();
void Decode_command();

/* Interrupt variable */
volatile String Order = "";
volatile int Command_ID;
volatile int Command_DATA;
volatile char Command_TYPE;

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
volatile float Kp = 50;
volatile float Ki = 110;
volatile float error = 0;

float refVelocity = 0;
int Steering = 0;
int power = 128;

enum Transmission {
  P = 0,
  R,
  N,
  D
};

class flag {
  public:
    volatile bool change;
    volatile bool AEB;
    volatile bool LED;
    volatile bool Sleep;
    volatile int curT;

    flag();
    ~flag();
};
flag::flag() {
  change = false;
  AEB = false;
  LED = false;
  Sleep = true;
  curT = P;
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
  Serial2.begin(9600);
  Serial.println("Operating...");
  delay(2000);
  Order.reserve(200);

  Servo_power(0);
  refVelocity = 0;
  Forward();

  int waitCount = 0;
  Serial2.println("Ready");
  //  while (waitCount < 1000) {
  //    if (digitalRead(Ignition))
  //      waitCount++;
  //    else
  //      ;
  //    delay(1);
  //#if (DEBUG == ENABLE)
  //    Serial.println(waitCount);
  //#endif
  //  }
#if (DEBUG == ENABLE)
  Serial.println("Key On");
#endif
  //  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void loop() {
  // put your main code here, to run repeatedly:

  Sensing();

  Actuating();

  delay(50);
}

void Sensing() {

  brightness = analogRead(cdSensor);
  frontDistance = analogRead(irSensor);

  if (brightness > 600) {
    state->LED = true;
  }
  else {
    state->LED = false;

  }

  if (frontDistance > 300) {
    state->AEB = true;
  }
  else {
    state->AEB = false;

  }
}

void Actuating() {
  if (state->change) {
    state->change = false;

#if (DEBUG == ENABLE)
    Serial.print("Command ID: ");
    Serial.println(Command_ID);
    Serial.print("Command DATA: ");
    Serial.println(Command_DATA);
#endif

    Decode_command();
    Order = "";
  }

  if (state->LED)
    digitalWrite(Head_LED, HIGH);
  else
    digitalWrite(Head_LED, LOW);

  if (Steering < -90)
    Servo_power(-90);
  else if (Steering > 90)
    Servo_power(90);
  else
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

void PI_Controller() {
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
  else if (PI_Control < -255)
    PI_Control = -255;
}
ISR(TIMER1_COMPA_vect) {
  distance = (float)EncoderGear / (float)Resolution * pulseCount / (float) WheelGear * TwoPiR;
  velocity = distance / SamplingTime;
  pulseCount = 0;

  if (!state->AEB) {
    switch (state->curT) {
      case P:
        break;
        
      case R:
        PI_Controller();
        if (PI_Control >= 0) {
          Forward();
          Motor_power(PI_Control);
        }
        else {
          Backward();
          Motor_power(PI_Control);
        }
        break;

      case N:
        break;

      case D:
        PI_Controller();
        if (PI_Control >= 0) {
          Forward();
          Motor_power(PI_Control);
        }
        else {
          Motor_power(0);
        }
        break;

      default:
        break;
    }


    digitalWrite(Rear_LED, LOW);
  }
  else
    AEB_system();
}   // SamplingTime초마다 동작




ISR(INT0_vect) {
  if (!digitalRead(Phase_B))
    pulseCount++;
  else
    pulseCount--;

}   // Phase A

ISR(INT2_vect) {
  Motor_power(0);
  state->Sleep ^= 1;

  if (state->Sleep) {
    Serial2.println("Sleep");
#if (DEBUG == ENABLE)
    Serial.println("Sleep");
#endif
    delay(5000);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
  else {
    Serial2.println("Wake Up");
#if (DEBUG == ENABLE)
    Serial.println("Wake Up");
#endif
  }
}   // Sleep mode Wake up


void Pin_Init() {
  /* Input */
  pinMode(Phase_A, INPUT_PULLUP);     // Encoder Phase A
  pinMode(Phase_B, INPUT_PULLUP);     // Encoder Phase B
  pinMode(WakeUp,  INPUT_PULLUP);     // Wake up pin
  pinMode(Ignition, INPUT_PULLUP);    // Wake up pin

  /* Output */
  pinMode(Motor, OUTPUT);             // Motor PWM
  pinMode(servo, OUTPUT);             // Servo PWM
#if (MOTOR_DRIVER_SPEC == L298N)
  pinMode(IN1, OUTPUT);               // IN 1
  pinMode(IN2, OUTPUT);               // IN 2
#elif (MOTOR_DRIVER_SPEC == MD10C)
  pinMode(Dir, OUTPUT);               // Dir
#else
#error "Check Motor Driver Spec!!!"
#endif
  pinMode(Left_LED, OUTPUT);          // 왼쪽 방향 지시등
  pinMode(Right_LED, OUTPUT);         // 오른쪽 방향 지시등
  pinMode(Head_LED, OUTPUT);          // 헤드 램프
  pinMode(Rear_LED, OUTPUT);          // 브레이크 등
}

void Interrupt_Init() {
  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);       // INT0 Falling Edge

  EICRA &= ~(1 << ISC21);
  EICRA |= (1 << ISC20);       // INT2 Falling Edge

  EIFR |= (1 << INTF0);         // Clear INT0 Flag
  EIFR |= (1 << INTF2);         // Clear INT2 Flag

  EIMSK |= (1 << INT0);         // Enable INT0
  EIMSK |= (1 << INT2);         // Enable INT2

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

  OCR1A = (16000000 / 64) * SamplingTime - 1;                // Sampling Time을 Tick으로 계산 (Sec 단위)
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

  ICR4 = 4999;

  TCCR4B |= (1 << WGM43);
  TCCR4B |= (1 << WGM42);
  TCCR4A |= (1 << WGM41);
  TCCR4A &= ~(1 << WGM40);           // Fast PWM, TOP:ICR4

  TCCR4A |= (1 << COM4A1);
  TCCR4A &= ~(1 << COM4A0);         // OC4A Clear on compare match

  TCCR4B &= ~(1 << CS42);
  TCCR4B |= (1 << CS41);
  TCCR4B |= (1 << CS40);            // CLK/1

  TCNT4 = 0;
}
void Forward() {
#if (MOTOR_DRIVER_SPEC == L298N)
  digitalWrite(IN1, LOW);
  //  PORTE &= ~(1 << PORTE5);          // IN1 LOW
  digitalWrite(IN2, HIGH);
  //  PORTG |= (1 << PORTG5);           // IN2 HIGH
#elif (MOTOR_DRIVER_SPEC == MD10C)
  digitalWrite(Dir, HIGH);               // Dir
#else
#error "Check Motor Driver Spec!!!"
#endif
}

void Backward() {
#if (MOTOR_DRIVER_SPEC == L298N)
  digitalWrite(IN1, HIGH);
  //  PORTE |= (1 << PORTE5);           // IN1 HIGH
  digitalWrite(IN2, LOW);
  //  PORTG &= ~(1 << PORTG5);          // IN2 LOW
#elif (MOTOR_DRIVER_SPEC == MD10C)
  digitalWrite(Dir, LOW);               // Dir
#else
#error "Check Motor Driver Spec!!!"
#endif


}

void Servo_power(int angle) {
  uint16_t value;

  value = (float)(angle + 90) / 180 * Servo_MAXus + Servo_MINus;
  OCR4A = value;
}

void Motor_power(int value) {
  // OCR3A = value;
  OCR3B = value;
}

void Decode_command() {
  String Return_Data = "$";
  int state_data = 0;

  switch (Command_ID) {
    case Command_Servo:
      if (Command_TYPE == 'r') {
        Return_Data += "0/" + String(Steering) + "%";
        Serial.println(Return_Data);
      }
      else if (Command_TYPE == 'w') {
        Steering = Command_DATA;
      }
      break;
    case Command_Velocity:
      if (Command_TYPE == 'r') {
        Return_Data += "0/" + String(velocity) + "%";
        Serial.println(Return_Data);
      }
      else if (Command_TYPE == 'w') {
        refVelocity = Command_DATA;
      }
      break;
    case Command_Transmission:
      if (Command_TYPE == 'r') {
        Return_Data += "0/" + String(state->curT) + "%";
        Serial.println(Return_Data);
      }
      else if (Command_TYPE == 'w') {
        state->curT = Command_DATA;
      }
      break;
    case Command_Battery:
      if (Command_TYPE == 'r') {
        Return_Data += "0/" + String(state->curT) + "%";
        Serial.println(Return_Data);
      }
      else if (Command_TYPE == 'w') {
        ;
      }
      break;
    case Command_CarState:
      if (Command_TYPE == 'r') {
        state_data = (state->AEB << 2) | (state->LED << 1) | (state->Sleep);
        Return_Data += "0/" + String(state_data) + "%";
        Serial.println(Return_Data);
      }
      else if (Command_TYPE == 'w') {
        ;
      }
      break;
      
    default: 
      break;
  }
}
#if (DEBUG == ENABLE)
void serialEvent() {
  String DebugData = "";
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    DebugData += inChar;
    delay(10);
  }

  int indexP, indexS;
  String temp;
  if (DebugData[0] == '$') {
    Command_TYPE = DebugData[1];
    indexP = DebugData.indexOf('%');
    if (indexP == -1)
      ;
    else {
      Order = DebugData.substring(2, indexP);
      indexS = Order.indexOf('/');
      temp = Order.substring(0, indexS);
      Command_ID = temp.toInt();
      if (Command_ID == Command_Velocity) {
        temp = Order.substring(indexS + 1);
        Command_DATA = temp.toFloat();
      }
      else {
        temp = Order.substring(indexS + 1);
        Command_DATA = temp.toInt();
      }
      state->change = true;
    }
  }
  else
    ;
}
#endif
void serialEvent2() {
  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    Order += inChar;
    delay(10);
  }

  state->change = true;
}
