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



using namespace std;
typedef char char_t;
std::string GetTempPath()
{
  DWORD result = ::GetTempPath(0, "");
  if(result == 0)
    throw std::runtime_error("Could not get system temp path");

  std::vector<char_t> tempPath(result + 1);
  result = ::GetTempPath(static_cast<DWORD>(tempPath.size()), &tempPath[0]);
  if((result == 0) || (result >= tempPath.size()))
    throw std::runtime_error("Could not get system temp path");

  std::string u(
    tempPath.begin(), tempPath.begin() + static_cast<std::size_t>(result)
  );

std::cout<<"TEMP "<<u<<std::endl;
return u;
}


std::string homepath()
{return ".\\";
	std::string s="%HOMEPATH%";
	const int bul = 33000;
	char bu[bul];
	ExpandEnvironmentStrings(s.c_str(),bu,bul);


std::cout<<"HOME "<<bu<<std::endl;
	return bu;
}

#endif
