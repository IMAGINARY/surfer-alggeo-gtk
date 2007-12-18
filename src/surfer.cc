/***************************************************************************
 *   Copyright (C) 2007 by Henning Meyer, University of Kaiserslautern   *
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



#include <gtkmm/main.h>
#include "bwindow_gdk.hh"

std::string temp_dir = "/tmp/";

extern bool no_full;

int main (int argc, char *argv[])
{



#ifdef WIN32
std::string GetTempPath();
temp_dir = GetTempPath();
#endif

  Gtk::Main kit(argc, argv);

 // HelloWorld helloworld;
  bool personalized = false;
  std::istream* i = NULL;

  if(argc>1)
  {
	if(std::string(argv[1])=="-")
		i = &std::cin;
	else if(std::string(argv[1])=="-f")
	{
		//std::vector<parsepic_out> R;
		//add_data(R);
		//std::cout<<"generating thumbnails"<<std::endl;
		//make_thumbs(R);
		//std::cout<<"generating thumbnails finished"<<std::endl;
		//return 0;
		no_full = !no_full;
		
	}
	else
	{
		personalized = true;
		i = new std::ifstream(argv[1]);
	}
  }else i = new std::istringstream("");

  std::string optfile = fix_file("~" DIR_SEP HIDDEN_MARKER "surfer");

  surfer_options so = read_settings_from_file(optfile.c_str());
  try{
	std::ofstream f(optfile.c_str());
	if(f.good()&&!f.bad())
	write(so,f);
  }
  catch(...){}

  std::vector<gallery> G =  read_galleries_new(so.gallery_path,so.upscale);

  SurfBWindow sbw(*i, G,so,(argc>1 && std::string(argv[1])=="-f")  ,personalized);

  

  sbw.start();
  //Shows the window and returns when it is closed.
  //for(;;)
	Gtk::Main::run(sbw);

  return 0;
}










