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

//const double UPSCALING = 1.0;

#include "bwindow_gdk.hh"
std::string basename(const std::string& f);

extern bool ignore_bg_in_gallery;

void gallery_override(parse_result& P)
{
	global_parse G = global_defaults();
	if(!ignore_bg_in_gallery) return;

	color bg;
	bg.red = bg.green = bg.blue = MAIN_COLOR_LUMINA;

	P.global_data.background = bg;
	P.global_data.antialiasing = 4;
}

gallery read_gallery_file(std::istream& f, const std::string& xpath, const std::string& dname, double upscaling)
{
	//path contains DIR_SEP
	std::string path = fix_path(xpath);
	gallery G;
	getline(f,G.name);
	G.path = path;
	

	std::string L;
	std::string t;
	while(!getline(f,t).eof())
	{
		
		int i = t.find(':');
		if(i==-1) break;
		std::string t1 = t.substr(0,i);
		std::string t2 = t.substr(i+1);
		i = t2.find(':');
		std::string t3 = t2.substr(0,i);
		std::string t4 = t2.substr(i+1);
		
		if(G.file.empty() || basename(G.file.back().filepath) != basename(t1))
		{
			std::ifstream pf((path+t1).c_str());
			parse_result P;
			
			P.global_data = global_defaults();
			P.data = read_pic(pf,P.global_data);
			if(!P.data.empty())
			{

				gallery_override(P);

				P.global_data.name = basename(t1);
				
				for(unsigned i = 0; i < P.data.size(); i++)
				P.data[i].desc = path+basename(t1)+".png";
				P.filepath = path+basename(t1);

				G.file.push_back(P);

			}
			else
			continue;
		}

		if(t3=="name")
		G.file.back().global_data.name = t4;
		else if(t3=="lowres"||t3=="lores")
		{
			int x = G.file.back().global_data.lores;
			std::istringstream w(t4);
			w>>x;
			x=int(upscaling*x);
			G.file.back().global_data.lores = x;
		}
		else if(t3=="highres"||t3=="hires")
		{
			int x = G.file.back().global_data.hires;
			std::istringstream w(t4);
			w>>x;
			x=int(upscaling*x);
			G.file.back().global_data.hires = x;
		}
		
	}

	G.image = path+dname+".ppm";
	G.desc = path+dname+".png";
	

	if(!G.file.empty())
	try{
		std::ifstream fi(G.image.c_str());
		if(!fi.is_open())G.image = thumbnail(G.file[0]);
		}catch(...){
	G.image = thumbnail(G.file[0]);
	}

	return G;
}

void write_gallery_file(std::ostream& f, const gallery& G)
{
	//path contains DIR_SEP

	//gallery G;
	f<<G.name<<std::endl;
	//G.path = path;

	std::string L;
	std::string t;

	for(unsigned i = 0; i < G.file.size(); i++)
	{
		f<<basename(G.file[i].filepath)<<":name:"<<G.file[i].global_data.name<<std::endl;
		f<<basename(G.file[i].filepath)<<":lowres:"<<G.file[i].global_data.lores<<std::endl;
		f<<basename(G.file[i].filepath)<<":highres:"<<G.file[i].global_data.hires<<std::endl;

		
	}

}





std::vector<gallery> read_galleries_file(std::istream& f, const std::string& xpath, double upscaling)
{
	//path contains DIR_SEP
	std::string path = fix_path(xpath);

	std::vector<gallery> V;

	std::string name;
	
	std::string L;
	std::string t;
	while(!getline(f,t).eof())
	{
		std::string d (t);
		if(!d.empty() && d[0]!='.')
		{
			gallery G = read_gallery(path+d,d,upscaling);

			if(!G.path.empty()) V.push_back(G);
		}
	}
	return V;
}



std::vector<gallery> read_galleries_new(const std::string& xpath, double upscaling)
{
	std::string path = fix_path(xpath);
	try{
	std::ifstream f((path+"galleries.txt").c_str());



	if(f.is_open())
	return read_galleries_file(f,xpath,upscaling);
	}
	catch(...){}
	return read_galleries_old(path,upscaling);

}

