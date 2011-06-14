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


#ifndef PRESENTATION_MODE
#define PRESENTATION_MODE 0
#endif

#ifndef MAX_SURFACES
#if PRESENTATION_MODE == 1
#define MAX_SURFACES 1
#else
#define MAX_SURFACES 8
#endif
#endif

#include "bwindow_gdk.hh"
#include "gdkmm/pixbuf.h"

#include "matrix.hh"

std::string revstring = "$Revision$";
std::string datestring = "$Date$";


int fastaa = 1;//"antialiasing=2;";

//#define SURF_CMD "surf"

bool no_full = true;
bool no_info = false;
bool no_gallery = false;
bool printing = false;
bool no_log = true;
bool no_zero = false;

bool no_modify = false;

bool no_keys = false;
bool no_new_features = false;

#ifndef SHIPS_WITH_MODIFIED_SURF
#ifdef WIN32
#define SHIPS_WITH_MODIFIED_SURF 1
#else
#define SHIPS_WITH_MODIFIED_SURF 0
#endif
#endif


bool no_new_surf_features = !SHIPS_WITH_MODIFIED_SURF;

extern const char * flag_ca_ES[];
extern const char * flag_es_ES[];
extern const char * flag_de_DE[];
extern const char * flag_de_AT[];
extern const char * flag_en_US[];
extern const char * flag_en_UK[];

#define MOD if(!no_modify)

double current_x = 0;
double current_y = 0;
double current_z = 0;

const int MSECS = 40;





bool button_down = false;

int gui_x;
int gui_y;


std::vector< Glib::RefPtr<Gdk::Pixbuf> > thumbs;
int total_x = 0;
int moving_x = 0;





SurfBWindow::SurfBWindow(std::istream& i, const std::vector<gallery>& G, surfer_options so,bool f, bool p)
:


personalized(p),
waiting(true),
valid(true),
fullscreen_mode(f),
draw_coords(false),
current_gal(),
current_surf(),
pichange(0),
opt(so),
gal(G),
kill_list(0),

data(),
data_index(),
global_data(),

conn(),

m_tab(3,2),
m_draw(),

m_gallconvpx(1,0,1),
m_gallconvpy(1,0,1),
m_gallconvp(m_gallconvpx,m_gallconvpy),
m_gallcon(),

m_hscale(0,1.05,0.05,"a"),
m_hscale2(0,1.05,0.05,"b"),
m_hscale3(-1,1.05,0.05,"c"),
m_hscale4(1,10.05,0.05,"d"),
m_vscale(-1,1,0.05),

m_entryframe(),
m_bft(),
m_entryfield(3,1),
m_entryinside(),
m_zoom_table(1,3),

m_tool(),
m_info(),
m_zoom_free(),
m_leave_image(Gtk::Stock::LEAVE_FULLSCREEN,Gtk::ICON_SIZE_BUTTON),
m_zoom_image(Gtk::Stock::ZOOM_FIT,Gtk::ICON_SIZE_BUTTON),
m_scale_free(),

m_spin(),

m_aframe("",Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,1.0,false),
m_bframe(),
m_cframe(_("side 1"), Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 1.0, false ),
m_iframe(_("side 2"), Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 1.0, false),

m_entry(),
m_colors(),
m_colors_inside(),
m_colors_back(),
m_movie(),

m_note(),


m_ctab(3,3),
m_utab(1,1),
m_ltab(3,3),

m_gtab(),



m_about(Gtk::Stock::ABOUT),
m_full(Gtk::Stock::FULLSCREEN),
m_leave(),
m_img_left(Gtk::Stock::GO_BACK,Gtk::ICON_SIZE_BUTTON),
m_left(),

m_next(Gtk::Stock::GO_FORWARD),


m_new_surface(Gtk::Stock::NEW),

m_prev(Gtk::Stock::GO_BACK),

m_print(),

m_img_right(Gtk::Stock::GO_FORWARD,Gtk::ICON_SIZE_BUTTON),
m_right(),
m_save(),
m_savefile(Gtk::Stock::SAVE),
m_special(_("Details")),

m_bbox(),
m_fbox(),
v_butt(),
v_fbutt(),

m_backfor(),
m_avalue(),
m_avalue2(),
m_error(),
m_printing(_("Printing image...")),
m_zero("=0"),
m_background(),

m_gdraw(),
m_gframe(),
mr_AG(),
mr_UIM(),

//m_animate(Gtk::Stock::MEDIA_RECORD),
m_ani(*this),
w_sfx(*this),

inside_buffer(),
outside_buffer(),
m_inside_image(),
m_outside_image(),
first_time_full(true),
kill_w(NULL),
m_reset(Gtk::Stock::STOP),
m_last_action(time(NULL)),
m_waiting_mode(0)
{

       
	m_print.set_image(*new Gtk::Image(Gtk::Stock::PRINT,Gtk::ICON_SIZE_BUTTON));
	m_save.set_image(*new Gtk::Image(Gtk::Stock::SAVE,Gtk::ICON_SIZE_BUTTON));
	m_left.set_image(m_img_left);
	m_right.set_image(m_img_right);


	// change window background to white
	MOD{
            modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
	    m_draw.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
            m_left.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
            m_right.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
        }

	current_gal=0;
	if(!gal.empty())current_surf = gal[0].file.size()-1;

	m_vscale.set_draw_value(false);
	
	global_data = global_defaults();
	data = read_pic(i,global_data);
	data_index = 0;
	

	m_draw.set_size_request(100, 100);
	
	m_colors.set_size_request(COLORTABLE_SIZE, COLORTABLE_SIZE);
	m_colors_inside.set_size_request(COLORTABLE_SIZE, COLORTABLE_SIZE);
	
		
	if(!opt.entryfont.empty()) m_entry.modify_font(Pango::FontDescription(opt.entryfont));

	m_inside_image = make_colortable(true,inside_buffer);
	m_outside_image = make_colortable(false,outside_buffer);

	this->signal_key_press_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_key_press_event_func) );

	m_draw.signal_expose_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_expose_event_func) );
	m_draw.signal_motion_notify_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_motion_notify_event_func) );
	m_draw.signal_button_press_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_button_press_event_func) );
	m_draw.signal_button_release_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_button_release_event_func) );
	m_draw.add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);

	m_colors.signal_button_press_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_color_button_press_event_func) );
	m_colors.signal_motion_notify_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_color_motion_notify_event_func) );
	m_colors.signal_button_release_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_color_button_release_event_func) );
	m_colors.signal_expose_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_color_expose_event_func) );
	m_colors.add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);

	m_special.signal_clicked().connect(sigc::mem_fun(*this,&SurfBWindow::on_special_clicked));

	m_colors_inside.signal_button_press_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_inside_button_press_event_func) );
	m_colors_inside.signal_motion_notify_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_inside_motion_notify_event_func) );
	m_colors_inside.signal_button_release_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_inside_button_release_event_func) );
	m_colors_inside.signal_expose_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_inside_expose_event_func) );
	m_colors_inside.add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);


	m_colors_back.signal_button_press_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_back_button_press_event_func) );
	m_colors_back.signal_motion_notify_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_back_motion_notify_event_func) );
	m_colors_back.signal_button_release_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_back_button_release_event_func) );
	m_colors_back.signal_expose_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_back_expose_event_func) );
	m_colors_back.add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);

	m_draw.signal_scroll_event().connect(sigc::mem_fun(*this, &SurfBWindow::on_scroll_event_func));

	m_new_surface.signal_clicked().connect(sigc::mem_fun(*this,&SurfBWindow::on_new_surface_clicked));

	m_entry.signal_insert_text().connect(sigc::mem_fun(*this, &SurfBWindow::on_insert_text_func));
	m_entry.signal_delete_text().connect(sigc::mem_fun(*this, &SurfBWindow::on_delete_text_func));
	m_entry.add_events(Gdk::ALL_EVENTS_MASK);
	m_vscale.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_value_changed_func));
	m_spin.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_spin_changed_func));
	m_hscale.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_para_changed_func));
	
	m_hscale2.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_para2_changed_func));
	m_hscale3.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_para3_changed_func));
	m_hscale4.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_para4_changed_func));

	m_vscale.signal_button_release_event().connect(sigc::mem_fun(*this,&SurfBWindow::on_button_changed_func));

	m_hscale.set_value(global_data.para[0]);
	m_hscale.set_increments(0.01,0.050);
	
	m_hscale2.set_value(global_data.para[1]);
	m_hscale2.set_increments(0.01,0.050);

	m_hscale3.set_value(global_data.para[2]);
	m_hscale3.set_increments(0.01,0.050);

	m_hscale4.set_value(global_data.para[3]);
	m_hscale4.set_increments(0.01,0.050);

	m_vscale.set_value(0);
	m_vscale.set_increments(0.01,0.050);

