/* Simple C program that allows for quick, one-line notes to be recorded from the command line into a single file to be parsed later. 
   
   TO-DO:    
	- error handling
	- if files/ directories don't exist, create them
	- instant note: user types ' rnote - blah blah blah' straight into daily note without having to hit return
	- insert time stamp &  write conditionals based on time stamp (i.e. when to create a new note and cycle old ones)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

    
 // PARSE OPTIONS
    int parse_opts(char *opt)
     {
       int h = (strcmp(opt, "-h") || strcmp(opt, "--help"));
      
        if ((strcmp(opt, "-t") == 0)) {
            terminal_note(opt);
        }
        if ((strcmp(opt, "-h") == 0) || (strcmp(opt, "--help") == 0)) {
            menu();
        }
      
      return 0;
     }
     
// OPTIONS MENU
   int menu()       
  {
   printf("\n RNote Usage Menu\n");  
   printf("\t\n usage: rnote [options] \n"); 
   printf(" Options: \n");
   printf("\t  [no option] \t: Write Rnote to standard files\n"); 
   printf("\t  -e \t\t: Edit current day's notes (using Vim)\n");
   printf("\t  -ea \t\t: Edit all notes in the master RNote file (using Vim)\n");
   printf("\t  -c \t\t: Open RNote config file for editing in Vim\n");
   printf("\t  --help or -h \t: Open RNote Help\n");
   
   return 0;

 }
    
// main note: edit notes in vis 
    int main_note(char *opt_ed)    // 
    {
        char today_note[] = "rnote_test";    // File variables
				char *file = today_note, command[20];
            sprintf(command, "vis %s", file);
        system(command); 
				return 0;
    }
/*
* terminal note on command line
*/    
    int terminal_note() 
    {
         char note[1001];      // initialize variables
       printf("RNote:_"); 
         scanf("%[^\n]", note);        // scanf reads input from stdin until user hits return

         FILE *src_p;
        src_p = fopen("rnote-tline-test", "w"); 
          fprintf(src_p, "%s", note);       
        fclose(src_p);
       
        return 0;
    }

/*
* main
*/
      int main(int argc, char *argv[]) 
{
      if ( argc == 0 )
				main_note(argv[1]);
			else 
				parse_opts(argv[1]);
    return 0;
}
