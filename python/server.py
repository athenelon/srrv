import socket
import cv2
import numpy as np

def recvAll( client ):
	msgSize = 1366*768*3
	data = b''

	while True:
		msg = client.recv( 3147264 )
		data += msg
		msgSize -= len( msg )
		if( msgSize <= 0 ):
			break ;

	return data

sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

sock.bind(( 'localhost', 5000 ))
sock.listen( 1 )
client, addr = sock.accept( )
print ("ACCEPTED")

while True:
	img = recvAll( client )

	frame = np.fromstring( img, dtype='uint8').reshape( 768, 1366, 3 )

	cv2.imshow( "frame", frame )

	if cv2.waitKey( 1 ) == 27:
		break


cv2.destroyAllWindows( )