MOD{
	m_hscale.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
	m_hscale2.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
	m_hscale3.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
	m_hscale4.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);


	m_vscale.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
	m_vscale.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
	m_vscale.modify_fg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
	m_vscale.modify_text(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
}

	m_reset.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_reset_clicked));

	m_leave.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_noscreen_clicked));
	m_save.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_save_clicked));
	m_savefile.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_save_file_clicked));
	m_print.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_print_clicked));
	m_full.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_fullscreen_clicked));
	m_about.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_about_clicked));
	m_left.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_left_clicked));
	m_right.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_right_clicked));

	m_next.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_next_clicked));
	m_prev.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_prev_clicked));

	set_title("Surfer");

	m_note.set_tab_pos(Gtk::POS_BOTTOM);
	m_hscale.set_value_pos(Gtk::POS_LEFT);
	m_hscale2.set_value_pos(Gtk::POS_RIGHT);
	m_hscale3.set_value_pos(Gtk::POS_LEFT);
	m_hscale4.set_value_pos(Gtk::POS_RIGHT);
	
	m_aframe.add(m_draw);
        m_aframe.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
        m_aframe.set_shadow_type(Gtk::SHADOW_NONE);
	MOD{ m_aframe.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
	     m_aframe.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); }


	add(m_utab);

	m_zoom_free.set_size_request(8,200);
	m_zoom_table.attach(m_zoom_free,0,1,0,1,Gtk::SHRINK,Gtk::SHRINK);
	
	m_zoom_table.attach(m_zoom_image,0,1,1,2,Gtk::SHRINK,Gtk::SHRINK);
	m_zoom_table.attach(m_vscale,0,1,2,3,Gtk::SHRINK);

	if(fullscreen_mode)
	{
		
		m_utab.attach(m_aframe,0,2,0+1,2+1);
		m_utab.attach(m_avalue,3,4,0+1,1+1,Gtk::SHRINK);
		m_utab.attach(m_leave,3,4,1+1,2+1,Gtk::SHRINK,Gtk::SHRINK);

		m_utab.attach(m_zoom_table,2,3,0+1,2+1,Gtk::SHRINK);

		

		m_utab.attach(m_scale_free,0,1,2+1,3+1,Gtk::SHRINK,Gtk::SHRINK);
		m_utab.attach(m_hscale,0,1,3+1,4+1,Gtk::FILL,Gtk::SHRINK);
		m_utab.attach(m_hscale2,1,2,3+1,4+1,Gtk::FILL,Gtk::SHRINK);
		m_utab.attach(m_hscale3,0,1,2+1,3+1,Gtk::FILL,Gtk::SHRINK);
		m_utab.attach(m_hscale4,1,2,2+1,3+1,Gtk::FILL,Gtk::SHRINK);


		m_leave.set_image(m_leave_image);

                MOD{m_leave.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);}

		set_border_width(0);
		if(!no_info)set_size_request(600,500);
		show();
		maximize();
		fullscreen();
		present();
		raise();
	}
	else
	{

		set_up_the_menu();

		m_utab.attach(m_aframe,0, 2, 0+1, 1+1 ,Gtk::EXPAND|Gtk::FILL,Gtk::EXPAND|Gtk::FILL);
		m_utab.attach(m_bbox,0, 2, 3+1, 4+1 ,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);


		m_utab.attach(m_entryfield,0,2,4+1,5+1,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);

		m_utab.attach(m_zoom_table,2,3,0+1,3+1,Gtk::SHRINK);
		
		//m_utab.attach(m_new_surface,2,3,3+1,4+1,Gtk::SHRINK,Gtk::SHRINK);
		
		if(!no_new_features)
		m_tool.attach(m_spin,3,4,5,6,Gtk::SHRINK,Gtk::SHRINK);

                m_spin.set_range(1,1);
		
		m_spin.set_value(1);
		m_spin.set_wrap();
		m_spin.set_increments(1,1);

		MOD{
                m_spin.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
                m_spin.modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
                
                }

		m_utab.attach(m_scale_free,0,1,1+1,2+1,Gtk::SHRINK,Gtk::SHRINK);
		m_utab.attach(m_hscale,0,1,2+1,3+1,Gtk::FILL,Gtk::SHRINK);
		m_utab.attach(m_hscale2,1,2,2+1,3+1,Gtk::FILL,Gtk::SHRINK);


		m_utab.attach(m_hscale3,0,1,1+1,2+1,Gtk::FILL,Gtk::SHRINK);
		m_utab.attach(m_hscale4,1,2,1+1,2+1,Gtk::FILL,Gtk::SHRINK);

		//m_utab.attach(m_bft,3, 4, 3+1, 5+1 ,Gtk::SHRINK,Gtk::SHRINK);
		m_utab.attach(m_note,3,6,0+1,3+1,Gtk::SHRINK);
		
		set_border_width(0);

		if(!no_keys)
		{
			char s[] = "abxyz+-*^.()0123456789D";
			char b[2] = {0,0};
			for(unsigned i = 0; s[i] != 0; i++)
			{
				b[0] = s[i];

				Gtk::Button* B = NULL;

				
				
				B = Gtk::manage(new Gtk::Button(b));
				B->signal_clicked().connect( sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_letter_clicked_func) ,b[0]));
                                
                                MOD{B->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
                                    B->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
                                    B->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
                                    B->modify_base(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
                                   }

                                if(s[i]=='D')
				{
					
					B->set_label(_("Delete"));
				}
                                else
                                B->set_label(b);

				m_bbox.add(*B);


			}}

			
			m_error.set_markup("<span foreground=\"#FF0000\" weight=\"bold\">!</span>");
			m_error.set_selectable(false);

			m_utab.attach(m_error,2,3,4+1,5+1,Gtk::SHRINK,Gtk::SHRINK);

			
			//m_fbox.add(m_savefile);
			//m_fbox.add(m_about);


			//m_fbox.set_child_min_width(1);


			m_backfor.add(m_prev);
			//m_backfor.add(m_full);
			m_backfor.add(m_next);
			
			m_backfor.set_child_min_width(60);


			m_entryfield.attach(m_left,0,1,0,1,Gtk::SHRINK,Gtk::SHRINK);
			m_entryfield.attach(m_entryframe,1,2,0,1,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
			m_entryinside.attach(m_entry,1,2,0,1,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
			m_entryframe.add(m_entryinside);
			m_entryframe.set_shadow_type(Gtk::SHADOW_IN);
			m_entry.set_has_frame(false);

                        MOD{
                        m_entry.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
                        m_entry.modify_bg(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);

                        m_entryframe.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
                        m_entryframe.modify_bg(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);

                        m_entry.modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			m_entry.modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			m_entry.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
			
			m_entry.modify_fg(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			m_entry.modify_text(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			m_entry.modify_base(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);

                        }

			if(!no_zero)
			{
				m_entryinside.attach(m_zero,2,3,0,1,Gtk::SHRINK,Gtk::SHRINK);
				MOD{ m_zero.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
				m_zero.modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);}
			}

			m_entryfield.attach(m_right,3,4,0,1,Gtk::SHRINK,Gtk::SHRINK);

			m_entry.set_text(data[data_index].public_eq);

			
			MOD{
			m_note.modify_fg(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);
			m_note.modify_fg(Gtk::STATE_ACTIVE, MAIN_COLOR_GDK);
			m_note.modify_fg(Gtk::STATE_SELECTED, MAIN_COLOR_GDK);
			m_note.modify_fg(Gtk::STATE_PRELIGHT, MAIN_COLOR_GDK);
			m_note.modify_fg(Gtk::STATE_INSENSITIVE, MAIN_COLOR_GDK);

			m_note.modify_base(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);
			m_note.modify_base(Gtk::STATE_ACTIVE, MAIN_COLOR_GDK);
			m_note.modify_base(Gtk::STATE_SELECTED, MAIN_COLOR_GDK);
			m_note.modify_base(Gtk::STATE_PRELIGHT, MAIN_COLOR_GDK);
			m_note.modify_base(Gtk::STATE_INSENSITIVE, MAIN_COLOR_GDK);

			m_note.modify_bg(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);
			m_note.modify_bg(Gtk::STATE_ACTIVE, MAIN_COLOR_GDK);
			m_note.modify_bg(Gtk::STATE_SELECTED, MAIN_COLOR_GDK);
			m_note.modify_bg(Gtk::STATE_PRELIGHT, MAIN_COLOR_GDK);
			m_note.modify_bg(Gtk::STATE_INSENSITIVE, MAIN_COLOR_GDK);
			}
			


			Gtk::Label* v_co = Gtk::manage(new Gtk::Label(_("Coloring")));
			MOD{
			v_co->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_co->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_co->modify_base(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			
			v_co->modify_fg(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_co->modify_text(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_co->modify_base(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			}
			m_note.append_page(m_ctab,*v_co);





			//if(!PRESENTATION_MODE)m_ctab.attach(m_special,1,2,0,1,Gtk::SHRINK,Gtk::SHRINK);

			m_ctab.attach(*Gtk::manage(new Gtk::Image),1,2,1,2);
			m_ctab.attach(m_cframe,1,2,2,3,Gtk::EXPAND,Gtk::SHRINK);//,Gtk::SHRINK|Gtk::FILL,Gtk::SHRINK|Gtk::FILL);
			m_cframe.add(m_colors);

			m_ctab.attach(m_iframe,1,2,3,4,Gtk::EXPAND,Gtk::SHRINK);//,Gtk::SHRINK|Gtk::FILL,Gtk::SHRINK|Gtk::FILL);
			m_iframe.add(m_colors_inside);



			MOD{
			m_cframe.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
			m_cframe.modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			m_cframe.modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);

			m_cframe.get_label_widget()->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			m_cframe.get_label_widget()->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);

			m_iframe.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
			m_iframe.modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			m_iframe.modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);

			m_iframe.get_label_widget()->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			m_iframe.get_label_widget()->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);

                    	

			}

			Gtk::Label *v_ga = Gtk::manage(new Gtk::Label(_("Gallery")));

			MOD {
			v_ga->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_ga->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_ga->modify_base(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			
			v_ga->modify_fg(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_ga->modify_text(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_ga->modify_base(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			}


                        m_gallconvp.add(m_gtab);

			if(!no_new_features || true)
                        m_gallcon.add(m_gallconvp);



			MOD{
                        m_background.modify_fg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
			m_background.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
			m_background.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
                        
                        m_background.modify_fg(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);
			m_background.modify_bg(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);
			m_background.modify_base(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);

			m_background.modify_fg(Gtk::STATE_PRELIGHT,MAIN_COLOR_GDK);
			m_background.modify_bg(Gtk::STATE_PRELIGHT,MAIN_COLOR_GDK);
			m_background.modify_base(Gtk::STATE_PRELIGHT,MAIN_COLOR_GDK);

			m_background.modify_fg(Gtk::STATE_SELECTED,MAIN_COLOR_GDK);
			m_background.modify_bg(Gtk::STATE_SELECTED,MAIN_COLOR_GDK);
			m_background.modify_base(Gtk::STATE_SELECTED,MAIN_COLOR_GDK);

			m_background.modify_fg(Gtk::STATE_INSENSITIVE,MAIN_COLOR_GDK);
			m_background.modify_bg(Gtk::STATE_INSENSITIVE,MAIN_COLOR_GDK);
			m_background.modify_base(Gtk::STATE_INSENSITIVE,MAIN_COLOR_GDK);

			}


			
			MOD{
                        m_gallconvp.modify_fg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
			m_gallconvp.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
			m_gallconvp.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
                        
                        m_gallconvp.modify_fg(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);
			m_gallconvp.modify_bg(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);
			m_gallconvp.modify_base(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);

			m_gallconvp.modify_fg(Gtk::STATE_PRELIGHT,MAIN_COLOR_GDK);
			m_gallconvp.modify_bg(Gtk::STATE_PRELIGHT,MAIN_COLOR_GDK);
			m_gallconvp.modify_base(Gtk::STATE_PRELIGHT,MAIN_COLOR_GDK);

			m_gallconvp.modify_fg(Gtk::STATE_SELECTED,MAIN_COLOR_GDK);
			m_gallconvp.modify_bg(Gtk::STATE_SELECTED,MAIN_COLOR_GDK);
			m_gallconvp.modify_base(Gtk::STATE_SELECTED,MAIN_COLOR_GDK);

			m_gallconvp.modify_fg(Gtk::STATE_INSENSITIVE,MAIN_COLOR_GDK);
			m_gallconvp.modify_bg(Gtk::STATE_INSENSITIVE,MAIN_COLOR_GDK);
			m_gallconvp.modify_base(Gtk::STATE_INSENSITIVE,MAIN_COLOR_GDK);

			}


			



			m_gallcon.set_shadow_type(Gtk::SHADOW_NONE);
			m_gallcon.set_policy(Gtk::POLICY_NEVER,Gtk::POLICY_AUTOMATIC);

                        
			MOD{
			m_gallcon.modify_base(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);
			m_gallcon.modify_base(Gtk::STATE_ACTIVE, MAIN_COLOR_GDK);
			m_gallcon.modify_base(Gtk::STATE_SELECTED, MAIN_COLOR_GDK);
			m_gallcon.modify_base(Gtk::STATE_PRELIGHT, MAIN_COLOR_GDK);
			m_gallcon.modify_base(Gtk::STATE_INSENSITIVE, MAIN_COLOR_GDK);

			m_gallcon.modify_bg(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);
			m_gallcon.modify_bg(Gtk::STATE_ACTIVE, MAIN_COLOR_GDK);
			m_gallcon.modify_bg(Gtk::STATE_SELECTED, MAIN_COLOR_GDK);
			m_gallcon.modify_bg(Gtk::STATE_PRELIGHT, MAIN_COLOR_GDK);
			m_gallcon.modify_bg(Gtk::STATE_INSENSITIVE, MAIN_COLOR_GDK);


			m_gallcon.modify_fg(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);
			m_gallcon.modify_fg(Gtk::STATE_ACTIVE, MAIN_COLOR_GDK);
			m_gallcon.modify_fg(Gtk::STATE_SELECTED, MAIN_COLOR_GDK);
			m_gallcon.modify_fg(Gtk::STATE_PRELIGHT, MAIN_COLOR_GDK);
			m_gallcon.modify_fg(Gtk::STATE_INSENSITIVE, MAIN_COLOR_GDK);

                        m_gtab.modify_base(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);
			m_gtab.modify_bg(Gtk::STATE_NORMAL, MAIN_COLOR_GDK);

			m_gtab.modify_bg(Gtk::STATE_ACTIVE, MAIN_COLOR_GDK);
			m_gtab.modify_bg(Gtk::STATE_SELECTED, MAIN_COLOR_GDK);
			m_gtab.modify_bg(Gtk::STATE_PRELIGHT, MAIN_COLOR_GDK);
			m_gtab.modify_bg(Gtk::STATE_INSENSITIVE, MAIN_COLOR_GDK);
			}

                       
                        
                        if(!no_new_features || true)
			{if(!no_gallery)m_note.append_page(m_gallcon,*v_ga);}
                        else
                        {if(!no_gallery)m_note.append_page(m_gtab,*v_ga);}


			Gtk::Label *v_in = Gtk::manage(new Gtk::Label(_("Info")));

			MOD {
			v_in->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_in->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_in->modify_base(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			
			v_in->modify_fg(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_in->modify_text(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_in->modify_base(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK); }



			if(!no_info)m_note.append_page(m_info,*v_in);


			if(!no_new_features)
			{
			Gtk::Label *v_ani = Gtk::manage(new Gtk::Label(_("Animate")));

			MOD {
			v_ani->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_ani->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			v_ani->modify_base(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
			
			v_ani->modify_fg(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_ani->modify_text(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK);
			v_ani->modify_base(Gtk::STATE_ACTIVE,CONTRAST_COLOR_GDK); }



			m_note.append_page(m_ani,*v_ani);
			}
//			if(!(no_full && personalized)) m_info.set_size_request(300,600);
			

			m_gtab.attach(*Gtk::manage(new Gtk::Image),1,2,0,1);
			for(unsigned i = 0; i < gal.size(); i++)
			{
				


				Gtk::Frame* v_gframe = Gtk::manage(new Gtk::Frame(gal[i].name));

				// white background for gallery widgets
				Gtk::Widget* W = v_gframe->get_label_widget();
				MOD {
				W->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
				
				
				W->modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK); 
				W->modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK); 
				

				v_gframe->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
				v_gframe->modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);
                                v_gframe->modify_bg(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK); 
				v_gframe->modify_base(Gtk::STATE_ACTIVE,MAIN_COLOR_GDK);
                                }

				m_gtab.attach(*v_gframe,1,2,i+1,i+2,Gtk::EXPAND|Gtk::FILL,Gtk::SHRINK|Gtk::FILL);

				Gtk::EventBox* v_eb = Gtk::manage(new Gtk::EventBox);
				Gtk::Image* v_im = Gtk::manage(new Gtk::Image);
				v_im->set(gal[i].image);
				MOD {
				v_eb->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
				v_eb->modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); }
				
			
				v_eb->signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this,&SurfBWindow::on_gallery_press_event),i));
				v_gframe->add(*v_eb);
				v_im->set_size_request(100,100);
				v_gframe->set_size_request(120,119);
				v_eb->add(*v_im);
				

				v_eb->add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);
			}
			//m_gtab.attach(m_background,1,2,0,1+gal.size());

			//m_bft.attach(m_backfor,0,1,0,1,Gtk::FILL,Gtk::SHRINK);
			//m_bft.attach(m_fbox,0,1,1,2,Gtk::SHRINK,Gtk::SHRINK);
			
			
	}



	m_entry.grab_focus();
	
	show_all_children();
	if(MAX_SURFACES == 1) m_spin.hide();

	m_utab.attach(m_printing,0,2,3+1,4+1/*,Gtk::SHRINK,Gtk::SHRINK*/);	

	adjust_visibility();
	sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);

// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, MSECS);

	sigc::slot<bool> wait_slot = sigc::mem_fun(*this, &SurfBWindow::on_wait_elapsed);

// This is where we connect the slot to the Glib::signal_timeout()
	m_wait_conn = Glib::signal_timeout().connect(wait_slot, 60*1000);


}

void SurfBWindow::start()
{
		std::string image = TEMP_ROOT_SEP+"surfb.ppm";
		int n = num_threads();
		std::remove((image).c_str());
		for(int i = 2; i <= n; i++)
		{
			std::ostringstream o;
			o<<i;

			std::remove((image+o.str() ).c_str());
		}

	std::vector<parse_result> R;
	
	button_down = true;
	pichange=5;
	on_timer_event_func(0);
	button_down = false;

	
	if(!personalized) on_next_clicked();
	
}


bool SurfBWindow::on_expose_event_func(GdkEventExpose*)
{
	waiting = true;
	on_timer_event_func(0);
	//pichange = 5;
	return true;
	
}

void SurfBWindow::on_reset_clicked()
{

m_last_action = time(NULL);
m_waiting_mode = 0;


#ifndef WIN32
int result = system("killall -9 surf");
#endif

std::string t = opt.reset_file;
		try{

		std::ifstream f(t.c_str());
		
			parse_result P;
			
			P.global_data = global_defaults();
			P.data = read_pic(f,P.global_data);
		
		if(!P.data.empty())
		{

			data = P.data;
			global_data = P.global_data;

			data_index = 0;
			update_visuals();
			
		}


		}
		catch(...)
		{}

	
}

bool SurfBWindow::on_wait_elapsed()
{
	time_t d = time(NULL) - m_last_action;
	//std::cout<<"checking time "<<d<<" | "<<m_waiting_mode<<std::endl;





if(m_waiting_mode == 0 && d > 5 * 60 )
{


for(unsigned i = 0; i < gal.size(); i++)
if(gal[i].path.find(opt.screen_saver_gallery) != std::string::npos)
{
current_gal = i;

current_surf = 0;
	
	
	data =  gal[current_gal].file[current_surf].data;
	global_data =  gal[current_gal].file[current_surf].global_data;
update_visuals();
m_waiting_mode = 1;
}

}
else if(m_waiting_mode == 1 && d > 1 * 60 )
{
on_next_clicked();
m_waiting_mode = 1;
}


	return true;
}


color_double colormap(double bx, double by, double Z)
{
	color_double c;
	double x = bx/2;
	double y = COLORTABLE_SIZE/2.-by/2.;
	
		double l1 = x/(COLORTABLE_SIZE/2.);
		double l2 = 1 - l1;
		double u1 = y/(COLORTABLE_SIZE/2.);
		double u2 = 1 - u1;


		double d1 = 1-l1*l1-u1*u1;
		double d2 = 1-l1*l1-u2*u2;
		double d3 = 1-l2*l2-u1*u1;
		double d4 = 1-l2*l2-u2*u2;
		if(d1<0) d1 = 0;
		if(d2<0) d2 = 0;
		if(d3<0) d3 = 0;
		if(d4<0) d4 = 0;

		double fd1 = (d1>0.5)?1.5*(d1-0.5):0;
		double fd2 = (d2>0.5)?1.5*(d2-0.5):0;
		double fd3 = (d3>0.5)?1.5*(d3-0.5):0;

		double r = (d1*1 + d2*0 + fd3*1 + d4*Z);
		double g = (d1*0 + fd2*1 + d3*1 + d4*Z);
		double b = (fd1*0 + d2*1 + d3*0 + d4*Z);

		if(r>1)r=1;
		if(g>1)g=1;
		if(b>1)b=1;

		if(r<0)r=0;
		if(g<0)g=0;
		if(b<0)b=0;


		c.red = r;
		c.green = g;
		c.blue = b;


	return c;
}


bool SurfBWindow::on_color_button_press_event_func(GdkEventButton* event)
{
m_last_action = time(NULL);
m_waiting_mode = 0;

		data[data_index].outside = colormap(event->x,event->y).surf_colors();
		button_down = true;
		pichange = 5;
		
	return false;
}





bool SurfBWindow::on_button_press_event_func(GdkEventButton* event)
{
m_last_action = time(NULL);
m_waiting_mode = 0;

// This is where we draw on the window
	Glib::RefPtr<Gdk::Window> window = m_draw.get_window();
	if(window)
	{
	
// coordinates for the center of the window
		int xc, yc;
		xc = int(event->x);
		yc = int(event->y);
	
		gui_x = xc;
		gui_y = yc;
	
		button_down = true;
	
		current_x = 0;
		current_y = 0;
		current_z = 0;
		
		//if(event->button == 3) hide();
		
	}
	return false;
}



bool SurfBWindow::on_button_release_event_func(GdkEventButton* )
{
m_last_action = time(NULL);
m_waiting_mode = 0;

// This is where we draw on the window

	button_down = false;

	pichange = 3;

	return false;
}


bool SurfBWindow::on_color_button_release_event_func(GdkEventButton* event)
{
m_last_action = time(NULL);
m_waiting_mode = 0;


	button_down = false;

	double x, y;
	x = event->x;
	y = event->y;
	
	data[data_index].outside = colormap(x,y).surf_colors();

	pichange = 3;

	return false;
}


bool SurfBWindow::on_motion_notify_event_func(GdkEventMotion* event)
{
m_last_action = time(NULL);
m_waiting_mode = 0;

// This is where we draw on the window
	Glib::RefPtr<Gdk::Window> window = m_draw.get_window();
	if(window && button_down)
	{
		Gtk::Allocation allocation = m_draw.get_allocation();
		const int width = allocation.get_width();
		//const int height = allocation.get_height();

// coordinates for the center of the window
		int xc, yc;
		xc = int(event->x);
		yc = int(event->y);
	
		//double tx = xc - double(width)/2;
		//double ty = yc - double(width)/2;
		//double tgx = gui_x - double(width)/2;
		//double tgy = gui_y - double(width)/2;



		double fx = -(xc-gui_x)/double(width)*3.1415926;
		double fy = -(yc-gui_y)/double(width)*3.1415926;


		double c_y = 0.0;
		double c_x = 0.0;
		double c_z = 0.0;


		c_y = fx;
		c_x = fy;
		

		if(c_y != 0.0) global_data.rot = global_data.rot * roty(c_y);
		if(c_x != 0.0) global_data.rot = global_data.rot * rotx(c_x);
		if(c_z != 0.0) global_data.rot = global_data.rot * rotz(c_z);

		
		gui_x = xc;
		gui_y = yc;

		w_sfx.recapture();
		pichange=5;
	//dr->draw_line(some_gc,gui_x,gui_y,xc,yc);
	
	
		
	}
	return false;
}




bool SurfBWindow::on_color_motion_notify_event_func(GdkEventMotion* event)
{
m_last_action = time(NULL);
m_waiting_mode = 0;

	if(button_down)
	{
		double x, y;
		x = event->x;
		y = event->y;
		data[data_index].outside = colormap(x,y).surf_colors();
		pichange = 5;
		
	}
	return false;
}


void SurfBWindow::on_insert_text_func(const Glib::ustring&,int*)
{
m_last_action = time(NULL);
m_waiting_mode = 0;
	
	std::string t2 = fix_input(m_entry.get_text());
	std::string t3;
	for(unsigned x = 0; x < t2.size(); x++)
		switch(t2[x])
	{
		case 'x': t3+='u';break;
		case 'y': t3+='v';break;
		case 'z': t3+='w';break;
		default:  t3+=t2[x];break;
	}

	if(check_input(t2))
	{
	data[data_index].equation = t3;
	data[data_index].public_eq = t2;
	}
	else
	data[data_index].equation=1;

	//pichange = 5;
	adjust_visibility();
	refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);
	pichange=15;
}


