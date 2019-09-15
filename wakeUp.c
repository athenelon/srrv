#ifndef F_CPU
#define F_CPU 1000000UL // 1 MHz clock speed
#endif

#define D4 eS_PORTB4
#define D5 eS_PORTB5
#define D6 eS_PORTB6
#define D7 eS_PORTB7
#define RS eS_PORTD4
#define EN eS_PORTD0

#define TH2 0
#define TH1 1
#define TM2 2
#define TM1 3

#define SS2 4
#define SS1 5

#define AH2 6
#define AH1 7
#define AM2 8
#define AM1 9

#define BLOCK 255
#define CMP_VAL_T1 15624
#define CLK_CALIBRATION_DELAY_MINUTE 354
#define CLK_CALIBRATION_DELAY_HOUR 25000

#define MODE_ALARM 0
#define MODE_OFF 1
#define MODE_MANUAL 2

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

struct lcdClock
{
	unsigned short mode ;
	
	unsigned short tArray[ 10 ] ;
} ;

//init
void systemInit( ) ;
void lcdInit( struct lcdClock , unsigned short ) ;

//run
void lcdModeOperation( struct lcdClock ) ;
void timer1EventFire( struct lcdClock *, unsigned short ) ;
void clockSim( struct lcdClock * ) ;
void writeLcdHigh( struct lcdClock, unsigned short ) ;
void writeLcdLow( struct lcdClock, unsigned short ) ;
void buttonIncrement( struct lcdClock *, unsigned short ) ;
unsigned short buttonPress( struct lcdClock *, unsigned short * ) ;


volatile unsigned short timer1Flag = 0 ;

ISR( TIMER1_COMPA_vect )
{
	timer1Flag = 1 ;
}

void systemInit( )
{
	DDRA = 0x00 ;
	DDRB = 0xFF ;
	DDRD = 0xFF ;
	
	//ADC Aref = Avcc, 8 prescaler
	ADMUX = ( 1 << REFS0 ) | ( 5 << MUX0 ) ;
	ADCSRA = ( 1 << ADEN ) | ( 3 << ADPS0 ) ;

	//16bit timer1, CTC mode, 64 prescaler
	TCCR1B |= ( 3 << CS10) | ( 1 << WGM12 ) ;
	OCR1A = CMP_VAL_T1 ;
	TIMSK |= ( 1 << OCIE1A ) ;
	
	TCNT1 = 0 ;
	
	//8bit timer2 as PWM [ non inverting, fast PWM ]
	TCCR2 |= ( 1 << COM21 ) | ( 1 << WGM21 ) | ( 1 << WGM20 )  ;
	
	OCR2 = 0 ;
	
	sei( ) ;
}

void lcdInit( struct lcdClock myClock, unsigned short blockPos )
{
	Lcd4_Init( );
	_delay_ms( 50 ) ;

	writeLcdHigh( myClock, blockPos ) ;
	writeLcdLow( myClock, blockPos ) ;
}

void timer1EventFire( struct lcdClock *myClock, unsigned short blockPos )
{
	static short clkMinDelay = 0 ;
	static short clkHourDelay = 0 ;
	
	if( timer1Flag )
	{
		++clkHourDelay ;
		if(( ++clkMinDelay ) <= CLK_CALIBRATION_DELAY_MINUTE )
		{
			if( clkHourDelay <= CLK_CALIBRATION_DELAY_HOUR )
			{
				clockSim( myClock ) ;
				writeLcdHigh( *myClock, blockPos ) ;
			}
			else
				clkHourDelay = 0 ;
		}
		else
			clkMinDelay = 0 ;
		
		timer1Flag = 0 ;
	}
}

