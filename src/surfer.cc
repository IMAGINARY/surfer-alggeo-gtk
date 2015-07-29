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
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "bwindow_gdk.hh"

std::string temp_dir = "/tmp/";

extern char * surfer_small_xpm[] ;
extern char * surfer_xpm[] ;

/*extern*/ bool no_full = true;
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

#define setenv(a,b,c) _putenv_s(a,b)

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

	do_postpare();

  return 0;
}

void wipe_dir(const std::string& path)
{
	DIR* gd = opendir(path.c_str());
	if(gd==NULL) return ;
	dirent* R = readdir(gd);
	while(R)
	{
		std::string d (R->d_name);
		if(!d.empty() && d[0]!='.' ) {
			if (R->d_type==DT_REG)
				std::remove((path+DIR_SEP+R->d_name).c_str());
			else if (R->d_type==DT_DIR)
				wipe_dir((path+DIR_SEP+R->d_name).c_str());
		}
		R = readdir(gd);
	}
	closedir(gd);
	std::remove(path.c_str());
}

int main (int argc, char *argv[])
{
//char LANG_DE[] = "de_DE.UTF-8";

//char LANG_EN[] = "en_US.UTF-8";

//char *LANG = LANG_EN;

  Gtk::Main kit(argc, argv);

 // HelloWorld helloworld;
  bool personalized = false;
char *ui_xml = NULL;
  std::istream* i = new std::istringstream("");

arg_inspect:
  if(argc>1)
  {
	if(std::string(argv[1])=="-")
		i = &std::cin;
	else if(std::string(argv[1])=="--version")
	{
#if 1
		std::cout<< PACKAGE_VERSION <<std::endl;
#else
		std::cout<<"Surfer "<<get_revision()<<std::endl;
#endif
		return 0;
	}
	else if(std::string(argv[1])=="-h" || std::string(argv[1])=="--help")
	{
		std::cout<<
		_(/*"Surfer - visualizing algebraic geometry\n"*/
		"Usage: surfer-alggeo-gtk [-f] [-h] [-i] [-s] [-V] [-l LANG] [--uixml UIXMLFILE] [SCRIPT]\n"
		"Options:\n"
		"  -f             toggles between fullscreen and windowed mode\n"
		"  -g             hides the gallery\n"
		"  -i             hides the information for gallery entries\n"
		"  -s             small mode: no fullscreen, no gallery, no information\n"
		"  -V             verbose operation\n"
		"  -t             removes animation und multiple surfaces from user interface \n"
		"  -l             sets the language, e.g. to LANG=de_DE.utf8\n"
		"     --uixml     uses the given XML file as an UI definiton\n"
		"\n"
		"  -h, --help     display this help and exit\n"
		"      --version  display version tuple and exit\n"
		"\n"
		"                 SCRIPT is a surf or surfer script\n"
		"\n");
		return 0;
	}
	else if(std::string(argv[1])=="--uixml")
	{
		ui_xml = argv[2];
		argc -= 2;
		argv += 2;
		goto arg_inspect;
	}
	else if(std::string(argv[1])=="-l")
	{
#ifdef WIN32
		printf( "-l option will probably not work on windows ... sorry\n" );
#endif
		setenv("LANGUAGE",argv[2],1);
		setenv("LANG",argv[2],1);

		argc -= 2;
		argv += 2;
		goto arg_inspect;
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
#if 0
	else if(std::string(argv[1])=="-m")
	{
		argc--;
		argv++;
		goto arg_inspect;
	}
#endif
	else
	{
		personalized = true;
		i = new std::ifstream(argv[1]);
	}
  }

#ifdef WIN32
std::string GetTempPath();
temp_dir = GetTempPath();
#else
{

  char ftemplate[] = "surfer-XXXXXX";

  std::string xs = "/dev/shm";
	struct stat st;

	std::string s = "/tmp";

	if(stat(xs.c_str(),&st) == 0)
	s = xs;

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

  std::string optfile = fix_path(fix_file("~"))+(HIDDEN_MARKER "surfer" EXT_MARKER);

  surfer_options so = default_settings();
	if(!rewrite_config) read_settings_from_file(so, optfile.c_str());
	if( ui_xml != NULL )
		so.ui_xml = std::string( ui_xml );

  if(rewrite_config)
  try{
	std::ofstream f(optfile.c_str());
	if(f.good()&&!f.bad())
	write(so,f);
  }
  catch(...){}

   main_details(i,so,!no_full,personalized);

   wipe_dir(temp_dir.c_str());

   return 0;
}

void check_surf(const surfer_options& opt)
{
	std::string image = TEMP_ROOT_SEP+"surfb_it.ppm";
	std::string script = TEMP_ROOT_SEP+"surfb_it.pic";

	std::remove(image.c_str());
	std::remove(script.c_str());

	std::ofstream f(script.c_str(),FILE_WRITE_MODE);
	if(! f.is_open())
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
            exit(105);
        }

        std::ifstream f2(image.c_str());
	if(! f2.is_open())
        {
            std::cerr<<"cannot open file "<<image<<std::endl;
            exit(107);
	}

        std::remove(image.c_str());
        std::remove(script.c_str());
}

bool check_encoder()
{
return false;
}

int log_system(const std::string& s, bool verbosity)
{
	if(!no_log) std::cout<<"executing "<<s<<std::endl;
	int r = system(s.c_str());
	if((r != 0) && (verbosity))
	{
		std::cerr<<"surfer: system call failed"<<std::endl;
		std::cerr<<"call was: "<<s<<std::endl;
	}
	return r;
}

SurfBWindow* main_work(const surfer_options& so, bool b, bool personalized, SurfBWindow* k)
{
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);

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
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);

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

