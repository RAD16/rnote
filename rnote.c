/*
* Open editor to write a note.
* C program -- first arg becomes file name
*	notes saved in $HOME/NOTES_DIR
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*Name of directory where notes are stored */
#define NOTES_DIR "/notes/"

void
write(char *file) {
	char com[50];
	sprintf(com, "vis %s", file);
        system(com); 
}

char
*tstamp(char opt) {
		char *stamp;
	        time_t ts;
		struct tm *stmp;
		stamp = malloc(12 * sizeof(char));
	
		time(&ts);
		stmp = localtime(&ts);

	if(opt == 'd') {
		strftime(stamp, 12, "%Y-%m-%d", stmp);
		
		printf("Inside Function:\n");
		printf("Option D: %s\n", stamp);

	} else {
		strftime(stamp, 12, "%T", stmp);

		printf("Inside Function:\n");
		printf("Option T: %s\n", stamp);
	}

	return stamp;
}

void
mkfile(int argc, char *fname) {
	char *file;
	if(argc == 1) {
		char stamp[12];
	        time_t ts;
		struct tm *date;
	
		time(&ts);
		date = localtime(&ts);
		strftime(stamp, 12, "%Y-%m-%d", date);

		strcat(file, stamp);

	} else if(argc == 2) {
		strcat(file, fname);
	}
	
	write(file);			
}

int
cli_note(void) {
	char buf[1001], *stamp, *file;

	file = getenv("HOME");
	strcat(file, NOTES_DIR);

	printf("Note: ");
	scanf("%[^\n]", buf); 

	stamp = tstamp('d');
	strcat(file, stamp);
	printf("Stamp: %s \n", stamp);
	printf("File: %s \n", file);
	free(stamp);

	FILE *bp;
	bp = fopen(file, "a+");
	stamp = tstamp('t');
	fprintf(bp, "\n\n%s \n", stamp);
	fprintf(bp, "%s", buf);

	fclose(bp);
}



int
main(int argc, char *argv[]) {

	if(argc == 1) {
		char *file, *stamp;
	
		file = getenv("HOME");
		strcat(file, NOTES_DIR);
		stamp = tstamp('d');
		strcat(file, stamp);
		write(file);			
		free(stamp);
	} else 
		if(argc > 3) {
		printf("Too many args. Give 0-1 filenames.\n");
		exit(0);	
	} else
		if(argv[1][0] != '-') {
			char *file;
		
			file = getenv("HOME");
			strcat(file, NOTES_DIR);
			strcat(file, argv[1]);
			write(file);			
	} else 
		if(argv[1][0] == '-') {
			char opt;
			opt = argv[1][1];
			switch(opt) {
				case 'm':
					cli_note();
					break;
				
				default :
					printf("Not an option. Try again.");
					break;
			}
		}
	 
	
	return 0;
}