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
std::string lstrip(const std::string& s)
{
	if(!s.empty() && s[0]==' ') return lstrip(s.substr(1));
	else return s;
}

std::string rstrip(const std::string& s)
{
	if(!s.empty() && s[s.size()-1]==' ') return rstrip(s.substr(0,s.size()-1));
	else return s;
}

std::string strip(const std::string& s)
{
	return lstrip(rstrip(s));
}



parsepic_out parse_pic(std::istream& f, bool strict)
{
	//std::cout<<"PARSE PIC"<<std::endl;
	parsepic_out R;
	std::ostringstream gen;
	bool pm = false;	
	std::string peq= "x^3+y^3+z^3+1-0.25*(x+y+z+1)^3";
	std::string eq = trans_eq(peq);
	std::string anti = "antialiasing=6;";

	if(strict)
	{
		peq ="";
		eq="";
	}
	int cr = 0;
	int cg = 156;
	int cb = 18;

	int cir = 150;
	int cig = 236;
	int cib = 68;

	int cbr = 255;
	int cbg = 255;
	int cbb = 255;

	std::string f1;

	double x = 0;
	double y = 0;
	double z = 0;
	
	double da = 0.0;
	double db = 1.0;

	double sc = 1;

	std::string t;

	int line_count=0;

	while(!getline(f,t).eof()  && f.good() && !f.bad() && !f.fail() )
	{
		//if(!(f.good()&&!f.bad()&&!f.fail())) return R;
		//std::cout<<++line_count<<": "<<t<<std::endl;
		std::string T=t;
	while(T.find(';')!=-1)
	{
		t = T.substr(0,T.find(';')+1);
		T = T.substr(T.find(';')+1);

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


		//std::cout << "t1 "<<t1<<"t2 "<<std::endl;
		
		if(t1=="rot_x")
		{
			std::istringstream w(t2);
			w>>x;

			R.rot = R.rot * rotx(x);
		}
		if(t1=="scale_x")
		{
			std::istringstream w(t2);
			w>>sc;
			if(sc==0) sc = 1;
		}
		else if(t1=="rot_y")
		{
			std::istringstream w(t2);
			R.rot = R.rot * roty(y);
		}
		else if(t1=="rot_z")
		{
			std::istringstream w(t2);
			w>>z;
			R.rot = R.rot * rotz(z);
		}
		else if(t1=="surface_red")
		{
			std::istringstream w(t2);
			w>>cr;
		}
		else if(t1=="surface_green")
		{
			std::istringstream w(t2);
			w>>cg;
		}
		else if(t1=="surface_blue")
		{
			std::istringstream w(t2);
			w>>cb;
		}
		else if(t1=="inside_green")
		{
			std::istringstream w(t2);
			w>>cig;
		}
		else if(t1=="inside_blue")
		{
			std::istringstream w(t2);
			w>>cib;
		}
		else if(t1=="inside_red")
		{
			std::istringstream w(t2);
			w>>cir;
		}
		else if(t1=="background_green")
		{
			std::istringstream w(t2);
			if(pm)w>>cbg;
		}
		else if(t1=="background_blue")
		{
			std::istringstream w(t2);
			if(pm)w>>cbb;
		}
		else if(t1=="background_red")
		{
			std::istringstream w(t2);
			if(pm)w>>cbr;
		}
		else if(t1=="antialiasing")
		{
			//anti=t;
		}
		else if(t1 == "double")
		{
			//std::cout << "DOUBLE "<<t2<<std::endl;

			int j = t2.find('=');
			std::string t3=t2.substr(0,j);
			std::string t4=t2.substr(j+1);

			if(strip(t3)=="a")
			{std::istringstream w(t4);
			w>>da;}
			else if(strip(t3)=="b")
			{std::istringstream w(t4);
			w>>db;}
			else
			gen<<t<<std::endl;
		}
		else if(t1 =="poly")
		{
			std::istringstream si(t2);
			std::string name;
			std::string eq;
			std::string val;
			si>>name;
			si>>eq;
			si>>val;
			while(si.good() && !si.eof() && !si.fail() && !si.bad())
			{std::string tmp;si>>tmp;val+=tmp;}
			if(name=="f1" && eq=="=")
			f1 = val;
			gen<<t<<std::endl;
			

		}
		else if(t1=="surface")
		{
			
			if(t2.find("rotate")==-1)
			{
				if(t2=="f1")
				{
					eq = trans_eq(f1);
					peq = f1;
				}
				else
				{
				eq = trans_eq(t2);
				peq = t2;
				}
			}
			else
			{
				int a = t2.find(',');
				std::string tr1 = t2.substr(a+1);
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
					R.rot = R.rot * rotx(-f);
				}
				else if(tr2=="yAxis")
				{
					R.rot = R.rot * roty(f);
				}
				else if(tr2=="zAxis")
				{
					R.rot = R.rot * rotz(-f);
				}
			}
		}
		else if(t1=="poly")
		{
			gen<<"poly "<<trans_eq(t2)<<";\n";
		}
		else if(t1=="draw_surface"||
			t1=="filename"||
			t1=="save_color_image"||
			t1=="color_file_format"||
			t1=="clear_screen"||
			t1=="dither_surface"||
			t1=="dither_file_format"||
			t1=="first"||
			t1=="second"||
			t1=="third"||
			t1=="inside_red"||
			t1=="inside_green"||
			t1=="inside_blue"||
			t1=="save_dithered_image"||
			t1=="illumination"||
			t1=="light1_x"||
			t1=="light1_y"||
			t1=="light1_z"||
			t1=="light1_red"||
			t1=="light1_blue"||
			t1=="light1_green"||
			t1=="light2_red"||
			t1=="light2_blue"||
			t1=="light2_green"||
			t1=="light3_red"||
			t1=="light3_blue"||
			t1=="light3_green"||
			t1=="light4_red"||
			t1=="light4_blue"||
			t1=="light4_green"||
			t1=="light5_red"||
			t1=="light5_blue"||
			t1=="light5_green"||
			t1=="light6_red"||
			t1=="light6_blue"||
			t1=="light6_green"||
			t1=="light7_red"||
			t1=="light7_blue"||
			t1=="light7_green"||
			t1=="light8_red"||
			t1=="light8_blue"||
			t1=="light8_green"||
			t1=="light1_vol"||
			t1=="light2_x"||
			t1=="light2_y"||
			t1=="light2_z"||
			t1=="light2_vol"||
			t1=="light3_x"||
			t1=="light3_y"||
			t1=="light3_z"||
			t1=="light3_vol"||
			t1=="light4_x"||
			t1=="light4_y"||
			t1=="light4_z"||
			t1=="light4_vol"||
			t1=="light5_x"||
			t1=="light5_y"||
			t1=="light5_z"||
			t1=="light5_vol"||
			t1=="light6_x"||
			t1=="light6_y"||
			t1=="light6_z"||
			t1=="light6_vol"||
			t1=="light7_x"||
			t1=="light7_y"||
			t1=="light7_z"||
			t1=="light7_vol"||
			t1=="light8_x"||
			t1=="light8_y"||
			t1=="light8_z"||
			t1=="light8_vol"||
			t1=="light9_x"||
			t1=="light9_y"||
			t1=="light9_z"||
			t1=="light9_vol"||
			t1=="light9_red"||
			t1=="light9_blue"||
			t1=="light9_green"||
			t1=="epsilon"||
			t1=="iterations"||
			t1=="root_finder"||
			t1=="spec_z"||
			t1=="origin_z"||
			t1=="spec_y"||
			t1=="origin_y"||
			t1=="spec_x"||
			t1=="origin_x"
			
			)
		{
		}
		else if(t1=="width"||t1=="height")
		{
			//std::istringstream w(t2);
			//w>>width;
		}
		else
		{
			gen<<t<<std::endl;
		}
		
	
	}
	}

	R.upside.red = cr;
	R.upside.green = cg;
	R.upside.blue = cb;	
	
	R.inside.red = cir;
	R.inside.green = cig;
	R.inside.blue = cib;	

	R.background.red = cbr;
	R.background.green = cbg;
	R.background.blue = cbb;	
		

	R.antialiasing = anti;
	R.equation = eq;
	R.public_eq = peq;
	R.general_stuff = gen.str();
	//R.initial_x = x;
	//R.initial_y = y;
	//R.initial_z = z;
	R.initial_scale = sc;

	R.scale = 1;
	
	R.para_a = da;
	R.para_b = db;
	R.lores = PREVIEW_SIZE;
	R.hires = 1024;

	return R;
}
