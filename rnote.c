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
	char buf[80];
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
get_filename(char *path, char *filename) {
	size_t len = 100;
	char *name;

	if(filename) name = filename;
	else name = tstamp("%Y-%m-%d");

	if(strlcat(path, name, len) > len)
		die("Unable to complete file path.");
	
	if(!filename) free(name);
}

void
get_dir(char *dir) {
	size_t len = 100;	
	char *home;
	
	home = getenv("HOME");
	printf("get_dir 1 Dir: %s\n", dir);
	if(!snprintf(dir, len, "%s%s", home, NOTES_DIR))
		die("Couldn't create path to ~/notes directory.");

	if(!opendir(dir)) {
		puts("ERROR:Could not open ~/notes/ directory."
		"Should we create it? (y/n)");
		char ans[1];
		ans[0] = fgetc(stdin);
		if(ans[0] == 'y')  
			mkdir(dir, 0750);
		else 
			die("Note not written. Filepath nonexistent.");
	}
	chdir(dir);
}

void
inline_note(char *file, size_t len, char *line) {
	FILE *bp;
	char *stamp;

	get_filename(file, NULL);
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
	for(i = 0; n < 2 && i < strlen(line); ++i) {
		if(isspace(line[i])) 
			++n;
		title[i] = line[i];
	}
	if(n == 2) title[i - 1] = '\0';
	else title[i] = '\0';
		
	printf("After loop:\ni:%d, n:%d\n", i, n);
	printf("title[i]: %c\n", title[i]);
	printf("title[i - 1]: %c\n", title[i - 1]);
	printf("title[i - 2]: %c\n", title[i - 2]);
	printf("title: %s\n", title);
	printf("strlen(title):%d\n sizeof(title):%zu\n", strlen(title), sizeof(title));

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
	get_dir(file);

	if(argc == 1) {
		get_filename(file, NULL);	
		write_note(file, EDITOR);			

	} else if(argc == 2 && check_space(argv[1])) {
		inline_note(file, sizeof(file), argv[1]);

	} else if(argc == 2 && argv[1][0] != '-') {
		get_filename(file, argv[1]);
		write_note(file, EDITOR);			

	} else if(argc < 5 && argv[1][0] == '-') {

		char opt = argv[1][1];
		switch(opt) {
			case 'e' :
				if(!argv[2]) 
					die("Please specify an editor.");
				if(argv[3]) {
					get_filename(file, argv[3]);	
					write_note(file, argv[2]);			
				 } else {
					get_filename(file, NULL);	
					write_note(file, argv[2]);			
				}
				break;
			case 'l':
				if(argv[2]) die("Option 'l' takes no arguments.");
				list_notes();
				break;
			default :
				puts("Not an option. Try again.");
				break;
		}
	} else die("Too many arguments.");

	return 0;
}
