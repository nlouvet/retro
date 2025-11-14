#!/usr/bin/env python3

import argparse, re

# dictionnary mapping token-numbers to their token-strings
keywords = { 128: 'END', 129: 'FOR', 130: 'NEXT', 131: 'DATA', 132: 'INPUT',
	     133: 'DIM', 134: 'READ', 135: 'LET', 136: 'GOTO', 137: 'RUN',
	     138: 'IF', 139: 'RESTORE', 140: 'GOSUB', 141: 'RETURN', 142: 'REM',
	     143: 'STOP', 144: 'ON', 145: 'LPRINT', 146: 'DEF', 147: 'POKE',
	     148: 'PRINT', 149: 'CONT', 150: 'LIST', 151: 'LLIST', 152: 'CLEAR',
	     153: 'RENUM', 154: 'AUTO', 155: 'LOAD', 156: 'SAVE', 157: 'CLOAD',
	     158: 'CSAVE', 159: 'CALL', 160: 'INIT', 161: 'SOUND', 162: 'PLAY',
	     163: 'TX', 164: 'GR', 165: 'SCREEN', 166: 'DISPLAY', 167: 'STORE',
	     168: 'SCROLL', 169: 'PAGE', 170: 'DELIM', 171: 'SETE', 172: 'ET',
	     173: 'EG', 174: 'CURSOR', 175: 'DISK', 176: 'MODEM', 177: 'NEW',
	     178: 'TAB(', 179: 'TO', 180: 'FN', 181: 'SPC(', 182: 'THEN',
	     183: 'NOT', 184: 'STEP', 185: '+', 186: '-', 187: '*', 188: '/',
	     189: '^', 190: 'AND', 191: 'OR', 192: '>', 193: '=', 194: '<',
	     195: 'SGN', 196: 'INT', 197: 'ABS', 198: 'USR', 199: 'FRE',
	     200: 'LPOS', 201: 'POS', 202: 'SQR', 203: 'RND', 204: 'LOG',
	     205: 'EXP', 206: 'COS', 207: 'SIN', 208: 'TAN', 209: 'ATN',
	     210: 'PEEK', 211: 'LEN', 212: 'STR$', 213: 'VAL', 214: 'ASC',
	     215: 'STICKX', 216: 'STICKY', 217: 'ACTION', 218: 'KEY', 219: 'LPEN',
	     220: 'CHR$', 221: 'LEFT$', 222: 'RIGHT$', 223: 'MID$' }

# dictionnary mapping token-strings to token-numbers
tokens = { keywords[n] : n for n in keywords }

# list of the "small" tokens
oplist = ['+', '-', '*', '/', '^', '>', '=', '<']

# list of the tokens ending with a '('
plist = ['TAB(', 'SPC(']

# list of the tokens ending with a '$'
dlist = ['STR$', 'CHR$', 'LEFT$', 'RIGHT$', 'MID$']

# preparing a regular expression to match the token-strings
patterns = [t for t in tokens if not (t in oplist + plist + dlist)]
patterns += ['\\' + op for op in oplist]
patterns += [t.replace('(', '[(]') for t in plist]
patterns += [t.replace('$', '[$]') for t in dlist]
patterns = '|'.join(patterns)
patterns = re.compile(patterns)

class line_tokenizer:
	
	text = ''
	length = 0
	
	def __init__(self, textin):
		self.text = textin
		self.length = len(self.text)

	def get_code(self):
		num = None
		code = b''
		i = 0
		m = re.match(r'^\s*$', self.text)
		if m: # empty lines are ignored
			return None
		m = re.match(r'^ *([0-9]+) ?', self.text)
		if m:
			num = int(m.group(1)) # line number
			i = m.end(0) # current index in the line
		while i < self.length:
			# try to extract a potential basic word to tokenize
			m = patterns.match(self.text[i:self.length])
			if m:
				# a token was found, we convert it into a byte
				t = m.group(0)
				code += tokens[t].to_bytes(1, 'little')
				i += len(t)
				if t == 'REM':
					# we just encode the rest of the line
					code += self.text[i:self.length].encode('ascii')
					i = self.length
				elif t == 'GOTO' or t == 'GOSUB':
					# try to extract a line number after these tokens
					m = re.match(r' *[0-9]+', self.text[i:self.length])
					if (not m) or (int(m.group(0)) > 65535):
						print("Invalid line number at {}".format(i))
						return None
					else:
						# if a line number is found, the token
						# (GOTO or GOSUB) is followed by a line
						# number indicator 0x0E, then by the line
						# number as an integer encoded on 2 bytes
						n = m.group(0)
						code += b'\x0e'
						code += int.to_bytes(int(n), 2, 'little')
						i += len(n)
			elif self.text[i] == '"':
				code += self.text[i].encode('ascii')
				i += 1
				# must match the rest of a string
				m = re.match(r'[^"]*["]', self.text[i:self.length])
				if m:
					r = m.group(0)
					code += r.encode('ascii')
					i += len(r)
				else:
					print("Unterminated string at {}".format(i))
					return None
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
        
        next_add = 0x4a01
        code = b''
        for line in infile:
                print(line.strip())
                lt = line_tokenizer(line.strip())
                res = lt.get_code()
                if res:
                        (num, cl) = res
                        next_add += len(cl) + 2 + 2 + 1
                        codeline = b''
                        codeline += int.to_bytes(next_add - 5, 2, 'little')
                        codeline += int.to_bytes(num, 2, 'little')
                        codeline += cl
                        codeline += b'\x00'
                        print('  =>', ''.join(f'{w:02X}' for w in codeline))
                        code += codeline
        infile.close()

        code += b'\x00\x00'
	
        try:
                outfile = open(args.outfile, 'wb')
                outfile.write(code)
                outfile.close()
        except Exception as err:
                print(err)
                exit(1)