void SurfBWindow::on_delete_text_func(int,int)
{
m_last_action = time(NULL);
m_waiting_mode = 0;
	
	std::string t2 = fix_input(m_entry.get_text());
	std::string t3;
	for(unsigned x = 0; x < t2.size(); x++)
		switch(t2[x])
	{
		case 'x': t3+='u';break;
		case 'y': t3+='v';break;
		case 'z': t3+='w';break;
		default:  t3+=t2[x];break;
	}

	if(check_input(t2))
	{
	data[data_index].equation = t3;
	data[data_index].public_eq = t2;
	}

	
	pichange = 15;
	adjust_visibility();
}


void SurfBWindow::on_value_changed_func()
{
m_last_action = time(NULL);
m_waiting_mode = 0;
	
	global_data.scale = exp(log(10.0)*m_vscale.get_value());
	
	
	
	pichange = 15;
}

void SurfBWindow::on_spin_changed_func()
{
	
	data_index = int(m_spin.get_value())-1;	
	update_visuals();
}



void SurfBWindow::on_para_changed_func()
{
m_last_action = time(NULL);
m_waiting_mode = 0;
	
	
	global_data.para[0] = m_hscale.get_value();
	pichange = 15;
}

void SurfBWindow::on_para2_changed_func()
{
m_last_action = time(NULL);
m_waiting_mode = 0;
	
	
	global_data.para[1] = m_hscale2.get_value();
	pichange = 15;
}