void lcdModeOperation( struct lcdClock myClock)
{
	unsigned short adcValue = 0 ;
	
	if( myClock.mode == MODE_OFF )
	{
		OCR2 = 0 ;
		_delay_ms( 60 ) ;
		TCCR2 &= ~( 1 << CS20 ) ;
	}
	else if( myClock.mode == MODE_MANUAL )
	{
		ADCSRA |= ( 1 << ADSC ) ;
		while( ADCSRA & ( 1 << ADSC )) ;
		
		adcValue = ADC / 4 ;
		
		if( adcValue <= 0 )
		{
			TCCR2 &= ~( 1 << CS20 ) ;
			OCR2 = 0 ;
		}
		else if( adcValue >= 240 )
		{
			TCCR2 |= ( 1 << CS20 ) ;
			OCR2 = 240 ;
		}
		else
		{		
			TCCR2 |= ( 1 << CS20 ) ;
			OCR2 = adcValue ;
		}
	}
	else if( myClock.mode == MODE_ALARM )
	{
		if( myClock.tArray[ TH1 ] == myClock.tArray[ AH1 ] && myClock.tArray[ TH2 ] == myClock.tArray[ AH2 ] &&
		myClock.tArray[ TM1 ] == myClock.tArray[ AM1 ] && myClock.tArray[ TM2 ] == myClock.tArray[ AM2 ])
		{
			TCCR2 |= ( 1 << CS20 ) ;	
			
			if( OCR2 >= 240 )
				OCR2 = 240 ;
			else
				OCR2++ ;
				
			_delay_ms( 60 ) ;
		}
	}
	
}

void clockSim( struct lcdClock *myClock )
{
	short flag ;
	
	if(( myClock->tArray[ TH2 ]) == 2 )
		flag = 1 ;
	else
		flag = 0 ;
		
	if(( ++myClock->tArray[ SS1 ]) >= 10 )
	{
		myClock->tArray[ SS1 ] = 0 ;
		if(( ++myClock->tArray[ SS2 ]) >= 6 )
		{
			myClock->tArray[ SS2 ] = 0 ;
			if(( ++myClock->tArray[ TM1 ]) >= 10 )
			{
				myClock->tArray[ TM1 ] = 0 ;
				if(( ++myClock->tArray[ TM2 ]) >= 6 )
				{
					myClock->tArray[ TM2 ] = 0 ;
					if((( ++myClock->tArray[ TH1 ]) >= 10 ) || ( flag == 1 && myClock->tArray[ TH1 ] >= 4 ))
					{
						myClock->tArray[ TH1 ] = 0 ;
						
						if(( ++myClock->tArray[ TH2 ]) >= 3 )
						{
							myClock->tArray[ TH2 ] = 0 ;
							
						}
						
					}
				}
			}
		}
	}
}

void writeLcdHigh( struct lcdClock myClock, unsigned short blockPos )
{	
	char msgH[ 17 ] ;

	int i, cnt = 0 ;
	
	
	for( i = 0 ; i < 16 ; ++i )
	{
		if( i == 2 || i == 5 || i == 13 )
		{
			++cnt ;
			msgH[ i ] = ':' ;
		}
		else if( i == 8 || i == 10 )
		{
			++cnt ;
			msgH[ i ] = ' ' ;
		}
		else if( i == 9 )
		{
			++cnt ;
			if( myClock.mode == MODE_OFF )
				msgH[ i ] = 'X' ;
			else if( myClock.mode == MODE_ALARM )
				msgH[ i ] = 'A' ;
			else if( myClock.mode == MODE_MANUAL )
				msgH[ i ] = 'M' ;
		}
		else
			msgH[ i ] = ( myClock.tArray[ i - cnt ] + '0' ) ;	
	}
	msgH[ 16 ] = '\0' ;

	Lcd4_Set_Cursor( 1, 0 ) ;
	Lcd4_Write_String( msgH ) ;
}

void writeLcdLow( struct lcdClock myClock, unsigned short blockPos )
{
	static char msgL[ 17 ]  = "  Time   M Alarm\0" ;
	
	Lcd4_Set_Cursor( 2, 0 ) ;	
	Lcd4_Write_String( msgL ) ;
		
	Lcd4_Set_Cursor( 2, blockPos ) ;
	Lcd4_Write_Char( BLOCK ) ;		
}

