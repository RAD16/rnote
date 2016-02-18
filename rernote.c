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

char notefile[100];
char *MeTime;

int timing() {
	time_t ts;
	time(&ts);
	MeTime = ctime(&ts);
	return 0;
}

int CreateNote(char *note) {
	char wholeFile[100];
	char *notedir = NOTES_DIRECTORY;
	
	strcpy(notefile, notedir);
	strcat(notefile, note);

	FILE *fp;
	fp = fopen(notefile, "a");
		fprintf(fp, "\n------------------------\n");
		fprintf(fp, MeTime);
		fprintf(fp, "------------------------\n");
	fclose(fp);
	return 0;

}

/*
* main note: edit notes in vis 
*/

int EditNote() {    // 
	char command[50];
	sprintf(command, "vis %s", notefile);
        system(command); 
	return 0;
    }

int main(int argc, char *argv[]) {
	if(argc == 1) {
		printf("Must give an argument.\n");
	} else {
		timing();
		CreateNote(argv[1]);
		EditNote();	
	}	
	return 0;
}