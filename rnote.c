/*
* Open editor to write a note.
* C program -- first arg becomes file name
*	notes saved in $HOME/NOTES_DIR
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
write(char *file) {
	char com[50];
	sprintf(com, "vis %s", file);
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

	if(argc >= 3) {
		printf("ERROR: Too many args.\n");
		printf("Give 0-1 filenames, ");
		printf("or use '-m' for cli note.\n");
		exit(0);	
	} else 
		if(argc == 1) {
			char *file;
			file = mkfile('d', NULL);	
			write(file);			
	} else
		if(argv[1][0] != '-') {
			char *file;
			file = mkfile('n', argv[1]);
			write(file);			
	} else 
		if(argv[1][0] == '-') {
			char opt;
			opt = argv[1][1];
			switch(opt) {
				case 'm':
					cli_note();
					break;
				
				case 'r':
					printf("Ryan is awesome! Slap his hand!\n");
					break;

				case 'b':
					printf("Boooooobs!\n");
					break;

				case 'p':
					printf("Panis panis panis!\n");
					break;
				default :
					printf("Not an option. Try again.\n");
					break;
			}
		}
	
	return 0;
}