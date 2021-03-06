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
	std::string user_gallery_path;
	int resolution;
	std::string format;

	double upscale;

	std::string surf_cmd;
	std::string entryfont;
	
	std::string helpfile;
	std::string help_cmd;

	std::string ui_xml;

	std::string mencoder_cmd;
	std::string ffmpeg_cmd;

	int video_frame_rate;
	int video_resolution;
	int video_bitrate;

	bool modified_surf;

        bool change_lang;

	std::string reset_file;
	std::string screen_saver_gallery;
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


























class SurfBWindow ;


class LightWidget: public Gtk::Table
{
public:

LightWidget();

void set_light(const surf_light& L);
surf_light get_light() const;

virtual void on_light_changed() {
#ifndef NO_MORE_WEIRD_CRASH
return;
#endif

	m_vol.update();
	m_x.update();
	m_y.update();
	m_z.update();

light_changed.emit();
}

sigc::signal<void>& signal_light_changed() {return light_changed;}

private:

Gtk::Label m_lcolor;
Gtk::ColorSelection m_color;

Gtk::Label m_lvol;
Gtk::SpinButton m_vol;

Gtk::Label m_lx;
Gtk::SpinButton m_x;


Gtk::Label m_ly;
Gtk::SpinButton m_y;


Gtk::Label m_lz;
Gtk::SpinButton m_z;

Gtk::VSeparator m_sep;

sigc::signal<void> light_changed;

};

class SpecialEffects: public Gtk::Window
{
public:
SpecialEffects(SurfBWindow& g);

void recapture();

private:

SurfBWindow& gui;

void on_apply_toggle();

Gtk::Table m_tab;
Gtk::CheckButton m_apply;
Gtk::Table m_pos;

Gtk::Label m_lrotx;
Gtk::Label m_lroty;
Gtk::Label m_lrotz;

Gtk::SpinButton m_rotx;
Gtk::SpinButton m_roty;
Gtk::SpinButton m_rotz;

void on_rot_change();

Gtk::Label m_pos_langle;
Gtk::SpinButton m_pos_angle;

Gtk::Notebook m_note;

Gtk::ColorSelection m_c1;
Gtk::ColorSelection m_c2;
Gtk::ColorSelection m_cb;

Gtk::Label m_lc1;
Gtk::Label m_lc2;
Gtk::Label m_lcb;

Gtk::Table m_mat;
Gtk::Table m_surf;
Gtk::Table m_light;
Gtk::Table m_res;

Gtk::SpinButton m_res_save;
Gtk::SpinButton m_res_fast;
Gtk::SpinButton m_res_fine;
Gtk::SpinButton m_res_aa;

Gtk::SpinButton m_res_video;
Gtk::SpinButton m_frame_video;
Gtk::SpinButton m_bitrate_video;


Gtk::Label m_lres_video;
Gtk::Label m_lframe_video;
Gtk::Label m_lbitrate_video;


Gtk::Label m_res_lsave;
Gtk::Label m_res_lfast;
Gtk::Label m_res_lfine;
Gtk::Label m_res_laa;


Gtk::SpinButton m_transp;
Gtk::AccelLabel m_ltransp;


Gtk::SpinButton m_thick;
Gtk::AccelLabel m_lthick;


Gtk::SpinButton m_smooth;
Gtk::AccelLabel m_lsmooth;


Gtk::SpinButton m_ambient;
Gtk::AccelLabel m_lambient;


Gtk::SpinButton m_diffuse;
Gtk::AccelLabel m_ldiffuse;


Gtk::SpinButton m_reflected;
Gtk::AccelLabel m_lreflected;

Gtk::TextView m_code;

LightWidget m_alight[9];

Gtk::Notebook m_lights;



void on_c1_changed_func();
void on_c2_changed_func();
void on_cb_changed_func();

void on_code_changed_func();


void on_res_changed();

void on_trans_changed_func();
void on_thick_changed_func();
void on_smooth_changed_func();
void on_ambient_changed_func();
void on_diffuse_changed_func();
void on_reflected_changed_func();

void on_light_changed(unsigned);

private:
//surface_data& data;
//global_parse& global_data;
//int& pichange;
bool refreshing;
};

class AniWindow: public Gtk::Table
{
public:
AniWindow(SurfBWindow& g);

private:
SurfBWindow& gui;



//Gtk::Label m_lres;
//Gtk::SpinButton m_res;

Gtk::Table m_tab;

Gtk::TreeView m_TreeView;

Gtk::ScrolledWindow m_scroll;

Gtk::ProgressBar m_prog;

Gtk::Button m_edit;
Gtk::Button m_save;
Gtk::Button m_stop;
Gtk::Frame m_fpreview;
unsigned m_pad_to;

void on_pause();
void on_save();

bool on_button_press_event_func(GdkEventButton* event);

class ModelColumns : public Gtk::TreeModelColumnRecord
{
public:

  ModelColumns()
    { add(m_col_number); add(m_col_pic); add(m_col_duration); add(m_col_state);}

  //Gtk::TreeModelColumn<Glib::ustring> m_col_text;
  Gtk::TreeModelColumn<int> m_col_number;
  Gtk::TreeModelColumn<int> m_col_duration;
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > m_col_pic;
  Gtk::TreeModelColumn<parse_result> m_col_state;
};

ModelColumns m_Columns;

void add_entry(const parse_result& P,
        unsigned duration , int idx);


unsigned fcount;
Glib::RefPtr<Gtk::ListStore> m_refListStore ;

bool on_view_key_press(GdkEventKey* event);
bool on_timer(int);
sigc::connection conn;

Gtk::Image m_preview;

std::vector<Glib::RefPtr<Gdk::Pixbuf> > m_movie;
std::vector<std::string > m_movie_file;
int m_movie_frame;

Gtk::Button m_record;
Gtk::Button m_add_frame;
Gtk::Button m_delete_frame;
Gtk::Button m_pause_cont;

void on_record();
void on_delete_frame();
void on_pause_cont();
void compute();
void on_edit();

bool invalidated;

void on_my_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);