void buttonIncrement( struct lcdClock *myClock, unsigned short blockPos )
{
	switch( blockPos )
	{
		case 0: myClock->tArray[ TH2 ]++ ;
	
				if( myClock->tArray[ TH2 ] == 3 )
					myClock->tArray[ TH2 ] = 0 ;
				else if( myClock->tArray[ TH2 ] == 2 && myClock->tArray[ TH1 ] > 3 )
					myClock->tArray[ TH2 ] = 0 ;
				break ;
		
		case 1: myClock->tArray[ TH1 ]++ ;
			
				if( myClock->tArray[ TH1 ] == 4 && myClock->tArray[ TH2 ] > 1 )
					myClock->tArray[ TH1 ] = 0 ;
				else if( myClock->tArray[ TH1 ] == 10 )
					myClock->tArray[ TH1 ] = 0 ;
				break ;
	
		case 3: myClock->tArray[ TM2 ]++ ;
	
				if( myClock->tArray[ TM2 ] == 6 )
					myClock->tArray[ TM2 ] = 0 ;
				break ;
	
		case 4: myClock->tArray[ TM1 ]++ ;
	
				if( myClock->tArray[ TM1 ] == 10 )
					myClock->tArray[ TM1 ] = 0 ;
				break ;
			
		case 6: myClock->tArray[ SS2 ]++ ;
	
				if( myClock->tArray[ SS2 ] == 6 )
					myClock->tArray[ SS2 ] = 0 ;
				break ;
	
		case 7: myClock->tArray[ SS1 ]++ ;
	
				if( myClock->tArray[ SS1 ] == 10 )
					myClock->tArray[ SS1 ] = 0 ;
				break ;
				
		case 9: if(( ++myClock->mode ) > MODE_MANUAL )
					myClock->mode = MODE_ALARM ;
				break ;
				
		case 11: myClock->tArray[ AH2 ]++ ;
	
				if( myClock->tArray[ AH2 ] == 3 )
					myClock->tArray[ AH2 ] = 0 ;
				else if( myClock->tArray[ AH2 ] == 2 && myClock->tArray[ AH1 ] > 3 )
					myClock->tArray[ AH2 ] = 0 ;
				break ;
	
		case 12: myClock->tArray[ AH1 ]++ ;
	
				if( myClock->tArray[ AH1 ] == 4 && myClock->tArray[ AH2 ] > 1 )
					myClock->tArray[ AH1 ] = 0 ;
				else if( myClock->tArray[ AH1 ] == 10 )
					myClock->tArray[ AH1 ] = 0 ;
				break ;
	
		case 14: myClock->tArray[ AM2 ]++ ;
	
				if( myClock->tArray[ AM2 ] == 6 )
					myClock->tArray[ AM2 ] = 0 ;
				break ;
	
		case 15: myClock->tArray[ AM1 ]++ ;
	
				if( myClock->tArray[ AM1 ] == 10 )
					myClock->tArray[ AM1 ] = 0 ;
				break ;
	}
}

unsigned short buttonPress( struct lcdClock *myClock, unsigned short *blockPos )
{
	static unsigned short buttonFlag1 = 0 ;
	static unsigned short buttonFlag2 = 0 ;
	static unsigned short buttonFlag3 = 0 ;
	
	if(!( PINA &( 1 << PINA0 )) && *blockPos > 0 )
	{
		if( buttonFlag1 )
		{
			( *blockPos )-- ;
			_delay_ms( 10 ) ;
			buttonFlag1 = 0 ;
			
			writeLcdHigh( *myClock, *blockPos ) ;
			writeLcdLow( *myClock, *blockPos ) ;
		}
	}
	else
		buttonFlag1 = 1 ;
	if(!( PINA &( 1 << PINA1 )))
	{
		if( buttonFlag2 )
		{
			buttonIncrement( myClock, *blockPos ) ;
			
			_delay_ms( 10 ) ;
			buttonFlag2 = 0 ;
			
			writeLcdHigh( *myClock, *blockPos ) ;
			writeLcdLow( *myClock, *blockPos ) ;
		}
	}
	else
		buttonFlag2 = 1 ;
	if(!( PINA &( 1 << PINA3 )) && *blockPos < 15 )
	{
		if( buttonFlag3 )
		{
			( *blockPos )++ ;
			_delay_ms( 10 ) ;
			buttonFlag3 = 0 ;
			
			writeLcdHigh( *myClock, *blockPos ) ;
			writeLcdLow( *myClock, *blockPos ) ;
		}
	}
	else
		buttonFlag3 = 1 ;
	return *blockPos ;
}

int main(void)
{
	struct lcdClock myClock = { MODE_OFF, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}} ;
	unsigned short blockPos = 0 ;

	systemInit( ) ;
	lcdInit( myClock, blockPos ) ;

	while( 1 )
	{
		timer1EventFire( &myClock, blockPos ) ;
		
		blockPos = buttonPress( &myClock, &blockPos ) ;
		
		lcdModeOperation( myClock ) ;
	}
}