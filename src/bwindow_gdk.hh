/***************************************************************************
 *   Copyright (C) 2007 by Henning Meyer                                   *
 *   hmeyer@mathematik.uni-kl.de                                           *
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

#include <config.h>

#include <gtkmm.h>
#include <glibmm/i18n.h>
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


//#define BACK_IN_BLACK

#ifdef BACK_IN_BLACK

#define MAIN_COLOR_STRING "black"
#define CONTRAST_COLOR_STRING "white"
#define MAIN_COLOR_LUMINA 0

#else

#define MAIN_COLOR_STRING "white"
#define CONTRAST_COLOR_STRING "black"
#define MAIN_COLOR_LUMINA 255
 
#endif


#define MAIN_COLOR_GDK Gdk::Color(MAIN_COLOR_STRING)
#define CONTRAST_COLOR_GDK Gdk::Color(CONTRAST_COLOR_STRING)



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

struct surf_light
{
	double x;
	double y;
	double z;
	int volume;
	color_rgb256 color;
	bool rotate;
};


typedef color_rgb256 color;


struct surface_data
{

color outside;
color inside;


int ambient;
int diffuse;
int reflected;
int transmitted;
int smoothness;
int transparence;
int thickness;

std::string equation;
std::string public_eq;
	
public:
private:
public:
std::string desc;
};

surface_data local_defaults();

enum surf_illumination
{
	ambient_light = 1,
	diffuse_light = 2,
	reflected_light = 4,
	transmitted_light = 8
};

struct global_parse
{
	color background;
	double spec_z;
	double initial_scale;
	double scale;
	double normalize_factor;
	bool normalize_brightness;
	bool depth_cueing;
	double depth_value;
	double para[4];
	
	int illumination;

	surf_light light[9];

	int antialiasing;
	matrix<double> rot;

	global_parse():rot(unitmat<double>(3,1.0,0.0))
	{}

	int lores;
	int hires;
	int saveres;

	std::string general_stuff;
	std::string name;
};

global_parse global_defaults();

//parsepic_out parse_pic(std::istream& f, bool strict = false);

std::vector<surface_data> read_pic(std::istream& f, global_parse& g, const std::string default_eq = "x*x+y*y-z*z");
struct parse_result
{
	global_parse global_data;
	std::vector<surface_data> data;
	std::string filepath;
};

std::string thumbnail(const parse_result& P);

struct gallery
{
	std::string name;
	std::string path;
	std::vector<parse_result> file;
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
	int print_resolution;

	std::string save_cmd;
	std::string gallery_path;
	int resolution;
	std::string format;

	double upscale;

	std::string surf_cmd;
	std::string entryfont;
	
	std::string helpfile;
	std::string help_cmd;

	std::string ui_xml;
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
	os<<_("Zoom")<<" "<<std::exp(std::log(10.0)*(2-value))<<"%";
	return os.str();
}
};


class AScale: public Gtk::HScale
{
public:
AScale(double a, double b, double c, const std::string& v): Gtk::HScale(a,b,c),var(v) 
{
modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK); 
				
}
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
Gtk::Label m_zero;
AScale m_hscale;
AScale m_hscale2;
Gtk::Button m_special;
Gtk::Label m_printing;

AScale m_hscale3;
AScale m_hscale4;

Gtk::Button m_animate;

void on_animate_click();

ZScale m_vscale;
Gtk::Table m_entryinside;
Gtk::Frame m_entryframe;
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
Gtk::Button m_new_surface;
Gtk::Button m_savefile;
Gtk::SpinButton m_spin;

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

Gtk::Button m_about;

Gtk::HButtonBox m_backfor;

int current_gal;
int current_surf;

bool valid;
void check_image(const std::string& script, const std::string& image);
std::vector<Gtk::DrawingArea> m_gdraw;
std::vector<Gtk::AspectFrame> m_gframe;


bool fullscreen_mode;

void adjust_printing();

bool on_expose_event_func(GdkEventExpose* event);
bool on_motion_notify_event_func(GdkEventMotion* event);
bool on_button_press_event_func(GdkEventButton* event);
bool on_button_release_event_func(GdkEventButton* event);
bool on_timer_event_func(int extra);
void on_insert_text_func(const Glib::ustring&,int*);
void on_delete_text_func(int,int);	

void refresh(const std::string& script, const std::string& image, int aa , bool full = false);
void refresh_image(const std::string& script, const std::string& image, const int aa , bool full, const int n = num_threads(), bool max_res = false, bool max_res2 = true);


void refresh_display(const std::string& image, bool full = false);
void refresh_print(const std::string& image);

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

bool on_scroll_event_func(GdkEventScroll*);


void on_mode_clicked_func(char s);
void on_letter_clicked_func(char s);
void on_special_clicked();
void on_fullscreen_clicked();
void on_print_clicked();
void on_about_clicked();
void on_save_clicked();
void on_save_file_clicked();
void on_open_file_clicked();
void on_noscreen_clicked();
void on_left_clicked();
void on_right_clicked();
void on_next_clicked();
void on_prev_clicked();

void update_visuals();

bool on_surface_expose_event_func(GdkEventExpose* event);
bool on_surface_button_press_event_func(GdkEventButton* event);

sigc::connection conn;

void on_export_pic_clicked();
void on_export_png_clicked();

void do_file_save(bool,bool);

void on_value_changed_func();
void on_spin_changed_func();
void on_para_changed_func();
void on_para2_changed_func();
void on_para3_changed_func();
void on_para4_changed_func();
void on_new_surface_clicked();

void on_delete_surface_clicked();

void set_up_the_menu();
Glib::RefPtr<Gtk::ActionGroup> mr_AG;
Glib::RefPtr<Gtk::UIManager> mr_UIM ;

bool on_button_changed_func(GdkEventButton*);

bool draw_coords;
void draw_grid();

std::vector<surface_data> data;
unsigned data_index;
global_parse global_data;

public:
surfer_options opt;
PID_type kill_list;


bool on_key_press_event_func(GdkEventKey* event);

void on_manual_clicked();
void on_homepage_clicked();

};

void show_the_manual(Gtk::Window* wnd);

std::ostream& write_surf(const global_parse& g, std::ostream& o,  int antialias = 0);
std::ostream& write_surf(const surface_data& f, const int nn, std::ostream& o);


void show_gallery(const gallery& g);

std::string trans_eq(const std::string& t2);

void add_data(std::vector<parse_result>& R);
void make_thumbs(const std::vector<parse_result>& R, const surfer_options& opt);




surface_data surfer_local();
surfer_options read_settings_from_file(const std::string& filename);
std::ostream& write(const surfer_options& so, std::ostream& f);

std::string fix_file(const std::string& s);







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
  parse_result ret;
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
std::string get_revision();
surfer_options default_settings();

class AboutWindow: public Gtk::Dialog
{
public:
AboutWindow(const surfer_options& so, Gtk::Window& parent);
private:
Gtk::Table m_tab;

Gtk::Image i_mfo;
Gtk::Label l_mfo;

Gtk::Image i_bmbf;
Gtk::Label l_bmbf;

Gtk::Image i_surf;
Gtk::Label l_surf;

Gtk::Image i_surfer;
Gtk::Image i_math;
Gtk::Label l_surfer1;
Gtk::Label l_surfer2;
Gtk::Label l_surfer3;
Gtk::Label l_surfer4;

Gtk::Label l_info;
Gtk::Label l_contact;
Gtk::Label l_copy;
Gtk::Label l_credits;

Gtk::Button m_guide;

Gtk::Button m_close;
Gtk::VBox m_b;

Gtk::Alignment a1;
Gtk::Alignment a2;
Gtk::Alignment a3;
Gtk::Alignment a4;
Gtk::Alignment a5,a7,a8;
Gtk::Alignment a6;


void on_close_click();
void on_help_click();
surfer_options opt;
public:
};


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

extern bool no_log;

#define REDIRECTION_APEX (no_log?">/dev/null 2>/dev/null":"")
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


void log_system(const std::string& s);
