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
#include <iostream>

bool my_alpha(const char x)
{
	switch(x)
	{
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'x':
		case 'y':
		case 'z':
		return true;
		default:
		return false;
	}
}

std::string fix_input(const std::string& s)
{
	//return s;
	std::string T;
	for(unsigned i = 0; i < s.size(); i++)
	{
		if(i)
		{
			if(my_alpha(s[i-1]) && my_alpha(s[i]))
			{T+='*';T+=s[i];}
			else if(my_alpha(s[i-1]) && isdigit(s[i]))
			{T+='^';T+=s[i];}
			else if(isdigit(s[i-1]) && my_alpha(s[i]))
			{T+='*';T+=s[i];}
			else
			{T+=s[i];}
		}
		else
		T+=s[i];
	}


	return T;
}


std::string fix_input_for_display(const std::string& s)
{
	if(s.size()<3) return s;

	if(s.substr(2) == "1*") return(s.substr(2,s.size()-2));
	
	std::string t = s;
	while(t.find("+1*") != -1L)
	t=t.replace(t.find("+1*"),3,"+");

	while(t.find("-1*") != -1L)
	t=t.replace(t.find("-1*"),3,"-");

	return t;
}


std::string fix_input_for_surf(const std::string& s)
{
	
	std::string t = s;
	while(t.find("-x") != -1L)
	t=t.replace(t.find("-x"),2,"-1*x");

	while(t.find("-y") != -1L)
	t=t.replace(t.find("-y"),2,"-1*y");

	while(t.find("-z") != -1L)
	t=t.replace(t.find("-z"),2,"-1*z");

	while(t.find("-a") != -1L)
	t=t.replace(t.find("-a"),2,"-1*a");

	while(t.find("-b") != -1L)
	t=t.replace(t.find("-b"),2,"-1*b");

	
	while(t.find("-u") != -1L)
	t=t.replace(t.find("-u"),2,"-1*u");

	while(t.find("-v") != -1L)
	t=t.replace(t.find("-v"),2,"-1*v");

	while(t.find("-w") != -1L)
	t=t.replace(t.find("-w"),2,"-1*w");

	while(t.find("-c") != -1L)
	t=t.replace(t.find("-c"),2,"-1*c");

	while(t.find("-d") != -1L)
	t=t.replace(t.find("-d"),2,"-1*d");



	return t;
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