void SurfBWindow::on_para3_changed_func()
{
m_last_action = time(NULL);
m_waiting_mode = 0;

	global_data.para[2] = m_hscale3.get_value();
	pichange = 15;
}

void SurfBWindow::on_para4_changed_func()
{
m_last_action = time(NULL);
m_waiting_mode = 0;

	global_data.para[3] = m_hscale4.get_value();
	pichange = 15;
}


bool SurfBWindow::on_button_changed_func(GdkEventButton* )
{
	return true;
}




bool SurfBWindow::on_timer_event_func(int)
{
	if(printing)
	{
		refresh_print(TEMP_ROOT_SEP+"surfb_f_p.ppm");
		return true;
	}
	if(get_window())
	{
		if(!no_full && first_time_full)
		{
			fullscreen();
			first_time_full = false;
		}
                if(kill_w)
                {kill_w->hide(); delete kill_w; kill_w = NULL;}
	}
	
	Glib::RefPtr<Gdk::Window> window = m_draw.get_window();
	if( pichange>=5)
	{
		
		conn.disconnect();
		sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);

		refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);

// This is where we connect the slot to the Glib::signal_timeout()
		conn = Glib::signal_timeout().connect(my_slot, MSECS);

		if(pichange==5)
			pichange = 3;
		else pichange--;
		waiting = true;
		
	}
	else if( !button_down && pichange == 3)
	{
		
		conn.disconnect();
		
		sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);
	
		refresh(TEMP_ROOT_SEP +"surfb_f.pic",TEMP_ROOT_SEP +"surfb_f.ppm",0,true);
		conn = Glib::signal_timeout().connect(my_slot, MSECS);

		pichange = 0;
		waiting = true;
	}
	else if( !button_down && pichange == 0)
	{
		if(waiting)
		refresh_display(TEMP_ROOT_SEP +"surfb_f.ppm",true);
		
	}
	return true;
}


void SurfBWindow::refresh_image(const std::string& script, const std::string& image, const int aa, bool full, const int n , bool max_res, bool max_res2)
{

//std::cout<<"action at "<<m_last_action<<std::endl;



	max_res2 &= max_res;

	if(!valid) return;
	//if(fullscreen_mode)
	
		
	Gtk::Allocation allocation = m_draw.get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	if(width < 41)
	{
		return;
	}
	// coordinates for the center of the window
	
	
	
	
	int w = full ? global_data.hires : global_data.lores;
	
	if(w>width-40 && !max_res) w = width-40;
	if(w>height-40  && !max_res) w = height-40;
	

	for(int k=1; k<= n; k++)
	{
	std::ostringstream os;
	os<<k;
	std::string sn = os.str();
	if(k==1) sn="";
	std::ofstream f((script+sn).c_str(),FILE_WRITE_MODE);
	
	
	
	f<<"// This file is intended for use with surf (http://surf.sf.net/)"<<"\n";
	f<<"// or Surfer (http://surfer.imaginary2008.de/)"<<"\n";
	f<<"// This file was generated by Surfer."<<"\n\n"<<std::flush;

	if(!f.is_open() || f.fail() || f.bad())
	{
		std::cerr << "could not write to file "<< (script+sn)<<std::endl;
		exit(1437);
	}

	f<<"width="<< w <<";\n";
	f<<"height="<< w <<";\n";
	
	

	write_surf(global_data,f,aa);
for(unsigned k = 0; k < data.size(); k++)
{
	write_surf(data[k],k,f);

}

	
	
	

	//f<<"transparency="<<50<<";\n";
	f<<"filename=\""<<image<<sn<<"\";\n";
	f<<"color_file_format="<<opt.format<<";\n";
	f<<"draw_surface;\n";
	f<<"save_color_image;\n";
	f.close();
	}

	
    			
    //} else {
        
        //gtk_widget_set_sensitive(this->window, true);
    //}
		
	if(max_res2)
	{
		log_system((opt.surf_cmd+" -n \""+script+"\" " +REDIRECTION_APEX).c_str());
	}
	else
	if(full)
	{
		
		//my_kill(kill_list);
		#ifndef WIN32
		//system(("killall surf" + std::string(REDIRECTION_APEX)).c_str());
		#endif
		
		std::remove((image).c_str());
		for(int i = 2; i <= n; i++)
		{
			std::ostringstream o;
			o<<i;

			std::remove((image+o.str() ).c_str());
		}
		

		//kill_list = system_async(script,"25",opt);
		parallel_surf(script,true,w,opt);

		Gdk::Cursor hglass(Gdk::WATCH);
    		Glib::RefPtr<Gdk::Window> wi = get_window();
    //if( strcmp(name, "hourglass") == 0 ) {
		if(wi)
		wi->set_cursor(hglass);
        //gtk_widget_set_sensitive(this->window, false);

		
	}
	else
	{
		parallel_surf(script,false,w,opt);
		
		//my_system(script," " REDIRECTION_APEX ,"5",opt);
		//if(w)gdk_window_set_cursor(w, NULL);
	}
	

}

void SurfBWindow::refresh_display(const std::string& image, bool )
{

	
	Glib::RefPtr<Gdk::Window> window = m_draw.get_window();
	if(window)
	{
		Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
	//some_gc.create(get_window());
		
		Gtk::Allocation allocation = m_draw.get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();
	bool finished = true;
	
		Glib::RefPtr<Gdk::Drawable> dr(window);
		
			//if(full)
			{

const int n = num_threads();



for(int i = 0; i < n; i++)
{
std::ostringstream o;
if(i)o<<(i+1);

try{
Glib::RefPtr<Gdk::Pixbuf> surface2 = Gdk::Pixbuf::create_from_file((image+o.str()).c_str());
Glib::RefPtr<Gdk::Pixbuf> sface2 = surface2->scale_simple(width-40,height-40,Gdk::INTERP_BILINEAR);

dr->draw_pixbuf(some_gc,sface2,0,i*(height-40)/n,20,20+i*(height-40)/n,width-40,(height-40)/n+((i==n-1)?0:1),Gdk::RGB_DITHER_NONE,0,0);
}
catch(...){finished=false;}



}

				
			Gdk::Cursor hptr(Gdk::LEFT_PTR);
			Glib::RefPtr<Gdk::Window> w = get_window();
    //if( strcmp(name, "hourglass") == 0 ) {
        		if(w&&finished)w->set_cursor(hptr);
			if(finished) waiting = false;	
			
			
		}
	
		
	}


}

