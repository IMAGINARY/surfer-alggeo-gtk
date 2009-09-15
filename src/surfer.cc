/***************************************************************************
 *   Copyright (C) 2007 by Henning Meyer, University of Kaiserslautern     *
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



#include <gtkmm/main.h>
#include "bwindow_gdk.hh"

std::string temp_dir = "/tmp/";

extern char * surfer_small_xpm[] ;
extern char * surfer_xpm[] ;

extern bool no_full;
extern bool no_info;
extern bool no_gallery;
extern bool no_log;
extern bool rewrite_config;
extern bool no_modify;
extern bool no_new_features;

bool do_restart = false;


#ifdef WIN32
int main (int argc, char *argv[]);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	return main( __argc, __argv );
}
#endif

void check_surf(const surfer_options& opt);

class TWindow: public Gtk::Window
{
public:
SurfBWindow& sbw;

TWindow(SurfBWindow& s):sbw(s)
{

}

virtual bool on_expose_event(GdkEventExpose*)
{

	if(!get_window()) return false;
	sbw.show();
	sbw.set_modal();
	sbw.fullscreen();
	Gtk::Main::run(sbw);
	hide();
	return true;
}

};


SurfBWindow* main_work(std::istream* i, const surfer_options& so, bool b, bool personalized, SurfBWindow* k);

int main_details(std::istream* i, const surfer_options& so, bool b, bool personalized)
{

check_surf(so);


#ifndef WIN32
Gtk::Window::set_default_icon( Gdk::Pixbuf::create_from_xpm_data(surfer_xpm));
#endif


    main_work(i, so,  b, personalized, NULL);
Gtk::Main::run();

while(do_restart)
{
  do_restart = false;
  main_work(so,  b, personalized, NULL);

} 

parallel_clear();

  return 0;
}


int main (int argc, char *argv[])
{
//char LANG_DE[] = "de_DE.UTF-8";

//char LANG_EN[] = "en_US.UTF-8";

//char *LANG = LANG_EN;





#ifdef WIN32
std::string GetTempPath();
temp_dir = GetTempPath();
#else
{

        char ftemplate[] = "surfer-XXXXXX";
	std::string s = "/tmp"; 
	if( std::getenv("TMPDIR"))
		s = std::getenv("TMPDIR");
	if(s.empty()) s = "/tmp";


	temp_dir = s + "/" + ftemplate;
        

	char *L = new char[temp_dir.size() + 1];
	strcpy(L,temp_dir.c_str());
	
	char * tst = mkdtemp(L);
        if(!tst)
        {std::cerr<<"mkdtemp failed on "<<L<<std::endl;abort();}
	
	temp_dir = L;
	delete L;

	if(temp_dir.empty() or temp_dir[temp_dir.size()-1] != DIR_SEP_CHAR)
		temp_dir += DIR_SEP;
        
}
#endif


  if(argc>1 and std::string(argv[1])=="--version")
	{
		std::cout<<"Surfer "<<get_revision()<<std::endl;
		return 0;
		
	}

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
                "       -g hides the gallery\n"
		"       -i hides the information for gallery entries\n"
                "       -s small mode: no fullscreen, no gallery, no information\n"
                "       -V verbose operation\n"
                "       -t removes animation und multiple surfaces from user interface \n"
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
	else if(std::string(argv[1])=="-g")
	{
		no_gallery = !no_gallery;
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else if(std::string(argv[1])=="-t")
	{
		no_new_features = !no_new_features;
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
	else if(std::string(argv[1])=="-d")
	{
		no_modify = true;
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
	else if(std::string(argv[1])=="-r")
	{
		rewrite_config = !rewrite_config;
		argc--;
		argv++;
		goto arg_inspect;
		
	}
	else if(std::string(argv[1])=="-m")
	{
		no_new_surf_features = !no_new_surf_features;
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

  std::string optfile = fix_path(fix_file("~"))+(HIDDEN_MARKER "surfer");

  surfer_options so = default_settings();
if(!rewrite_config) so = read_settings_from_file(optfile.c_str());

  no_new_surf_features = !so.modified_surf;

  if(rewrite_config)
  try{
	std::ofstream f(optfile.c_str());
	if(f.good()&&!f.bad())
	write(so,f);
  }
  catch(...){}

   main_details(i,so,(argc>1 && std::string(argv[1])=="-f") ,personalized);

   #ifndef WIN32

   std::remove(temp_dir.c_str());

   #endif

   return 0;
}

void check_surf(const surfer_options& opt)
{
        std::string image = TEMP_ROOT_SEP+"surfb_it.ppm";
	std::string script = TEMP_ROOT_SEP+"surfb_it.pic";

        std::remove(image.c_str());
        std::remove(script.c_str());

	std::ofstream f(script.c_str(),FILE_WRITE_MODE);
        if(not f.is_open())
        {
           std::cerr<<"cannot write file "<<script<<std::endl;
           exit(101);
        }
	
/*	f<<"rot_x="<<data.initial_x+current_x<<";\n";
	f<<"rot_y="<<data.initial_y+current_y<<";\n";
	f<<"rot_z="<<data.initial_z+current_z<<";\n";
*/
	//rot_data t = rot_yxz(data.rot);
	//f<<"rot_x="<<t.rot_x<<";\n";
	//f<<"rot_y="<<t.rot_y<<";\n";
	//f<<"rot_z="<<t.rot_z<<";\n";

	
	
	
		int w = 1;
		f<<"width="<< w <<";\n";
		f<<"height="<< w <<";\n";
	

	

	f<<"surface="<< "x*x + y*y - z" <<";\n";

	

	//f<<"transparency="<<50<<";\n";
	f<<"filename=\""<<image<<"\";\n";
	f<<"color_file_format=ppm;\n";
	f<<"draw_surface;\n";
	f<<"save_color_image;\n";
	f.close();

	std::string cmd = opt.surf_cmd+" "+QUIET_SURF+" -n \""+script+"\" " +REDIRECTION_APEX;
	
	int r =log_system(cmd.c_str());
        if(r != 0)
        {
            std::cerr<<"calling surf failed"<<std::endl;

            exit(105);
        }

        std::ifstream f2(image.c_str());
	if(not f2.is_open())
        {
            std::cerr<<"cannot open file "<<image<<std::endl;
            exit(107);
	}

        std::remove(image.c_str());
        std::remove(script.c_str());

        
}

