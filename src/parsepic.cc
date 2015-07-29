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

#include "bwindow_gdk.hh"

std::string write_illu(const int n)
{
	std::string s;

	if(n & 1) s="ambient_light";

	if(n & 2) s=s+(s.empty()?"":"+")+"diffuse_light";

	if(n & 4) s=s+(s.empty()?"":"+")+"reflected_light";

	if(n & 8) s=s+(s.empty()?"":"+")+"transmitted_light";

	return s;
}

int read_illu(const std::string s)
{
	int n = 0;

	if(s.find("ambient_light")!=-1L) n+=1;
	if(s.find("diffuse_light")!=-1L) n+=2;
	if(s.find("reflected_light")!=-1L) n+=4;
	if(s.find("transmitted_light")!=-1L) n+=8;

	return n;
}

std::string trans_eq(const std::string& t2)
{
	std::string t3;
			for(unsigned x = 0; x < t2.size(); x++)
			switch(t2[x])
			{
				case 'x': t3+='u';break;
				case 'y': t3+='v';break;
				case 'z': t3+='w';break;
				default:  t3+=t2[x];break;
			}
	return t3;
}

std::string inv_trans_eq(const std::string& t2)
{
	std::string t3;
			for(unsigned x = 0; x < t2.size(); x++)
			switch(t2[x])
			{
				case 'u': t3+='x';break;
				case 'v': t3+='y';break;
				case 'w': t3+='z';break;
				default:  t3+=t2[x];break;
			}
	return t3;
}


global_parse global_defaults()
{
global_parse g;

g.spec_z = 100.0;
g.scale = 1.0;
g.initial_scale = 1.0;

g.normalize_brightness  = true;
g.normalize_factor = 1.0;

g.antialiasing = 1;
g.depth_cueing = true;

g.depth_value = -14.0;
g.illumination = ambient_light + diffuse_light + reflected_light;

color white;
white.red = white.green = white.blue = 255;

color bg;
bg.red = bg.green = bg.blue = 0;

g.background = bg;

g.light[0].x = -100.0;
g.light[0].y =  100.0;
g.light[0].z =  100.0;
g.light[0].volume = 50;
g.light[0].color = white;

for(unsigned i = 1; i < 9; i++)
{

g.light[i].x =  100.0;
g.light[i].y =  100.0;
g.light[i].z =  100.0;
g.light[i].volume = 0;
g.light[i].color = white;

}
g.light[1].x = 0.0;
g.light[2].volume = 70;
g.light[3].x = -100.0;
g.light[3].y = 0.0;
g.light[4].x = 0.0;
g.light[4].y = 0.0;
g.light[5].y = 0.0;
g.light[6].x = -100.0;
g.light[6].y = -100.0;
g.light[7].x = 0.0;
g.light[7].y = -100.0;
g.light[7].volume = 30;
g.light[8].y = -100.0;






g.rot = unitmat<double>(3,1.0,0.0);

g.general_stuff = std::string("double PI = 2*arcsin(1);\n")
				+"root_finder=d_chain_newton;\n"
				   +"epsilon=0.0000000000001;\n"
				   +"iterations=1000;\n";


g.hires = 700;
g.lores = 200;
g.saveres = g.hires;

g.para[0] = 0.0;
g.para[1] = 1.0;
g.para[2] = 0.0;
g.para[3] = 1.0;

return g;
}


surface_data local_defaults()
{

surface_data f;

f.outside.red = 123;
f.outside.green = 104;
f.outside.blue = 238;


f.inside.red = 230;
f.inside.green = 180;
f.inside.blue = 30;

f.ambient = 35;
f.diffuse = 60;
f.reflected = 60;
f.transmitted = 60;
f.smoothness = 13;
f.transparence = 80;
f.thickness = 10;

f.public_eq = "";
f.equation = "";

return f;

}


surface_data surfer_local()
{

surface_data f = local_defaults();

f.transparence = 0;


f.public_eq = "x";
f.equation = trans_eq(f.public_eq);

return f;

}


