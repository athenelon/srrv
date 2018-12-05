#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct
{
  char ime[ 10 ] ;
  char prezime[ 20 ] ;
  int index ;
} student ;



void toString( student s )
{
  int i ;
  printf( "    Ime: %s\n    Prezime:%s\n    Index:%d\n", s.ime, s.prezime, s.index ) ;
}

int mainMenu( )
{
  int unos = 0 ;
  printf( "\n---------Studentation---------\n\n\n" ) ;
  printf( "    1. Unos novog studenta\n    2. Pretraga po prezimenu\n    3. Ispis svih studenata\n    4. Korigovanje podataka\n" ) ;
  printf( "    5. Izlaz\n\nVas unos: " ) ;
  scanf( "%d", &unos ) ;

  if( unos < 1 || unos > 5 )
    unos = 0 ;

  return unos ;
}

student newStudent(  )
{
  student s = { "", "", 0 } ; 

  printf( "    Unos novog studenta...\n\nIme: " ) ;
  scanf( "%s", s.ime ) ;
  printf( "Prezime: " ) ;
  scanf( "%s", s.prezime ) ;
  printf( "Index: " ) ;
  scanf( "%d", &s.index ) ;

  printf( "    Unos studenta zavrsen.\n\n" ) ;
  return s ;  
}

int search( student s[ ], size_t size )
{
  int match = 0 ;
  int i ;
  char prezime ;

  printf( "Uneti prvo slovo prezimena: " ) ;
  scanf( " %c", &prezime ) ;
  for( i = 0 ; i < size ; ++i )
  {
    if( s[ i ].prezime[ 0 ] == prezime )
    {
      match += 1 ;
      toString( s[ i ]) ;
    }
  }
  if( match == 0 )
    printf( "    Prezime ni jednog studenta ne pocinje na slovo '%c'\n\n", prezime ) ;
  return match ;
}

void ispis( student s[ ], size_t size )
{
  int i ;
  for( i = 0 ; i < size ; ++i )
    if( strcmp( s[ i ].ime, "" ) != 0 )
    {
      printf( "\n    R.br: %d\n", i + 1 ) ;
      toString( s[ i ]) ;
    }
}

int modify( student s[], size_t size )
{
  int unos = 0 ;
  int n ;
  
  printf( "Izaberite redni broj studenta za menjanje: " );
  scanf( "%d", &n ) ;

  if( n <= 0 || n > size )
    return -1 ;
  --n ;
  toString( s[ n ]) ;
  do
  {
    printf( "    Odaberite polje za promenu... \n\n    1. Ime\n    2. Prezime\n    3. Index\n    4. Izlaz\n\nVas unos: " ) ;
    scanf( "%d", &unos ) ;

    if( unos == 1 )
    {
      printf( "Novo ime: " ) ;
      scanf( "%s", s[ n ].ime ) ;
    }
    else if( unos == 2 )
    {
      printf( "Novo prezime: " ) ;
      scanf( "%s", s[ n ].prezime ) ;
    }
    else if( unos == 3 )
    {
      printf( "Novi index: " ) ;
      scanf( "%d", &s[ n ].index ) ;
    }
    else
      printf( "    Nekorektan unos\n\n" ) ;
    
  } while( unos != 4 ) ;

  return 0 ;
}

int main( )
{
  FILE* file ;
  student s[ 10 ] = { "", "", 0 } ;
  int cnt = 0 ;
  int unos ;
  int i ;
  size_t size = sizeof( s ) / sizeof( student ) ;
  size_t nob = sizeof( student ) ;
  char* name = "studenti.txt" ;
  char* str ;
  
  if(!( file = fopen( name, "r" )))
  {
    file = fopen( name, "w" ) ;
    fclose( file ) ;
  }
  else
  {
    str = ( char* ) malloc( nob + 1 ) ;
    while(! feof( file )) 
    {
      if( getline( &str, &nob, file ) == -1 )
	continue ;
      
      strcpy( s[ cnt ].ime, strtok( str, " " )) ;
      strcpy( s[ cnt ].prezime, strtok( NULL, " " )) ;
      s[ cnt ].index = atoi( strtok( NULL, " " )) ;

      cnt++ ;
    } 
    free( str ) ;
    fclose( file ) ;
  }
  do
  {
    unos = mainMenu( ) ;

    if( unos == 1 )
    {
      if( cnt >= size )
      {
	printf( "Kapacitet popunjen, ne moze vise studenata da se unese!\n" ) ;
	continue ;
      }
      s[ cnt++ ] = newStudent( ) ;
    }
    else if( unos == 2 )
      search( s, size ) ;
    else if( unos == 3 )
      ispis( s, size ) ;
    else if( unos == 4 )
      modify( s, size ) ;
  } while( unos != 5 ) ;

  file = fopen( name, "w" ) ;
  for( i = 0 ; i < cnt ; ++i )
    fprintf( file, "%s %s %d \n", s[ i ].ime, s[ i ].prezime, s[ i ].index ) ;

  return 0 ;
}