bool check_encoder()
{

}

int log_system(const std::string& s)
{
	if(!no_log) std::cout<<"executing "<<s<<std::endl;
	int r = system(s.c_str());
	if(r != 0)
	{
		std::cerr<<"surfer: system call failed"<<std::endl;
		std::cerr<<"call was \""<<s<<"\""<<std::endl;
	}
	return r;
}






SurfBWindow* main_work(const surfer_options& so, bool b, bool personalized, SurfBWindow* k)
{


bindtextdomain(PACKAGE, LOCALEDIR);
bind_textdomain_codeset(PACKAGE, "UTF-8");
textdomain(PACKAGE);
setlocale(LC_ALL,"") ;

  std::vector<gallery> G =  read_galleries_new(fix_path(so.gallery_path+"/"+_("gallery-en")),so.upscale);
  std::vector<gallery> Gu =  read_galleries_old(fix_path(so.user_gallery_path),so.upscale);

  G.insert(G.end(),Gu.begin(),Gu.end());


  std::istringstream i("");
  SurfBWindow* sbw = new SurfBWindow(i, G,so,b ,personalized);
  sbw->start();
  sbw->kill_w = k;

  sbw->show();
  
  return sbw;
}

SurfBWindow* main_work(std::istream* i,const surfer_options& so, bool b, bool personalized, SurfBWindow* k)
{


bindtextdomain(PACKAGE, LOCALEDIR);
bind_textdomain_codeset(PACKAGE, "UTF-8");
textdomain(PACKAGE);
setlocale(LC_ALL,"") ;

  std::vector<gallery> G =  read_galleries_new(fix_path(so.gallery_path+"/"+_("gallery-en")),so.upscale);
  std::vector<gallery> Gu =  read_galleries_old(fix_path(so.user_gallery_path),so.upscale);

  G.insert(G.end(),Gu.begin(),Gu.end());


  SurfBWindow* sbw = new SurfBWindow(*i, G,so,b ,personalized);
  sbw->start();
  sbw->kill_w = k;

  sbw->show();
  //Gtk::Main::run(*sbw);
  
  return sbw;
}

