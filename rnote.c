/* 
* rnote - (c) 2016 Ryan Donnelly <herbivore dot linux at gmail dot com>
*	rnote is licensed under the GPL v2.1
*	rnote is available at https://gihub.com/rad16/rnote
*
* Open editor to write a note.
*
*/

#define VERSION "rnote-0.4" 
#define YEAR "2016" 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define NOTES_DIR "/notes/"
#define EDITOR "vis"
#define STAMP_SIZ 21 /* timestamp size */

static void
die(const char *s, int eflag) 
{
	if (!eflag)
		puts(s);
	else if (errno)
		perror(s); 
	else
		printf("ERROR: %s\n", s);
	exit(1);
}

static void
timestamp(char *s, const char *fmt) 
{
	struct tm *stmp;
        time_t ts;

	time(&ts);
	stmp = localtime(&ts);
	strftime(s, STAMP_SIZ, fmt, stmp);
}

static void
get_dir(char *dir) 
{
	char *dirp[100];
	
	*dirp = dir;
		
	if (!snprintf(*dirp, sizeof(dirp), "%s%s", getenv("HOME"), NOTES_DIR))
		die("Couldn't create path to ~/notes directory.", 1);

	if (chdir(*dirp) != 0)
		die("Couldn't change into ~/home directory.", 1);
}

static void 
get_filename(char *path, char *name) 
{
	char *pathp[100], time[STAMP_SIZ];
	char *namep;
	
	timestamp(time, "%Y-%m-%d");
	
	namep = (name) ?: time;
	*pathp = path;
	
	get_dir(path);

	if (strlcat(*pathp, namep, sizeof(pathp)) >= sizeof(pathp))
		die("Unable to add file name to path.", 1);
	else	
		path = *pathp;
	
	if (!name) 
		free(namep);
}


static void
write_note(char *path, char *name) 
{
	char cmd[100];
	size_t len = (strlen(EDITOR) + strlen(path) + 1);

	get_filename(path, name);

	if (len > sizeof(cmd))
		puts("WARNING:Filepath truncated. Filename likely too long.");
	
	snprintf(cmd, sizeof(cmd), "%s %s", EDITOR, path);
	
	execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
}

static void
list_notes() 
{
	int n;
	char file[50];
	struct dirent **d;
	
	get_dir(file);

	n = scandir(file, &d, 0, alphasort);
	if (n < 0)
		die("Couldn't open ~/notes directory.", 1);
	
	while (n--) {
		if ((*d)->d_name[0] != '.')
			printf("%s\n", (*d)->d_name);
		++d;
	}
	free(d);
}

static void
append_note(char *file, char *s) 
{
	FILE *fp;
	int i, n = 0;
	char title[100], msg[100], time[STAMP_SIZ];
	char *pt = title;
	char *pl = s;

	get_filename(file, NULL);
	timestamp(time, "%T");
	
	fp = fopen(file, "a+");
	if (!fp) 
		die("Couldn't open file.", 1);
	
	fprintf(fp, "\n\n%s \n", time);
	fprintf(fp, "%s", s);
	
	/* Parse spaces to create note title: 3 words max (n < 3) */
	for (i = strlen(s); n < 3 && i--; *pt++ = *pl++)
		if (isspace((*s++)))
			n++;
	
	/*  If we count 3 spaces, the NULL goes at *(pt - 1), else at *pt */
	pt[(n == 3) ? -1 : 0] = '\0';

	if (strlen(title) > sizeof(title)) {
		puts("Title bonked, but we recorded your note!");
	} else	{
		snprintf(msg, sizeof(msg), "> Note \"%s\" written to file %s", title, file);
		puts(msg);
	}
	
	fclose(fp);
}

int
main(int argc, char *argv[]) 
{
	int i;
	char file[100], opt = '\0';

	/* check args for option flag, grab it if present */
	for (i = 1; i < argc && opt == '\0'; i++)
		if (argv[i][0] == '-' && argv[i][1])
			opt = argv[i][1];

	if (argc == 1) {
		write_note(file, NULL);	
		return 0;
	} 
	
	if (!opt) {
		if( argc > 2) 
			printf("***Ignoring extra arguments.");
			
		/* spaces within argv[1] indicate a note to be appended */
		if (strstr(argv[1], " ")) {
			append_note(file, argv[1]);
		} else if (argv[1][0] != '-') {
			write_note(file, argv[1]);			
		} else
			die("Empty option flag.", 1);		
	} else {
		switch (opt) {
		case 'l':
			if (argv[2]) {
				printf("Running option \'-%c\'. ", opt);
				puts("Ignoring other arguments.");
			}
			list_notes();
			break;
		case 'v':
			if (argv[2]) 
				puts("***Ignoring extra arguments.");
			printf("%s, (c) %s Ryan Donnelly\n", VERSION, YEAR);
			break;
		default :
			die("Not a valid option.", 0);
			break;
		}
	}
	return 0;
}
