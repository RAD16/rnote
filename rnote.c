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

#define NOTES_DIR "/notes/"
#define EDITOR "vis"

static void
die(const char *message) 
{
	char buf[80];
	snprintf(buf, sizeof(buf), "ERROR: %s\n", message);
	fputs(buf, stderr);
	exit(1);
}

static char
*tstamp(const char *opt) 
{
	struct tm *stmp;
	char *stamp;
        time_t ts;

	time(&ts);
	stmp = localtime(&ts);
	stamp = malloc(21 * sizeof(char));
	if (!stamp) 
		die("Memory error.");

	strftime(stamp, 21, opt, stmp);

	return stamp;
	free(stamp);
}

static void 
get_filename(char *path, char *filename) 
{
	size_t len = 100;
	char *name;

	name = filename ?: tstamp("%Y-%m-%d");

	if (strlcat(path, name, len) > len)
		die("Unable to complete file path.");
	
	if (!filename) 
		free(name);
}

static void
get_dir(char *dir) 
{
	size_t len = 100;	
	char *home;
	
	home = getenv("HOME");
	if (!home)
		die("Couldn't retrieve ~/home path.");
		
	if (!snprintf(dir, len, "%s%s", home, NOTES_DIR))
		die("Couldn't create path to ~/notes directory.");

	if (!opendir(dir)) {
		puts("ERROR:Could not open ~/notes/ directory.\n"
			"Should we create it? (y/n)");
		char ans;
		ans = fgetc(stdin);
		if (ans == 'y')  
			mkdir(dir, 0750);
		else 
			die("Note not written. Filepath nonexistent.");
	}
	if (chdir(dir) != 0)
		die("Couldn't change into ~/home directory.");
}

static void
write_note(char *note) 
{
	char com[50];
	
	snprintf(com, sizeof(com), "%s %s", EDITOR, note);
	execl("/bin/sh", "sh", "-c", com, (char *)NULL);
}

static void
list_notes() 
{
	int i, n;
	char *f, file[50];
	struct dirent **namelist;
	
	f = getenv("HOME");
	if (strlcpy(file, f, sizeof(file)) >= sizeof(file))
		die("Couldn't initiate filepath.");
		
	if (strlcat(file, NOTES_DIR, sizeof(file)) >= sizeof(file))
		die("File path truncated.");
	
	n = scandir(file, &namelist, 0, alphasort);
	if (n < 0) {
		die("Couldn't open ~/notes directory.");
	} else {
		for (i = 0; i < n; i++) {
			if (namelist[i]->d_name[0] != '.') {
				printf("%s\n", namelist[i]->d_name);
				free(namelist[i]);
			}
		}
	}
	free(namelist);
}

static void
delete_note(int count, char *target[]) 
{ 
	int c, *ptg;
	char **tg;
	int targind[20] = {};
		
	tg = target;
	ptg = targind;
	
	for (c = 2; c < count; c++) {
		FILE *fp;
		char path[75];
		
		get_dir(path);
		if (strlcat(path, tg[c], 75) >= 75)
			die("Truncation occured catting delete targ to path.");
		
		/* 
		*  If target file exists, store its argv number in targind array.
		*/
		if ((fp = fopen(path, "r"))) {
			*ptg = c;
			ptg++;
		} else {
			printf("***No such file:\t%s\n", tg[c]);
		}
	}
	
	/*
	*  Print target files by referencing their index value
	*  as stored in targind[]
	*/
	ptg = targind;
	if (*ptg) {
		puts("Files to be deleted:");
		for (; *ptg; ptg++) 
			printf("-> %s\n", tg[*ptg]);
		puts("Confirm delete? (Upper-case \'Y\')");
		if (getchar() == 'Y') {
			for (ptg = targind; *ptg; ptg++)
				remove(tg[*ptg]);
			puts("Files deleted.");
		} else {
			puts("Aborted.");
		}
	}
}

static void
inline_note(char *file, size_t len, char *line) 
{
	FILE *bp;
	int i, n;
	char title[40], msg[70];
	char *stamp, *pt, *pl;
	
	pt = title;
	pl = line;

	get_filename(file, NULL);
	
	stamp = tstamp("%T");
	
	bp = fopen(file, "a+");
	if (!bp) 
		die("Couldn't open file.");

	fprintf(bp, "\n\n%s \n", stamp);
	fprintf(bp, "%s", line);
	free(stamp);
	
	/*
	*  Parse spaces to create note title
	*  Title has 3 Words (n < 3) 
	*/
	for (n = 0, i = 0; n < 3 && i < strlen(line); ++i) {
		if (isspace(line[i])) 
			++n;
		*pt++ = *pl++;
	}
	
	/* Terminate resulting string dynamically */
	title[i - ((n == 3) ? 1 : 0)] = '\0';

	if (strlen(title) > sizeof(title)) {
		puts("Title bonked, but we recorded your note!");
	} else	{
		snprintf(msg, sizeof(msg), "> Note \"%s\" written to file %s", title, file);
		puts(msg);
	}
	
	fclose(bp);
}

int
main(int argc, char *argv[]) 
{
	char file[100];
	get_dir(file);

	if (argc == 1) {
		get_filename(file, NULL);	
		write_note(file);			

	} else if (argc == 2 && strstr(argv[1], " ")) {
		inline_note(file, sizeof(file), argv[1]);

	} else if (argc == 2 && argv[1][0] != '-') {
		get_filename(file, argv[1]);
		write_note(file);			
		
	} else if (argv[1][0] == '-') {

		char opt = argv[1][1];
		switch (opt) {
			case 'l':
				if (argv[2]) 
					die("Option 'l' takes no arguments.");
				list_notes();
				break;
			case 'd':
				if (!argv[2]) 
					die("Please provide files for deletion.");
				delete_note(argc, argv);
				break;
			case 'v':
				if (argv[2]) 
					die("Option 'v' takes no arguments.");
				printf("%s, (c) %s Ryan Donnelly\n", VERSION, YEAR);
				break;
			default :
				puts("Not an option. Try again.");
				break;
		}
	} else 
		die("Too many arguments.");

	return 0;
}
