/*
* Open editor to write a note.
*/
/* 
*	TODO:
*		- Feature: passing argv[1] searches NOTES_DIR for pre-existing
*		file of name argv[1]. If none is found, create new file of that name.
*
*	  Testing:
*		- Need way to check if ~/notes exists, otherwise throw error. 
*		currently, prog will have vis open a non-existant file path 
*		and complain that it cannot write to it, once note is written.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define NOTES_DIR "/notes/"
#define EDITOR "vis"

void
die(const char *message) {
	printf("ERROR: %s\n", message);
	exit(1);
}

void
write_note(char *note, char *editor) {
	char com[50];
	
	if(editor == NULL)
		editor = EDITOR;
	sprintf(com, "%s %s", editor, note);
        system(com);
}

char
*tstamp(char opt) {
	struct tm *stmp;
	char *stamp;
        time_t ts;

	time(&ts);
	stmp = localtime(&ts);

	stamp = malloc(12 * sizeof(char));
	if(!stamp) die("Memory error.");

	if(opt == 'd') {
		strftime(stamp, 12, "%Y-%m-%d", stmp);
	} else {
		strftime(stamp, 12, "%T", stmp);
	}

	return stamp;
}

char 
*mkfile(char opt, char *filename) {
	char *file, *stamp;

	file = getenv("HOME");
	strcat(file, NOTES_DIR);
	chdir(file);
	
	if(opt == 'd') { 
		stamp = tstamp('d');
		strcat(file, stamp);
		free(stamp);
	} else 
		strcat(file, filename);
	
	return file;
}

void
cli_note(void) {
	char buf[1001], *stamp, *file;

	file = mkfile('d', NULL);

	printf("Note: ");
	scanf("%[^\n]", buf); 

	FILE *bp;
	bp = fopen(file, "a+");
	if(!bp) die("Couldn't open file.");

	stamp = tstamp('t');

	fprintf(bp, "\n\n%s \n", stamp);
	fprintf(bp, "%s", buf);

	free(stamp);
	fclose(bp);
}

int
main(int argc, char *argv[]) {
	char *file;

	if(argc == 1) {
		file = mkfile('d', NULL);	
		write_note(file, NULL);			

	} else if(argc == 2 && argv[1][0] != '-') {
		file = mkfile('n', argv[1]);
		write_note(file, NULL);			

	} else if(argc > 1 && argc < 5 && argv[1][0] == '-') {
		char opt;
		opt = argv[1][1];
		switch(opt) {
			case 'e' :
				if(!argv[2]) 
					die("Please specify and editor.");
				if(argv[3]) {
					file = mkfile('n', argv[3]);	
				} else {
					file = mkfile('d', NULL);	
				} 
				write_note(file, argv[2]);			
				break;
			case 'm':
				cli_note();
				break;
			default :
				printf("Not an option. Try again.\n");
				break;
		}
	} else die("Too many arguments.");

	return 0;
}