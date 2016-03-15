/*
* Open editor to write a note.
*
*	TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>

#define NOTES_DIR "/notes/" 	/* Directory where notes are stored */
#define EDITOR "vis"		/* Text editor of choice */

void
die(const char *message) {
	printf("ERROR: %s\n", message);
	exit(1);
}

/* custom strncat implementation guards against truncation errors */
char
*r_strncat(char *dest, char *src) {
	if(strlen(src) + 1 > sizeof(dest) - strlen(dest))
		die("File would be truncated.");
	else
		strncat(dest, src, sizeof(dest) - strlen(dest) - 1);

	return dest;
}

void
write_note(char *note, char *editor) {
	char com[50];
	
	snprintf(com, sizeof(com), "%s %s", editor, note);
	execl("/bin/sh", "sh", "-c", com, (char *)NULL);
}
 
void
list_notes() {
	int i, n;
	char *file;
	struct dirent **namelist;
	
	file = getenv("HOME");
	r_strncat(file, NOTES_DIR);
	
	n = scandir(file, &namelist, 0, alphasort);
	if(n < 0) 
		die("Couldn't open ~/notes directory.");
	 else {
		for(i = 0; i < n; i++) {
			if(namelist[i]->d_name[0] != '.') {
				printf("%s\n", namelist[i]->d_name);
				free(namelist[i]);
			} else 	continue;
		}
	}
	free(namelist);
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
	r_strncat(file, NOTES_DIR);

	if(!opendir(file)) die("Could not open ~/notes directory.");
	chdir(file);

	if(filename) { 
		r_strncat(file, filename);
	} else 
		r_strncat(file, stamp);
		free(stamp);
	
	return file;
}

void
cli_note(char *line) {
	FILE *bp;
	char *file, *stamp, buf[1000];

	file = mkfile(NULL);
	stamp = tstamp("%T");
	bp = fopen(file, "a+");
	if(!bp) 
		die("Couldn't open file.");

	if(line) {
		
		fprintf(bp, "\n\n%s \n", stamp);
		fprintf(bp, "%s", line);
		
		printf("> Note written to file \"%s\"\n", file);
	} else {
		fputs("Note: ", stdout);
		fgets(buf, 1000, stdin);
	
		fprintf(bp, "\n\n%s \n", stamp);
		fprintf(bp, "%s", buf);
	}

	free(stamp);
	fclose(bp);
}

void
inline_note(char *line) {
	
	FILE *bp;
	char *file, *stamp;

	file = mkfile(NULL);
	stamp = tstamp("%T");
	bp = fopen(file, "a+");
	if(!bp) 
		die("Couldn't open file.");

	fprintf(bp, "\n\n%s \n", stamp);
	fprintf(bp, "%s", line);

	free(stamp);
	fclose(bp);
	
	printf("> Note written to file \"%s\"\n", file);
}


int
check_space(char *string) {
	int i, n; 

	n = 0;
	if(string){
		for(i = 0; string[i] != '\0'; i++) {
			if(isspace(string[i])) 
				n++;
		}
		if(n > 0) {
			return 1;
		}
	} 
	return 0;
}

int
main(int argc, char *argv[]) {
	char *file;

	if(argc == 1) {
		file = mkfile(NULL);	
		write_note(file, EDITOR);			

	} else if(argc == 2 && check_space(argv[1]) == 1) {
		cli_note(argv[1]);

	} else if(argc == 2 && argv[1][0] != '-') {
		file = mkfile(argv[1]);
		write_note(file, EDITOR);			

	} else if(argc < 5 && argv[1][0] == '"') {
		puts("It's a quotation mark!");

	} else if(argc < 5 && argv[1][0] == '-') {

		char opt = argv[1][1];
		switch(opt) {
			case 'e' :
				if(!argv[2]) 
					die("Please specify an editor.");
				if(argv[3]) 
					file = mkfile(argv[3]);	
				 else 
					file = mkfile(NULL);	
				 
				write_note(file, argv[2]);			
				break;
			case 'm':
				if(argv[2]) die("Option 'm' takes no arguments.");
				cli_note(NULL);
				break;
			case 'l':
				if(argv[2]) die("Option 'l' takes no arguments.");
				list_notes();
				break;
			default :
				printf("Not an option. Try again.\n");
				break;
		}
	} else die("Too many arguments.");

	return 0;
}
