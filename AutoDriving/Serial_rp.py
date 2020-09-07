import serial
import time

cmd0 = "$w/0/0%\n"
cmd1 = "$w/1/0%\n"
cmd2 = "$w/2/0%\n"
cmd3 = "$r/3/0%\n"
cmd4 = "$r/4/0%\n"

check = False

ser=serial.Serial(
	port='/dev/ttyACM0',
	baudrate=9600,
)
while True:
	print("insert op:", end='')
	op = input()

	if op == '0':
		ser.write(cmd0.encode())
		check = True
	elif op == '1':
		ser.write(cmd1.encode())
		check = True
	elif op == '2':
		ser.write(cmd2.encode())
		check = True
	elif op == '3':
		ser.write(cmd3.encode())
		check = True
	elif op == '4':
		ser.write(cmd4.encode())
		check = True
	else :
		print("error")
	
	while check:
		print("check")
		if ser.readable():
			res = ""
			res = ser.readline()
			time.sleep(0.1)
			print(res.decode()[:-2])
			check = False
