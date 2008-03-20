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
	return ".\\";
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



class AniDia: public Gtk::Dialog
{

};

    /**
     * Sets the value of this quaternion to the rotational component of
     * the passed matrix.
     * @param m1 the matrix3f
     */
matrix<double>    rot2quat(const matrix<double>& m1)
    {
	matrix<double> quat(4,1);

	double ww = 0.25*(m1.el(1,1) + m1.el(2,2) + m1.el(3,3) + 1.0);

	if (ww >= 0) {
	    if (ww >= EPS2) {
		quat.el(1,1) = Math.sqrt(ww);
		ww = 0.25/quat.el(1,1);
		quat.el(2,1) = ((m1.m21 - m1.m12)*ww);
		quat.el(3,1) = ((m1.m02 - m1.m20)*ww);
		quat.el(4,1) = ((m1.m10 - m1.m01)*ww);
		return;
	    }
	} else {
	    this.w = 0;
	    this.x = 0;
	    this.y = 0;
	    this.z = 1;
	    return;
	}

	this.w = 0;
	ww = -0.5*(m1.m11 + m1.m22);
	if (ww >= 0) {
	    if (ww >= EPS2) {
		this.x = Math.sqrt(ww);
		ww = 0.5/this.x;
		this.y = (m1.m10*ww);
		this.z = (m1.m20*ww);
		return;
	    }
	} else {
	    this.x = 0;
	    this.y = 0;
	    this.z = 1;
	    return;
	}
     
	this.x = 0;
	ww =  0.5*(1.0 - m1.m22);
	if (ww >= EPS2) {
	    this.y = Math.sqrt(ww);
	    this.z = (m1.m21/(2.0*this.y));
	}
     
	this.y =  0;
	this.z =  1;
    }



#endif
