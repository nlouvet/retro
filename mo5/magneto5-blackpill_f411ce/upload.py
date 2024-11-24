#!/usr/bin/env python3

import serial
from serpackets import SerPackets

import argparse

import time
import os

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
		print("MAGNETO5:", line)
	
	print('getting ready to send the file...')	
	sp = SerPackets(ser)
	sp.wait_ready()
	print("ready packet received...")
	print("size = ", size)
	sp.send_prelude(size)
	while True:
		data = f.read(256)
		l = len(data)
		if l == 0:
			break
		sp.send_data(data)
		sp.wait_ack()
	sp.send_end()
	f.close()
	print('file sent, everything seems right.')

	while line != 'END':
		line = ser.readline().decode('ascii').strip();
		print("MAGNETO5:", line)
	
	ser.close()

