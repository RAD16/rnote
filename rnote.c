/*
* Open editor to write a note.
*/

/*	TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define NOTES_DIR "/notes/" 	/* Directory where notes are stored */
#define EDITOR "vis"		/* Text editor of choice */

void
die(const char *message) {
	printf("ERROR: %s\n", message);
	exit(1);
}

void
write_note(char *note, char *editor) {
	char com[50];
	
	sprintf(com, "%s %s", editor, note);
        system(com);
}
 
/*
*  Goal: write function to list contents of ~/notes to stdout
*  	>> alphabetize
*/

void
ls_notes() {
	int i, n;
	char *file;
	struct dirent **namelist;
	
	file = getenv("HOME");
	strncat(file, NOTES_DIR, 8);
	
	n = scandir(file, &namelist, 0, alphasort);
	if(n < 0) die("Couldn't open ~/notes directory.");
	 else {
		for(i = 0; i < n; i++) {
			printf("%s\n", namelist[i]->d_name);
			free(namelist[i]);
		}
	free(namelist);
	}
}

char
*tstamp(const char *opt) {
	struct tm *stmp;
	char *stamp;
        time_t ts;
	time(&ts);
	stmp = localtime(&ts);
	stamp = malloc(21 * sizeof(char));
	if(!stamp) die("Memory error.");

	strftime(stamp, 21, opt, stmp);

	return stamp;
}

char 
*mkfile(char *filename) {
	char *file, *stamp;

	file = getenv("HOME");
	stamp = tstamp("%Y-%m-%d");

	if(strlen(NOTES_DIR) + 1 > sizeof(file) - strlen(file))
		die("File would be truncated.");
	else
		strncat(file, NOTES_DIR, sizeof(file) - strlen(file) - 1);
	
	if(!opendir(file)) die("Could not open ~/notes directory.");
	chdir(file);
	
	if(filename) { 
		if(strlen(filename) + 1 > sizeof(file) - strlen(file))
			die("File would be truncated.");
		else
			strncat(file, filename, sizeof(file) - strlen(file) - 1);
	} else 
		strncat(file, stamp, strlen(stamp));
		free(stamp);
	
	return file;
}

void
cli_note(void) {
	FILE *bp;
	char *file, *stamp, buf[1001];

	file = mkfile(NULL);
	stamp = tstamp("%T");
	bp = fopen(file, "a+");
	if(!bp) die("Couldn't open file.");

	printf("Note: ");
	scanf("%[^\n]", buf); 

	fprintf(bp, "\n\n%s \n", stamp);
	fprintf(bp, "%s", buf);

	free(stamp);
	fclose(bp);
}

int
main(int argc, char *argv[]) {
	char *file;

	if(argc == 1) {
		file = mkfile(NULL);	
		write_note(file, EDITOR);			
	} else if(argc == 2 && argv[1][0] != '-') {
		file = mkfile(argv[1]);
		write_note(file, EDITOR);			
	} else if(argc < 5 && argv[1][0] == '-') {
		char opt;
		opt = argv[1][1];
		switch(opt) {
			case 'e' :
				if(!argv[2]) 
					die("Please specify and editor.");
				if(argv[3]) {
					file = mkfile(argv[3]);	
				} else {
					file = mkfile(NULL);	
				} 
				write_note(file, argv[2]);			
				break;
			case 'm':
				if(argv[2]) die("Option 'm' takes no arguments.");
				cli_note();
				break;
			case 'l':
				ls_notes();
				break;
			default :
				printf("Not an option. Try again.\n");
				break;
		}
	} else die("Too many arguments.");

	return 0;
}
