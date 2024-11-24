#!/usr/bin/env python3 

import argparse
import re

class line_tokenizer:

	code_of_token = {
		'END':		b'\x80',	'FOR':		b'\x81',	'NEXT':		b'\x82',	'DATA':		b'\x83',
		'DIM':		b'\x84',	'READ':		b'\x85',	#'':		b'\x86',
		'GO':		b'\x87',	'RUN':		b'\x88',	'IF':		b'\x89',	'RESTORE':	b'\x8A',
		'RETURN':	b'\x8B',	'REM':		b'\x8C',	#'':		b'\x8D',
		'STOP':		b'\x8E',	'ELSE':		b'\x8F',	'TRON':		b'\x90',	'TROFF':	b'\x91',
		'DEFSTR':	b'\x92',	'DEFINT':	b'\x93',	'DEFSNG':	b'\x94',	#'':		b'\x95',
		'ON':		b'\x96',	'TUNE':		b'\x97',	'ERROR':	b'\x98',	'RESUME':	b'\x99',
		'AUTO':		b'\x9A',	'DELETE':	b'\x9B',	'LOCATE':	b'\x9C',	'CLS':		b'\x9D',
		'CONSOLE':	b'\x9E',	'PSET':		b'\x9F',	'MOTOR':	b'\xA0',	'SKIPF':	b'\xA1',
		'EXEC':		b'\xA2',	'BEEP':		b'\xA3',	'COLOR':	b'\xA4',	'LINE':		b'\xA5',
		'BOX':		b'\xA6',	#'':        b'\xA7',
		'ATTRB':	b'\xA8',	'DEF':		b'\xA9',	'POKE':		b'\xAA',	'PRINT':	b'\xAB',
		'CONT':		b'\xAC',	'LIST':		b'\xAD',	'CLEAR':	b'\xAE',	'DOS':		b'\xAF',
		#'':		b'\xB0',
		'NEW':		b'\xB1',	'SAVE':		b'\xB2',	'LOAD':		b'\xB3',	'MERGE':	b'\xB4',
		'OPEN':		b'\xB5',	'CLOSE':	b'\xB6',	'INPEN':	b'\xB7',	'PEN':		b'\xB8',
		'PLAY':		b'\xB9',	'TAB(':		b'\xBA',	'TO':		b'\xBB',	'SUB':		b'\xBC',
		'FN':		b'\xBD',	'SPC(':		b'\xBE',	'USING':	b'\xBF',	'USR':		b'\xC0',
		'ERL':		b'\xC1',	'ERR':		b'\xC2',	'OFF':		b'\xC3',	'THEN':		b'\xC4',
		'NOT':		b'\xC5',	'STEP':		b'\xC6',
		'+':		b'\xC7',	'-':		b'\xC8',	'*':		b'\xC9',	'/':		b'\xCA',
		'^':		b'\xCB',	'AND':		b'\xCC',	'OR':		b'\xCD',	'XOR':		b'\xCE',
		'EQV':		b'\xCF',	'IMP':		b'\xD0',	'MOD':		b'\xD1',	'@':		b'\xD2',
		'>':		b'\xD3',	'=':		b'\xD4',	'<':		b'\xD5',
		'SGN':		b'\xFF\x80',	'INT':		b'\xFF\x81',	'ABS':		b'\xFF\x82',
		'FRE':		b'\xFF\x83',	'SQR':		b'\xFF\x84',	'LOG':		b'\xFF\x85',
		'EXP':		b'\xFF\x86',	'COS':		b'\xFF\x87',	'SIN':		b'\xFF\x88',
		'TAN':		b'\xFF\x89',	'PEEK':		b'\xFF\x8A',	'LEN':		b'\xFF\x8B',
		'STR$':		b'\xFF\x8C',	'VAL':		b'\xFF\x8D',	'ASC':		b'\xFF\x8E',
		'CHR$':		b'\xFF\x8F',	'EOF':		b'\xFF\x90',	'CINT':		b'\xFF\x91',
		#'':		b'\xFF\x92',
		#'':		b'\xFF\x93',
		'FIX':		b'\xFF\x94',
		'HEX$':		b'\xFF\x95',
		#'':		b'\xFF\x96',
		'STICK':	b'\xFF\x97',
		'STRIG':	b'\xFF\x98',	'GR$':		b'\xFF\x99',	'LEFT$':	b'\xFF\x9A',
		'RIGHT$':	b'\xFF\x9B',	'MID$':		b'\xFF\x9C',	'INSTR':	b'\xFF\x9D',
		'VARPTR':	b'\xFF\x9E',	'RND':		b'\xFF\x9F',	'INKEY$':	b'\xFF\xA0',
		'INPUT':	b'\xFF\xA1',	'CSRLIN':	b'\xFF\xA2',	'POINT':	b'\xFF\xA3',
		'SCREEN':	b'\xFF\xA4',	'POS':		b'\xFF\xA5',	'PTRIG':	b'\xFF\xA6'
	}

	words = {
		1 : ["'", '+', '-', '*', '/', '^', '@', '>', '=', '<'],
		2 : ['GO', 'IF', 'ON', 'TO', 'FN', 'OR'],
		3 : ['END', 'FOR', 'DIM', 'RUN', 'REM', 'CLS', 'BOX', 'DEF',
				'DOS', 'NEW', 'PEN', 'SUB', 'USR', 'ERL', 'ERR', 'OFF',
				'NOT', 'AND', 'XOR', 'EQV', 'IMP', 'MOD', 'SGN', 'INT',
				'ABS', 'FRE', 'SQR', 'LOG', 'EXP', 'COS', 'SIN', 'TAN',
				'LEN', 'VAL', 'ASC', 'EOF', 'FIX', 'GR$', 'RND', 'POS'],
		4 : ['NEXT', 'DATA', 'READ', 'STOP', 'ELSE', 'TRON', 'TUNE', 'AUTO',
				'PSET', 'EXEC', 'BEEP', 'LINE', 'POKE', 'CONT', 'LIST', 'SAVE',
				'LOAD', 'OPEN', 'PLAY', 'TAB(', 'SPC(', 'THEN', 'STEP', 'PEEK',
				'STR$', 'CHR$', 'CINT', 'HEX$', 'MID$'],
		5 : ['TROFF', 'ERROR', 'MOTOR', 'SKIPF', 'COLOR', 'ATTRB', 'PRINT',
				'CLEAR', 'MERGE', 'CLOSE', 'INPEN', 'USING', 'STICK', 'STRIG',
				'LEFT$', 'INSTR', 'INPUT', 'POINT', 'PTRIG'],
		6 : ['RETURN', 'DEFSTR', 'DEFINT', 'DEFSNG', 'RESUME', 'DELETE',
				'LOCATE', 'RIGHT$', 'VARPTR', 'INKEY$', 'CSRLIN', 'SCREEN'],
		7 : ['RESTORE', 'CONSOLE']
	}

	text = ''
	length = 0

	def __init__(self, textin):
		self.text = textin
		self.length = len(self.text)
	
	def match_words(self, i):
		for l in range(7, 0, -1): # l = 7,...,1
			w = self.text[i:i+l]
			if w in self.words[l]:
				return (w, l)
		return None
	
	def search_prefix_word(self, v):
		for l in range(7, 0, -1): # l = 7,...,1
			w = v[0:l]
			if w in self.words[l]:
				return (w, l)
		return None

	def is_symbol(self, c):
		return (c in self.words[1])

	def get_code(self):
		num = -1
		code = b''
		m = re.match(r'^\s*$', self.text)
		if m: # empty lines are ignored
			return None
		m = re.match(r'^ *([0-9]+) ?', self.text)
		if not m:
			print("lines must start with a line number...")
			return None
		num = int(m.group(1)) # line number
		i = m.end(0) ###
		while i < self.length:
			# try to extract a potential basic word to tokenize
			m = re.match(r'^([A-Z]+)\$?\(?', self.text[i:self.length])
			if m:
				# try to find the longest prefix of the matched
				# word that corresponds to a basic word
				n = self.search_prefix_word(m.group(0))
				if n:
					# we have found a basic word of length l
					(w, l) = n
					code += self.code_of_token[w]
					i += l
					if w == 'REM': # comment
						code += self.text[i:-1].encode('ascii')
						i = self.length
				else:
					# no basic word found as a prefix
					code += m.group(1).encode('ascii')
					i += len(m.group(1))
			elif self.is_symbol(self.text[i]):
				code += self.code_of_token[self.text[i]]
				i += 1
				if self.text[i] == "'": # comment
					code += self.text[i:-1].encode('ascii')
					i = self.length
			else:
				code += self.text[i].encode('ascii')
				i += 1
		return (num, code)

if __name__ == '__main__':
	ap = argparse.ArgumentParser()
	ap.add_argument('infile')
	ap.add_argument('outfile')
	args = ap.parse_args()

	print(f'input file name : {args.infile}')
	print(f'output file name: {args.outfile}')

	try:
		infile = open(args.infile, 'r')
	except Exception as err:
		print(err)
		exit(1)
	
	next_add = 9636
	code = b''
	for line in infile:
		print(line.strip())
		lt = line_tokenizer(line.strip())
		res = lt.get_code()
		if res:
			(num, cl) = res
			next_add += len(cl) + 5
			codeline = b''
			codeline += int.to_bytes(next_add, 2, 'big')
			codeline += int.to_bytes(num, 2, 'big')
			codeline += cl
			codeline += b'\x00'
			print('  =>', ''.join(f'{w:02X}' for w in codeline))
			code += codeline
	infile.close()

	code += b'\x00\x00'
	length = len(code)
	code = b'\xFF' + int.to_bytes(length, 2, 'big') + code

	try:
		outfile = open(args.outfile, 'wb')
		outfile.write(code)
		outfile.close()
	except Exception as err:
		print(err)
		exit(1)
