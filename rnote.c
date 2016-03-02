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
#define TIME_SIZE 12

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
	stamp = malloc(TIME_SIZE * sizeof(char));

	time(&ts);
	stmp = localtime(&ts);

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
	printf("In fn Before Assmt:\n%s \n", file);
	
	if(opt == 'd') { 
		stamp = tstamp('d');
		strcat(file, stamp);
		free(stamp);
		printf("In fn Post Assmt:\n%s \n", file);
	} else {
		strcat(file, filename);
		printf("In fn Post Assmt:\n%s \n", file);
	}
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
	stamp = tstamp('t');
	fprintf(bp, "\n\n%s \n", stamp);
	fprintf(bp, "%s", buf);
	free(stamp);

	fclose(bp);
}



int
main(int argc, char *argv[]) {

	if(argc == 1) {
		char *file;
		file = mkfile('d', NULL);	
		write(file);			
	} else 
		if(argc > 3) {
		printf("Too many args. Give 0-1 filenames.\n");
		exit(0);	
	} else
		if(argv[1][0] != '-') {
			char *file;
			file = mkfile('t', argv[1]);
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