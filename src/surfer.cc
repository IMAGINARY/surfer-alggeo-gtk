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

extern char * surfer_small_xpm[] ;

extern bool no_full;
extern bool no_info;
extern bool no_gallery;
extern bool rewrite_config;

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

bindtextdomain(PACKAGE, LOCALEDIR);
bind_textdomain_codeset(PACKAGE, "UTF-8");
textdomain(PACKAGE);


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
	else if(std::string(argv[1])=="-h" || std::string(argv[1])=="--help")
	{
		std::cout<<
		_("Surfer - visualizing algebraic geometry\n"
		"usage: surfer [-f] [-h] [-i] [-s] [-V] [file]\n"
		"       -f toggles between fullscreen and windowed mode\n"
                "       -h hides the gallery\n"
		"       -i hides the information for gallery entries\n"
                "       -s small mode: no fullscreen, no gallery, no information\n"
                "       -V verbose operation\n"
                "       file is a surf or surfer script\n");
		return 0;
		
	}
	else if(std::string(argv[1])=="-f")
	{
		no_full = !no_full;
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else if(std::string(argv[1])=="-h")
	{
		no_gallery = !no_gallery;
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else if(std::string(argv[1])=="-i")
	{
		no_info = !no_info;
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else if(std::string(argv[1])=="-s")
	{
		no_full = true;
		no_info=true;
		no_gallery=true;
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else if(std::string(argv[1])=="-V")
	{
		no_log = false;
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else if(std::string(argv[1])=="--version")
	{
		std::cout<<"Surfer "<<get_revision()<<std::endl;
		return 0;
		
	}
	else if(std::string(argv[1])=="-r")
	{
		rewrite_config = !rewrite_config;
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

  surfer_options so = default_settings();
if(!rewrite_config) so = read_settings_from_file(optfile.c_str());

  if(rewrite_config)
  try{
	std::ofstream f(optfile.c_str());
	if(f.good()&&!f.bad())
	write(so,f);
  }
  catch(...){}

  std::vector<gallery> G =  read_galleries_new(so.gallery_path,so.upscale);

Gtk::Window::set_default_icon( Gdk::Pixbuf::create_from_xpm_data(surfer_small_xpm));

  SurfBWindow sbw(*i, G,so,(argc>1 && std::string(argv[1])=="-f")  ,personalized);



  sbw.start();
	
  TWindow t(sbw);
if(!no_full)	Gtk::Main::run(t);
else Gtk::Main::run(sbw);

  return 0;
}



void log_system(const std::string& s)
{
	if(!no_log) std::cout<<"executing "<<s<<std::endl;
	system(s.c_str());
}







