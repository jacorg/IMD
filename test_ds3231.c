#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 32
static char receive[BUFFER_LENGTH];

int main(){
	int ret,fd,cant_lectura;
	char texto[BUFFER_LENGTH];
	
	printf("Open device driver.\n");
	fd = open("/dev/ds3231", O_RDWR);
	if (fd < 0){
		printf("Failure...");
		return -1;
	}

	while(1)
	{
		printf("===========================================\r\n");
		printf("1 > Ver info RTC DS3231 DD:MM:YY & HH:MM:SS\r\n");
		printf("0 > Salida\r\n");
		scanf("%s", texto);
		
		if(strlen(texto) > BUFFER_LENGTH)
			texto[BUFFER_LENGTH-1] = '\0';
			
		if(texto[0]=='0')
			break;	
		ret = write (fd,texto,strlen(texto)+1);
		if (ret < 0){
			printf("......Error read device......");
			close(fd);
			return -1;
		}

	}
	close(fd);
	return 0;
}
