#include <stdio.h>
#include <time.h>

/* DEV file for rnote timestamp */

	// This function will full date and time 
	void stamp()
{
        time_t time_raw_format;
		struct tm * time_struct;

		char sep[10] = "-------\n";
		char tstamp[100];
		time ( &time_raw_format );
		time_struct = localtime ( &time_raw_format );
	
		strftime (tstamp,100,"%Y-%m-%d\n%T\n", time_struct);
}

int main ()
	{   
	    char ans[100], tstamp[100];
	    printf("Hello baby! Give something to stamp!\n");
	    	scanf("%[^\n]", ans);

	   /*Print this message to file, prefix w/
	   timestamp */

		FILE *fp;
		fp = fopen("pound", "a");
		stamp();
		    fprintf(fp,"%s", tstamp);
		    fprintf(fp,"%s", ans);
		fclose(fp);

	    return 0;
	}
	
