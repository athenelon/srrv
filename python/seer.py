import numpy as np
import pyscreenshot as ss
import socket
import sys
import cv2

sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
sock.connect(( 'localhost', 5000 ))

while True:
	img = ss.grab( )	
	img_np = np.array(img)
	frame = cv2.cvtColor(img_np, cv2.COLOR_BGR2RGB)

	sock.sendall( frame )

sock.close( )