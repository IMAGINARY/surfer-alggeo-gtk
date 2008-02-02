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

#ifndef GALLERY_PATH
#define GALLERY_PATH_USE ""
#else
#define xstr(s) astr(s)
#define astr(s) #s
#define GALLERY_PATH_USE xstr(GALLERY_PATH)
#endif


#include "bwindow_gdk.hh"
#include <cstdlib>


std::string home_dir()
{
	#ifdef WIN32
	std::string homepath();
	return homepath();
	#endif
	std::string s = std::getenv("HOME");
	return s;
}

std::string fix_file(const std::string& s)
{

	std::string r = s;

	
	//if(!s.empty()&&s[0]=='~')
	//r = home_dir()+s.substr(1);
	
	//std::cout<<"R "<<r<<std::endl;
	while(r.find("~")!=-1)
	{
		
		int i = r.find("~");
		//std::cout<<i<<std::endl;
		r = r.replace(i,1,home_dir());
	}

	while(r.find(WRONG_DIR_SEP_CHAR)!=-1)
	{
		
		int i = r.find(WRONG_DIR_SEP_CHAR);
		//std::cout<<i<<std::endl;
		r[i] = DIR_SEP_CHAR;
	}

	return r;
}

std::string fix_path(const std::string& s)
{
	if(!s.empty() && s[s.size()-1]!=DIR_SEP_CHAR)
	return s+DIR_SEP_CHAR;
	else return s;
}


surfer_options default_settings()
{
	surfer_options so;
	so.gallery_path = GALLERY_PATH_USE;
	//so.save_cmd = "convert # \"~/public_html/surface_\"`date +%Y%m%d_%k%d`\".jpg\"";
	//so.print_cmd = "convert # /tmp/surf_print.ps && lpr /tmp/surf_print.ps";
	so.save_cmd = "";
	so.print_cmd = "";
	
	so.format = "ppm";
	so.resolution = PREVIEW_SIZE;
	so.surf_cmd="surf";
	so.upscale = 1;
	return so;
}

std::ostream& write(const surfer_options& so, std::ostream& f)
{
	f<<"gallery="<<so.gallery_path<<std::endl;
	f<<"save="<<so.save_cmd<<std::endl;
	f<<"print="<<so.print_cmd<<std::endl;
	f<<"format="<<so.format<<std::endl;
	f<<"resolution="<<so.resolution<<std::endl;
	f<<"surf="<<so.surf_cmd<<std::endl;
	f<<"upscale="<<so.upscale<<std::endl;
	f<<"entryfont="<<so.entryfont<<std::endl;
	f<<"help="<<so.help_cmd<<std::endl;
	f<<"helpfile="<<so.helpfile<<std::endl;
	return f;
}

surfer_options read_settings_from_file(const std::string& filename)
{
	
	
	surfer_options so = default_settings();
	
	std::ifstream f;
	try{
		f.open(filename.c_str());
	}
	catch(...){return so;}


	if(f.bad()||f.eof()||!f.good()) return so;




	std::string t;
	while(!getline(f,t).eof())
	{
		std::string t1;
		std::string t2;
		int i = 0;
		//remove leading whitespace
		while(!t.empty()&&t[0]<=' ')
		t = t.substr(1);

		//breaking into pieces
		for(;i<t.size() && (t[i]>' '&&t[i]!='='&&t[i]!=';') ;i++)
		t1+=t[i];

		while(i<t.size() && (t[i]<=' '|| t[i]=='='||t[i]==';'))
		i++;

		for(;i<t.size() && (t[i]!=';') ;i++)
		t2+=t[i];

		if(t1 == "gallery") so.gallery_path = fix_file(t2);
		if(t1 == "print") so.print_cmd = t2;
		if(t1 == "save") so.save_cmd = t2;
		if(t1 == "entryfont") so.entryfont = t2;
		if(t1 == "help") so.help_cmd = t2;
		if(t1 == "helpfile") so.helpfile = t2;
		if(t1 == "format") so.format = t2;
		if(t1 == "resolution")
		{
			std::istringstream iss(t2);
			iss>>so.resolution;
		}
		if(t1 == "upscale"||t1=="upscaling")
		{
			std::istringstream iss(t2);
			iss>>so.upscale;
		}
		if(t1=="surf")
		so.surf_cmd = t2;
	}





	return so;
}
