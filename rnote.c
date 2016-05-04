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
*timestamp(const char *fmt) 
{
	struct tm *stmp;
	char *stamp;
        time_t ts;

	time(&ts);
	stmp = localtime(&ts);
	stamp = malloc(21 * sizeof(char));
	if (stamp) 
		strftime(stamp, STAMP_SIZ, fmt, stmp);
	else
		die("Memory error.");

	return stamp;
	free(stamp);
}

static void
get_dir(char *dir) 
{
	char *dirp[100];
	
	*dirp = dir;
		
	if (!snprintf(*dirp, sizeof(dirp), "%s%s", getenv("HOME"), NOTES_DIR))
		die("Couldn't create path to ~/notes directory.");

	if (chdir(*dirp) != 0)
		die("Couldn't change into ~/home directory.");
}

static void 
get_filename(char *path, char *name) 
{
	char *namep, *pathp[100];
	
	*pathp = path;
	namep = (name) ?:timestamp("%Y-%m-%d");
	
	get_dir(path);

	if (strlcat(*pathp, namep, sizeof(pathp)) >= sizeof(pathp))
		die("Unable to add file name to path.");
	else	
		path = *pathp;
	
	if (!name) 
		free(namep);
}


static void
write_note(char *path, char *name) 
{
	char com[100];
	size_t len = (strlen(EDITOR) + strlen(path) + 1);

	get_filename(path, name);

	if (len > sizeof(com))
		puts("WARNING:Filepath truncated. Filename likely too long.");
	
	snprintf(com, sizeof(com), "%s %s", EDITOR, path);
	
	execl("/bin/sh", "sh", "-c", com, (char *)NULL);
}

static void
list_notes() 
{
	int n;
	char file[50];
	struct dirent **namelist;
	
	get_dir(file);

	n = scandir(file, &namelist, 0, alphasort);
	if (n < 0)
		die("Couldn't open ~/notes directory.");
	
	while (n--) {
		if ((*namelist)->d_name[0] != '.') {
			printf("%s\n", (*namelist)->d_name);
		}
		namelist++;
	}
	free(namelist);
}

static void
delete_note(int count, char *target[]) 
{ 
	int i, c, *tap;
	char **tp;
	int tarray[20] = {};
		
	tp = target;
	tap = tarray;
	
	for (i = 1; --count; i++) {
		FILE *fp;
		char path[75];
		
		if (tp[i][0] != '-') {
			get_dir(path);
			if (strlcat(path, tp[i], sizeof(path)) >= sizeof(path))
				die("Truncated path to deletion target.");
			
			/*  If target exists, store its argv index in tarray. */
			if ((fp = fopen(path, "r"))) {
				*tap++ = i;
			}
			else
				printf("***No such file:\t%s\n", tp[i]);
		}
	}
	
	/*  Print targets via their index value stored in tarray */
	tap = tarray;
	if (*tap) {
		puts("Files to be deleted:");
		for (; *tap; tap++) 
			printf("-> %s\n", tp[*tap]);
			
		puts("Confirm delete? (Upper-case \'Y\')");
		if (getchar() == 'Y') {
			for (tap = tarray; *tap; tap++)
				remove(tp[*tap]);
			puts("Files deleted.");
		} else {
			puts("Aborted.");
		}
	} else 
		puts("No files to delete.");
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
	
	stamp = timestamp("%T");
	
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
	
	for (i = strlen(line); n < 3 && i--; *pt++ = *pl++)
		if (isspace((*line++)))
			n++;
	
	/*  If we count 3 spaces, the NULL goes at *(pt - 1), else at *pt */
	pt[(n == 3) ? -1 : 0] = '\0';

	if (strlen(title) > sizeof(title)) {
		puts("Title bonked, but we recorded your note!");
	} else	{
		snprintf(msg, sizeof(msg), "> Note \"%s\" written to file %s",
			 title, file);
		puts(msg);
	}
	
	fclose(bp);
}

int
main(int argc, char *argv[]) 
{
	int i;
	char file[100], opt = '\0';

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
			
		if (strstr(argv[1], " ")) {
			inline_note(file, sizeof(file), argv[1]);
		} else if (argv[1][0] != '-') {
			write_note(file, argv[1]);			
		} else
			die("Empty option flag.");		
	} else {
		switch (opt) {
			case 'l':
				if (argv[2]) 
					printf("Running option \'-%c\'. ", opt);
					puts("Ignoring other arguments.");
				list_notes();
				break;
			case 'd':
				if (!argv[2]) 
					die("Please provide files for deletion.");
				delete_note(argc, argv);
				break;
			case 'v':
				if (argv[2]) 
					puts("***Ignoring extra arguments.");
				printf("%s, (c) %s Ryan Donnelly\n",
					 VERSION, YEAR);
				break;
			default :
				puts("Not a valid option.");
				break;
		}
	}
	return 0;
}