void set_up_the_ani_menu();

Glib::RefPtr<Gtk::ActionGroup> mr_AG;
Glib::RefPtr<Gtk::UIManager> mr_UIM ;


public:
void ani_add();
};



class Language_box: public Gtk::ComboBox
{

};


typedef unsigned char rgb_triplet[3];

#ifndef COLORTABLE_SIZE
#define COLORTABLE_SIZE 250
#endif

Glib::RefPtr<Gdk::Pixbuf> make_colortable(bool flip,rgb_triplet buffer[COLORTABLE_SIZE][COLORTABLE_SIZE]);

class SurfBWindow : public Gtk::Window
{
public:
SurfBWindow(std::istream& i,const std::vector<gallery>& G, surfer_options so, bool f = false, bool personalized = false);

void start();
private:

void restart_with_lang(const char* LANG);

bool personalized;
bool waiting;

bool valid;

bool fullscreen_mode;
bool draw_coords;

int current_gal;
int current_surf;

int pichange;

surfer_options opt;
std::vector<gallery> gal;


PID_type kill_list;





std::vector<surface_data> data;
unsigned data_index;
global_parse global_data;





sigc::connection conn;

void adjust_visibility();


Gtk::Table m_tab;
Gtk::DrawingArea m_draw;

Gtk::Adjustment m_gallconvpx;
Gtk::Adjustment m_gallconvpy;
Gtk::Viewport m_gallconvp;
Gtk::ScrolledWindow m_gallcon;

AScale m_hscale;
AScale m_hscale2;
AScale m_hscale3;
AScale m_hscale4;
ZScale m_vscale;
void on_animate_click();



Gtk::Frame m_entryframe;


Gtk::Table m_bft;
Gtk::Table m_entryfield;
Gtk::Table m_entryinside;
Gtk::Table m_zoom_table;
Gtk::Table m_tool;

Gtk::Image m_info;
Gtk::Image m_zoom_free;
Gtk::Image m_leave_image;
Gtk::Image m_zoom_image;
Gtk::Image m_scale_free;


Gtk::SpinButton m_spin;

protected:
virtual bool on_delete_event  	(  	GdkEventAny *   	 event  	 );
private:
bool on_gallery_press_event(GdkEventButton* e, int i);



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
Gtk::Table m_utab;
Gtk::Table m_ltab;
Gtk::Table m_gtab;


Gtk::Button m_about;
Gtk::Button m_full;
Gtk::Button m_leave;
Gtk::Image m_img_left;
Gtk::Button m_left;
Gtk::Button m_next;
Gtk::Button m_new_surface;
Gtk::Button m_prev;
Gtk::Button m_print;
Gtk::Image m_img_right;
Gtk::Button m_right;
Gtk::Button m_save;
Gtk::Button m_savefile;
Gtk::Button m_special;


Gtk::HBox m_bbox;
Gtk::HButtonBox m_fbox;

std::vector<Gtk::Button> v_butt;
std::vector<Gtk::RadioButton*> v_fbutt;



Gtk::HButtonBox m_backfor;

Gtk::Label m_avalue;
Gtk::Label m_avalue2;
Gtk::Label m_error;
Gtk::Label m_printing;
Gtk::Label m_zero;

Gtk::DrawingArea m_background;

void check_image(const std::string& script, const std::string& image);

std::vector<Gtk::DrawingArea> m_gdraw;
std::vector<Gtk::AspectFrame> m_gframe;

Gtk::Button m_reset;
time_t m_last_action;
unsigned m_waiting_mode;
sigc::connection m_wait_conn;

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

void on_reset_clicked();
bool on_wait_elapsed();

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

void update_visuals(bool f = true);

bool on_surface_expose_event_func(GdkEventExpose* event);
bool on_surface_button_press_event_func(GdkEventButton* event);



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




bool on_key_press_event_func(GdkEventKey* event);

void on_manual_clicked();
void on_homepage_clicked();

friend class AniWindow;
friend class SpecialEffects;
AniWindow m_ani;
SpecialEffects w_sfx;



rgb_triplet inside_buffer[COLORTABLE_SIZE][COLORTABLE_SIZE];
rgb_triplet outside_buffer[COLORTABLE_SIZE][COLORTABLE_SIZE];
Glib::RefPtr<Gdk::Pixbuf> m_inside_image;
Glib::RefPtr<Gdk::Pixbuf> m_outside_image;

Language_box m_lang;

bool first_time_full;
public:
 SurfBWindow* kill_w;
};

void show_the_manual(Gtk::Window* wnd, surfer_options);

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

extern bool no_log;

#define QUIET_SURF (std::string((no_log && !no_new_surf_features)?(" -q "):("")))




#ifndef WIN32
#ifdef WIN_32
#define WIN32 WIN_32
#endif
#endif

#ifndef TEMP_ROOT_SEP

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

#ifndef EXT_MARKER
#ifndef WIN32
#define EXT_MARKER ""
#else
#define EXT_MARKER ".ini"
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


extern bool no_new_surf_features;
int log_system(const std::string& s);

extern bool do_restart;

void parallel_clear();

SurfBWindow* main_work(const surfer_options& so, bool b, bool personalized, SurfBWindow* k);

