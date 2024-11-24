#!/usr/bin/env python3 

import argparse

# file type: 00=Basic 01=Data 02=Binaire
BASIC_FILE = b'\x00'
DATA_FILE = b'\x01'
BINARY_FILE = b'\x02'
# file mode: 00=Binaire FF=Texte
BINARY_MODE = b'\x00'
TEXT_MODE = b'\xFF'

class Converter:
	infile = None
	data = b''
	basename = 'NONAME'
	extension = 'EXT'
	conv_type = ''
	file_type = BASIC_FILE
	file_mode = TEXT_MODE
	start_add = 0x6000
	exec_add = 0x0000

	def prep_basic(self):
		try:
			f = open(self.infile, 'r')
		except Exception as err:
			print(err)
			exit(1)
		self.data = '\r'.encode('ascii')
		for line in f:
			line = line.strip()
			try:
				self.data += (line.encode('ascii') + b'\x0d')
			except:
				print("ASCII problem with:", line)
				exit(1)
		f.close()

	def prep_prgbin(self):
		try:
			f = open(self.infile, 'rb')
		except Exception as err:
			print(err)
			exit(1)
		self.data = f.read()
		f.close()

	def prep_plainbin(self):
		try:
			f = open(self.infile, 'rb')
		except Exception as err:
			print(err)
			exit(1)
		self.data = b''
		self.data += BINARY_MODE
		tmp = f.read()
		l = int.to_bytes(len(tmp), 2, 'big')
		self.data += l
		self.data += int.to_bytes(self.start_add, 2, 'big')
		self.data += tmp
		self.data += b'\xff\x00\x00' + int.to_bytes(self.exec_add, 2, 'big')
		f.close()

	def set_infile(self, name):
		self.infile = name

	def set_file_name(self, filename):
		fn = filename.split('.')
		if len(fn) > 2:
			print('Invalid file name provided.')
			return
		if len(fn[0]) <= 8:
			self.basename = fn[0].upper()
		else:
			print('Invalid file name provided.')
			return
		if len(fn) == 1:
			self.extension = ''
		elif len(fn[1]) <= 3:
			self.extension = fn[1].upper()
		else:
			print('Invalid file name provided.')
			return

	def set_infile(self, name):
		self.infile = name

	def set_start_add(self, add):
		self.start_add = add
	
	def set_exec_add(self, add):
		self.exec_add = add

	def set_conv_type(self, type):
		if type == 'bastxt':
			self.conv_type = 'bastxt'
			self.file_type =  BASIC_FILE
			self.file_mode = TEXT_MODE
		elif type == 'bastok':
			self.conv_type = 'bastok'
			self.file_type =  BASIC_FILE
			self.file_mode = BINARY_MODE
		elif type == 'plainbin':
			self.conv_type = 'plainbin'
			self.file_type =  BINARY_FILE
			self.file_mode = BINARY_MODE
		elif type == 'prgbin':
			self.conv_type = 'prgbin'
			self.file_type =  BINARY_FILE
			self.file_mode = BINARY_MODE
		else:
			print('Unknown conversion type...')

	def __init__(self):
		pass
	
	def checksum(sefl, chunck):
		c = 0
		for w in chunck:
			c = (c + w) % 256
		return c

	def gen_synchro(self):
		return b'\x01' * 16 + b'\x3c\x5a'
	
	def gen_head_block(self):
		block = self.gen_synchro()
		# block type is "heading" = \x00
		block += b'\x00'
		# block length is 16 = \x10
		block += b'\x10'
		content = b''
		# file name
		content += '{:8s}'.format(self.basename).encode('ascii')
		#file extension
		content += '{:3s}'.format(self.extension).encode('ascii')
		# file type: 00=Basic 01=Data 02=Binaire
		content += self.file_type
		# file mode: 00=Binaire FF=Texte
		content += self.file_mode
		# same byte as the previous one
		content += self.file_mode
		block += content
		# checksum
		block += (256 - self.checksum(content)).to_bytes(1, 'little')
		return block
	
	def gen_end_block(self):
		code = self.gen_synchro()
		# block type is "ending"
		code += b'\xff'
		# block length is 2 = \x02
		code += b'\x02'
		# checksum is 0
		code += b'\x00'   
		return code 

	def gen_block(self, data):
		block = self.gen_synchro()
		# block type is "file content"
		block += b'\x01'
		# block length
		block += ((len(data) + 2) % 256).to_bytes(1, 'little')
		# block content
		block += data
		# checksum
		block += ((256 - self.checksum(data)) % 256).to_bytes(1, 'little')
		return block

	def gen_blocks(self):
		blocks = []
		l = len(self.data)
		i = 0
		j = 254
		while j < l:
			tmp = self.gen_block(self.data[i:j])
			blocks.append(self.gen_block(self.data[i:j]))
			i += 254
			j += 254
		if i < l:
			blocks.append(self.gen_block(self.data[i:l]))
		return blocks

	def print_block(self, block):
		print(f'[{len(block):d}] = ', end='')
		print('{', end='')
		print(''.join(f'0x{w:02x}, ' for w in block[0:-1]), end='')
		print(f'0x{block[-1]:02x}', end='')
		print('}')
	
	def cvt(self, verbose = False):
		if self.conv_type == 'bastxt':
			self.prep_basic()
		elif self.conv_type == 'plainbin':
			self.prep_plainbin()
		elif self.conv_type in ['prgbin', 'bastok']:
			self.prep_prgbin()
		else:
			pass

		outcode = b''
		lengths = []

		head = self.gen_head_block()
		outcode += head
		l = len(head)
		lengths.append(l)
		print(f'Head blockk generated ({l:d} bytes)')

		blocks = self.gen_blocks()
		i = 0
		for block in blocks:
			outcode += block
			l = len(block)
			lengths.append(len(block))
			print(f'Block #{i:d} generated ({l:d} bytes)')
			i += 1
		
		ending = self.gen_end_block()
		outcode += ending
		l = len(ending)
		lengths.append(len(ending))
		print(f'End block #{i:d} generated ({l:d} bytes)')

		if verbose:
			print('Code:')
			print('{', end='')
			i = 1
			for w in outcode:
				if i == len(outcode):
					print(f'0x{w:02x}'+'};', end='')
				else:
					print(f'0x{w:02x},', end='')
				if i%8 == 0:
					print()
					print(' ', end='')
				else:
					print(' ', end='')
				i += 1
			print()

			print('Lengths:')
			print('{', end='')
			i = 1
			for l in lengths:
				if i == len(lengths):
					print(f'{l:d}'+'};', end='')
				else:
					print(f'{l:d}, ', end='')
				i += 1
			
			print()

		return outcode

if __name__ == '__main__':
	ap = argparse.ArgumentParser()
	ap.add_argument('infile')
	ap.add_argument('outfile')
	ap.add_argument('filename')
	ap.add_argument('--start', type=str)
	ap.add_argument('--exec', type=str)
	ap.add_argument('--type', type=str, choices=['bastxt', 'bastok', 'plainbin', 'prgbin'])

	args = ap.parse_args()

	conv = Converter()

	fn = args.filename.split('.')
	print(f'Input file name : {args.infile}')
	print(f'Output file name: {args.outfile}')
	print(f'File name       : {args.filename}')

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
		print(f'Start address   : 0x{start_add:04x}')
		conv.set_start_add(start_add)

	if args.exec:
		exec_add = int(args.exec, 16)
		print(f'Exec address    : 0x{exec_add:04x}')
		conv.set_exec_add(exec_add)

	conv.set_infile(args.infile)
	conv.set_file_name(args.filename)
	outcode = conv.cvt(verbose = False)
	outfile.write(outcode)
	outfile.close()
	
	exit(0)

