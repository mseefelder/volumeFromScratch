import struct
import ctypes

width = input("Width? ")
height = input("Height? ")
pixels = width*height;

target = open("transfer.raw", 'a')

mys = ''
for i in range(pixels):
	target.write(struct.pack('B', max(((i-(pixels/10))%256), 0)))
	target.write(struct.pack('B', 255))
	target.write(struct.pack('B', 255))
	target.write(struct.pack('B', max(((i-(pixels/10))%256), 0)))

target.close()