#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

int main( )
{
  FILE* fControl ;
  FILE* fPower ;
  float diodePower = 0.2 ;
  long int diodePeriod = 20000L ;
  char* readPower ;
  size_t numOfBytes = 1 ;
  
  while( 1 )
  {
    fControl = fopen( "diodeControl.txt", "w" ) ;
    fputs( "1111", fControl ) ;
    fclose( fControl ) ;
    
    if(!( fPower = fopen( "diodePower.txt", "r" )))
       printf( "File diodePower.txt failed to open\n" ) ;

    readPower = ( char* ) malloc( numOfBytes + 1 ) ;

    getline( &readPower, &numOfBytes, fPower ) ;
    sscanf( readPower, "%f", &diodePower ) ;

    free( readPower ) ;
    fclose( fPower ) ;

    usleep( diodePower * diodePeriod ) ;

    fControl = fopen( "diodeControl.txt", "w" ) ;
    fputs( "0000", fControl ) ;
    fclose( fControl ) ;

    usleep(( 1 - diodePower ) * diodePeriod ) ;

    fprintf( stdout, "Diode Power -> %f\n", diodePower ) ;
    fflush( stdout ) ;
  }
  return 0 ;
}
