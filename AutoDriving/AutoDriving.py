# -*- coding: utf-8 -*- 

import serial
import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)            # GPIO

syncArduino = 12
syncRaspPi = 16

GPIO.setup(syncArduino, GPIO.IN)
GPIO.setup(syncRaspPi, GPIO.OUT, initial=GPIO.LOW)

while GPIO.input(syncArduino) != 1:                 # Arduino가 Init이 끝날때까지 대기
    print("Arduino Operating...")
    time.sleep(1)

print("RaspberryPi Operating....")

GPIO.output(syncRaspPi, GPIO.HIGH)                  # Arduino에게 Raspberry Pi가 준비됐다고 알림

ser=serial.Serial(
	port='/dev/ttyACM0',
	baudrate=115200,
)

# 영상처리 함수 선언부 #


#         end         #

cmd = "$w2/3%"
ser.write(cmd.encode())                     # D단으로 변경

time.sleep(0.1)
cmd = "$w1/0%"
ser.write(cmd.encode())                     # 속도 0으로 고정

while True:
    angleError = 0

    # 영상처리 코드 #

    #     end      #

    cmd = "$w0/" + str(angleError) + "%"
    ser.write(cmd.encode())                 # 조향 오차 전송


