import serial
import time

check = False

cmd0 = "$r/0/0%\n"
cmd1 = "$r/1/0%\n"
cmd2 = "$r/2/0%\n"
cmd3 = "$r/3/0%\n"
cmd4 = "$r/4/0%\n"

ser=serial.Serial(
	port='/dev/ttyACM0',
	baudrate=9600,
)

def send_cmd(cmd, ser):
	ser.write(cmd.encode())
	
def decode_message(cmd, message):

	if message.startswith('$') == False:
		send_cmd(cmd, ser)
		return 5,0 
	elif message.endswith('%') == False:
		send_cmd(cmd, ser)
		return 5,0
	else:
		f_n = message.find('/')
		s_n = message.find('%',f_n)
		id = message[1:f_n]
		val = message[f_n+1:s_n]
		return id, val

while True:
	print("insert op:", end='')
	op = input()

	if op == '0':
		cmd = cmd0
		send_cmd(cmd, ser)
		check = True
	elif op == '1':
		cmd = cmd1
		send_cmd(cmd, ser)
		check = True
	elif op == '2':
		cmd = cmd2
		send_cmd(cmd, ser)
		check = True
	elif op == '3':
		cmd = cmd3
		send_cmd(cmd, ser)
		check = True
	elif op == '4':
		cmd = cmd4
		send_cmd(cmd, ser)
		check = True
	else :
		print("error")

	while check:
		if ser.readable():
			res = ""
			res = ser.readline()
			time.sleep(0.1)
			print(res.decode()[:-2])
			message = res.decode()[:-2]
			id ,val = decode_message(cmd, message)
			if id == 5:
				check = True
			else:
				check = False
	
	if id=='0':
		val0 = int(val)
		print(id, val0)
	elif id=='1':
		val1 = int(val)
		print(id, val1)
	elif id=='2':
		val2 = int(val)
		print(id, val2)
	elif id=='3':
		val3 = int(val)
		print(id, val3)
	elif id=='4':
		val4 = int(val)
		print(id, val4)
