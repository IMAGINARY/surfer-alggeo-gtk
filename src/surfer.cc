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

#ifdef WIN32
int main (int argc, char *argv[]);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	return main( __argc, __argv );
}
#endif


class TWindow: public Gtk::Window
{
public:
SurfBWindow& sbw;

TWindow(SurfBWindow& s):sbw(s)
{

}

virtual bool on_expose_event(GdkEventExpose*)
{

//virtual bool on_button_press_event(GdkEventButton* )
//{
	if(!get_window()) return false;
	sbw.show();
	sbw.set_modal();
	sbw.fullscreen();
	Gtk::Main::run(sbw);
	hide();
	return true;
}
};

int main (int argc, char *argv[])
{



#ifdef WIN32
std::string GetTempPath();
temp_dir = GetTempPath();
#else
{
	std::string s = std::getenv("USER");
	temp_dir += s;
}
#endif

  Gtk::Main kit(argc, argv);

 // HelloWorld helloworld;
  bool personalized = false;
  std::istream* i = new std::istringstream("");

  init_threads();

arg_inspect:
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
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else
	{
		personalized = true;
		i = new std::ifstream(argv[1]);
	}
  }

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
//sbw.show();
//	sbw.set_modal();
//	sbw.fullscreen();
	
  TWindow t(sbw);
  //Shows the window and returns when it is closed.
  //for(;;)
if(!no_full)	Gtk::Main::run(t);
else Gtk::Main::run(sbw);

  return 0;
}











