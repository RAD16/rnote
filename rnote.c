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
#define STAMP_SIZ 21

static void
die(const char *msg) 
{
	if (errno) 
		perror(msg);
	else
		printf("ERROR: %s\n", msg);
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
	if (stamp) 
		strftime(stamp, STAMP_SIZ, opt, stmp);
	else
		die("Memory error.");

	return stamp;
	free(stamp);
}

static void 
get_filename(char *path, char *name) 
{
	char *pn, *pp[100];
	
	*pp = path;
	pn = name ?: tstamp("%Y-%m-%d");
	
	if (strlcat(*pp, pn, sizeof(pp)) >= sizeof(pp))
		die("Unable to add file name to path.");
	else	
		path = *pp;
	
	if (!name) 
		free(pn);
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
write_note(char *path) 
{
	char com[100];
	size_t plen = (strlen(EDITOR) + strlen(path) + 1);

	if (plen > sizeof(com))
		die("File path has been truncated. The name is probably too long.");
	
	snprintf(com, sizeof(com), "%s %s", EDITOR, path);
	
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
	int c, *pta;
	char **tg;
	int targarray[20] = {};
		
	tg = target;
	pta = targarray;
	
	for (c = 2; c < count; c++) {
		FILE *fp;
		char path[75];
		
		get_dir(path);
		if (strlcat(path, tg[c], sizeof(path)) >= sizeof(path))
			die("Truncation occured catting delete target onto path.");
		
		/*  If target exists, store its argv index in targarray. */
		if ((fp = fopen(path, "r")))
			*pta++ = c;
		else
			printf("***No such file:\t%s\n", tg[c]);
	}
	
	/*  Print targets via their index value stored in targarray */
	pta = targarray;
	if (*pta) {
		puts("Files to be deleted:");
		for (; *pta; pta++) 
			printf("-> %s\n", tg[*pta]);
		puts("Confirm delete? (Upper-case \'Y\')");
		if (getchar() == 'Y') {
			for (pta = targarray; *pta; pta++)
				remove(tg[*pta]);
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
	int i, n = 0;
	char title[100], msg[100];
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
	*  Title has 3 Words maximum (n < 3) 
	*/

	i = strlen(line);
	while (n < 3 && i--) {
		if (isspace(*line++))
			n++;
		*pt++ = *pl++;
	}
	
	/*  If we count 3 spaces, the NULL goes at *(pt - 1), else at *pt */
	pt[(n == 3) ? -1 : 0] = '\0';
	

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
