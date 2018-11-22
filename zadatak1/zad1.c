#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	FILE * fp; //Deklaracija pokazivaca fp tipa FILE
	FILE *button1;
	FILE *button2;
	int val1, val2; 
	float percentage = 1 ; // ovo ce nam biti procenat jacine kojom diode svetle, inicijalizujemo na 100% odnosno 1 
	long int period = 2000 ; // perioda paljenja i gasenja diode
	char *str; // pokazivac str tipa char. Ovo je zapravo neka vrsta Stringa. U C-u se to ovako odradjuje. Zamisli ovo kao niz : char str[ 20 ] ;
	size_t NoB = 1; // number of bytes
	int flag1 = 0, flag2 = 0 ; // flags za kontrolu pritiska i pustanja

	while(1) // beskonacna petlja 
	{
		button1 = fopen("/sys/class/gpio/gpio956/value", "r"); // otvaramo file koji se nalazi na putanji "/sys/class.../value" u read modu odnosno "r". Taj file smestamo u button1
															   // unutar ovog fajla nalazi se vrednost prvog dugmeta. 1 ako je pritisnuto, 0 ako je pusteno.
		if (button1 == NULL) // Ukoluko nismo uspeli da otvorimo file, button1 nece imati vrednost pa cemo poslati poruku o tome na keran u obliku printf-a
			printf("Problem pri otvaranju 1. fajla");

		str = ( char *)malloc(NoB+1); // ovo je dinamicka alokacija memorije. String str ce biti 'niz' duzine nob+1 karaktera. Odnosno 1+1 karaktera. 
							          // Zbog sigurnosnih razloga uvek se uzima onoliko koliko ti treba + 1
									  // Zasto alociramo samo 2 karaktera, zato sto se u tom fajlu uvek nalazi samo jedan broj, koji moramo da ocitamo.
									  // Odnosno nalazi se stanje dugmeta 1 ili 0 
		getline(&str, &NoB, button1); // getline uzima liniju iz fajla na koji pokazuje button1( to je onaj VALUE file ), i smesta ga u string str, nob je broj bajta koji ce uzeti

		fclose(button1); // zatvaramo file na koji pokazuje button1

		sscanf( str, "%d", &val1); // funkcijom getline smo u string str smestili vrednost dugmeta, i sada ga samo upisujemo u promenjivu val1( value1 ) tipa int 
		free(str); // oslobadjamo prostor koji smo uzeli

		button2 = fopen("/sys/class/gpio/gpio957/value", "r");// odavde sve do linije 45, odnosno free(str) ; je isti princip, sve se isto radi samo nad drugim fajlom, odnosno onim koji sadrzi vrednost drugog dugmeta
		if ( button2 == NULL) // ako nismo uspeli da otvorimo file, saljemo poruku o neuspehu
			printf("Problem pri otvaranju 2.fajla");

		str = (char *) malloc(NoB+1);// alociramo 2 karaktera memorije
		getline(&str, &NoB, button2); // uzimamo 1 karakter iz button2 i smestamo u str

		fclose(button2); // zatvaramo file

		sscanf( str, "%d", &val2); // prenosimo vrednost iz str u val2
		free(str); // vracamo sistemu memoriju koju smo uzeli

		// Sve ispred ovog je manje vise bilo napisano od strane asistenta, ovo je ono sto su ocekivali od nas da uradimo

		// Samo jedna stvar. Kad se na ploci pritisne dugme, ono pokazuje stanje 1. Medjutim posto ovaj nas program ima beskonacnu petlju, on bit svakom sledecom
		// iteracijom opet usao u ovaj if, i open bi povecao vrednost percentage. U zadatku kaze da kad ti pritisnes dugme on ce percentage povecati samo za 0.2, koliko god da dugo drzis

		// Tako da se ovde uvode 2 flag-a. flag1 za pritisak prvog dugmeta i flag2 za drugo.

		// Logika je ta da cim ti pritisnes dugme, ovaj uslov ispod ce proci i percentage ce se povecati za 0.2, posle toga cemo staviti flag1 na 1 kako ovaj uslov ispod nikad vise ne bi
		// bio tacan, sve dok se dugme ne pusti, uslov na liniji 62.

		if (val1 == 1 && flag1 == 0 ) // znaci, kad pritisnemo prvo dugme, val1 ce biti 1, i ako je to prvi put da je kod naleteo na ovaj uslov od kako smo stisnuli dugme on ce proci
		{
			if( percentage == 1 ) percentage = 1 ;
			else percentage+=0.2; // povecace percentage za 0.2
			flag1 = 1 ; // i postavice flag1 na 1, kako bi signalizirao da smo vec bili u ovom ifu i da je percentage vec povecan. Ovim, koliko god dugo drzali dugme on ce samo jednom uvecati percentage
		}
		else if( val1 == 0 && flag1 == 1 )// sada ukoliko pustimo prvo dugme
			flag1 = 0 ; // flag1 stavljamo na 0 i tim govorimo da ce se sledecim pritiskom tastera percentage open moci povecati za 0.2, odnosno onaj predgodni if ce opet proci
		if(val2 == 1 && flag2 == 0) // ista stvar ali za dugme2... ako smo pritisnuli dugme i nismo usli u if otkako je pritisnuto
		{
			if( percentage == 0 ) percentage = 0 ;
			else percentage -= 0.2 ; // percentage se smanjuje za 0.2
			flag2 = 1 ;// i flag2 ide na 1 kako ne bi opet usli u if bez pustanja tastera
		}
		else if( val2 == 0 && flag2 == 1 ) // kad pustimo taster
			flag2 = 0 ;// postavi flag opet na 0 


		fp = fopen("/proc/myled", "w"); // otvaramo file myled ovaj put u write modu "w". Ovaj file kontrolise koje diode su upaljene koje ne; pomocu hexa broja od 0-f
		if (fp == NULL)//ako se nije dobro otvorio prijavimo gresku
		{
			printf ("Cannot open /proc/myled for write1\n");
			return -1;
		}
		fputs("0x0F\n", fp); // fputs je funkcija koja ce string staviti u file na koji pokazuje fp, u ovom slucaju je to myled
							 // ovim smo mi zapravo upalili sve diode, 0x0F 
		fclose(fp); // zatvorili smo file
		usleep(percentage*period); // usleep je funkcija koja uspava program za odredjeni broj *mislim* mili sekundi. u ovom slucaju uspavljuje program za onaj nas percentage * periodu koju smo definisali na pocetku zadatka

		fp = fopen("/proc/myled", "w");// ista stvar ali sada gasimo diode. otvaramo myled u write modu
		if( fp == NULL) // prijava greske
		{
			printf("Cannot opet /proc/myled for write2\n");
			return -1;
		}
		
		fputs("0x00\n", fp);// sada postavljamo vrednost 0 u file, sto mu govori ugasi sve diode
		fclose(fp); // zatvaramo file
		usleep((1-percentage)*period); // i uspavljujemo program za odredjeni broj mili sekundi.
	}
	return 0;
}
// e sad ovaj poslednji deo oko usleep-a
// mi zapravo ne ogranicavamo napon diodama ili tako nesto, vec ih brzo palimo i gasimo. Posto mi ne vidimo njihovo paljenje i gasenje, sto su duze one ugasene to se nama cini
// da su vise zamracene, odnosno dimmovane.
// tako da, sto je ovaj DRUGI usleep veci to ce one biti mracnije, a sto ovaj prvi usleep bude veci to ce se one ciniti svetlijim
// iako one zapravo uvek svetle isto samo je razlika u vremenu upaljenosti i ugasenosti dioda.

// i jos ovo
// if( percentage == 1 ) percentage = 1 ; ovo sam radio zato sto ce program prvo naleteti na ovaj uslov, i ako je on tacan nece uopste gledati ono pod else
// moglo se to bolje napisati, ali moze ostati i ovako, ne smeta uopste.. isto tako i za onaj percentage = 0 ;

// ako nesto nije jasno pitaj slobodno, ja vecetas vrv necu ici malo duze na spavanje