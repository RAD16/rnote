/*
* Open editor to write a note.
* C program -- first arg becomes file name
*	notes saved in $HOME/NOTES_DIR
*/

/* 
*	TODO:
	
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

#define TIME_SIZE 12

char *config[] = {
/*
*	Configuration: Notes dir is where notes are stored 
*	and is within the /home/ dir, so "/notes/" will be 
*	expanded to "/home/username/notes/". 
*
*	Notes dir	Editor					
*/	"/notes/",	"vis"	};

void
die(const char *message) {
	if(errno) perror(message);
	else printf("ERROR: %s\n", message);
	exit(1);
}

void
write(char *file, char *editor) {
	char com[50];
	if(editor == NULL)
		/* Default Editor */
		editor = config[1];
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
	strcat(file, config[0]);
	
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

	} else if(argc == 2 && argv[1][0] != '-') {
		file = mkfile('n', argv[1]);
		write(file, NULL);			

	} else if(argc > 1 && argc < 5 && argv[1][0] == '-') {
		char opt;
		opt = argv[1][1];
		switch(opt) {
			
			/* Specify alternate editor */
			case 'e' :
				if(!argv[2]) 
					die("Please specify and editor.");
				if(argv[3]) {
					file = mkfile('n', argv[3]);	
				} else {
					file = mkfile('d', NULL);	
				} 
				write(file, argv[2]);			
				break;

			/* Single line note appended to dated file */
			case 'm':
				cli_note();
				break;
			
			default :
				printf("Not an option. Try again.\n");
				break;
		}
	} else {
		printf("ERROR: Too many arguments.\n");
		printf("Usage:\n");
		printf("\trnote [-m] <filename>\n");
		printf("\trnote [-e] EDITOR <filename>\n");
		printf("'-m' for cli note.\n");
		printf("'-e' and specify editor.\n");
		printf("Not specifying a filename creates a note with date as title.\n");
		exit(0);	
		}
	return 0;
}