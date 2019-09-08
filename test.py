import cv2
import tkinter as tk
from PIL import Image, ImageTk

class Effects:

	setEffect = 0 
	setFlipMode = 1
	setImage = 0

	buttonImgHeight = 60
	buttonImgWidth = 80

	def __init__( self, tkWin ):
		
		self.cap = cv2.VideoCapture( 0 )

		self.root = tkWin

		self.buttonFrame = tk.Frame( self.root )
		self.labelFrame = tk.Frame( self.root )
		self.scalerFrame = tk.Frame( self.root )

		self.buttonFrame.pack( side=tk.BOTTOM )
		self.labelFrame.pack( side=tk.BOTTOM )
		self.scalerFrame.pack( side=tk.BOTTOM )

		self.scale = tk.Scale( self.scalerFrame, length=500, orient=tk.HORIZONTAL )
		self.scale.pack( )

		self.button1 = tk.Button( self.buttonFrame, text="No effect", command=self.setNoEffect )
		self.button2 = tk.Button( self.buttonFrame, text="Negative", command=self.setNegative )
		self.button3 = tk.Button( self.buttonFrame, text="Edge Detection", command=self.setEdgeDetection )
		self.button4 = tk.Button( self.buttonFrame, text="Heat Map", command=self.setHeatMap )
		self.button5 = tk.Button( self.buttonFrame, text="Gray Scale", command=self.setGrayScale )
		self.button6 = tk.Button( self.buttonFrame, text="Binary", command=self.setBinary )
		self.button7 = tk.Button( self.buttonFrame, text="Flip", command=self.setFlip )
		self.button8 = tk.Button( self.buttonFrame, text="Load Web Camera", command=self.setWebCam )
		self.button9 = tk.Button( self.buttonFrame, text="Load Picture", command=self.setPicture )

		self.button1.pack( side=tk.LEFT )
		self.button2.pack( side=tk.LEFT )
		self.button3.pack( side=tk.LEFT )
		self.button4.pack( side=tk.LEFT )
		self.button5.pack( side=tk.LEFT )
		self.button6.pack( side=tk.LEFT )
		self.button7.pack( side=tk.LEFT )
		self.button8.pack( side=tk.LEFT )
		self.button9.pack( side=tk.LEFT )

		
		self.label1 = tk.Label( self.labelFrame, text="No effect" )
		self.label2 = tk.Label( self.labelFrame, text="Negative" )
		self.label3 = tk.Label( self.labelFrame, text="Edge Detection" )
		self.label4 = tk.Label( self.labelFrame, text="Heat Map" )
		self.label5 = tk.Label( self.labelFrame, text="Gray Scale" )
		self.label6 = tk.Label( self.labelFrame, text="Binary" )
		self.label7 = tk.Label( self.labelFrame, text="Flip" )
		self.label8 = tk.Label( self.labelFrame, text="Load Web Camera" )
		self.label9 = tk.Label( self.labelFrame, text="Load Picture" )

		self.labelFrame = tk.Label( root )
		self.labelFrame.pack( )
		self.label1.pack( side=tk.LEFT )
		self.label2.pack( side=tk.LEFT )
		self.label3.pack( side=tk.LEFT )
		self.label4.pack( side=tk.LEFT )
		self.label5.pack( side=tk.LEFT )
		self.label6.pack( side=tk.LEFT )
		self.label7.pack( side=tk.LEFT )
		self.label8.pack( side=tk.LEFT )
		self.label9.pack( side=tk.LEFT )

	def setNoEffect( self ):
		self.setEffect = 0
	def setNegative( self ):
		self.setEffect = 1
	def setEdgeDetection( self ):
		self.setEffect = 2
	def setHeatMap( self ):
		self.setEffect = 3
	def setGrayScale( self ):
		self.setEffect = 4
	def setBinary( self ):
		self.setEffect = 5
	
	def setFlip( self ):
		if( self.setFlipMode == 1 ):
			self.setFlipMode = 2
		elif( self.setFlipMode == 2 ):
			self.setFlipMode = 1 

	def setWebCam( self ):
		self.setImage = 0
	def setPicture( self ):
		self.setImage = 1

	def convertToPhoto( self, frame ):
		return ImageTk.PhotoImage(image=Image.fromarray( frame ))
	
	def convertToButtonPicture( self, frame ):

		frame = cv2.resize( frame, dsize=( self.buttonImgWidth, self.buttonImgHeight ))

		noEffect = self.doNoEffect( frame )
		negative = self.doNegative( frame )
		edgeDetection = self.doEdgeDetection( frame )
		heatMap = self.doHeatMap( frame )
		grayScale = self.doGrayScale( frame )
		binary = self.doBinary( frame )

		
		noEffect = self.convertToPhoto( noEffect )
		negative = self.convertToPhoto( negative )
		edgeDetection = self.convertToPhoto( edgeDetection )
		heatMap = self.convertToPhoto( heatMap )
		grayScale = self.convertToPhoto( grayScale )
		binary = self.convertToPhoto( binary )
		
		self.button1.configure( image=noEffect, height=self.buttonImgHeight , width=self.buttonImgWidth )
		self.button2.configure( image=negative, height=self.buttonImgHeight , width=self.buttonImgWidth )
		self.button3.configure( image=edgeDetection, height=self.buttonImgHeight , width=self.buttonImgWidth )
		self.button4.configure( image=heatMap, height=self.buttonImgHeight , width=self.buttonImgWidth )
		self.button5.configure( image=grayScale, height=self.buttonImgHeight , width=self.buttonImgWidth )
		self.button6.configure( image=binary, height=self.buttonImgHeight , width=self.buttonImgWidth )

		self.button1.photo = noEffect
		self.button2.photo = negative
		self.button3.photo = edgeDetection
		self.button4.photo = heatMap
		self.button5.photo = grayScale
		self.button6.photo = binary

	def doNoEffect( self, frame ):
		return frame
	def doNegative( self, frame ):
		return 255 - frame
	def doEdgeDetection( self, frame ):
		return cv2.Canny( frame, 100, 200 )
	def doHeatMap( self, frame ):
		return cv2.cvtColor( frame, cv2.COLOR_RGB2BGR )
	def doGrayScale( self, frame ):
		return cv2.cvtColor( frame, cv2.COLOR_RGB2GRAY )

	def doBinary( self, frame ):
		_, binary = cv2.threshold( self.doGrayScale( frame ), 100, 255, cv2.THRESH_BINARY )
		return binary

	def doFlip( self, frame ):
		if( self.setFlipMode == 1 ):
			return cv2.flip( frame, 1 )
		else:
			return frame

	def doRotate( self, frame ):#fix
		frame = Image.fromarray(frame)
		return frame.rotate( self.scale.get( ))

	def doWebCam( self, frame ):
		doEffect = 1
	def doPicture( self, frame ):
		doEffect = 0

	def show_frame( self ):

		_, frame = self.cap.read()
		
		frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

		frame = self.doFlip( frame )

		self.convertToButtonPicture( frame )

		if( self.setEffect == 0 ):
			frame = self.doNoEffect( frame )
		elif( self.setEffect == 1 ):
			frame = self.doNegative( frame )
		elif( self.setEffect == 2 ):
			frame = self.doEdgeDetection( frame )
		elif( self.setEffect == 3 ):
			frame = self.doHeatMap( frame )
		elif( self.setEffect == 4 ):
			frame = self.doGrayScale( frame )
		elif( self.setEffect == 5 ):
			frame = self.doBinary( frame )

		imgtk = self.doRotate( frame )
		imgtk = self.convertToPhoto( frame )
		
		

		self.labelFrame.imgtk = imgtk
		self.labelFrame.configure(image=imgtk)
		self.labelFrame.after(10, self.show_frame)


root = tk.Tk()
effects = Effects( root )

effects.show_frame()
root.mainloop()