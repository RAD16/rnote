﻿/* Simple C program that allows for quick, one-line notes to be recorded from the command line into a single file to be parsed later. 
   
   TO-DO: 
    - error handling
    - if files/ directories don't exist, create them
    - instant note: user types ' rnote - blah blah blah' straight into daily note without having to hit return
    - insert time stamp &  write conditionals based on time stamp (i.e. when to create a new note and cycle old ones)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------- Function workshop -------------

// INSTANT NOTE - under development

// TIMESTAMP function -- under development 

 -------- end workshop -------------*/

// WRITE NOTE main 
    int write_note() 
    {
         char note[1001];      // initialize variables
         char sep[]="\n--------------\n";
       
       printf("RNote:_"); 
         scanf("%[^\n]", note);        // scanf reads input from stdin until user hits return
        
// Investigate writing  with fputc ??  Maybe not --

         FILE *src_p;      // create file pointer
        src_p = fopen("RNOTE", "a"); // Write to master note  file

		char tstamp[100];
		time_t time_raw_format;
		struct tm * time_struct;

		time ( &time_raw_format );
		time_struct = localtime ( &time_raw_format );
		strftime (tstamp,100,"%Y-%m-%d\n%T\n", time_struct);

					fprintf(src_p,"%s\n", tstamp);
          fprintf(src_p, "\n%s", note);        // write note to file
          fprintf(src_p, "%s",sep);         // write in separator to keep notes file neat
        fclose(src_p);

        src_p = fopen("now", "a"); // Write to daily file
					fprintf(src_p,"%s\n", tstamp);
          fprintf(src_p, "%s", note);        // write note to file
          fprintf(src_p, "%s",sep);         // write in separator to keep notes file neat
        fclose(src_p);
       
        return 0;
    }
    
 // PARSE OPTIONS
    int parse_opts(char *opt)
     {
       int c = (strcmp(opt, "-c"));  // Config ? Not yet available
       int h = (strcmp(opt, "-h") || strcmp(opt, "--help"));
       
        if (strcmp(opt, "-d") == 0) {
            print_note_today();
        }
    
        if (strcmp(opt, "-a") == 0) {
            print_note_all();
        }
      
        if ((strcmp(opt, "-e") == 0) || (strcmp(opt, "-ea") == 0)) {
            edit_note(opt);
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
   printf("\t  -d \t\t: Display the day's notes\n");
   printf("\t  -a \t\t: Display all notes in the master RNote file\n");
   printf("\t  -e \t\t: Edit current day's notes (using Vim)\n");
   printf("\t  -ea \t\t: Edit all notes in the master RNote file (using Vim)\n");
   printf("\t  -c \t\t: Open RNote config file for editing in Vim\n");
   printf("\t  --help or -h \t: Open RNote Help\n");
   
   return 0;
      
  }
    
    
// File rotation still under dev
/* mv_now_yest and mv_yest_dbf to be replaced by single function that accepts args
    that specify the source and target files for copying */

    int mv_now_yest(char *src, char *targ)       // cycle: copy "now" note to "yesterday"
    {
        FILE *src_p, *targ_p;
        char buf;
        src_p = fopen(src, "r");
        targ_p = fopen(targ, "w");
            
            while( ( buf = fgetc(src_p) ) != EOF )
                fputc(buf, targ_p);
            
        fclose(src_p);
        fclose(targ_p);
    
    return 0;
}

int mv_yest_dbf()           // cycle: copy "yesterday" note to "day before"
{
    FILE *src_p, *targ_p;
        char buf;
        src_p = fopen("yest", "r");
        targ_p = fopen("dbf", "w");
            
            while( ( buf = fgetc(src_p) ) != EOF )
                fputc(buf, targ_p);
            
        fclose(src_p);
        fclose(targ_p);
 
    return 0;
}

// DISPLAY NOTES from current calendar day
    int print_note_today()  // Function has not been tested
    {
            
        FILE *src_p;
        char buf;
        src_p = fopen("now", "r");
            
            while( ( buf = fgetc(src_p) ) != EOF )
        	fputc(buf, stdout);     
        fclose(src_p);
        
        return 0;
    }

// DISPLAY ALL notes in RNotes file
// option  ' -a '
    int print_note_all() //Function tested and working!
    {
        FILE *src_p;
        char buf;
        src_p = fopen("RNOTE", "r");
            
            while( ( buf = fgetc(src_p) ) != EOF )
        	fputc(buf, stdout);     
            
        fclose(src_p);
     
        return 0;
    }
    
// EDIT notes in vim 
    int edit_note(char *opt_ed)    // 
    {
        char today_note[] = "now";    // File variables
        char all_notes[] = "RNOTE";
        char file[15], command[20];
        
        int tn = (strcmp(opt_ed, "-e"));
        int an = (strcmp(opt_ed, "-ea"));
        
        if(tn == 0) {
            char *file = today_note;
            sprintf(command, "vim %s", file);
        }
        else if(an == 0) {
            char *file = all_notes;
            sprintf(command, "vim %s", file);
        }
        
/* sprintf(command, "vim %s", file); ### If sprintf() called outside if statement like this, 
    file pointer is empty. WHY???

*/
        system(command); 
        
        return 0;
    }
    

// MAIN FUNCTION CALL
      int main(int argc, char *argv[]) 
{
       /* const char config_dir[] = "~/.rnote/";
       mkdir(config_dir, 0700); */
       
       switch(argc)
       {
           case 1 : write_note(); break;
            
           case 2 : parse_opts(argv[1]); break;
            
           default : menu(); break;
       }
       
    return 0;
}