std::ostream& write_surf(const surface_data& f, const int nn, std::ostream& o)
{

std::ostringstream sn;
if(nn) sn<<(nn+1);
std::string n = sn.str();


o <<"surface"<<n<<  "_red = " << f.outside.red <<";\n";
o <<"surface"<<n<<"_green = " << f.outside.green <<";\n";
o <<"surface"<<n<< "_blue = " << f.outside.blue <<";\n";

o <<"inside"<<n<<   "_red = " << f.inside.red  <<";\n";
o <<"inside"<<n<< "_green = " << f.inside.green  <<";\n";
o <<"inside"<<n<<  "_blue = " << f.inside.blue  <<";\n";

o <<"ambient"<<n <<     " = " << f.ambient  <<";\n";
o <<"diffuse"<<n <<     " = " << f.diffuse  <<";\n";
o <<"reflected"<<n <<   " = " << f.reflected  <<";\n";
o <<"transmitted"<<n << " = " << f.transmitted  <<";\n";
o <<"smoothness"<<n <<  " = " << f.smoothness  <<";\n";
o <<"transparence"<<n <<" = " << f.transparence  <<";\n";
o <<"thickness"<<n <<" = " << f.thickness  <<";\n";

o<<"surface"<<n<<" = "<<fix_input_for_surf(f.equation)  <<";\n";



return o;

}

std::string yesno(bool b)
{if(b) return "yes"; else return "no";}


std::ostream& write_surf(const global_parse& g, std::ostream& o,  int antialias )
{

if(antialias == 0)
antialias = g.antialiasing;



o <<"double a = " << g.para[0]  << ";\n";
o <<"double b = " << g.para[1] << ";\n";
o <<"double c = " << g.para[2] << ";\n";
o <<"double d = " << g.para[3] << ";\n";

o<<"poly u="<<g.rot.el(1,1)<<"*x+("<<g.rot.el(1,2)<<")*y+("<<g.rot.el(1,3)<<")*z;\n";
o<<"poly v="<<g.rot.el(2,1)<<"*x+("<<g.rot.el(2,2)<<")*y+("<<g.rot.el(2,3)<<")*z;\n";
o<<"poly w="<<g.rot.el(3,1)<<"*x+("<<g.rot.el(3,2)<<")*y+("<<g.rot.el(3,3)<<")*z;\n";



o<<"spec_z = " << g.spec_z << ";\n";
o<<"scale_x = " << g.scale * g.initial_scale <<";\n";
o<<"scale_y = " << g.scale * g.initial_scale <<";\n";
o<<"scale_z = " << g.scale * g.initial_scale <<";\n";

o<<"normalize_brightness = "<<yesno(g.normalize_brightness)<<";\n";
o<<"normalize_factor = "<<g.normalize_factor << ";\n";

o<<"antialiasing = " << antialias << ";\n";
o<<"depth_cueing = "<<yesno(g.depth_cueing) << ";\n";

o<<"depth = " << g.depth_value << ";\n";
o<<"illumination = "<<write_illu(g.illumination)  << ";\n";


o <<"background_red = " << g.background.red  <<";\n";
o <<"background_green = " << g.background.green  <<";\n";
o <<"background_blue = " << g.background.blue  <<";\n";


for(unsigned i = 0; i < 9; i++)
{
std::ostringstream sn;
sn<<(i+1);
std::string n = sn.str();


{
o<<"light"<<n<<"_x = " << g.light[i].x << ";\n";
o<<"light"<<n<<"_y = " << g.light[i].y  << ";\n";
o<<"light"<<n<<"_z = " << g.light[i].z  << ";\n";
o<<"light"<<n<<"_vol = " << g.light[i].volume  << ";\n";
o<<"light"<<n<<"_red = " << g.light[i].color.red  << ";\n";
o<<"light"<<n<<"_green = " << g.light[i].color.green  << ";\n";
o<<"light"<<n<<"_blue = " << g.light[i].color.blue  << ";\n";
}
}

o<<g.general_stuff;

return o;

}

void discard_comments(std::string& t)
{
	long i = t.find("//");
	if(i != -1L)
	t = t.substr(0,i);
}

void remove_ascii_whitespace(std::string& s)
{
	std::string t;
	for(unsigned i = 0; i < s.size(); i++)
	if(s[i]>' ') t += s[i];
	s=t;
}


template<typename T>
bool match(const std::string& s,const std::string& t,T& ref)
{
	long i = s.find("=");
	if(i == -1L) return false;

	std::string t1 = s.substr(0,i);
	std::string t2 = s.substr(i+1);


	//std::cout<<"matching \""<<t<<"\" against \""<<t1<<"\""<<std::endl;
	if(t1 == t)
	{
		std::istringstream in(t2);
		in >> ref;
		return true;
	}
	return false;
}


