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

void write(char *file) {
	char com[50];
	sprintf(com, "vis %s", file);
        system(com); 
}

void mkfile(int argc, char *fname) {
	
	char *file;
	file = getenv("HOME");
	strcat(file, NOTES_DIR);

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

int main(int argc, char *argv[]) {
	
	if(argc > 2) {
		printf("Too many args. Give 0-1 filenames.\n");
		exit(0);	
	} 

	mkfile(argc, argv[1]);

	return 0;
}