#!/usr/bin/env python3 

# see http://vg5000bazar.free.fr/html/articles/header_k7.htm

import argparse

# file type:
BASTOK = b'\x20'
BINARY = b'\x4D'

class Converter:
	infile = None
	filename = 'VG5000'
	data = b''
	conv_type = ''
	file_type = None
	
	def set_infile(self, name):
		self.infile = name

	def set_file_name(self, fn):
		self.filename = fn[0:6]
	
	def set_infile(self, name):
		self.infile = name

	def set_start_add(self, add):
		self.start_add = add
	
	def set_exec_add(self, add):
		self.exec_add = add

	def set_conv_type(self, type):
		if type == 'bastok':
			self.conv_type = 'bastok'
			self.file_type = BASTOK
		elif type == 'binary':
			self.conv_type = 'binary'
			self.file_type = BINARY
		else:
			print('Unknown conversion type...')

	def __init__(self):
		pass
	
	def checksum(self):
		c = 0
		for w in self.data:
			c = (c + w) % 65536
		return c
	
	def load_data(self):
		try:
			f = open(self.infile, 'rb')
		except Exception as err:
			print(err)
			exit(1)
		self.data = f.read()
		f.close()
                
	def gen_head_block(self):
		block = self.file_type
		tmp = self.filename[0:6].encode('ascii')
		block += tmp + b'\x00' * (6 - len(tmp))
		block += b'\x00'
		block += b'\x00' * 5
		block += b'\x00' * 3
		if self.conv_type == 'bask_tok':
			block += b'\xFC\x49'
		elif self.conv_type == 'binary':
			block += self.start_add.to_bytes(2, 'little')
		else:
			block += b'\xff\xff'
		# data length
		block += len(self.data).to_bytes(2, 'little')
		print(len(self.data))
		# checksum
		block += self.checksum().to_bytes(2, 'little')
		# 10 bytes 0xD6
		block += b'\xD6' * 10
		return block
	
	def gen_block(self):
		block = b'\xD3' * 10
		block += self.gen_head_block()
		block += self.data
		block += b'\x00' * 10
		return block

if __name__ == '__main__':
	ap = argparse.ArgumentParser()
	ap.add_argument('infile')
	ap.add_argument('outfile')
	ap.add_argument('filename')
	ap.add_argument('--start', type=str)
	ap.add_argument('--type', type=str, choices=['bastok', 'binary'])

	args = ap.parse_args()

	conv = Converter()

	fn = args.filename.upper()
	fn = fn[0:6]

	print(f'input file name : {args.infile}')
	print(f'output file name: {args.outfile}')
	print(f'file name       : {fn}')

	conv.set_infile(args.infile)

	try:
		outfile = open(args.outfile, 'wb')
	except Exception as err:
		print(err)
		exit(1)
	
	if args.type:
		conv.set_conv_type(args.type)
	else:
		conv.set_conv_type('bastxt')

	if args.start:
		start_add = int(args.start, 16)
		print(f'start address   : 0x{start_add:04x}')
		conv.set_start_add(start_add)
	
	conv.load_data()
	conv.set_infile(args.infile)
	conv.set_file_name(fn)
	outcode = conv.gen_block()
	outfile.write(outcode)
	outfile.close()
	
	exit(0)

