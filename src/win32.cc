/***************************************************************************
 *   Copyright (C) 2007 by Henning Meyer   *
 *   hmeyer@mathematik.uni-kl.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "bwindow_gdk.hh"

#ifdef WIN32

#include <shlobj.h>

using namespace std;
typedef char char_t;
std::string GetTempPath()
{

	// retrieve application data path
	LPWSTR temp_path = new TCHAR[ MAX_PATH ];
	for( int i = 0; i < MAX_PATH - 1; i++ )
		temp_path[ i ] = ' ';
	temp_path[ MAX_PATH - 1 ] = '\0';
	SHGetSpecialFolderPath( 0, temp_path, CSIDL_APPDATA, TRUE );

	// create surfer-directory
	wcscat( temp_path , L"\\Surfer" );
	CreateDirectory( temp_path, NULL);
	wcscat( temp_path , L"\\temp" );
	CreateDirectory( temp_path, NULL);
	wcscat( temp_path , L"\\" );

	// convert to char*
	char *temp_path_c = new char[ 5 * MAX_PATH ];
	wcstombs( temp_path_c, temp_path, 5 * MAX_PATH );

	return std::string( temp_path_c );
}

std::string homepath()
{
	return ".";
/*
	LPWSTR temp_path = new TCHAR[ MAX_PATH ];
	for( int i = 0; i < MAX_PATH - 1; i++ )
		temp_path[ i ] = ' ';
	temp_path[ MAX_PATH - 1 ] = '\0';
	SHGetSpecialFolderPath( 0, temp_path, CSIDL_APPDATA, TRUE );

	// create surfer-directory
	wcscat( temp_path , L"\\Surfer" );
	CreateDirectory( temp_path, NULL);
	wcscat( temp_path , L"\\" );

	// convert to char*
	char *temp_path_c = new char[ 5 * MAX_PATH ];
	wcstombs( temp_path_c, temp_path, 5 * MAX_PATH );

	return std::string( temp_path_c );
*/
/*
	std::string s="%HOMEPATH%";
	const int bul = 33000;
	char bu[bul];
	ExpandEnvironmentStrings(s.c_str(),bu,bul);


std::cout<<"HOME "<<bu<<std::endl;
	return bu;
*/
}






#endif
