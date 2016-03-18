/* SLOC: 188
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
#include <sys/stat.h>
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
	free(stamp);
}

char 
*mkfile(char *filename) {
	char *file, *stamp;

	file = getenv("HOME");
	stamp = tstamp("%Y-%m-%d");
	r_strncat(file, NOTES_DIR);

	if(!opendir(file)) {
		puts("ERROR:Could not open ~/notes directory.");
		puts("Should we create it? (y/n)");
		char ans[1];
		ans[0] = fgetc(stdin);
		if(ans[0] == 'y') {
			mkdir(file, 0750);
		} else {
			puts("Exiting.");
			exit(0);
		}
	}

	chdir(file);

	if(filename) { 
		r_strncat(file, filename);
	} else 
		r_strncat(file, stamp);
	free(stamp);
	
	return file;
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
	
	/* 
	 * Inline Note message testing suite
	 * 	Delete after running a while. 
	 */
/*	int i, n;
	char msg[70];
	char title[30];
	n = 0;
	for(i = 0; n <= 1 && i < strlen(line); ++i) {
		if(isspace(line[i])) 
			n++;
		printf("i = %d n = %d\n", i, n);
		if(n == 2) break;
		title[i] = line[i];
	}
	if(strlen(title) > sizeof(title))
		puts("We bonked.");
	
	puts(title);

	printf("int i: %d\nsizeof(title): %zu\n strlen(title): %zu\n", i, sizeof(title), strlen(title));
	printf("sizeof(line): %zu\n strlen(line): %zu\n", sizeof(line), strlen(line));
	
	snprintf(msg, sizeof(msg), "> Note \"%s\" written to file %s", title, file);
	puts(msg);
	
	fclose(bp);
*/
	int i, n;
	char msg[70];
	char title[40];
	n = 0;
	for(i = 0; n <= 1 && i < strlen(line); ++i) {
		if(isspace(line[i])) 
			n++;
		if(n == 2) break;
		title[i] = line[i];
	}
	if(strlen(title) > sizeof(title)) {
		puts("Title bonked, but we recorded your note!");
	} else	{
		snprintf(msg, sizeof(msg), "> Note \"%s\" written to file %s", title, file);
		puts(msg);
	}
	
	fclose(bp);
}

int
check_space(char *string) {

	if(string){
		int i; 
		for(i = 0; i < strlen(string); i++) {
			if(isspace(string[i])) 
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

	} else if(argc == 2 && check_space(argv[1])) {
		inline_note(argv[1]);

	} else if(argc == 2 && argv[1][0] != '-') {
		file = mkfile(argv[1]);
		write_note(file, EDITOR);			

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
