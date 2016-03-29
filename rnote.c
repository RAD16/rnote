/* 
* rnote - (c) 2016 Ryan Donnelly <herbivore dot linux at gmail dot com>
*	rnote is licensed under the GPL v2.1
*	rnote is available at https://gihub.com/rad16/rnote
*
* Open editor to write a note.
*
*/

#define VERSION "rnote-0.2" 
#define YEAR "2016" 

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

	name = filename ?: tstamp("%Y-%m-%d");

	if(strlcat(path, name, len) > len)
		die("Unable to complete file path.");
	
	if(!filename) free(name);
}

void
get_dir(char *dir) {
	size_t len = 100;	
	char *home;
	
	home = getenv("HOME");
	if(!snprintf(dir, len, "%s%s", home, NOTES_DIR))
		die("Couldn't create path to ~/notes directory.");

	if(!opendir(dir)) {
		puts("ERROR:Could not open ~/notes/ directory.\n"
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
write_note(char *note) {
	char com[50];
	
	snprintf(com, sizeof(com), "%s %s", EDITOR, note);
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
			}
		}
	}
	free(namelist);
}

void
delete_note(int count, char *target[]) { 
	int c, i, k = 0;
	int target_index[20] = {};
		
	for(c = 2; c < count; c++) {
		FILE *fp;
		char path[75];
		
		get_dir(path);
		strlcat(path, target[c], 75);
		
		/* 
		*  If target file exists, store its argv number in target_index array.
		*/
		if((fp = fopen(path, "r")) != NULL) {
			target_index[k] = c;
			k++;
		} else 
			printf("***No such file:\t%s\n", target[c]);
	}
	
	/*
	*  Print target files by referencing their index value
	*  as stored in target_index[]
	*/
	if(target_index[0] != 0) {
		puts("Files to be deleted:");
		for(i = 0; target_index[i] != '\0'; i++) {
			printf("-> %s\n", target[target_index[i]]);
		}
		puts("Confirm delete? (Upper-case \'Y\')");
		if(getchar() == 'Y') {
			for(i = 0; target_index[i] != '\0'; i++) 
				remove(target[target_index[i]]);
				
			puts("Files deleted.");
		} else 
			puts("Aborted.");
	}
}

void
inline_note(char *file, size_t len, char *line) {
	FILE *bp;
	int i, n;
	char *stamp;
	char title[40], msg[70];

	get_filename(file, NULL);
	stamp = tstamp("%T");
	bp = fopen(file, "a+");
	if(!bp) 
		die("Couldn't open file.");

	fprintf(bp, "\n\n%s \n", stamp);
	fprintf(bp, "%s", line);
	free(stamp);
	
	/*
	*  Parse spaces to create note title
	*  Title has 3 Words (n < 3) 
	*/
	for(n = 0, i = 0; n < 3 && i < strlen(line); ++i) {
		if(isspace(line[i])) 
			++n;
		title[i] = line[i];
	}
	
	/* Terminate resulting string dynamically */
	title[i - ((n == 3) ? 1:0)] = '\0';

	if(strlen(title) > sizeof(title)) {
		puts("Title bonked, but we recorded your note!");
	} else	{
		snprintf(msg, sizeof(msg), "> Note \"%s\" written to file %s", title, file);
		puts(msg);
	}
	
	fclose(bp);
}

int
main(int argc, char *argv[]) {
	char file[100];
	get_dir(file);

	if(argc == 1) {
		get_filename(file, NULL);	
		write_note(file);			

	} else if(argc == 2 && strstr(argv[1], " ")) {
		inline_note(file, sizeof(file), argv[1]);

	} else if(argc == 2 && argv[1][0] != '-') {
		get_filename(file, argv[1]);
		write_note(file);			
	} else if(argv[1][0] == '-') {

		char opt = argv[1][1];
		switch(opt) {
			case 'l':
				if(argv[2]) die("Option 'l' takes no arguments.");
				list_notes();
				break;
			case 'd':
				if(!argv[2]) die("Please provide files for deletion.");
				delete_note(argc, argv);
				break;
			case 'v':
				if(argv[2]) die("Option 'v' takes no arguments.");
				printf("%s, (c) %s Ryan Donnelly\n", VERSION, YEAR);
				break;
			default :
				puts("Not an option. Try again.");
				break;
		}
	} else die("Too many arguments.");

	return 0;
}
