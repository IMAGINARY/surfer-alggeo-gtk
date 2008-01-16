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


#include <gtkmm.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include "matrix.hh"
#include <cmath>

#include <gdkmm.h>

#ifdef WIN32

#include <windows.h>
typedef DWORD PID_type;
#else

#include <unistd.h>
#include <signal.h>
typedef pid_t PID_type;

#endif


bool check_input(const std::string& s);
std::string fix_input(const std::string& s);
std::string fix_input_for_display(const std::string& s);
std::string fix_input_for_surf(const std::string& s);

int num_threads();

struct color_rgb256
{
	int red;
	int green;
	int blue;
};

struct color_double
{
	double red;
	double green;
	double blue;

	color_rgb256 surf_colors() const{color_rgb256 x; x.red = int(256*red); x.green = int(256*green);x.blue = int(256*blue); return x;}
	Gdk::Color gdk_colors() const{Gdk::Color x; x.set_rgb( int(65535*red), int(65535*green), int(65535*blue)); return x;}
};

color_double colormap(double x, double y, double Z = 0.0);

typedef color_rgb256 color;

struct parsepic_out
{
	std::string general_stuff;
	std::string antialiasing;
	std::string equation;
	std::string public_eq;
	
	color upside;
	color inside;
	color background;

	double initial_scale;
	double scale;
	
	double para_a;
	double para_b;

	std::string name;
	std::string thumbnail;
	std::string desc;
	std::string filename;
	int lores;
	int hires;

	matrix<double> rot;

	parsepic_out():rot(unitmat<double>(3,1.0,0.0)){}
};

parsepic_out parse_pic(std::istream& f, bool strict = false);

struct gallery
{
	std::string name;
	std::string path;
	std::vector<parsepic_out> file;
	std::string image;
	std::string desc;
};

std::vector<gallery> read_galleries(const std::string& path, double upscaling);
std::vector<gallery> read_galleries_new(const std::string& path, double upscaling);
std::vector<gallery> read_galleries_old(const std::string& path, double upscaling);

gallery read_gallery(const std::string& path, const std::string& name, double upscaling);

struct surfer_options
{
	std::string print_cmd;
	std::string save_cmd;
	std::string gallery_path;
	int resolution;
	std::string format;

	double upscale;

	std::string surf_cmd;
	std::string surf_4[4];
	std::string surf_2[2];
	
	
};

class ZScale: public Gtk::VScale
{
public:
ZScale(double a, double b, double c): Gtk::VScale(a,b,c) {}
protected:
Glib::ustring on_format_value(double value)
{
	
	std::ostringstream os;
	os.precision(0);
	os.setf(std::ios_base::fixed);
	os<<"Zoom "<<std::exp(std::log(10.0)*(2-value))<<"%";
	return os.str();
}
};


class AScale: public Gtk::HScale
{
public:
AScale(double a, double b, double c, const std::string& v): Gtk::HScale(a,b,c),var(v) {}
protected:
Glib::ustring on_format_value(double value)
{
	std::ostringstream os;
	os.precision(2);
	os.setf(std::ios_base::fixed);
	os<<var<<"="<<value;
	
	return os.str();
}
private:
std::string var;
};