void SurfBWindow::refresh(const std::string& script, const std::string& image, const int aa, bool full)
{
	
	refresh_image(script,image,aa,full);
	refresh_display(image,full);
	

	

	

}





bool SurfBWindow::on_inside_expose_event_func(GdkEventExpose*)
{
	

	Glib::RefPtr<Gdk::Window> window = m_colors_inside.get_window();
	if(window)
	{
	Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
	Glib::RefPtr<Gdk::Drawable> dr(window);

	m_inside_image->render_to_drawable(dr,some_gc,0,0,0,0,COLORTABLE_SIZE,COLORTABLE_SIZE,Gdk::RGB_DITHER_NONE,0,0);
	}
	return true;
}





bool SurfBWindow::on_inside_button_press_event_func(GdkEventButton* event)
{
	
// This is where we draw on the window
	Glib::RefPtr<Gdk::Window> window = m_colors_inside.get_window();
	if(window)
	{
		Gtk::Allocation allocation = m_colors.get_allocation();

// coordinates for the center of the window
		double x, y;
		x = event->x;
		y = event->y;
	
	//std::c
		data[data_index].inside = colormap(x,COLORTABLE_SIZE-y).surf_colors();
		
		button_down = true;
		pichange = 5;
		
	}
	return false;
}


bool SurfBWindow::on_inside_button_release_event_func(GdkEventButton* event)
{

// This is where we draw on the window

	button_down = false;

	double x, y;
	x = event->x;
	y = event->y;
	
	data[data_index].inside = colormap(x,COLORTABLE_SIZE-y).surf_colors();
	

	pichange = 3;

	return false;
}



bool SurfBWindow::on_inside_motion_notify_event_func(GdkEventMotion* event)
{

// This is where we draw on the window

	if(button_down)
	{
		Gtk::Allocation allocation = m_draw.get_allocation();

// coordinates for the center of the window
		double x, y;
		x = event->x;
		y = event->y;



	data[data_index].inside = colormap(x,COLORTABLE_SIZE-y).surf_colors();
		




	
		pichange = 5;
	//dr->draw_line(some_gc,gui_x,gui_y,xc,yc);
	
	
		
	}
	return false;
}









bool SurfBWindow::on_back_expose_event_func(GdkEventExpose*)
{
	Glib::RefPtr<Gdk::Window> window = m_colors_back.get_window();
	Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
	Glib::RefPtr<Gdk::Drawable> dr(window);
	for(double x= 0; x <= 100; x = x+1)
		for(double y= 0; y <= 100; y = y+1)
	{
		Gdk::Color nc = colormap(x,y).gdk_colors();
		

		some_gc->set_rgb_fg_color(nc);
		dr->draw_point(some_gc,int(x),int(y));		
		
	}

	return true;
}





bool SurfBWindow::on_back_button_press_event_func(GdkEventButton* event)
{

	double x, y;
	x = event->x;
	y = event->y;
	global_data.background = colormap(x,y).surf_colors();
	
	button_down = true;
	pichange = 5;
	return false;
}


bool SurfBWindow::on_back_button_release_event_func(GdkEventButton* event)
{
button_down = false;
	double x, y;
	x = event->x;
	y = event->y;	
	global_data.background = colormap(x,y).surf_colors();
	pichange = 3;
	return false;
}



bool SurfBWindow::on_back_motion_notify_event_func(GdkEventMotion* event)
{
	if(button_down)
	{
		Gtk::Allocation allocation = m_draw.get_allocation();
		
		double x, y;
		x = event->x;
		y = event->y;
		global_data.background = colormap(x,y).surf_colors();
		pichange = 5;
	}
	return false;
}








void SurfBWindow::on_letter_clicked_func(char s)
{
	GdkEventKey K;
	K.type = GDK_KEY_PRESS;
	K.window = get_window()->gobj();
	K.send_event = 1;
	K.state = 0;
	K.keyval = GDK_A + s - 'a';
	if(s=='D')
	K.keyval = GDK_BackSpace;
	char t[2]={0,0};
	t[0]=s;
	if(t[0]=='D')
	{
		int i = m_entry.get_position();
		int a = 0;
		int b = 0;
		bool f = m_entry.get_selection_bounds(a,b);
		m_entry.grab_focus();
		if(f)
		{
			m_entry.select_region(a,b);
			m_entry.delete_selection();
			
			m_entry.set_position(a);
		}
		else
		{
		m_entry.delete_text(i-1,i);
		m_entry.set_position(i-1);
		}
	}
	else
	{
		int i = m_entry.get_position();
		m_entry.grab_focus();
		m_entry.insert_text(t,1,i);
		m_entry.set_position(i);
	}
	adjust_visibility();
	
	

	//m_entry.set_position(m_entry.get_position());
	return ;
}


void SurfBWindow::on_fullscreen_clicked()
{
	std::istringstream nothing("");
	SurfBWindow f(nothing,gal,opt,true);
	f.set_modal();
	
	
  f.opt = opt;
  
  f.start();
  f.data = data;
  f.global_data = global_data;

  f.m_entry.set_text(data[data_index].public_eq);
  
  //Shows the window and returns when it is closed.
  //hide();
  f.show();
//  f.fullscreen();
  f.update_visuals();
  
conn.disconnect();
  Gtk::Main::run(f);
	data = f.data;
       
  global_data = f.global_data;
  	update_visuals();

sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);

// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, MSECS);
	refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);
}

void generate_image()
{

}

void SurfBWindow::on_print_clicked()
{
/* HACK: remove stupid name button
Gtk::Dialog PD(_("Print image"),true);

PD.add_button(Gtk::Stock::OK,Gtk::RESPONSE_OK);
PD.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);

Gtk::Label label(_("Name:"));
PD.get_vbox()->add(label);
PD.get_vbox()->add(entry);
PD.show_all_children();
PD.set_default_response(Gtk::RESPONSE_OK);

int result = PD.run();
if(result != Gtk::RESPONSE_OK) return;
*/
Gtk::Entry entry;

//Bild berechnen

std::string image = TEMP_ROOT_SEP+"surfb_f_p.ppm";
		int n = num_threads();
		std::remove((image).c_str());
		for(int i = 2; i <= n; i++)
		{
			std::ostringstream o;
			o<<i;

			std::remove((image+o.str() ).c_str());
		}

std::remove((TEMP_ROOT_SEP+"surfb_p.png").c_str());
std::remove((TEMP_ROOT_SEP+"surfb_n.txt").c_str());

if(!entry.get_text().empty())
{
std::ofstream u((TEMP_ROOT_SEP+"surfb_n.txt").c_str());
u << entry.get_text()<<std::endl;
u.close();
}

int res = global_data.hires;
global_data.hires = opt.print_resolution;
refresh_image(TEMP_ROOT_SEP +"surfb_f_p.pic",TEMP_ROOT_SEP +"surfb_f_p.ppm",global_data.antialiasing,true,num_threads(),true,false);

global_data.hires = res;

printing = true;
adjust_printing();
refresh_print(TEMP_ROOT_SEP +"surfb_f_p.ppm");
Gtk::MessageDialog print_ack( *this, _("PrintAcknowledge"), false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true );
print_ack.set_decorated(false);
print_ack.run();
}

void SurfBWindow::on_save_clicked()
{
//FIXME : das geht nur singlecore
	std::string s1 = fix_file(opt.save_cmd);
	while(s1.find("#")!=std::string::npos)
	{
		int i = s1.find("#");
		s1.replace(i,1, TEMP_ROOT_SEP +"surfb_f.ppm");
	}
	log_system((""+s1).c_str());
}



void SurfBWindow::on_noscreen_clicked()
{
hide();
}

void SurfBWindow::on_about_clicked()
{

//AniWindow W(*this);

AboutWindow W(opt,*this);

W.show();
W.set_modal();
W.present();
//W.set_decorated(false);
//W.raise();
//present();
//present();
//raise();
Gtk::Main::run(W);
W.hide();
}


void SurfBWindow::on_left_clicked()
{
int i = m_entry.get_position()-1;
m_entry.grab_focus();
m_entry.set_position(i);
}

void SurfBWindow::on_right_clicked()
{
int i = m_entry.get_position()+1;
m_entry.grab_focus();
m_entry.set_position(i);



}


void SurfBWindow::on_next_clicked()
{
	if(gal.empty()) return;
	if(gal[current_gal].file.empty()) return;

	current_surf = (current_surf+1 )%gal[current_gal].file.size();


	data[data_index] =  gal[current_gal].file[current_surf].data[0];
	global_data =  gal[current_gal].file[current_surf].global_data;

	update_visuals();
}

void SurfBWindow::on_prev_clicked()
{
	if(gal.empty()) return;
	if(gal[current_gal].file.empty()) return;
	
	current_surf = (current_surf+gal[current_gal].file.size()-1 )%gal[current_gal].file.size();
	
	
	data[data_index] =  gal[current_gal].file[current_surf].data[0];
	global_data =  gal[current_gal].file[current_surf].global_data;

	update_visuals();

}


void SurfBWindow::update_visuals(bool f)
{
	
	//scale = exp(log10*v)
        //log(scale) = log10*v
	pichange=5;
	m_vscale.set_value(log(global_data.scale)/log(10.0));

	m_entry.set_text(data[data_index].public_eq);
	m_hscale.set_value(global_data.para[0]);
	m_hscale2.set_value(global_data.para[1]);
	m_hscale3.set_value(global_data.para[2]);
	m_hscale4.set_value(global_data.para[3]);
	adjust_visibility();
	if(f)
	try{
	std::ifstream fi(data[data_index].desc.c_str());
	if(fi.is_open())
	{
		m_info.set(data[data_index].desc);
		m_note.set_current_page(2);
	}
	else
	m_info.clear();}
	catch(...){m_info.clear();}
	w_sfx.recapture();
}


