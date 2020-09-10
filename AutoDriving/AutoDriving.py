import serial
import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)            # GPIO 사용 셋팅
syncArduino = 8
syncRaspPi = 10

GPIO.setup(syncArduino, GPIO.IN)
GPIO.setup(syncRaspPi, GPIO.OUT, initial=GPIO.LOW)

while GPIO.input(syncArduino) != 1:                 # Arduino가 Init이 끝날때까지 대기
    print("Arduino Operating...\r")
    time.sleep(1)

GPIO.output(syncRaspPi, GPIO.HIGH)                  # Arduino에게 Raspberry Pi가 준비됐다고 알림

ser=serial.Serial(
	port='/dev/ttyACM0',
	baudrate=115200,
)

# 영상처리 함수 선언부 #


#         end         #

cmd = "$w/2/3%"
ser.write(cmd.encode())                     # D단으로 변경

time.sleep(0.1)
cmd = "$w/1/0%"
ser.write(cmd.encode())                     # 속도 0으로 고정

while True:
    angleError = 0

    # 영상처리 코드 #

    #     end      #

    cmd = "$w/0/" + str(angleError) + "%"
    ser.write(cmd.encode())                 # 조향 오차 전송