class SurfBWindow : public Gtk::Window
{
public:
SurfBWindow(std::istream& i,const std::vector<gallery>& G, surfer_options so, bool f = false, bool personalized = false);
void start();
private:
void adjust_visibility();

std::vector<gallery> gal;
Gtk::Table m_tab;
Gtk::DrawingArea m_draw;
AScale m_hscale;
AScale m_hscale2;

ZScale m_vscale;

Gtk::HBox m_bbox;
std::vector<Gtk::Button> v_butt;

Gtk::HButtonBox m_fbox;
std::vector<Gtk::RadioButton*> v_fbutt;
Gtk::Table m_bft;
Gtk::Image m_info;
Gtk::Button m_left;
Gtk::Button m_right;
Gtk::Table m_entryfield;
Gtk::Label m_avalue;
Gtk::Label m_avalue2;
Gtk::Image m_zoom_free;
Gtk::Image m_leave_image;
Gtk::Image m_zoom_image;
Gtk::Table m_zoom_table;
Gtk::Image m_scale_free;
Gtk::Label m_error;

Gtk::Button m_savefile;


bool on_gallery_press_event(GdkEventButton* e, int i);

bool waiting;
bool personalized;
Gtk::AspectFrame m_aframe;
Gtk::AspectFrame m_bframe;
Gtk::AspectFrame m_cframe;
Gtk::AspectFrame m_iframe;

Gtk::Entry m_entry;

Gtk::DrawingArea m_colors;
Gtk::DrawingArea m_colors_inside;
Gtk::DrawingArea m_colors_back;
Gtk::DrawingArea m_movie;

Gtk::Notebook m_note;

Gtk::Table m_ctab;
Gtk::Button m_print;
Gtk::Button m_full;
Gtk::Button m_save;

Gtk::Table m_utab;
Gtk::Table m_ltab;
Gtk::Table m_gtab;
Gtk::Button m_leave;

Gtk::Button m_next;
Gtk::Button m_prev;

Gtk::HButtonBox m_backfor;

int current_gal;
int current_surf;

bool valid;
void check_image(const std::string& script, const std::string& image);
std::vector<Gtk::DrawingArea> m_gdraw;
std::vector<Gtk::AspectFrame> m_gframe;


bool fullscreen_mode;


bool on_expose_event_func(GdkEventExpose* event);
bool on_motion_notify_event_func(GdkEventMotion* event);
bool on_button_press_event_func(GdkEventButton* event);
bool on_button_release_event_func(GdkEventButton* event);
bool on_timer_event_func(int extra);
void on_insert_text_func(const Glib::ustring&,int*);
void on_delete_text_func(int,int);	

void refresh(const std::string& script, const std::string& image, const std::string& aa, bool full = false);
void refresh_image(const std::string& script, const std::string& image, const std::string& aa, bool full, const int n = num_threads(), bool max_res = false);


void refresh_display(const std::string& image, bool full = false);

bool on_color_button_press_event_func(GdkEventButton* event);
bool on_color_expose_event_func(GdkEventExpose* event);
bool on_color_button_release_event_func(GdkEventButton* event);
bool on_color_motion_notify_event_func(GdkEventMotion* event);



bool on_inside_button_press_event_func(GdkEventButton* event);
bool on_inside_expose_event_func(GdkEventExpose* event);
bool on_inside_button_release_event_func(GdkEventButton* event);
bool on_inside_motion_notify_event_func(GdkEventMotion* event);



bool on_back_button_press_event_func(GdkEventButton* event);
bool on_back_expose_event_func(GdkEventExpose* event);
bool on_back_button_release_event_func(GdkEventButton* event);
bool on_back_motion_notify_event_func(GdkEventMotion* event);



void on_mode_clicked_func(char s);
void on_letter_clicked_func(char s);

void on_fullscreen_clicked();
void on_print_clicked();
void on_save_clicked();
void on_save_file_clicked();
void on_noscreen_clicked();
void on_left_clicked();
void on_right_clicked();
void on_next_clicked();
void on_prev_clicked();

bool on_surface_expose_event_func(GdkEventExpose* event);
bool on_surface_button_press_event_func(GdkEventButton* event);

sigc::connection conn;
void on_value_changed_func();
void on_para_changed_func();
void on_para2_changed_func();
bool on_button_changed_func(GdkEventButton*);

parsepic_out data;
public:
surfer_options opt;
PID_type kill_list;
};



class SelectWindow : public Gtk::Window
{
public:
SelectWindow(const gallery&, const surfer_options&);
parsepic_out res;
private:

Gtk::Table m_tab;
std::vector<Gtk::DrawingArea*> vdraw;

bool on_surface_expose_event_func(GdkEventExpose*, int index);
bool on_surface_button_press_event_func(GdkEventButton*, int index);



std::vector<parsepic_out> R;
surfer_options opt;

};

void show_gallery(const gallery& g);

std::string trans_eq(const std::string& t2);