bool SurfBWindow::on_gallery_press_event(GdkEventButton*,int i)
{
	if(gal[i].file.empty()) return true;
	current_gal = i;
	
	GalleryWindow w(gal[i],opt);
	w.set_modal();
	w.show();
	w.maximize();
	w.fullscreen();
	w.present();
	w.raise();
	//w.fullscreen();
	Gtk::Main::run(w);

	data = w.ret.data;

	global_data = w.ret.global_data;;

	current_surf = w.isu;

	update_visuals();

	return true;
}

void SurfBWindow::adjust_visibility()
{
	bool fa = false;
	bool fb = false;
	bool fc = false;
	bool fd = false;

	for(unsigned i = 0; i < data.size(); i++)
	{
	fa = fa || (data[i].public_eq.find('a')!=std::string::npos);
	
	fb = fb || (data[i].public_eq.find('b')!=std::string::npos);

	std::string dc = data[i].public_eq;

	while(dc.find("cos")!=std::string::npos)
	{dc.replace(dc.find("cos"),3,"XOS");}

	while(dc.find("arc")!=std::string::npos)
	{dc.replace(dc.find("arc"),3,"ARX");}

	
	fc = fc || (dc.find('c')!=std::string::npos);
	
	fd = fd || (data[i].public_eq.find('d')!=std::string::npos);
	}

	m_hscale.show();
	m_scale_free.set_size_request(1,m_hscale.get_allocation().get_height());
	

	if(fa) m_hscale.show(); else m_hscale.hide();
	if(fb) m_hscale2.show(); else m_hscale2.hide();
	if(fc) m_hscale3.show(); else m_hscale3.hide();
	if(fd) m_hscale4.show(); else m_hscale4.hide();


	valid = check_input(fix_input(data[data_index].public_eq));
	if(valid)
	{
	std::remove(((TEMP_ROOT_SEP+"surfb_x.ppm").c_str()));
	check_image(TEMP_ROOT_SEP+"surfb_x.pic",TEMP_ROOT_SEP+"surfb_x.ppm");
	std::ifstream f((TEMP_ROOT_SEP+"surfb_x.ppm").c_str());
	valid=f.is_open();
	}

	if(valid) {
		m_error.hide();
		//if(data.size()>1)
		//m_spin.show();

		//if(MAX_SURFACES>1&&data.size()<MAX_SURFACES)
		//m_new_surface.show();
	}
	else
		{
			 m_error.show();
			//m_spin.hide();
			//m_new_surface.hide();
		}

   std::remove(((TEMP_ROOT_SEP+"surfb_x.ppm").c_str()));
   std::remove(((TEMP_ROOT_SEP+"surfb_x.pic").c_str()));
}




void SurfBWindow::check_image(const std::string& script, const std::string& image)
{
	
		
	
	
	std::ofstream f(script.c_str(),FILE_WRITE_MODE);
	
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
	

	

	f<<"double a = "<<global_data.para[0]<<";\n";
	f<<"double b = "<<global_data.para[1]<<";\n";
	f<<"double c = "<<global_data.para[2]<<";\n";
	f<<"double d = "<<global_data.para[3]<<";\n";
	
	f<<"poly u="<<global_data.rot.el(1,1)<<"*x+("<<global_data.rot.el(1,2)<<")*y+("<<global_data.rot.el(1,3)<<")*z;\n";
	f<<"poly v="<<global_data.rot.el(2,1)<<"*x+("<<global_data.rot.el(2,2)<<")*y+("<<global_data.rot.el(2,3)<<")*z;\n";
	f<<"poly w="<<global_data.rot.el(3,1)<<"*x+("<<global_data.rot.el(3,2)<<")*y+("<<global_data.rot.el(3,3)<<")*z;\n";

	f<<global_data.general_stuff;

	

	
	//f<<"surface="<<data.equation<<";\n";
	f<<"surface="<< data[data_index].equation <<";\n";

	

	//f<<"transparency="<<50<<";\n";
	f<<"filename=\""<<image<<"\";\n";
	f<<"color_file_format="<<opt.format<<";\n";
	f<<"draw_surface;\n";
	f<<"save_color_image;\n";
	f.close();
	

	
    			
		


	
	std::string cmd = opt.surf_cmd+" "+QUIET_SURF+" -n \""+script+"\" " +REDIRECTION_APEX;
	
	log_system(cmd.c_str());
		//if(w)gdk_window_set_cursor(w, NULL);
	
}

bool SurfBWindow::on_key_press_event_func(GdkEventKey* event)
{
	if((event && event->state == Gdk::CONTROL_MASK && event->keyval ==GDK_p))
	{
		on_print_clicked();
	}
	if((event && event->keyval ==GDK_Escape))
	{
		printing = false;
	adjust_printing();
	}
	return true;
}

bool SurfBWindow::on_scroll_event_func(GdkEventScroll* event)
{
	if(event)
	{
		if(event->direction == GDK_SCROLL_UP)
		m_vscale.set_value(m_vscale.get_value()-m_vscale.get_adjustment()->get_step_increment());

		if(event->direction == GDK_SCROLL_DOWN)
		m_vscale.set_value(m_vscale.get_value()+m_vscale.get_adjustment()->get_step_increment());

		if(m_hscale.is_visible())
		{

			if(event->direction == GDK_SCROLL_RIGHT)
			m_hscale.set_value(m_hscale.get_value()+m_hscale.get_adjustment()->get_step_increment());
	
			if(event->direction == GDK_SCROLL_LEFT)
			m_hscale.set_value(m_hscale.get_value()-m_hscale.get_adjustment()->get_step_increment());
		}
		else if(m_hscale2.is_visible())
		{

			if(event->direction == GDK_SCROLL_RIGHT)
			m_hscale2.set_value(m_hscale2.get_value()+m_hscale2.get_adjustment()->get_step_increment());
	
			if(event->direction == GDK_SCROLL_LEFT)
			m_hscale2.set_value(m_hscale2.get_value()-m_hscale2.get_adjustment()->get_step_increment());
		}
		else if(m_hscale3.is_visible())
		{

			if(event->direction == GDK_SCROLL_RIGHT)
			m_hscale3.set_value(m_hscale3.get_value()+m_hscale3.get_adjustment()->get_step_increment());
	
			if(event->direction == GDK_SCROLL_LEFT)
			m_hscale3.set_value(m_hscale3.get_value()-m_hscale3.get_adjustment()->get_step_increment());
		}
		else if(m_hscale4.is_visible())
		{

			if(event->direction == GDK_SCROLL_RIGHT)
			m_hscale4.set_value(m_hscale4.get_value()+m_hscale4.get_adjustment()->get_step_increment());
	
			if(event->direction == GDK_SCROLL_LEFT)
			m_hscale4.set_value(m_hscale4.get_value()-m_hscale4.get_adjustment()->get_step_increment());
		}
		
	}
	return true;
}


void SurfBWindow::refresh_print(const std::string& image)
{

	
	//Glib::RefPtr<Gdk::Window> window = m_draw.get_window();
	//if(window)
	//{
		//Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
	//some_gc.create(get_window());
		
		//Gtk::Allocation allocation = m_draw.get_allocation();
		//const int width = allocation.get_width();
		//const int height = allocation.get_height();
	bool finished = true;
	
		//Glib::RefPtr<Gdk::Drawable> dr(window);
		
			//if(full)
			//{

const int n = num_threads();



for(int i = 0; i < n; i++)
{
std::ostringstream o;
if(i)o<<(i+1);



try{
Glib::RefPtr<Gdk::Pixbuf> surface2 = Gdk::Pixbuf::create_from_file((image+o.str()).c_str());
//Glib::RefPtr<Gdk::Pixbuf> sface2 = surface2->scale_simple(width-40,height-40,Gdk::INTERP_BILINEAR);

//dr->draw_pixbuf(some_gc,sface2,0,i*(height-40)/n,20,20+i*(height-40)/n,width-40,(height-40)/n+((i==n-1)?0:1),Gdk::RGB_DITHER_NONE,0,0);
}
catch(...){finished=false;}



}

if(finished)
{

Glib::RefPtr<Gdk::Pixbuf> bs = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,opt.print_resolution,opt.print_resolution);

for(int i = 0; i < n; i++)
{
std::ostringstream o;
if(i)o<<(i+1);



Glib::RefPtr<Gdk::Pixbuf> surface2 = Gdk::Pixbuf::create_from_file((image+o.str()).c_str());
//Glib::RefPtr<Gdk::Pixbuf> sface2 = surface2->scale_simple(width-40,height-40,Gdk::INTERP_BILINEAR);

//bs->draw_pixbuf(some_gc,sface2,0,i*(height)/n,0,0+i*(height)/n,width,(height)/n+((i==n-1)?0:1),Gdk::RGB_DITHER_NONE,0,0);

surface2->copy_area(
                    0,
                    i*(opt.print_resolution)/n,
                    opt.print_resolution,
                    (opt.print_resolution)/n+((i==n-1)?0:1),
                    bs,
                    0,
                    i*(opt.print_resolution)/n
                    );

}
bs->save(TEMP_ROOT_SEP+"surfb_p.png","png");


std::ofstream f((TEMP_ROOT_SEP+"surfb_p_f.tex").c_str());
f<<"0 = "<<data[data_index].public_eq<<std::endl;
f.close();

if(!opt.print_cmd.empty())
log_system((opt.print_cmd+
            " \""+TEMP_ROOT_SEP+"surfb_p.png\""+
            " \""+TEMP_ROOT_SEP+"surfb_p_f.tex\""+
            " \""+TEMP_ROOT_SEP+"surfb_n.txt\""+
            " \""+TEMP_ROOT_SEP+"surfb_f_p.pic\"").c_str());



printing = false;
adjust_printing();
}

			
			
		
	
		
	
}

void SurfBWindow::adjust_printing()
{

if(printing)
{
m_printing.show();
m_bbox.hide();

		Gdk::Cursor hglass(Gdk::WATCH);
    		Glib::RefPtr<Gdk::Window> wi = get_window();
    //if( strcmp(name, "hourglass") == 0 ) {
		if(wi)
		wi->set_cursor(hglass);
}
else
{
m_printing.hide();
m_bbox.show();

			Gdk::Cursor hptr(Gdk::LEFT_PTR);
			Glib::RefPtr<Gdk::Window> w = get_window();
    //if( strcmp(name, "hourglass") == 0 ) {
        		if(w)w->set_cursor(hptr);
}

}

