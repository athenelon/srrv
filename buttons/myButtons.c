#include<stdlib.h>
#include<stdio.h>

int main( )
{
  
  FILE* fButton1 ;
  FILE* fButton2 ;
  FILE* fPower ;
  float button1Pressed = 0 ;
  float button2Pressed = 0 ;
  float diodePower = 1 ;
  size_t numOfBytes = 1 ;

  char* fileValue ;
  
  while( 1 )
  {
    if(!( fButton1 = fopen( "button1.txt", "r" )))
      printf( "File 'button1.txt' failed to open" ) ;
    if(!( fButton2 = fopen( "button2.txt", "r" )))
      printf( "File 'button2.txt' failed to open " ) ;

    fileValue = ( char* ) malloc( numOfBytes + 1 ) ;
    
    if(( getline( &fileValue, &numOfBytes, fButton1 )) == -1 )
      printf( "Could not read line from fButton1( button1.txt )" ) ;
    sscanf( fileValue, "%f", &button1Pressed ) ;

    if(( getline( &fileValue, &numOfBytes, fButton2 )) == -1 )
      printf( "Could not read line from fButton2( button2.txt )" ) ;
    sscanf( fileValue, "%f", &button2Pressed ) ;

    fclose( fButton1 ) ;
    fclose( fButton2 ) ;

    if(!( fPower = fopen( "/home/sinisa/prog/diodeDimmer/led/diodePower.txt", "r+" )))
      printf( "File diodePower.txt failed to open" ) ;
    if(( getline( &fileValue, &numOfBytes, fPower )) == -1 )
      printf( "Count not read line from fPower( diodePower.txt )" ) ;
    sscanf( fileValue, "%f", &diodePower ) ;
    fclose( fPower ) ;

    printf( "Upisati broj %f", diodePower ) ;
    scanf( "%f", &button1Pressed ) ;
    
    if(!( fPower = fopen( "/home/sinisa/prog/diodeDimmer/led/diodePower.txt", "w" )))
      printf( "File 'diodePower.txt' failed to open" ) ;

    diodePower += ( button1Pressed - button2Pressed ) / 10 ;

    if( diodePower >= 1 ) diodePower = 1 ;
    else if( diodePower < 0 ) diodePower = 0 ;
    
    fprintf( fPower, "%f", diodePower ) ;

    fclose( fPower ) ;
  }
  return 0 ;
}
