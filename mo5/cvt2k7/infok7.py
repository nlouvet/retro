#!/usr/bin/env python3 

import argparse

class Converter:

	infile = None
	data = b''

	def prep_basic(self):
		self.data = b''
		for line in infile:
			line = line.strip()
			try:
				self.data += (line.encode('ascii') + b'\x0d')
			except:
				print(line)
				exit(1)

	def __init__(self, infile_):
		self.infile = infile_
		self.infile.seek(0)
		self.prep_basic()

	def checksum(sefl, chunck):
		c = 0
		for w in chunck:
			c = (c + w) % 256
		return c

	def gen_synchro(self):
		return b'\x01' * 16 + b'\x3c\x5a'
	
	def gen_head_bloc(self, name, ext):
		bloc = self.gen_synchro()
		# bloc type is "heading" = \x00
		bloc += b'\x00'
		# bloc length is 16 = \x10
		bloc += b'\x10'
		content = b''
		# file name
		content += '{:8s}'.format(name).encode('ascii')
		#file extension
		content += '{:3s}'.format(ext).encode('ascii')
		# file type: 00=Basic 01=Data 02=Binaire
		content += b'\x00'
		# file mode: 00=Binaire FF=Texte
		content += b'\xff'
		# same byte as the previous one
		content += b'\xff'
		bloc += content
		# checksum
		bloc += (256 - checksum(content)).to_bytes(1, 'little')
		return bloc
	
	def gen_end_bloc(self):
		code = self.gen_synchro()
		# bloc type is "ending"
		code += b'\xff'
		# bloc length is 2 = \x02
		code += b'\x02'
		# checksum is 0
		code += b'\x00'   
		return code 

	def gen_bloc(self, data):
		bloc = self.gen_synchro()
		# bloc type is "file content"
		bloc += b'\x01'
		# bloc length
		bloc += ((len(data) + 2) % 256).to_bytes(1, 'little')
		# bloc content
		bloc += data
		# checksum
		bloc += ((256 - checksum(data)) % 256).to_bytes(1, 'little')
		return bloc

	def gen_blocs(self):
		blocs = []
		l = len(self.data)
		i = 0
		j = 254
		while j < l:
			tmp = self.gen_bloc(self.data[i:j])
			blocs.append(self.gen_bloc(self.data[i:j]))
			i += 254
			j += 254
		if i < l:
			blocs.append(self.gen_bloc(self.data[i:l]))
		return blocs

	def print_bloc(self, bloc):
		print(f'[{len(bloc):d}] = ', end='')
		print('{', end='')
		print(''.join(f'0x{w:02x}, ' for w in bloc[0:-1]), end='')
		print(f'0x{bloc[-1]:02x}', end='')
		print('}')

	def cvt_basic_fmt1(self, name, ext, verbose = False):
		outcode = b''
		head = self.gen_head_bloc(name, ext)
		outcode += head
		if verbose:
			print("Head bloc:")
			self.print_bloc(head)
		blocs = self.gen_blocs()
		i = 0
		for bloc in blocs:
			outcode += bloc
			if verbose:
				print(f'Bloc #{i:d}:')
				self.print_bloc(bloc)
				i += 1
		ending = self.gen_end_bloc()
		outcode += ending
		if verbose:
			print("End bloc:")
			self.print_bloc(ending)
		return outcode

	def cvt_basic_txt(self, name, ext, verbose = False):
		outcode = b''
		lengths = []

		head = self.gen_head_bloc(name, ext)
		outcode += head
		l = len(head)
		lengths.append(l)
		print(f'Head block generated ({l:d} bytes)')

		blocs = self.gen_blocs()
		i = 0
		for bloc in blocs:
			outcode += bloc
			l = len(bloc)
			lengths.append(len(bloc))
			print(f'Block #{i:d} generated ({l:d} bytes)')
			i += 1
		
		ending = self.gen_end_bloc()
		outcode += ending
		l = len(ending)
		lengths.append(len(ending))
		print(f'End block #{i:d} generated ({l:d} bytes)')

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


def prep_code(infile):
	code = b''
	for line in infile:
		#line = line.upper().strip()
		line = line.strip()
		#print(line)
		code += (line.encode('ascii') + b'\x0d')
	return code

def checksum(code):
	c = 0
	for w in code:
		c = (c + w) % 256
	return c

def synchro():
	return b'\x01' * 16 + b'\x3c\x5a'

def heading_bloc(name, ext):
	bloc = b''
	bloc += b'\x00'     # bloc type is "heading"
	bloc += b'\x10'     # bloc length is 16 = \x10
	data = b''
	data += '{:8s}'.format(name).encode('ascii') # file name
	data += '{:3s}'.format(ext).encode('ascii')  #file extension
	data += b'\x00'     # file type: 00=Basic 01=Data 02=Binaire
	data += b'\xff'     # file mode: 00=Binaire FF=Texte
	data += b'\xff'     # same byte as the previous one
	bloc += data
	bloc += (256 - checksum(data)).to_bytes(1, 'little')
	return bloc

def ending_bloc():
	code = b''
	# bloc type is "ending"
	code += b'\xff'
	# bloc length is 2 = \x02
	code += b'\x02'
	# checksum is 0
	code += b'\x00'   
	return code 


def file_bloc(data):
	bloc = b''
	# bloc type is "file content"
	bloc += b'\x01'
	# bloc length
	bloc += ((len(data) + 2) % 256).to_bytes(1, 'little')
	# bloc content
	bloc += data
	# checksum
	bloc += (256 - checksum(data)).to_bytes(1, 'little')
	return bloc

def file_blocs(data):
	code = b''
	l = len(data)
	i = 0
	j = 254
	while j < l:
		tmp = data[i:j]
		code += synchro()
		code += file_bloc(tmp)
		i += 254
		j += 254
	if i < l:
		tmp = data[i:l]
		code += synchro()
		code += file_bloc(tmp)
	return code

def cvt(infile, name, ext):
	outcode = b''
	outcode += synchro()
	outcode += heading_bloc(name, ext)
	outcode += file_blocs(prep_code(infile))
	outcode += synchro()
	outcode += ending_bloc()
	return outcode

def info_file(infile):
	
	i = 0

	while(True):

		print(f'Reading block {i:d}')
		cur_block = b''

		buffer = infile.read(18)
		if not buffer:
			print('end of file')
			return
		if len(buffer) != 18 or buffer != b'\x01' * 16 + b'\x3c' + b'\x5a':
			print('the file is ill-formed (1)')
			return
		cur_block += buffer

		buffer = infile.read(2)
		if len(buffer) != 2:
			print('the file is ill-formed (2)')
			return
		cur_block += buffer
		cur_block_type = buffer[0]
		cur_block_length = buffer[1] #int.from_bytes(buffer[1], 'little')
		if cur_block_length == 0:
			cur_block_length = 256
		
		buffer = infile.read(cur_block_length-1)
		if len(buffer) != cur_block_length-1:
			print('the file is ill-formed (2)')
			return
		cur_block += buffer

		print('  block type: {:02x}'.format(cur_block_type))
		print('  total size: {}'.format(len(cur_block)))
		i += 1

if __name__ == '__main__':
	ap = argparse.ArgumentParser()
	ap.add_argument('file')
	args = ap.parse_args()
	print(f'Input file name : {args.file}')

	try:
		infile = open(args.file, 'rb')
	except Exception as err:
		print(err)
		exit(1)
	
	info_file(infile)

	infile.close()
	
	exit(0)

