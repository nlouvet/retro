class SerPackets:
	
	MAX_SIZE = 320

	PRELUDE = b'\xFF'
	DATA = b'\xFE'
	END = b'\xFD'
	READY = b'\xFC'
	ACK = b'\xFB'

	ser = None
	
	def __init__(self, serobj):
		self.ser = serobj
	
	def close(self):
		self.ser.close()
	
	def send_prelude(self, size32):
		self.ser.write(self.PRELUDE)
		# 4 bytes encoding (uint32_t)
		fsize = size32.to_bytes(4, 'little')
		self.ser.write(fsize)
	
	def send_ready(self):
		self.ser.write(self.READY)
	
	def send_ack(self):
		self.ser.write(self.ACK)

	def send_end(self):
		self.ser.write(self.END)

	def send_data(self, buffer):
		#return self.__send_packet(self.DATA, buf, 0)
		self.ser.write(self.DATA)
		s = len(buffer)
		if s > self.MAX_SIZE:
			return False
		# 2 bytes encoding (uint16_t)
		pktsize = s.to_bytes(2, 'little')
		self.ser.write(pktsize)
		self.ser.write(buffer)
		
	def wait_prelude(self):
		recv_type = self.ser.read(1)
		if recv_type != self.PRELUDE:
			return 0
		else:
			# 4 bytes encoding (uint32_t)
			fsize = int.from_bytes(self.ser.read(4), 'little')
			return fsize

	def wait_ready(self):
		recv_type = self.ser.read(1)
		return recv_type == self.READY

	def wait_ack(self):
		recv_type = self.ser.read(1)
		return recv_type == self.ACK
	
	def wait_data(self):
		recv_type = self.ser.read(1)
		if recv_type != self.DATA:
			if recv_type == self.END:
				return b''
			else:
				return None
		# 2 bytes encoding (uint16_t)
		pktsize = int.from_bytes(self.ser.read(2), 'little')
		buffer = self.ser.read(pktsize)
		return buffer