void special_show(surface_data& p,global_parse& g,int& pi);

void SurfBWindow::on_special_clicked()
{


//Gtk::Main::run(se);
w_sfx.recapture();
if(!w_sfx.is_visible())
w_sfx.show();


}


void SurfBWindow::on_new_surface_clicked()
{


	if(data.size() < MAX_SURFACES)
	{
		std::istringstream i("");
		data.push_back(surfer_local());
		

		data_index = data.size() -1;

		m_spin.set_range(1,data.size());
		m_spin.set_value(data.size());

		update_visuals();
	}

}




void SurfBWindow::on_save_file_clicked()
{
	do_file_save(true,true);
}


void SurfBWindow::on_export_png_clicked()
{
	do_file_save(false,true);
}


void SurfBWindow::on_export_pic_clicked()
{
	do_file_save(true,false);
}

void SurfBWindow::do_file_save(bool do_pic,bool do_png)
{
	std::ostringstream title;

	if(do_pic && !do_png)
	title << _("Save surface");
	else
	if(do_pic&&do_png)
	title<<_("Save image and save surface");
	else
	title << _("Save image");

	
	

	Gtk::FileChooserDialog dialog(*this,title.str(),Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	Gtk::FileFilter filter_pic;
	  filter_pic.set_name(_("Surfer Script (.pic)"));
	  //filter_pic.add_mime_type("text/plain");
	  filter_pic.add_pattern("*.pic");

	Gtk::FileFilter filter_png;
	  filter_png.set_name(_("Portable Network Graphics (.png)"));
	  filter_png.add_mime_type("image/png");
	  dialog.add_filter(filter_png);
	

	if(do_png)
	{
	
	}
	if(do_pic){
	  dialog.add_filter(filter_pic);}


		/*
	  Gtk::FileFilter filter_ppm;
	  filter_ppm.set_name("Portable Pixmap-Bilder (.ppm)");
	  filter_ppm.add_mime_type("image/x-portable-bitmap");
	  filter_ppm.add_mime_type("image/x-portable-pixmap");
	  dialog.add_filter(filter_ppm);
	*/

	  Gtk::FileFilter filter_any;
	  filter_any.set_name(_("All files"));
	  filter_any.add_pattern("*");
	  dialog.add_filter(filter_any);
	  dialog.set_current_name(global_data.name+".png");

       dialog.show();
       dialog.present();
       dialog.raise();

	//Gtk::Main::run(dialog);

	int result = Gtk::RESPONSE_CANCEL;
        result = dialog.run();



	if(result == Gtk::RESPONSE_OK)
	{
		//std::string of = opt.format;
		//opt.format = "jpg";
		if(!no_log)
		{
		std::cout<<"file save: "<<dialog.get_filename()<<std::endl;
		std::cout<<dialog.get_filter()->get_name()<<std::endl;	
		}

		do_png = dialog.get_filter()->get_name() == filter_png.get_name();
		do_pic = dialog.get_filter()->get_name() == filter_pic.get_name();

		if(dialog.get_filter()->get_name() == filter_any.get_name())
		{do_pic = do_png = true; }

		std::string t = dialog.get_filename();

		if(t.find("\"")==std::string::npos && t.size()>=4 && t.substr(t.size()-4)==".pic")
		t = t.substr(0,t.size()-4);
		if(t.find("\"")==std::string::npos && t.find(".")==std::string::npos)
		t = t+".png";
		
		if(do_png)
		{
		try{
		int d = global_data.hires;
		global_data.hires = global_data.saveres;
		refresh_image(TEMP_ROOT_SEP+"surfb_s.pic",TEMP_ROOT_SEP+"surfb_s.ppm",global_data.antialiasing,true,1,true);

		global_data.hires = d;
		Glib::RefPtr<Gdk::Pixbuf> img = Gdk::Pixbuf::create_from_file(TEMP_ROOT_SEP+"surfb_s.ppm");


		img->save(t,"png");
		
		}
		catch(...)
		{}
		}

		if(do_pic)
		{
			try{

			std::string t2;

			if(t.size()>4 && t.substr(t.size()-4)==".png")
			t2 = t.substr(0,t.size()-4)+".pic";
			else
			t2 = t+".pic";


			int d = global_data.hires;
			global_data.hires = 1;
			refresh_image(TEMP_ROOT_SEP+"surfb_s.pic",TEMP_ROOT_SEP+"surfb_s.ppm",global_data.antialiasing,true,1,true);
	
			global_data.hires = d;
		



			std::ifstream inf((TEMP_ROOT_SEP+"surfb_s.pic").c_str());
			std::ofstream onf(t2.c_str());
			onf<<inf.rdbuf();
			onf.close();
			inf.close();
			}
			catch(...)
			{}
		}
		
		//opt.format = of;
	}
}

void SurfBWindow::on_open_file_clicked()
{

	std::ostringstream title;

	
	title << _("Open surface");
	
	
	

	Gtk::FileChooserDialog dialog(*this,title.str(),Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	Gtk::FileFilter filter_pic;
	  filter_pic.set_name(_("Surfer Script (.pic)"));
	  filter_pic.add_pattern("*.pic");
	  dialog.add_filter(filter_pic);


		/*
	  Gtk::FileFilter filter_ppm;
	  filter_ppm.set_name("Portable Pixmap-Bilder (.ppm)");
	  filter_ppm.add_mime_type("image/x-portable-bitmap");
	  filter_ppm.add_mime_type("image/x-portable-pixmap");
	  dialog.add_filter(filter_ppm);
	*/

	  Gtk::FileFilter filter_any;
	  filter_any.set_name(_("All files"));
	  filter_any.add_pattern("*");
	  dialog.add_filter(filter_any);
	  

       dialog.show();
       dialog.present();
       dialog.raise();

	//Gtk::Main::run(dialog);

	int result = Gtk::RESPONSE_CANCEL;
        result = dialog.run();



	if(result == Gtk::RESPONSE_OK)
	{
		//std::string of = opt.format;
		//opt.format = "jpg";
		
		std::string t = dialog.get_filename();

		try{

		std::ifstream f(t.c_str());
		
			parse_result P;
			
			P.global_data = global_defaults();
			P.data = read_pic(f,P.global_data);
		
		if(!P.data.empty())
		{

			data = P.data;
			global_data = P.global_data;

			data_index = 0;
			update_visuals();
			
		}


		}
		catch(...)
		{}
		
		//opt.format = of;
	}

}

void SurfBWindow::on_manual_clicked()
{
show_the_manual(this,opt);
}

void SurfBWindow::on_homepage_clicked()
{

}

void SurfBWindow::on_delete_surface_clicked()
{

	
	if(data.size() > 1)
	{
		data.erase(data.begin()+data_index);

		if(data_index) data_index--;
		
		m_spin.set_range(1,data.size());
		m_spin.set_value(data_index+1);

		update_visuals();
	}


}

void SurfBWindow::on_animate_click()
{
	//w_ani.ani_add();
	//if(!w_ani.is_visible())
	{
		//Gtk::Main::run(w_ani);
		//w_ani.show();
	}
	
}

void SurfBWindow::set_up_the_menu()
{

std::string amp = "&";

 mr_AG = Gtk::ActionGroup::create();

mr_AG->add(Gtk::Action::create("MenuFile",_("File")));
mr_AG->add(Gtk::Action::create("MenuSurface",_("Surface")));
mr_AG->add(Gtk::Action::create("MenuHelp",_("Help")));
mr_AG->add(Gtk::Action::create("MenuView",_("View")));
mr_AG->add(Gtk::Action::create("MenuGallery",_("Gallery")));

mr_AG->add(Gtk::Action::create("Animate",Gtk::Stock::MEDIA_RECORD), sigc::mem_fun(*this, &SurfBWindow::on_animate_click));

mr_AG->add(Gtk::Action::create("FileOpen",Gtk::Stock::OPEN), sigc::mem_fun(*this, &SurfBWindow::on_open_file_clicked));
mr_AG->add(Gtk::Action::create("FileSave",Gtk::Stock::SAVE), sigc::mem_fun(*this, &SurfBWindow::on_save_file_clicked));

mr_AG->add(Gtk::Action::create("FileExportPng",_("Save a copy of this image")), sigc::mem_fun(*this, &SurfBWindow::on_export_png_clicked));
mr_AG->add(Gtk::Action::create("FileExportPic",_("Save a copy of this surface")), sigc::mem_fun(*this, &SurfBWindow::on_export_pic_clicked));


mr_AG->add(Gtk::Action::create("FileQuit",Gtk::Stock::QUIT), sigc::mem_fun(*this, &SurfBWindow::on_noscreen_clicked) );


mr_AG->add(Gtk::Action::create("SurfaceNew",Gtk::Stock::NEW),sigc::mem_fun(*this, &SurfBWindow::on_new_surface_clicked));
//FIXME: Gtk::Stock::DELETE does not work under windows (with Gtkmm 2.10)
mr_AG->add(Gtk::Action::create("SurfaceDelete",Gtk::Stock::CLOSE),sigc::mem_fun(*this, &SurfBWindow::on_delete_surface_clicked));
mr_AG->add(Gtk::Action::create("SurfaceEdit",Gtk::Stock::PROPERTIES), sigc::mem_fun(*this, &SurfBWindow::on_special_clicked));

mr_AG->add(Gtk::Action::create("ViewFullscreen",Gtk::Stock::FULLSCREEN), sigc::mem_fun(*this, &SurfBWindow::on_fullscreen_clicked));

mr_AG->add(Gtk::Action::create("GalleryPrevious",Gtk::Stock::GO_BACK), sigc::mem_fun(*this, &SurfBWindow::on_prev_clicked));
mr_AG->add(Gtk::Action::create("GalleryNext",Gtk::Stock::GO_FORWARD), sigc::mem_fun(*this, &SurfBWindow::on_next_clicked));

mr_AG->add(Gtk::Action::create("HelpManual",Gtk::Stock::HELP), sigc::mem_fun(*this, &SurfBWindow::on_manual_clicked));
mr_AG->add(Gtk::Action::create("HelpHomepage",Gtk::Stock::HOME), sigc::mem_fun(*this,&SurfBWindow::on_homepage_clicked));
mr_AG->add(Gtk::Action::create("HelpAbout",Gtk::Stock::HELP), sigc::mem_fun(*this, &SurfBWindow::on_about_clicked));
mr_AG->add(Gtk::Action::create("FileReset",Gtk::Stock::STOP), sigc::mem_fun(*this, &SurfBWindow::on_reset_clicked));


Glib::RefPtr<Gtk::IconTheme> git = Gtk::IconTheme::get_default();
Glib::RefPtr<Gdk::Pixbuf> flag_german = Gdk::Pixbuf::create_from_xpm_data(flag_de_DE);
unsigned icon_size = flag_german->get_height();
git->add_builtin_icon("flag_de",icon_size,flag_german);

Glib::RefPtr<Gdk::Pixbuf> flag_english = Gdk::Pixbuf::create_from_xpm_data(flag_en_UK);
icon_size = flag_english->get_height();
git->add_builtin_icon("flag_en",icon_size,flag_english);

Glib::RefPtr<Gdk::Pixbuf> flag_spanish = Gdk::Pixbuf::create_from_xpm_data(flag_es_ES);
icon_size = flag_spanish->get_height();
git->add_builtin_icon("flag_es",icon_size,flag_spanish);

Glib::RefPtr<Gdk::Pixbuf> flag_catalan = Gdk::Pixbuf::create_from_xpm_data(flag_ca_ES);
icon_size = flag_catalan->get_height();
git->add_builtin_icon("flag_ca",icon_size,flag_catalan);


mr_AG->add(Gtk::Action::create_with_icon_name("LangDE","flag_de","Deutsch","Deutsch"), sigc::bind(sigc::mem_fun(*this, &SurfBWindow::restart_with_lang),"de_DE.UTF-8"));
mr_AG->add(Gtk::Action::create_with_icon_name("LangEN","flag_en","English","English"), sigc::bind(sigc::mem_fun(*this, &SurfBWindow::restart_with_lang),"en_GB.UTF-8"));
mr_AG->add(Gtk::Action::create_with_icon_name("LangES","flag_es","Español","Español"), sigc::bind(sigc::mem_fun(*this, &SurfBWindow::restart_with_lang),"es_ES.UTF-8"));
mr_AG->add(Gtk::Action::create_with_icon_name("LangCA","flag_ca","Català","Català"), sigc::bind(sigc::mem_fun(*this, &SurfBWindow::restart_with_lang),"ca_ES.UTF-8"));

mr_AG->add(Gtk::Action::create("Print",Gtk::Stock::PRINT), sigc::mem_fun(*this, &SurfBWindow::on_print_clicked));


//mr_AG->add(Gtk::Action::create("LangEN",Gtk::Stock::HELP), sigc::mem_fun(*this, &SurfBWindow::on_about_clicked));

mr_UIM = Gtk::UIManager::create();

mr_UIM->insert_action_group(mr_AG);



Glib::ustring ui_info = std::string(
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='FileOpen'/>"
    "      <menuitem action='FileSave'/>"
    "      <separator/>"
    "      <menuitem action='FileQuit'/>"
    "    </menu>"
    "    <menu action='MenuSurface'>"
    "      <menuitem action='SurfaceEdit'/>"
    "      <menuitem action='SurfaceDelete'/>"
    "      <separator/>"
    "      <menuitem action='SurfaceNew'/>"

    "    </menu>"
    "    <menu action='MenuView'>"
    "      <menuitem action='ViewFullscreen'/>"
    "    </menu>"
    "    <menu action='MenuHelp'>"
    "      <menuitem action='HelpManual'/>"
    "      <separator/>"
    "      <menuitem action='HelpAbout'/>"
    "    </menu>"
    "  </menubar>"
    "  <toolbar  name='NavBar'>"
    "    <toolitem action='GalleryPrevious'/>"

    "    <toolitem action='GalleryNext'/>"
"      <separator/>"
    "    <toolitem action='ViewFullscreen'/>"

    "      <separator/>"
//    "      <toolitem action='HelpManual'/>"
    "      <toolitem action='HelpAbout'/>"
    "  </toolbar>"
    
    "  <toolbar  name='DefaultBar'>"
    "    <toolitem action='FileOpen'/>"
    "    <toolitem action='FileSave'/>"
    "      <separator/>"
    "      <toolitem action='SurfaceNew'/>"
    "      <toolitem action='SurfaceDelete'/>"
    "      <separator/>"
    
    "      <toolitem action='SurfaceEdit'/>"
    "      <separator/>"
    "    <toolitem action='ViewFullscreen'/>"
    "      <separator/>"
//    "      <toolitem action='HelpManual'/>"
    "      <toolitem action='HelpAbout'/>"
    
    "  </toolbar>"


    "  <toolbar  name='MiniBar'>"
    "      <separator/>"
    "      <toolitem action='SurfaceNew'/>"
    "      <toolitem action='SurfaceDelete' />"
//"      <separator/>"
//    "      <toolitem action='Animate'/>"

)
+((opt.change_lang)?(
"      <toolitem action='LangES'/>"
"      <toolitem action='LangCA'/>"
"      <toolitem action='LangEN'/>"
"      <toolitem action='LangDE'/>"

"<separator/>"
):"")+
((opt.print_cmd != "")?"<toolitem action='Print'/>":"")+

    "  </toolbar>"

    "  <toolbar  name='TopBar'>"
"    <toolitem action='FileOpen'/>"
    "    <toolitem action='FileSave'/>"
    "      <separator/>"
    "    <toolitem action='GalleryPrevious'/>"

    "    <toolitem action='GalleryNext'/>"
"      <separator/>"
    "    <toolitem action='ViewFullscreen'/>"
    
    "  </toolbar>"


    "  <toolbar  name='HelpBar'>"
    "      <toolitem action='HelpAbout'/>"
    "  </toolbar>"
    "  <toolbar  name='PropBar'>"
    "      <toolitem action='SurfaceEdit'/>"
    "      <separator/>"
    //"      <toolitem action='FileReset'/>"
    "  </toolbar>"
    "</ui>";

if(!opt.ui_xml.empty())
{
	try{
	std::ifstream uif(opt.ui_xml.c_str());

	std::string t;
	std::string s;
	while(!uif.eof())
	{
		std::getline(uif,t);
		
		if(!t.empty())
		s+=t+'\n';
	}

	if(!s.empty())
	ui_info = s;

	}
	catch(...){}

}

mr_UIM->add_ui_from_string(ui_info);




Gtk::Toolbar* pToolbar = dynamic_cast<Gtk::Toolbar*>(mr_UIM->get_widget("/TopBar")) ;
  if(pToolbar)
{
pToolbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
//pToolbar->set_icon_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
    m_tool.attach(*pToolbar,3,5,4,5,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
pToolbar->set_show_arrow(false);

//pToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH_HORIZ);
MOD{pToolbar->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);}
}

Gtk::Toolbar* pHelpbar = dynamic_cast<Gtk::Toolbar*>(mr_UIM->get_widget("/HelpBar")) ;
  if(pHelpbar)
{
pHelpbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
//pHelpbar->set_icon_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
    m_tool.attach(*pHelpbar,5,6,4,5,Gtk::SHRINK,Gtk::SHRINK);
pHelpbar->set_show_arrow(false);

MOD{pHelpbar->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);}
//pToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH_HORIZ);
}


if(!no_new_features)
{
Gtk::Toolbar* pMiniBar = dynamic_cast<Gtk::Toolbar*>(mr_UIM->get_widget("/MiniBar"));
if(pMiniBar)
{
m_tool.attach(*pMiniBar, 4,5,5,6,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
pMiniBar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
pMiniBar->set_show_arrow(false);
//pMiniBar->set_icon_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
MOD{pMiniBar->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);}
}



Gtk::Toolbar* pPropbar = dynamic_cast<Gtk::Toolbar*>(mr_UIM->get_widget("/PropBar")) ;
  if(pPropbar)
{
pPropbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
//pPropbar->set_icon_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
    m_tool.attach(*pPropbar,5,6,5,6,Gtk::SHRINK,Gtk::SHRINK);
pPropbar->set_show_arrow(false);
//pToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH_HORIZ);
MOD{pPropbar->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);}
}



}
m_utab.attach(m_tool,3,6,4,6,Gtk::FILL,Gtk::SHRINK);

}




