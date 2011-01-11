#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#include <string.h>

namespace
{
	static char localeDir[ FILENAME_MAX ];
	char * getLocaleDir()
	{
		// backup current working directory
		char cwd_backup[ FILENAME_MAX ];
		_getcwd( cwd_backup, sizeof( cwd_backup ) );

		// go one directory upwards
		_chdir( ".." );		

		// save this path
		_getcwd( localeDir, sizeof( localeDir ) );

		// change back to previous directory
		_chdir( cwd_backup );
		
		// append relative path to locale dir
		return strcat( localeDir, "\\share\\locale\\" );
	}
};

#define LOCALEDIR getLocaleDir()
#define PACKAGE "surfer"