#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

#define D0 eS_PORTB0
#define D1 eS_PORTB1
#define D2 eS_PORTB2
#define D3 eS_PORTB3
#define D4 eS_PORTB4
#define D5 eS_PORTB5
#define D6 eS_PORTB6
#define D7 eS_PORTB7
#define RS eS_PORTD2
#define EN eS_PORTD0

#define TH2 0
#define TH1 1
#define TM2 2
#define TM1 3

#define MM2 4
#define MM1 5

#define AH2 6
#define AH1 7
#define AM2 8
#define AM1 9

#define BLOCK 255
#define CMP_VAL 31248

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"

struct lcdClock
{
	unsigned short Tss ;
	unsigned short on ;
	unsigned short ledFlag ;
	float ledTimer ;
	
	unsigned short tArray[ 10 ] ;
} ;

struct lcdClock clockSim( struct lcdClock ) ;
void writeLcd( struct lcdClock ) ;
void buttonIncrement( ) ;
void buttonPress( ) ;
void ledControl( ) ;

struct lcdClock myClock = { 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}} ;
unsigned short blockPos = 0 ;
unsigned long diodePeriod = 20000UL ;
volatile float diodePower = 0 ;

ISR( TIMER1_COMPA_vect )
{
	myClock = clockSim( myClock ) ;
	
	Lcd8_Clear();
	writeLcd( myClock ) ;
}

ISR( TIMER0_COMP_vect )
{
	if(( --myClock.ledTimer ) <= 0 )
	{
		diodePower += 0.01 ;
		myClock.ledTimer = 10 * ( 10 * ( myClock.tArray[ MM2 ] - '0') + ( myClock.tArray[ MM1 ] - '0' )) ;
	}
}

struct lcdClock clockSim( struct lcdClock myClock )
{

	static short flag = 0 ;
	
	//if(( ++myClock.Tss) >= 60 )
	{
		myClock.Tss = 0 ;
		if(( ++myClock.tArray[ TM1 ]) >= 10 )
		{
			myClock.tArray[ TM1 ] = 0 ;
			if(( ++myClock.tArray[ TM2 ]) >= 6 )
			{
				myClock.tArray[ TM2 ] = 0 ;
				if((( ++myClock.tArray[ TH1 ]) >= 10 ) || ( flag == 1 && myClock.tArray[ TH1 ] >= 4 ))
				{
					myClock.tArray[ TH1 ] = 0 ;
						
					if(( ++myClock.tArray[ TH2 ]) >= 3 )
					{
						myClock.tArray[ TH2 ] = 0 ;
						
					}
					if(( myClock.tArray[ TH2 ]) == 2 )
						flag = 1 ;
					else
						flag = 0 ;	
				}
			}
		}
	}
	return myClock ;
}

void writeLcd( struct lcdClock myClock )
{	
	char msgH[ 17 ] ;
	static char msgLon[ 17 ]  = "Time   on  Alarm\0" ;
	static char msgLoff[ 17 ] = "Time   off Alarm\0" ;
	int i, cnt = 0 ;
	
	
	for( i = 0 ; i < 16 ; ++i )
	{
		if( i == 2 || i == 13 )
		{
			++cnt ;
			msgH[ i ] = ':' ;
		}
		else if( i == 5 || i == 9 || i == 10 )
		{
			++cnt ;
			msgH[ i ] = ' ' ;
		}
		else if( i == 6 )
		{
			++cnt ;
			msgH[ i ] = 'T' ;
		}
		else
			msgH[ i ] = ( myClock.tArray[ i - cnt ] + '0' ) ;	
	}
	msgH[ 16 ] = '\0' ;

	Lcd8_Set_Cursor( 0, 0 ) ;
	Lcd8_Write_String( msgH ) ;
	
	Lcd8_Set_Cursor( 2, 0 ) ;
	if( myClock.on )
		Lcd8_Write_String( msgLon ) ;
	else
		Lcd8_Write_String( msgLoff ) ;
		
	Lcd8_Set_Cursor( 2, blockPos ) ;
	Lcd8_Write_Char( BLOCK ) ;

}

