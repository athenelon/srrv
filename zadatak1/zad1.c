#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	FILE * fp;
	FILE *button1;
	FILE *button2;
	int val1, val2;
	float percentage = 1;
	long int period =2000;
	char *str;
	size_t NoB = 1;
	int flag1 = 0, flag2 = 0 ;
	while(1) 
	{
		button1 = fopen("/sys/class/gpio/gpio956/value", "r");
		if (button1 == NULL)
			printf("Problem pri otvaranju 1. fajla");

		str = ( char *)malloc(NoB+1);
		getline(&str, &NoB, button1);

		fclose(button1);

		sscanf( str, "%d", &val1);
		free(str);

		button2 = fopen("/sys/class/gpio/gpio957/value", "r");
		if ( button2 == NULL)
			printf("Problem pri otvaranju 2.fajla");

		str = (char *) malloc(NoB+1);
		getline(&str, &NoB, button2);

		fclose(button2);

		sscanf( str, "%d", &val2);
		free(str);

		if (val1 == 1 && flag1 == 0 ) 
		{
			if( percentage == 1 ) percentage = 1 ;
			else percentage+=0.2;
			flag1 = 1 ;
		}
		else if( val1 == 0 && flag1 == 1 )
			flag1 = 0 ;
		if(val2 == 1 && flag2 == 0)
		{
			if( percentage == 0 ) percentage = 0 ;
			else percentage -= 0.2 ;
			flag2 = 1 ;
		}
		else if( val2 == 0 && flag2 == 1 )
			flag2 = 0 ;


		fp = fopen("/proc/myled", "w");
		if (fp == NULL)
		{
			printf ("Cannot open /proc/myled for write1\n");
			return -1;
		}
		fputs("0x0F\n", fp);
		fclose(fp);
		usleep(percentage*period);

		fp = fopen("/proc/myled", "w");
		if( fp == NULL)
		{
			printf("Cannot opet /proc/myled for write2\n");
			return -1;
		}
		
		fputs("0x00\n", fp);
		fclose(fp);
		usleep((1-percentage)*period);
	}
	return 0;
}
