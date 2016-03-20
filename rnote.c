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
#include <errno.h>

#define NOTES_DIR "/notes/" 	/* Directory where notes are stored */
#define EDITOR "vis"		/* Text editor of choice */

void
die(const char *message) {
	char buf[50];
	snprintf(buf, sizeof(buf), "ERROR: %s\n", message);
	fputs(buf, stderr);
	exit(1);
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
	char *f, file[50];
	struct dirent **namelist;
	
	f = getenv("HOME");
	strlcpy(file, f, sizeof(file));
	if(strlcat(file, NOTES_DIR, sizeof(file)) >= sizeof(file))
		die("File path truncated.");
	
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

void 
mkfile(char *infile, size_t len, char *filename) {
	char *home, *stamp, *file;

	home = getenv("HOME");
	stamp = tstamp("%Y-%m-%d");
	
	puts(home);
	puts(infile);
	if(strlcat(infile, home, len) >= len)
		die("Couldn't add home directory to filepath in mkfile.");
	printf("%s\n", infile);
	
	if(strlcat(infile, NOTES_DIR, len - 1) >= len)
		die("Couldn't add notes directory to filepath in mkfile.");

	printf("Infile: %zu\n", sizeof(infile));
	printf("Strlen Infile: %zu\n", strlen(infile));
	printf("Infile: %c\n", infile[18]);
	file = infile;
	DIR *dir;
	snprintf(file, 50, "%s", infile);
	dir = opendir(file);
	if(!dir) {
		perror("What's going on?");
		printf("ERROR:Could not open %s directory.", file);
		puts("Should we create it? (y/n)");
		char ans[1];
		ans[0] = fgetc(stdin);
		if(ans[0] == 'y') {
			mkdir(infile, 0750);
		} else {
			puts("Exiting.");
			exit(0);
		}
	}

	printf("%s\n", infile);
	chdir(infile);

	if(filename) { 
		if(strlcat(infile, filename, len) >= len)
			die("Couldn't add filename to path.");
	} else 
		if(strlcat(infile, stamp, len) >= len)
			die("Couldn't add timestamp to path.");
	printf("Final in mkfile: %s\n", infile);
	free(stamp);
}

void
inline_note(char *file, size_t len, char *line) {
	FILE *bp;
	char *stamp;

	mkfile(file, len, NULL);
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
	char file[100];

	if(argc == 1) {
		mkfile(file, sizeof(file), NULL);	
		write_note(file, EDITOR);			

	} else if(argc == 2 && check_space(argv[1])) {
		inline_note(file, sizeof(file), argv[1]);

	} else if(argc == 2 && argv[1][0] != '-') {
		mkfile(file, sizeof(file), argv[1]);
		write_note(file, EDITOR);			

	} else if(argc < 5 && argv[1][0] == '-') {

		char opt = argv[1][1];
		switch(opt) {
			case 'e' :
				if(!argv[2]) 
					die("Please specify an editor.");
				if(argv[3]) {
					mkfile(file, sizeof(file), argv[3]);	
					write_note(file, argv[2]);			
				 } else {
					mkfile(file, sizeof(file), NULL);	
					write_note(file, argv[2]);			
				}
				break;
			case 'l':
				if(argv[2]) die("Option 'l' takes no arguments.");
				list_notes();
				break;
			default :
				puts("Not an option. Try again.\n");
				break;
		}
	} else die("Too many arguments.");

	return 0;
}