void buttonIncrement( )
{
	switch( blockPos )
	{
	case 0: myClock.tArray[ TH2 ]++ ;
	
			if( myClock.tArray[ TH2 ] == 3 )
				myClock.tArray[ TH2 ] = 0 ;
			else if( myClock.tArray[ TH2 ] == 2 && myClock.tArray[ TH1 ] > 3 )
				myClock.tArray[ TH2 ] = 0 ;
			break ;
		
	case 1: myClock.tArray[ TH1 ]++ ;
			
			if( myClock.tArray[ TH1 ] == 4 && myClock.tArray[ TH2 ] > 1 )
				myClock.tArray[ TH1 ] = 0 ;
			else if( myClock.tArray[ TH1 ] == 10 )
				myClock.tArray[ TH1 ] = 0 ;
			break ;
	
	case 3: myClock.tArray[ TM2 ]++ ;
	
			if( myClock.tArray[ TM2 ] == 6 )
				myClock.tArray[ TM2 ] = 0 ;
			
			myClock.ledTimer = 10 * ( 10 * ( myClock.tArray[ MM2 ] - '0') + ( myClock.tArray[ MM1 ] - '0' )) ;
			break ;
	
	case 4: myClock.tArray[ TM1 ]++ ;
	
			if( myClock.tArray[ TM1 ] == 10 )
				myClock.tArray[ TM1 ] = 0 ;
			
			myClock.ledTimer =  10 * ( 10 * ( myClock.tArray[ MM2 ] - '0') + ( myClock.tArray[ MM1 ] - '0' )) ;
			break ;
			
	case 6: if( myClock.on )
				myClock.on = 0 ;
			else
				myClock.on = 1 ;
			break ;
			
	case 7: myClock.tArray[ MM2 ]++ ;
	
			if( myClock.tArray[ MM2 ] == 2 )
				myClock.tArray[ MM2 ] = 0 ;
			else if( myClock.tArray[ MM2 ] == 1 && myClock.tArray[ MM1 ] > 5 )
				myClock.tArray[ MM2 ] = 0 ;
			break ;
	
	case 8: myClock.tArray[ MM1 ]++ ;
	
			if( myClock.tArray[ MM1 ] == 10 )
				myClock.tArray[ MM1 ] = 0 ;
			else if( myClock.tArray[ MM1 ] == 6 && myClock.tArray[ MM2 ] > 0 )
				myClock.tArray[ MM1 ] = 0 ;
			break ;
	 
	case 11: myClock.tArray[ AH2 ]++ ;
	
			if( myClock.tArray[ AH2 ] == 3 )
				myClock.tArray[ AH2 ] = 0 ;
			else if( myClock.tArray[ AH2 ] == 2 && myClock.tArray[ AH1 ] > 3 )
				myClock.tArray[ AH2 ] = 0 ;
			break ;
	
	case 12: myClock.tArray[ AH1 ]++ ;
	
			if( myClock.tArray[ AH1 ] == 4 && myClock.tArray[ AH2 ] > 1 )
				myClock.tArray[ AH1 ] = 0 ;
			else if( myClock.tArray[ AH1 ] == 10 )
				myClock.tArray[ AH1 ] = 0 ;
			break ;
	
	case 14: myClock.tArray[ AM2 ]++ ;
	
			if( myClock.tArray[ AM2 ] == 6 )
				myClock.tArray[ AM2 ] = 0 ;
			break ;
	
	case 15: myClock.tArray[ AM1 ]++ ;
	
			if( myClock.tArray[ AM1 ] == 10 )
				myClock.tArray[ AM1 ] = 0 ;
			break ;
	}

}

void buttonPress( )
{
	static unsigned short buttonFlag1 = 0 ;
	static unsigned short buttonFlag2 = 0 ;
	static unsigned short buttonFlag3 = 0 ;
	
	if((! (PINA &( 1 << PINA0 ))) && blockPos < 15 )
	{
		if( buttonFlag1 )
		{
			blockPos++ ;
			_delay_ms( 10 ) ;
			buttonFlag1 = 0 ;
			
			Lcd8_Clear( ) ;
			writeLcd( myClock ) ;
		}
	}
	else
		buttonFlag1 = 1 ;
	if((! (PINA &( 1 << PINA1 ))))
	{
		if( buttonFlag2 )
		{
			buttonIncrement( ) ;
			
			_delay_ms( 10 ) ;
			buttonFlag2 = 0 ;
			
			Lcd8_Clear( ) ;
			writeLcd( myClock ) ;
		}
	}
	else
		buttonFlag2 = 1 ;
	if((! (PINA &( 1 << PINA2 ))) && blockPos > 0 )
	{
		if( buttonFlag3 )
		{
			blockPos-- ;
			_delay_ms( 10 ) ;
			buttonFlag3 = 0 ;
			
			Lcd8_Clear( ) ;
			writeLcd( myClock ) ;
		}
	}
	else
		buttonFlag3 = 1 ;
}

void my_delay( unsigned long ledTime )
{
	while(( --ledTime ) >= 0 )
		_delay_us( 1 ) ;
}


void ledControl( )
{	
	static int timerOn ;
	static int timerOff ;
	
	timerOn = diodePower * diodePeriod ;
	timerOff = (( 1 - diodePower ) * diodePeriod ) ; 
	
	PORTD |= ( 1 << PIND7 ) ;
	my_delay( timerOn ) ;
	
	PORTD &= ~( 1 << PIND7 ) ;
	my_delay( timerOff ) ;
	
}
int main(void)
{
	int overflow = 0 ;
	DDRA = 0x00 ;
	DDRB = 0xFF ;
	DDRD = 0xFF ;
	
	Lcd8_Init();
	
	_delay_ms( 50 ) ;
	
	//16bit timer1
	TCCR1B |= ( 1 << CS12 ) || ( 1 << WGM12 ) ;
	
	OCR1A = CMP_VAL; 
	
	TIMSK |= ( 1 << OCIE1A ) ;
	
	//8bit timer0
	TCCR0 |=  ( 1 << CS00 ) || ( 1 << CS02 ) || ( 1 << WGM01 ) ;
	
	OCR0 = 234 ;
	TIMSK |= ( 1 << OCR0 ) ;
	
	sei( ) ;

	while( 1 )
	{
		
		
		//ledControl( ) ;
		
		
		
		
		buttonPress( ) ;
		//ledControl( ) ; // redundant
		if( myClock.tArray[ TH1 ] == myClock.tArray[ AH1 ] && myClock.tArray[ TH2 ] == myClock.tArray[ AH2 ] &&
			myClock.tArray[ TM1 ] == myClock.tArray[ AM1 ] && myClock.tArray[ TM2 ] == myClock.tArray[ AM2 ] &&
			myClock.on )
		{	
			myClock.ledFlag = 1 ;
		}
		if( myClock.ledFlag )
		{
			if(! myClock.on )
			{
				myClock.ledFlag = 0 ;
				diodePower = 0 ;
				continue ;
			}
			ledControl( ) ;
		}
	}
}