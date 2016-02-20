/* Simple C program that allows for quick, one-line notes to be recorded from the command line
   into a single file to be parsed later. 
   
   TO-DO: 
    - learn to write to file in a different directory so R-Notes can be stored in home dir.
    - insert time stamp
    - write conditionals based on time stamp (i.e. when to create a new note and cycle old ones)
    - write function call to Vim for editing notes
 */

#include <stdio.h>
#include <stdlib.h>

// WRITE NOTE main 
    int write_note() 
    {
         char note[1001];      // initialize variables
       
       printf("RNote:_"); 
         scanf("%[^\n]", &note);        // scanf reads input from stdin until user hits return
        
         FILE *src_p;      // create file pointer
        src_p = fopen("RNOTE", "a"); // Write to master note  file
          char sep[]="\n--------------\n";
          fprintf(src_p, note);        // write note to file
          fprintf(src_p, sep);         // write in separator to keep notes file neat
        fclose(src_p);

        src_p = fopen("now", "a"); // Write to daily file
          fprintf(src_p, note);        // write note to file
          fprintf(src_p, sep);         // write in separator to keep notes file neat
        fclose(src_p);
        return 0;
    }

 /* PARSE OPTIONS
  Replace array of If statements with more efficient menu option (switches or something) 
  
  switch(opt)
    case "-d" :
        print_note_today(); break;
    
    case "-a" : 
        print_note_all(); break: 
    
    case "-Ed" || "-Ea" : 
        edit_note(); break;
    
    case "-c" : 
        config(); break; 
    
    case "-h" || "--help" : 
        menu(); break;
  */
    int parse_opts(char *opt)
     {
       int c = (strcmp(opt, "-c"));  // Config ? 
       int h = (strcmp(opt, "-h") || strcmp(opt, "--help"));
       
        if (strcmp(opt, "-d") == 0) {
            print_note_today();
        }
    
        if (strcmp(opt, "-a") == 0) {
            print_note_all();
        }
      
        if ((strcmp(opt, "-Ed") == 0) || (strcmp(opt, "-Ea") == 0)) {
            edit_note(opt);
        }
      
        if (h == 0) {
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
   printf("\t  -d \t\t: Display the current day's notes\n");
   printf("\t  -a \t\t: Display the master RNote file\n");
   printf("\t  -Ed \t\t: Open current day's notes for editing in Vim\n");
   printf("\t  -Ea \t\t: Open the master RNote file for editing in Vim\n");
   printf("\t  -c \t\t: Open RNote config file for editing in Vim\n");
   printf("\t  --help or -h \t: Open RNote Help\n");
   
   return 0;
      
  }
 

     
   //Below function should be new primary note-writing function, replace uncommented one below it. 
    /*
    int write_note_now(char *n)
te
{
    printf("Note: %s\n", n);
    FILE *np, *RN;
        char sep[] = "\n-----------\n";
        np =fopen("now", "a");
            fprintf(np, "%s", sep);
            fprintf(np, "%s\n", n); 
        fclose(np);
        RN = fopen("RNOTES", "a");
            fprintf(RN, "%s", sep);
            fprintf(RN, "%s\n", n); 
        fclose(RN);
        
    return 0;
}
    
    */

    
    
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

// display notes from current calendar day
    int print_note_today()  // Function has not been tested
    {
        FILE *src_p;
        char buf[1000];
        src_p = fopen("now", "r");
            
            while( ( *buf = fgetc(src_p) ) != EOF )
                fprintf("\n %s\n", *buf);
            
        fclose(src_p);
        
        return 0;
    }

// display all notes in RNotes file
// option  ' -a '
    int print_note_all() //Fucntion tested and working!
    {
        FILE *src_p;
        char buf;
        src_p = fopen("RNOTE", "r");
            
            while( ( buf = fgetc(src_p) ) != EOF )
        	fputc(buf, stdout);     
            
        fclose(src_p);
     
        return 0;
    }
    
/* open notes for editing in vim
    Create options to pass either current day or master file to function, so that
    ' *note_file ' gets assigned either the 'day' file or the 'RNOTE' file
*/
    int edit_note(char *opt_ed)    // 
    {
        char today_note[] = "now";    // File variables
        char all_notes[] = "RNOTE";
        char file[15], command[20];
        
        int tn = (strcmp(opt_ed, "-Ed"));
        int an = (strcmp(opt_ed, "-Ea"));
        
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
       switch(argc)
       {
           case 1 : write_note(); break;
            
           case 2 : parse_opts(argv[1]); break;
            
           default : menu(); break;
       }
       
    return 0;
}