bool SurfBWindow::on_color_expose_event_func(GdkEventExpose*)
{
	
	
	Glib::RefPtr<Gdk::Window> window = m_colors.get_window();
	if(window)
	{		
	Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
	Glib::RefPtr<Gdk::Drawable> dr(window);


	
	m_outside_image->render_to_drawable(dr,some_gc,0,0,0,0,COLORTABLE_SIZE,COLORTABLE_SIZE,Gdk::RGB_DITHER_NONE,0,0);

	}

	return true;
}









Glib::RefPtr<Gdk::Pixbuf> make_colortable(bool flip, rgb_triplet buffer[COLORTABLE_SIZE][COLORTABLE_SIZE])
{
	

	//Glib::RefPtr<Gdk::Window> window = m_colors_inside.get_window();
		
	//Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
	//Glib::RefPtr<Gdk::Drawable> dr(window);

	typedef unsigned char rgb_triplet[3];

	;


	for(int x= 0; x < COLORTABLE_SIZE; x = x+1)
		for(int y= 0; y < COLORTABLE_SIZE; y = y+1)
	{
		Gdk::Color nc = colormap(x,(!flip)?y:(COLORTABLE_SIZE-y)).gdk_colors();
		

		//some_gc->set_rgb_fg_color(nc);
		//dr->draw_point(some_gc,int(x),int(y));	

		buffer[y][x][0] = nc.get_red() / 256;
		buffer[y][x][1] = nc.get_green() / 256;
		buffer[y][x][2] = nc.get_blue() / 256;
		
	}

	Glib::RefPtr<Gdk::Pixbuf> f = Gdk::Pixbuf::create_from_data(&buffer[0][0][0],Gdk::COLORSPACE_RGB,false,8,COLORTABLE_SIZE,COLORTABLE_SIZE,3*COLORTABLE_SIZE);
	//f->save("/tmp/fa.png","png");
	return f;

}


void SurfBWindow::restart_with_lang(const char* LANG)
{

#ifdef WIN32
#define setenv(a,b,c) _putenv_s(a,b)
#endif

setenv("LANGUAGE",LANG,1);
setenv("LANG",LANG,1);

do_restart = true;


SurfBWindow* x = main_work(opt, fullscreen_mode, personalized, NULL);
x->show();
//hide();
Gtk::Main::iteration();
x-> kill_w = this;

}


bool SurfBWindow::on_delete_event(GdkEventAny* eve)
{

Gtk::Main::quit();
return false;
}

