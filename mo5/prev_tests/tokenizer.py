#!/usr/bin/env python3 

import argparse

code_of_token = {
    'END':		b'\x80',
    'FOR':		b'\x81',
    'NEXT':		b'\x82',
    'DATA':		b'\x83',
    'DIM':		b'\x84',
    'READ':		b'\x85',
    #'':		b'\x86',
    'GO':		b'\x87',
    'RUN':		b'\x88',
    'IF':		b'\x89',
    'RESTORE':	b'\x8A',
    'RETURN':	b'\x8B',
    'REM':		b'\x8C',
    #'':		b'\x8D',
    'STOP':		b'\x8E',
    'ELSE':		b'\x8F',
    'TRON':		b'\x90',
    'TROFF':	b'\x91',
    'DEFSTR':	b'\x92',
    'DEFINT':	b'\x93',
    'DEFSNG':	b'\x94',
    #'':		b'\x95',
    'ON':		b'\x96',
    'TUNE':		b'\x97',
    'ERROR':	b'\x98',
    'RESUME':	b'\x99',
    'AUTO':		b'\x9A',
    'DELETE':	b'\x9B',
    'LOCATE':	b'\x9C',
    'CLS':		b'\x9D',
    'CONSOLE':	b'\x9E',
    'PSET':		b'\x9F',
    'MOTOR':	b'\xA0',
    'SKIPF':	b'\xA1',
    'EXEC':		b'\xA2',
    'BEEP':		b'\xA3',
    'COLOR':	b'\xA4',
    'LINE':		b'\xA5',
    'BOX':		b'\xA6',
    #'':        b'\xA7',
    'ATTRB':	b'\xA8',
    #'DEF':		b'\xA9',
	'DEFGR$':	b'\xA9\xFF\x99', ####
    'POKE':		b'\xAA',
    'PRINT':	b'\xAB',
    'CONT':		b'\xAC',
    'LIST':		b'\xAD',
    'CLEAR':	b'\xAE',
    'DOS':		b'\xAF',
    #'':		b'\xB0',
    'NEW':		b'\xB1',
    'SAVE':		b'\xB2',
    'LOAD':		b'\xB3',
    'MERGE':	b'\xB4',
    'OPEN':		b'\xB5',
    'CLOSE':	b'\xB6',
    'INPEN':	b'\xB7',
    'PEN':		b'\xB8',
    'PLAY':		b'\xB9',
    'TAB(':		b'\xBA',
    'TO':		b'\xBB',
    'SUB':		b'\xBC',
    'FN':		b'\xBD',
    'SPC(':		b'\xBE',
    'USING':	b'\xBF',
    'USR':		b'\xC0',
    'ERL':		b'\xC1',
    'ERR':		b'\xC2',
    'OFF':		b'\xC3',
    'THEN':		b'\xC4',
    'NOT':		b'\xC5',
    'STEP':		b'\xC6',
    '+':		b'\xC7',
    '-':		b'\xC8',
    '*':		b'\xC9',
    '/':		b'\xCA',
    '^':		b'\xCB',
    'AND':		b'\xCC',
    'OR':		b'\xCD',
    'XOR':		b'\xCE',
    'EQV':		b'\xCF',
    'IMP':		b'\xD0',
    'MOD':		b'\xD1',
    '@':		b'\xD2',
    '>':		b'\xD3',
    '=':		b'\xD4',
    '<':		b'\xD5',
    'SGN':		b'\xFF\x80',
    'INT':		b'\xFF\x81',
    'ABS':		b'\xFF\x82',
    'FRE':		b'\xFF\x83',
    'SQR':		b'\xFF\x84',
    'LOG':		b'\xFF\x85',
    'EXP':		b'\xFF\x86',
    'COS':		b'\xFF\x87',
    'SIN':		b'\xFF\x88',
    'TAN':		b'\xFF\x89',
    'PEEK':		b'\xFF\x8A',
    'LEN':		b'\xFF\x8B',
    'STR$':		b'\xFF\x8C',
    'VAL':		b'\xFF\x8D',
    'ASC':		b'\xFF\x8E',
    'CHR$':		b'\xFF\x8F',
    'EOF':		b'\xFF\x90',
    'CINT':		b'\xFF\x91',
    'FF92':		b'\xFF\x92',
    'FF93':		b'\xFF\x93',
    'FIX':		b'\xFF\x94',
    'HEX$':		b'\xFF\x95',
    'FF96':		b'\xFF\x96',
    'STICK':	b'\xFF\x97',
    'STRIG':	b'\xFF\x98',
    'GR$':		b'\xFF\x99',
    'LEFT$':	b'\xFF\x9A',
    'RIGHT$':	b'\xFF\x9B',
    'MID$':		b'\xFF\x9C',
    'INSTR':	b'\xFF\x9D',
    'VARPTR':	b'\xFF\x9E',
    'RND':		b'\xFF\x9F',
    'INKEY$':	b'\xFF\xA0',
    'INPUT':	b'\xFF\xA1',
    'CSRLIN':	b'\xFF\xA2',
    'POINT':	b'\xFF\xA3',
    'SCREEN':	b'\xFF\xA4',
    'POS':		b'\xFF\xA5',
    'PTRIG':	b'\xFF\xA6'
}

