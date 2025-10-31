

# see https://github.com/linuxforum5/vg5000utils/blob/main/src/vg5000k72wav.c
# https://github.com/linuxforum5/vg5000utils/tree/main
# http://vg5000bazar.free.fr/html/articles/header_k7.htm
# https://github.com/richpl/PyBasic/tree/master
# https://www.triceraprog.fr/tag/vg5000.html
# https://github.com/Triceraprog/vg5000_tools/tree/master


import numpy as np
import sounddevice as sd
import math, os

# transmission of a 0 bit:
# - high during t/2 s
# - low during t/2 s
# transmission of a 1 bit:
# - high during t/4 s
# - low during t/4 s
# 1200 bauds -> t = 800us = 0.0008s
# 2400 bauds -> t = 400us = 0.0004s


AMPLITUDE = 0.8
SAMPLERATE = 44100.0 # Hz
SAMPLEDT = 1.0 / SAMPLERATE;
NSAMPDIV4 = math.ceil(200.0 * SAMPLERATE * 1e-6)
NSAMPDIV2 = 2 * NSAMPDIV4
NSAMPPBIT = 4 * NSAMPDIV4
NSAMPPBYTE = 11 * NSAMPPBIT

NSAMPSILENCE = 4096

NLEADINGBITS = 2048
NSAMPLEADING = NLEADINGBITS * NSAMPPBIT

class Player:
	
	data = None
	pos = 0
	
	def set_bit(self, b):
		if b == 0:
			for i in range(0, NSAMPDIV2):
				self.data[self.pos] = AMPLITUDE
				self.pos += 1
			for i in range(0, NSAMPDIV2):
				self.data[self.pos] = 0
				self.pos += 1
		else:		
			for i in range(0,2):
				for j in range(0, NSAMPDIV4):
					self.data[self.pos] = AMPLITUDE
					self.pos += 1
				for j in range(0, NSAMPDIV4):
					self.data[self.pos] = 0
					self.pos += 1
		
	def set_byte(self, byte):
		self.set_bit(0)
		mask = 0x01
		for i in range(0, 8):
			if (byte & mask) == 0:
				self.set_bit(0)
			else:
				self.set_bit(1)
			mask = mask << 1
		self.set_bit(1)
		self.set_bit(1)
	
	def set_silence(self):
		for i in range(0, NSAMPSILENCE):
			self.data[self.pos] = 0
			self.pos += 1
	
	def set_leading(self):
		for i in range(0, NLEADINGBITS):
			self.set_bit(1)
	
	def __init__(self, file):
		print("loading file {}".format(file))
		f = open(file, 'rb')
		f.seek(0, os.SEEK_END)
		size = f.tell()
		f.seek(0, os.SEEK_SET)
		print("size = {} bytes".format(size))
		
		data_size = size * NSAMPPBYTE + 3 * NSAMPSILENCE + 2 * NSAMPLEADING
		self.data = np.zeros(data_size, dtype=np.float32)
		print("data_size = {}".format(data_size))
		
		self.set_silence()
		self.set_leading()
		
		# we first read the 32 header bytes
		header = f.read(32)
		l = len(header)
		if l != 32:
			print("we've got a problem...")
		for i in range(0, l):
			self.set_byte(header[i])

		self.print_header_info(header)
		
		self.set_silence()
		self.set_leading()
		
		# then we read the begining sequence of ten 0xd3 bytes
		bseq = f.read(10)
		l = len(bseq)
		if l != 10:
			print("we've got a problem...")
		for i in range(0, l):
			self.set_byte(bseq[i])
		
		checksum = 0
		while True:
			d = f.read(256)
			l = len(d)
			if l == 0:
				break
			else:
				for i in range(0, l):
					self.set_byte(d[i])
					checksum = (checksum + d[i]) % 65536
		
		f.close()
		
		self.set_silence()
		
		print("computed checksum = {}".format(checksum))
		print("{} loaded and ready to play".format(file))
	
	def print_header_info(self, header):
		type = header[10:11] # 1 byte
		name = header[11:17].decode('ascii') # 6 bytes
		size = int.from_bytes(header[28:30], 'little') # 2 bytes
		chks = int.from_bytes(header[30:32], 'little') # 2 bytes
		print("header content:")
		print("- file type  = 0x{} = '{}'".format(type.hex(), type.decode('ascii')))
		print("- file name  = '{}'".format(name))
		print("- data size  = {} bytes".format(size))
		# total size = data size + 32 bytes header
		#              + 10 0xd6 begining bytes
		#              + 10 0x00 ending bytes
		print("- total size = {} bytes".format(size + 52))
		print("- checksum   = {} = {}".format(hex(chks), chks))
	
	def play(self):
		print("start playing")
		sd.check_output_settings(samplerate=SAMPLERATE)
		sd.play(self.data, samplerate=SAMPLERATE, blocking=True, loop=False)
		print("end of play")
	
#t = np.arange(0, 10, SAMPLEDT);
#d = AMPLITUDE * np.sin(2 * np.pi * FREQUENCY * t)
#print(len(t))
#sd.play(d, samplerate=SAMPLERATE, blocking=True, loop=False)

p = Player("blitz_vg5000.k7")
p.play()
