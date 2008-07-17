/***************************************************************************
 *   Copyright (C) 2007 by Henning Meyer                                   *
 *   surfer@imaginary2008.de                                               *
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

#ifndef USER_GALLERY_PATH

#ifdef WIN32
#define USER_GALLERY_PATH GALLERY_PATH_USE
#else
#define USER_GALLERY_PATH "~/surfer_gallery"
#endif

#endif

#include "bwindow_gdk.hh"
#include <cstdlib>


bool rewrite_config = false;

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

	


	

	while(r.find("~")!=-1)
	{
		
		int i = r.find("~");

		r = r.replace(i,1,home_dir());
	}

	while(r.find(WRONG_DIR_SEP_CHAR)!=-1)
	{
		
		int i = r.find(WRONG_DIR_SEP_CHAR);

		r[i] = DIR_SEP_CHAR;
	}

	return r;
}

std::string fix_path(const std::string& s)
{
	if(!s.empty() && s[0]=='\"') return fix_path(s.substr(1));
	if(!s.empty() && s[s.size()-1]=='\"') return fix_path(s.substr(0,s.size()-1));
	if(!s.empty() && s[s.size()-1]!=DIR_SEP_CHAR)
	return s+DIR_SEP_CHAR;
	else return s;
}


surfer_options default_settings()
{
	surfer_options so;
	so.gallery_path = GALLERY_PATH_USE;
	so.user_gallery_path = fix_path(fix_file(USER_GALLERY_PATH));


	so.save_cmd = "";
	so.print_cmd = "";
	
	so.format = "ppm";
	so.resolution = PREVIEW_SIZE;
	so.surf_cmd="surf";
	so.upscale = 1;
	so.print_resolution = 1740;
	so.ui_xml = "";

	so.video_frame_rate = 10;
	so.video_bitrate = 1800;
	so.video_resolution = 300;

	#ifndef WIN32
	so.mencoder_cmd = "mencoder";
	so.ffmpeg_cmd = "ffmpeg";
	#else
	so.mencoder_cmd = "";
	so.ffmpeg_cmd = "";
	#endif


	so.modified_surf = !no_new_surf_features;

	return so;
}

std::ostream& write(const surfer_options& so, std::ostream& f)
{
	

	f<<"gallery="<<so.gallery_path<<std::endl;
	f<<"user_gallery="<<so.gallery_path<<std::endl;
	f<<"save="<<so.save_cmd<<std::endl;
	f<<"print="<<so.print_cmd<<std::endl;
	f<<"format="<<so.format<<std::endl;
	f<<"resolution="<<so.resolution<<std::endl;
	f<<"print_resolution="<<so.print_resolution<<std::endl;
	f<<"surf="<<so.surf_cmd<<std::endl;
	f<<"upscale="<<so.upscale<<std::endl;
	f<<"entryfont="<<so.entryfont<<std::endl;
	f<<"help="<<so.help_cmd<<std::endl;
	f<<"helpfile="<<so.helpfile<<std::endl;
	f<<"uixml="<<so.ui_xml<<std::endl;
	f<<"mencoder="<<so.mencoder_cmd<<std::endl;
	f<<"ffmpeg="<<so.ffmpeg_cmd<<std::endl;
	f<<"modified_surf="<<so.modified_surf<<std::endl;
	return f;
}

surfer_options read_settings_from_file(const std::string& filename)
{
	

	surfer_options so = default_settings();
	
	std::ifstream f;
	try{
		f.open(filename.c_str());
	}
	catch(...){rewrite_config=true;return so;}


	if(f.bad()||f.eof()||!f.good()||!f.is_open()) {rewrite_config = true; return so;}




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
		if(t1 == "user_gallery") so.user_gallery_path = fix_file(t2);
		if(t1 == "print") so.print_cmd = t2;
		if(t1 == "save") so.save_cmd = t2;
		if(t1 == "entryfont") so.entryfont = t2;
		if(t1 == "help") so.help_cmd = t2;
		if(t1 == "helpfile") so.helpfile = t2;
		if(t1 == "format") so.format = t2;
		if(t1 == "uixml") so.ui_xml = t2;
		if(t1 == "mencoder") so.mencoder_cmd = t2;
		if(t1 == "ffmpeg") so.ffmpeg_cmd = t2;
		if(t1 == "resolution")
		{
			std::istringstream iss(t2);
			iss>>so.resolution;
		}
		if(t1 == "print_resolution")
		{
			std::istringstream iss(t2);
			iss>>so.print_resolution;
		}
		if(t1 == "upscale"||t1=="upscaling")
		{
			std::istringstream iss(t2);
			iss>>so.upscale;
		}
		if(t1 == "modified_surf")
		{
			std::istringstream iss(t2);
			iss>>so.modified_surf;
		}
		if(t1=="surf")
		so.surf_cmd = t2;
	}





	return so;
}