template<>
bool match<bool>(const std::string& s,const std::string& t,bool& ref)
{
	long i = s.find("=");
	if(i == -1L) return false;

	std::string t1 = s.substr(0,i);
	std::string t2 = s.substr(i+1);

	//std::cout<<"matching "<<t<<" against "<<t1<<std::endl;
	if(t1 == t)
	{
		
		std::istringstream in(t2);
		in >> ref;
		if(t2 == "yes") ref = true;
		return true;
	}
	return false;
}

void read_rotation(const std::string& s, matrix<double>& rot, unsigned r)
{
	//s = a*x+(b)*y+(c)*z
	{
	std::istringstream i(s);

	i >> rot.el(r,1);
}
	long j = s.find("(");

	std::string s2 = s.substr(j+1);

	{
	std::istringstream i(s2);
	i >> rot.el(r,2);
	}

	j = s2.find("(");

	std::string s3 = s2.substr(j+1);

	{
	std::istringstream i(s3);
	i >> rot.el(r,3);
	}

}


std::vector<surface_data> read_pic(std::istream& f, global_parse& g, const std::string default_eq )
{

	std::string t;

	std::string line;

	std::vector<surface_data> S;

	for(unsigned i = 0; i < 9; i++)
	S.push_back(local_defaults());

	S[0].public_eq = default_eq;

	std::string f1;

	while(!getline(f,t).eof()  && f.good() && !f.bad() && !f.fail() )
	{
		//std::cout <<"Reading "<< t<<std::endl;
		discard_comments(t);

		line = line+t;

		long i;

		i = line.find(";");
		
		std::string sr;

		while(i != -1L)
		{
			std::string s = line.substr(0,i);
			line = line.substr(i+1);
			i = line.find(";");


			while(s.find("double ")!= -1L)
			s = s.replace(s.find("double "),7,"double=");

			while(s.find("poly ")!= -1L)
			s = s.replace(s.find("poly "),5,"poly=");

			remove_ascii_whitespace(s);
			
			//std::cout<<"   line "<<s<<"  // "<<line<<std::endl;

			if(match(s,"spec_z",g.spec_z)) goto next_command;
			if(match(s,"scale_x",g.initial_scale)) goto next_command;
			if(match(s,"scale_y",g.initial_scale)) goto next_command;
			if(match(s,"scale_z",g.initial_scale)) goto next_command;

			if(match(s,"normalize_brightness",g.normalize_brightness)) goto next_command;
			if(match(s,"normalize_factor",g.normalize_factor)) goto next_command;
			if(match(s,"antialiasing",g.antialiasing)) goto next_command;
			if(match(s,"depth_cueing",g.depth_cueing)) goto next_command;
			if(match(s,"depth",g.depth_value)) goto next_command;

			if(match(s,"background_red",g.background.red)) goto next_command;
			if(match(s,"background_green",g.background.green)) goto next_command;
			if(match(s,"background_blue",g.background.blue)) goto next_command;
				

			for(unsigned i = 0; i < 9; i++)
			{
				std::ostringstream sn;
				sn<<(i+1);
				std::string n = sn.str();
			
				if(match(s,"light"+n+"_x",g.light[i].x)) goto next_command;
				if(match(s,"light"+n+"_y",g.light[i].y)) goto next_command;
				if(match(s,"light"+n+"_z",g.light[i].z)) goto next_command;
				if(match(s,"light"+n+"_red",g.light[i].color.red)) goto next_command;
				if(match(s,"light"+n+"_green",g.light[i].color.green)) goto next_command;
				if(match(s,"light"+n+"_blue",g.light[i].color.blue)) goto next_command;
				if(match(s,"light"+n+"_vol",g.light[i].volume)) goto next_command;
			}

			

			for(unsigned i = 0; i < 9; i++)
			{
				std::ostringstream sn;
				if(i)sn<<(i+1);
				std::string n = sn.str();
				
				
				
				if(match(s,"surface"+n+"_red",S[i].outside.red)) goto next_command;
				if(match(s,"surface"+n+"_green",S[i].outside.green)) goto next_command;
				if(match(s,"surface"+n+"_blue",S[i].outside.blue)) goto next_command;

				if(match(s,"inside"+n+"_red",S[i].inside.red)) goto next_command;
				if(match(s,"inside"+n+"_green",S[i].inside.green)) goto next_command;
				if(match(s,"inside"+n+"_blue",S[i].inside.blue)) goto next_command;

				if(match(s,"ambient"+n,S[i].ambient)) goto next_command;
				if(match(s,"diffuse"+n,S[i].diffuse)) goto next_command;
				if(match(s,"reflected"+n,S[i].reflected)) goto next_command;
				if(match(s,"transmitted"+n,S[i].transmitted)) goto next_command;
				if(match(s,"smoothness"+n,S[i].smoothness)) goto next_command;
				if(match(s,"transparence"+n,S[i].transparence)) goto next_command;
				if(match(s,"thickness"+n,S[i].thickness)) goto next_command;

				
				if(match(s,"surface"+n,sr))
				{			
					{
						
						if(sr.find("rotate")==-1)
						{
							if(sr=="f1")
							{
								//surfex-fix
								S[i].public_eq = f1;
							}
							else
							{
							
							S[i].public_eq = sr;
							}
						}
						else
						{
							int a = sr.find(',');
							std::string tr1 = sr.substr(a+1);
							int b = tr1.find(',');
							std::string tr2 = tr1.substr(b+1);
							tr1 = tr1.substr(0,b);
							int c = tr2.find(')');
							tr2 = tr2.substr(0,c);
							
							std::istringstream fli(tr1);
							double f=0;
							fli>>f;
							
							if(tr2=="xAxis")
							{
								g.rot = g.rot * rotx(-f);
							}
							else if(tr2=="yAxis")
							{
								g.rot = g.rot * roty(f);
							}
							else if(tr2=="zAxis")
							{
								g.rot = g.rot * rotz(-f);
							}
						}
					}
			


					goto next_command; //this will be postprocessed
				}


				
				
			}

			if(match(s,"illumination",sr))
			{
				
				g.illumination = read_illu(sr);

				goto next_command;
			}
			
			if(match(s,"double",sr))
			{
				if(match(sr,"a",g.para[0])) goto next_command;
				if(match(sr,"b",g.para[1])) goto next_command;
				if(match(sr,"c",g.para[2])) goto next_command;
				if(match(sr,"d",g.para[3])) goto next_command;

				std::string st;
				if(match(sr,"PI",st)) goto next_command;
				
				g.general_stuff+="double "+sr+";\n";

				goto next_command;
			}

			if(match(s,"poly",sr))
			{
				if(match(sr,"f1",f1)) goto next_command;

				std::string st;
				//FIXME: read rotation matrix
				if(match(sr,"u",st))
				{
					read_rotation(st,g.rot,1);
					goto next_command;
				}
				if(match(sr,"v",st))
				{
					read_rotation(st,g.rot,2);
					goto next_command;
				}
				if(match(sr,"w",st))
				{
					read_rotation(st,g.rot,3);
					goto next_command;
				}
				
				g.general_stuff+="poly "+sr+";\n";

				goto next_command;
			}


			{
			double d = 0.0;

			if(match(s,"rot_x",d))
			{g.rot = g.rot * rotx(d);
			goto next_command;}
			if(match(s,"rot_y",d))
			{g.rot = g.rot * roty(d);
			goto next_command;}
			if(match(s,"rot_z",d))
			{g.rot = g.rot * rotz(d);
			goto next_command;}
			}

			/* ignoring commands*/
			if(match(s,"filename",sr)) goto next_command;
			if(match(s,"width",sr)) goto next_command;
			if(match(s,"height",sr)) goto next_command;
			if(match(s,"radius",sr)) goto next_command;
			if(match(s,"color_file_format",sr)) goto next_command;
			
			

			if(match(s,"epsilon",sr)) goto next_command;
			if(match(s,"root_finder",sr)) goto next_command;
			//if(match(s,"clip",sr)) goto next_command;
			
			for(unsigned i = 0; i < 9; i++)
			{
			std::ostringstream sn;
				if(i)sn<<(i+1);
				std::string n = sn.str();
				
			//if(match(s,"clip_surface"+n,sr)) goto next_command;
			}

			if(match(s,"illumination",sr)) goto next_command;

			if(s=="draw_surface") goto next_command;
			if(s=="save_color_image") goto next_command;
			

			g.general_stuff+=s+";\n";

			continue;
			next_command:;

		}

	}


	while(!S.empty() && S.back().public_eq.empty())
	S.pop_back();

	for(unsigned i = 0; i < S.size(); i++)
	{
		S[i].equation = trans_eq(S[i].public_eq);
		S[i].public_eq = inv_trans_eq(fix_input_for_display(S[i].public_eq));
	}

	return S;

}

