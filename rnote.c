/*
* Open editor to write a note.
* C program -- first arg becomes file name
*	notes saved in $HOME/NOTES_DIR
*/

/* 
*	TODO:
		Alt Editor: arg parsing needs fixing. 
	
*	  Testing:
*		- Need way to check if ~/notes exists, otherwise throw error. 
*		currently, prog will have vis open a non-existant file path 
*		and complain that it cannot write to it, once note is written.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

/*Name of directory where notes are stored */
#define NOTES_DIR "/notes/"
#define TIME_SIZE 12

void
die(const char *message) {
	if(errno) perror(message);
	else printf("ERROR, %s\n");
	exit(1);
}

void
write(char *file, char *editor) {
	char com[50];
	if(editor == NULL)
		editor = "vis";
	sprintf(com, "%s %s", editor, file);
        system(com);
}

char
*tstamp(char opt) {
	struct tm *stmp;
	char *stamp;
        time_t ts;

	time(&ts);
	stmp = localtime(&ts);

	stamp = malloc(TIME_SIZE * sizeof(char));
	if(!stamp) die("Memory error.");

	if(opt == 'd') {
		strftime(stamp, TIME_SIZE, "%Y-%m-%d", stmp);
	} else {
		strftime(stamp, TIME_SIZE, "%T", stmp);
	}

	return stamp;
}

char 
*mkfile(char opt, char *filename) {
	char *file, *stamp;

	file = getenv("HOME");
	strcat(file, NOTES_DIR);
	
	if(opt == 'd') { 
		stamp = tstamp('d');
		strcat(file, stamp);
		free(stamp);
	} else 
		strcat(file, filename);
	
	return file;
}

int
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
		write(file, NULL);			
	} else if(argc > 1 && argc < 5 && argv[1][0] == '-') {
		char opt;
		opt = argv[1][1];
		switch(opt) {
			
			/* Specify alternate editor */
			/* Needs arg parse tweaking */
			case 'e' :
				if(argv[3]) {
					file = mkfile('n', argv[3]);	
				} else {
					file = mkfile('d', NULL);	
				} 
				write(file, argv[2]);			
				break;

			case 'm':
				cli_note();
				break;
			
			default :
				printf("Not an option. Try again.\n");
				break;
		}
	} else if(argv[1][0] != '-') {
			file = mkfile('n', argv[1]);
			write(file, NULL);			
	} else {
		printf("ERROR: Too many args.\n");
		printf("Give 0-1 filenames, ");
		printf("use '-m' for cli note.\n");
		printf("or use '-e' and specify editor.\n");
		exit(0);	
		}
	
	return 0;
}