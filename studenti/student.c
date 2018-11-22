#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

struct student
{
  char ime[ 10 ] ;
  char prezime[ 20 ] ;
  char index[ 10 ] ;
  int ocene[ 6 ] ;
} ;

int mainMenu( )
{
  int i = 0 ;
  while( 1 )
  {
    printf( "\n-----Studentation-----\n\n" ) ;
    printf( "  1. Izlistavanje studenta.\n  2. Pretrazivanje studenta po prezimenu.\n  3. Unos novog studenta.\n  4. Izlaz.\n" ) ;
    printf( "Vas unos: " ) ;
    scanf( "%d", &i ) ;

    if( i < 1 || i > 5 )
      printf( "Nekorektan unos!\n" ) ;
    else
      break ;
  }
  return i ;
}

void newStudent( )
{
  FILE* file ;
  struct student s = { "", "", "", { 0 }} ;
  int i ;
  int temp ;
  char student[ 100 ]  ;
  char ocenaToChar ;
  
  printf("Unosenje podataka o studentu...\nNaponema: prilikom unosa ocena bilo koji broj van opsega 5-10 prekida unos!\nIme: " ) ;
  scanf( "%s", s.ime ) ;

  printf( "Prezime: " ) ;
  scanf( "%s", s.prezime ) ;

  printf( "Index: " ) ;
  scanf( "%s", s.index ) ;

  printf( "Ocene: " ) ;
  for( i = 0 ; i < sizeof( s.ocene ) / sizeof( int ) ; ++i )
  {
    scanf( "%d", &temp ) ;
    if( temp >= 5 && temp <= 10 )
      s.ocene[ i ] = temp ;
    else
      break ;    
  }
  printf( "\nNovi student kreiran, dodajem u bazu podataka.\n" ) ;

  file = fopen( "/tmp/studenti.dat", "a" ) ;
  
  fprintf( file, "%s;%s;%s;", s.ime, s.prezime, s.index ) ;
  
  for( i = 0 ; i < sizeof( s.ocene ) / sizeof( int ) ; ++i )
  {
    fprintf( file, "%d", s.ocene[ i ] ) ;
    if( i < ( sizeof( s.ocene ) / sizeof( int )) - 1 )
      fprintf( file, "," ) ;
    else
      fprintf( file, "\n" ) ;
  }
  

  fclose( file ) ;
}

void fromDataBase( )
{
  FILE* file ;
  char* studentInfo ;
  char* studentLine ;
  size_t nob = 0 ;
  
  if(( file = fopen( "/tmp/studenti.dat", "r+" )) == NULL )
    printf( "Could not open file!\n" ) ;
  printf( "\nIzlistavanje studenata...\n\n" ) ;

  studentLine = ( char* ) malloc( 50 ) ;
  studentInfo = ( char* ) malloc( 20 ) ;
  
  do
  {
    if( getline( &studentLine, &nob, file ) == -1 )
      continue ;

    
    studentInfo = strtok( studentLine, ";" ) ;
    printf( "Ime:%s\n", studentInfo ) ;

    studentInfo = strtok( NULL, ";" ) ;
    printf( "Prezime:%s\n", studentInfo ) ;

    studentInfo = strtok( NULL, ";" ) ;
    printf( "Index:%s\n", studentInfo ) ;

    studentInfo = strtok( NULL, ";" ) ;
    printf( "Ocene:%s\n", studentInfo ) ;
  } while( !feof( file )) ;

  printf( "\nIzlistavanje zavrseno.\n\n" ) ;
  
}

void searchBySurname( )
{
  FILE* file ;
  char* prezime ;
  char* match ;
  char* line ;
  
  size_t NoB = 1 ;
  int flag = 0 ;
  
  file = fopen( "/tmp/studenti.dat", "r" ) ;

  prezime = ( char* ) malloc( 21 ) ;
  match = ( char* ) malloc( 21 ) ;
  line = ( char* ) malloc( 50 ) ;
  
  scanf( "%s", prezime ) ;
  do
  {
    if( getline( &line, &NoB, file ) == -1 )
      continue ;

    strtok( line, ";" ) ;
    if( strcmp( prezime, ( match = strtok( NULL, ";" ))))
      printf( "%s", match ) ;
    
  } while( !feof( file )) ;

  if( flag == 0 )
    printf( "Match not found!" ) ;
}



int main( )
{
  FILE* studFile ;
  int menuChoise = 0 ;
  
  if( access( "/tmp/studenti.dat", F_OK ) == - 1 )
  {
    studFile = fopen( "/tmp/studenti.dat", "w" ) ;
    fclose( studFile ) ;
  }
  while( 1 )
  {
    menuChoise = mainMenu( ) ;
    if( menuChoise == 1 )
    {
      fromDataBase( ) ;
    }
    else if( menuChoise == 2 )
    {
      searchBySurname( ) ;
    }
    else if( menuChoise == 3 )
    {
      newStudent( ) ;
    }
    else if( menuChoise == 4 )
    {
      break ;
    }
  }
  return 0 ;
}