void add_data(std::vector<parsepic_out>& R);
void make_thumbs(const std::vector<parsepic_out>& R, const surfer_options& opt);


 void read_thumbs(std::vector<parsepic_out>& R);


surfer_options read_settings_from_file(const std::string& filename);
std::ostream& write(const surfer_options& so, std::ostream& f);

std::string fix_file(const std::string& s);


void my_system(const std::string& s, const std::string& s2, const std::string& ulimit, const surfer_options& opt);
void my_kill(PID_type pid);






class GalleryWindow : public Gtk::Window
{
public:
  GalleryWindow(const gallery& gal, const surfer_options& opt);
  ~GalleryWindow();

protected:
  // Signal handlers:
  //
  //void on_button_quit();
  void on_item_activated(const Gtk::TreeModel::Path& path);
  void on_selection_changed();

  void add_entry(const std::string& filename, const Glib::ustring& description, int idx);

  // Tree model columns:
  //
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    {
      add(m_col_filename);
      add(m_col_description);
      add(m_col_pixbuf);
      add(m_col_index);
    }

    Gtk::TreeModelColumn<std::string> m_col_filename;
    Gtk::TreeModelColumn<Glib::ustring>  m_col_description;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > m_col_pixbuf;
    Gtk::TreeModelColumn<int> m_col_index;
  };

  ModelColumns m_Columns;

  // Child widgets:
  Gtk::VBox m_VBox;
  Gtk::Image m_desc;
  Gtk::Table m_tab;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::IconView m_IconView;
  Glib::RefPtr<Gtk::ListStore> m_refListModel;
  Gtk::Frame m_frame;
  Gtk::Label m_please;
  //Gtk::HButtonBox m_ButtonBox;
  //Gtk::Button m_Button_Quit;
  gallery gal;
public:
  parsepic_out ret;
	int isu;
  surfer_options opt;
};
std::string fix_path(const std::string& s);
extern std::string temp_dir;


PID_type execute_async(const std::string& s);
void kill_pid(PID_type pid);


void write_gallery_file(std::ostream& f, const gallery& G);
gallery read_gallery_file(std::istream& f, const std::string& path, const std::string& dname,double);
PID_type system_async(const std::string& s, const std::string& ulimit, const surfer_options& opt);


void parallel_surf(const std::string& script,bool sync, int width, const surfer_options& opt);

void init_threads();


#ifndef WIN32
#ifdef WIN_32
#define WIN32 WIN_32
#endif
#endif

#ifndef TEMP_ROOT_SEP
//#ifdef WIN32
//#define TEMP_ROOT_SEP ""
//#else
//#define TEMP_ROOT_SEP "/tmp/"
//#endif
#define TEMP_ROOT_SEP temp_dir

#endif

#ifndef DIR_SEP
#ifdef WIN32
#define DIR_SEP "\\"
#else
#define DIR_SEP "/"
#endif
#endif


#ifndef HIDDEN_MARKER
#ifdef WIN32
#define HIDDEN_MARKER ""
#else
#define HIDDEN_MARKER "."
#endif
#endif



#ifndef DIR_SEP_CHAR
#ifdef WIN32
#define DIR_SEP_CHAR '\\'
#else
#define DIR_SEP_CHAR '/'
#endif
#endif

#ifndef WRONG_DIR_SEP_CHAR
#ifdef WIN32
#define WRONG_DIR_SEP_CHAR '/'
#else
#define WRONG_DIR_SEP_CHAR '\\'
#endif
#endif


#ifndef FILE_WRITE_MODE
#ifdef WIN32
#define FILE_WRITE_MODE std::ios_base::out|std::ios_base::binary
#else
#define FILE_WRITE_MODE std::ios_base::out
#endif
#endif

#ifndef REDIRECTION_APEX
#ifdef WIN32
#define REDIRECTION_APEX ""
#else
#define REDIRECTION_APEX ">/dev/null 2>/dev/null"
#endif
#endif

#ifndef DAEMONIZE
#ifdef WIN32
#define DAEMONIZE ""
#else
#define DAEMONIZE " &"
#endif
#endif

#ifndef PREVIEW_SIZE
#define PREVIEW_SIZE 150
#endif


