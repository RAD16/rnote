		char tstamp[100];
		time_t time_raw_format;
		struct tm * time_struct;

		time ( &time_raw_format );
		time_struct = localtime ( &time_raw_format );
		strftime (tstamp,100,"%Y-%m-%d\n%T\n", time_struct);

		    fprintf(fp,"%s\n", tstamp);

