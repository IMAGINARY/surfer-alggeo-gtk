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

#include <string>
#include <ctype.h>
#include <sstream>


std::string fix_input(const std::string& s)
{
	return s;
	std::string T;
	for(unsigned i = 0; i < s.size(); i++)
	{
		if(i)
		{
			if(isalpha(s[i-1]) && isalpha(s[i]))
			{T+='*';T+=s[i];}
			else if(isalpha(s[i-1]) && isdigit(s[i]))
			{T+='^';T+=s[i];}
			else if(isdigit(s[i-1]) && isalpha(s[i]))
			{T+='*';T+=s[i];}
			else
			{T+=s[i];}
		}
		else
		T+=s[i];
	}
	return T;
}

bool check_input(const std::string& s)
{
	for(unsigned i = 0; i < s.size(); i++)
	{
		if(s[i]=='^')
		{
			std::istringstream f(s.c_str()+i+1);
			int k = 0;
			f>>k;
			if(k>=30)
				return false;
		}
	}
	return true;
}