basic_symbols = ['+', '-', '*', '/', '^', '@', '>', '=', '<']

class line_tokenizer:
    text = ''
    l = 0
    i = 0

    def __init__(self, textin):
        self.text = textin
        self.l = len(self.text)
        self.i = 0
    
    def get_char(self):
        if self.i < self.l:
            c = self.text[self.i]
            self.i += 1
        else:
            c = ''
        return c

    def syntax_error(self):
        print("Syntax error")

    def get_code(self):
        code = b''
        num = -1

        c = self.get_char()
        while c == ' ':
            c = self.get_char()
        if c.isdigit():
            prefix = ''
            while c.isdigit():
                prefix += c
                c = self.get_char()
            num = int(prefix)
            if c == ' ':
                c = self.get_char()
        # here, c is the first non-digit character
        while c != '':
            if c == '"': # read a string
                prefix = '"'
                while True:
                    c = self.get_char()
                    prefix += c
                    if c == '"':
                        code += prefix.encode('ascii')
                        c = self.get_char()
                        break
                    elif c == '':
                        syntax_error()
                        return b''
            elif c.isdigit() or c == '.':
                # read a decimal number
                prefix = ''
                nbpts = 0 # point symbol count
                nbexp = 0 # E symbol count
                nbdap = 0 # digits after the point
                while True:
                    if c.isdigit():
                        if nbpts == 1:
                            nbdap += 1
                        prefix += c
                        c = self.get_char()
                    elif  c == '.':
                        nbpts += 1
                        if nbpts == 2 or nbexp == 1:
                            syntax_error()
                            return b''
                        prefix += c
                        c = self.get_char()
                    elif c == 'E':
                        nbexp += 1
                        if nbexp == 2:
                            syntax_error()
                            return b''
                        prefix += c
                        c = self.get_char()
                    else:
                        if nbpts == 1 and nbdap == 0:
                            syntax_error()
                            return b''
                        # finished reading a decimal number
                        # c is not part of this number
                        code += prefix.encode('ascii')
                        break
            elif c.isalpha():
                # read a basic word or a variable name
                prefix = ''
                c = c.upper()
                while True:
                    if c.isalpha():
                        prefix += c.upper()
                        if prefix in code_of_token:
                            # finished reading a basic word
                            code += code_of_token[prefix]
                            c = self.get_char()
                            break
                        else:
                            c = self.get_char()
                    elif c == '(' or c == '$':
                        if prefix + c in code_of_token:
                            # finished reading a basic word
                            prefix += c
                            code += code_of_token[prefix]
                            c = self.get_char()
                            break
                        else:
                            # finished reading a variable name
                            # c is not part of this name
                            code += prefix.encode('ascii')
                            break
                    elif c.isdigit():
                        prefix += c
                        c = self.get_char()
                    else:
                        # finished reading a variable name
                        # c is not part of this name
                        code += prefix.encode('ascii')
                        break
            elif c in basic_symbols:
                code += code_of_token[c]
                c = self.get_char()
            else:
                code += c.encode('ascii')
                c = self.get_char()
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
		(num, cl) = lt.get_code()
		next_add += len(cl) + 5
		codeline = b''
		codeline += int.to_bytes(next_add, 2, 'big')
		codeline += int.to_bytes(num, 2, 'big')
		codeline += cl
		codeline += b'\x00'
		print('=>', ''.join(f'{w:02X}' for w in codeline))
		code += codeline
	infile.close()

	code += b'\x00\x00'
	length = len(code)
	code = b'\xFF' + int.to_bytes(length, 2, 'big') + code
	print('code produced:')
	print(''.join(f'{w:02X}' for w in code))

	try:
		outfile = open(args.outfile, 'wb')
		outfile.write(code)
		outfile.close()
	except Exception as err:
		print(err)
		exit(1)
