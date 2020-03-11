#ifndef __MOTORCONTROL_H__
#define __MOTORCONTROL_H__

extern void Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel);
extern void Servo_Angle(int8_t angle);

extern void Motor_Power(int16_t power);
#endif
