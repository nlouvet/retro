#!/usr/bin/env python3

import serial
import argparse
import time
import os

class SerialPackets:
	
	ser = None
	
	def __init__(self, serobj):
		self.ser = serobj
	
	def close(self):
		self.ser.close()
	
	def send_packet(self, buffer):
		l = len(buffer)
		# atmega chips are little endian
		pktsize = l.to_bytes(2, 'little')
		r = self.ser.write(pktsize)
		r = self.ser.write(buffer)
	
	def recv_packet(self):
		pktsize = int.from_bytes(self.ser.read(2), 'little')
		buffer = self.ser.read(pktsize)
		return buffer
	
	def send_ready(self):
		self.send_packet('READY'.encode('ascii'))
	
	def wait_ready(self):
		buffer = self.recv_packet()
		if buffer != 'READY'.encode('ascii'):
			print('READY expected but received:')
			print(buffer)
			print()
			return False
		else:
			return True

	def send_ack(self):
		self.send_packet('ACK'.encode('ascii'))

	def send_nack(self):
		self.send_packet('NACK'.encode('ascii'))
	
	def wait_ack(self):
		buffer = self.recv_packet()
		if buffer != 'ACK'.encode('ascii'):
			print('ACK expected but received:')
			print(buffer)
			print()
			return False
		else:
			return True

if __name__ == '__main__':
	ap = argparse.ArgumentParser()
	ap.add_argument('file')
	ap.add_argument('--port', type=str)
	args = ap.parse_args()
	
	file = args.file
	port = '/dev/ttyUSB0'
	if args.port:
		port = args.port
	
	print(f'input file :{file}')
	print(f'serial port:{port}')
	
	try:
		ser = serial.Serial(port,
				baudrate=115200,
				bytesize=serial.EIGHTBITS,
				parity=serial.PARITY_NONE,
				stopbits=serial.STOPBITS_ONE,
				timeout=30,
				xonxoff=0,
				rtscts=0)
	except:
		print('could not open the serial port')
		exit(0)
	
	try:
		f = open(file, 'rb')
		f.seek(0, os.SEEK_END)
		size = f.tell()
		f.seek(0, os.SEEK_SET)
	except:
		print('could not open file')
		ser.close()
		exit(0)
	
	line = ''
	while line != 'READY':
		line = ser.readline().decode('ascii').strip();
		print("READER:", line)
	
	ser.write((str(size) + '\n').encode('ascii'))
	line = ser.readline().decode('ascii').strip();
	print("READER:", line)

	print('sending the file...')	
	sp = SerialPackets(ser);
	sp.wait_ready();
	while True:
		data = f.read(256)
		l = len(data)
		if l == 0:
			break
		else:
			sp.send_packet(data)
			sp.wait_ack()
	f.close()
	print('finished!')

	while line != 'END':
		line = ser.readline().decode('ascii').strip();
		print("READER:", line)
	
	ser.close()

