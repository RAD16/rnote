/*
* Open editor to write a note.
* C program -- first arg becomes file name
* 	program prints time stamp as first entry
*	notes saved in NOTES_DIRECTORY
*
*
*
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NOTES_DIRECTORY "/home/rad16/notes/"

char file[100];
char notedir[] = NOTES_DIRECTORY;
char rtime[20];

/*
int timing() {
	time_t ts;
	time(&ts);
	MeTime = ctime(&ts);
	return 0;
}
*/

void stamp() {

        time_t ts;
	struct tm *date;

	time(&ts);
	date = localtime(&ts);
	strftime(rtime, 20, "%Y-%m-%d", date);
}

int mknote() {
	
	stamp();

	strcpy(file, notedir);
	strcat(file, rtime);

/*	FILE *fp;
	fp = fopen(file, "a");
	fclose(fp);
*/
	return 0;
}

int namenote(char *name) {
	strcpy(file, notedir);
	strcat(file, name);

/*	FILE *fp;
	fp = fopen(file, "a");
	fclose(fp);
*/
	return 0;
}

int edit() {    // 
	char com[50];

	sprintf(com, "vis %s", file);
        system(com); 

	return 0;
    }

int main(int argc, char *argv[]) {
	if(argc == 1) {
		mknote();
	} else if(argc == 2) {
		namenote(argv[1]);
	} else {
		printf("Too many arguments. Enter 0-1 file names.\n");
	}
	edit();

	return 0;
}