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


/* 
This file contains non-ascii characters, encoding UTF-8.
*/

#include "bwindow_gdk.hh"
#include "gdkmm/pixbuf.h"

#include "matrix.hh"

char fastaa[] = "";//"antialiasing=2;";

//#define SURF_CMD "surf"

bool no_full = true;

double current_x = 0;
double current_y = 0;
double current_z = 0;

const int MSECS = 40;

int pichange = 0;



bool button_down = false;

int gui_x;
int gui_y;


std::vector< Glib::RefPtr<Gdk::Pixbuf> > thumbs;
int total_x = 0;
int moving_x = 0;



enum mouse_mode
{
	rotate_around_z='z',
	rotate_around_y='y',
	rotate_around_x='x',
	zoom='Z',
	change_param='P'
	
};

mouse_mode next_action = rotate_around_z;







SurfBWindow::SurfBWindow(std::istream& i, const std::vector<gallery>& G, surfer_options so,bool f, bool p)
:personalized(p),m_tab(3,2),m_ctab(3,3),m_utab(1,1),m_ltab(3,3),m_aframe("", /* label */
Gtk::ALIGN_CENTER, /* center x */
Gtk::ALIGN_CENTER, /* center y */
1.0, /* xsize/ysize = 1 */
false /* ignore child's aspect */),
waiting(true),valid(true),
m_bframe("Hintergrund", /* label */
		Gtk::ALIGN_CENTER, /* center x */
		Gtk::ALIGN_CENTER, /* center y */
		1.0, /* xsize/ysize = 1 */
		false /* ignore child's aspect */),
m_cframe("Flächenseite 1", /* label */
		Gtk::ALIGN_CENTER, /* center x */
		Gtk::ALIGN_CENTER, /* center y */
		1.0, /* xsize/ysize = 1 */
		false /* ignore child's aspect */),
m_full(Gtk::Stock::FULLSCREEN),
m_iframe("Flächenseite 2", /* label */
		Gtk::ALIGN_CENTER, /* center x */
		Gtk::ALIGN_CENTER, /* center y */
		1.0, /* xsize/ysize = 1 */
		false /* ignore child's aspect */),
kill_list(0),

m_vscale(-1,1,0.05),
m_hscale(0,1.05,0.05,"a"),
m_hscale2(0,1.05,0.05,"b"),
fullscreen_mode(f),
gal(G),
opt(so),
//m_print(Gtk::Stock::PRINT),
//m_save(Gtk::Stock::SAVE),
//m_full(Gtk::Stock::FULLSCREEN),
m_entryfield(3,1),
m_prev(Gtk::Stock::GO_BACK),
m_next(Gtk::Stock::GO_FORWARD),
m_leave_image(Gtk::Stock::LEAVE_FULLSCREEN,Gtk::ICON_SIZE_BUTTON),
m_zoom_image(Gtk::Stock::ZOOM_FIT,Gtk::ICON_SIZE_BUTTON),
m_zoom_table(1,3),



m_savefile(Gtk::Stock::SAVE)

{
	// change window background to white
	modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white")); 
	m_draw.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white"));

	current_gal=0;
	if(!gal.empty())current_surf = gal[0].file.size()-1;

	m_vscale.set_draw_value(false);
	

	data = parse_pic(i);

	m_draw.set_size_request(100, 100);
	m_colors.set_size_request(200, 200);
	//m_movie.set_size_request(100, 100);
	m_colors_inside.set_size_request(200, 200);
	//m_colors_back.set_size_request(200, 200);





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


	m_movie.signal_expose_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_surface_expose_event_func) );
	m_movie.signal_button_press_event().connect( sigc::mem_fun(*this, &SurfBWindow::on_surface_button_press_event_func) );
	m_movie.add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);


	m_entry.signal_insert_text().connect(sigc::mem_fun(*this, &SurfBWindow::on_insert_text_func));
	m_entry.signal_delete_text().connect(sigc::mem_fun(*this, &SurfBWindow::on_delete_text_func));
	m_entry.add_events(Gdk::ALL_EVENTS_MASK);
	m_vscale.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_value_changed_func));
	m_hscale.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_para_changed_func));
	m_hscale2.signal_value_changed().connect(sigc::mem_fun(*this,&SurfBWindow::on_para2_changed_func));
	m_vscale.signal_button_release_event().connect(sigc::mem_fun(*this,&SurfBWindow::on_button_changed_func));
	m_hscale.set_value(data.para_a);
	m_hscale2.set_value(data.para_b);
	m_hscale.set_increments(0.01,0.050);
	m_hscale2.set_increments(0.01,0.050);
	m_vscale.set_value(0);
	m_vscale.set_increments(0.01,0.050);


	m_leave.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_noscreen_clicked));
	m_save.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_save_clicked));
	m_savefile.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_save_file_clicked));
	m_print.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_print_clicked));
	m_full.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_fullscreen_clicked));

	m_left.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_left_clicked));
	m_right.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_right_clicked));

	m_next.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_next_clicked));
	m_prev.signal_clicked().connect(sigc::mem_fun(*this, &SurfBWindow::on_prev_clicked));
//Glib::

// Add the aspect frame to our toplevel window:
//add(m_aframe);


// This just sets the title of our new window.
	set_title("Surfer");

// sets the border width of the window.
	//
	m_note.set_tab_pos(Gtk::POS_BOTTOM);
	m_hscale.set_value_pos(Gtk::POS_LEFT);
	m_hscale2.set_value_pos(Gtk::POS_RIGHT);
	m_aframe.add(m_draw);
        m_aframe.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
        m_aframe.set_shadow_type(Gtk::SHADOW_NONE);
	m_aframe.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white")); 
	m_aframe.modify_base(Gtk::STATE_NORMAL,Gdk::Color("white"));


	add(m_utab);

	m_zoom_free.set_size_request(8,200);
	m_zoom_table.attach(m_zoom_free,0,1,0,1,Gtk::SHRINK,Gtk::SHRINK);
	
	m_zoom_table.attach(m_zoom_image,0,1,1,2,Gtk::SHRINK,Gtk::SHRINK);
	m_zoom_table.attach(m_vscale,0,1,2,3,Gtk::SHRINK);

	if(fullscreen_mode)
	{
		
		m_utab.attach(m_aframe,0,2,0,2);
		m_utab.attach(m_avalue,3,4,0,1,Gtk::SHRINK);
		m_utab.attach(m_leave,3,4,1,2,Gtk::SHRINK,Gtk::SHRINK);

		m_utab.attach(m_zoom_table,2,3,0,2,Gtk::SHRINK);

		m_utab.attach(m_scale_free,0,1,2,3,Gtk::SHRINK,Gtk::SHRINK);
		m_utab.attach(m_hscale,0,1,2,3,Gtk::FILL,Gtk::SHRINK);
		m_utab.attach(m_hscale2,1,2,2,3,Gtk::FILL,Gtk::SHRINK);


		m_leave.set_image(m_leave_image);
		set_border_width(0);
		fullscreen();
	}
	else
	{

		

		m_utab.attach(m_aframe,0, 2, 0, 1 ,Gtk::EXPAND|Gtk::FILL,Gtk::EXPAND|Gtk::FILL);
		//m_tab.attach(m_utab,0, 3, 0, 1 ,Gtk::EXPAND|Gtk::FILL,Gtk::EXPAND|Gtk::FILL);
		//m_ltab.attach(m_bbox,0, 2, 2, 3 ,Gtk::SHRINK,Gtk::SHRINK);
		m_utab.attach(m_bbox,0, 2, 2, 3 ,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
		//m_tab.attach(m_ltab,0, 1, 1, 2 ,Gtk::SHRINK,Gtk::SHRINK);

		m_utab.attach(m_entryfield,0,2,3,4,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);

		m_utab.attach(m_zoom_table,2,3,0,2,Gtk::SHRINK);
		
		
		
		m_utab.attach(m_scale_free,0,1,1,2,Gtk::SHRINK,Gtk::SHRINK);
		m_utab.attach(m_hscale,0,1,1,2,Gtk::FILL,Gtk::SHRINK);
		m_utab.attach(m_hscale2,1,2,1,2,Gtk::FILL,Gtk::SHRINK);

		//m_ltab.attach(m_hscale,0,1,1,2,Gtk::FILL,Gtk::SHRINK);
		//m_ltab.attach(m_hscale2,1,2,1,2,Gtk::FILL,Gtk::SHRINK);


		m_utab.attach(m_bft,3, 4, 2, 4 ,Gtk::SHRINK,Gtk::SHRINK);
		m_utab.attach(m_note,3,4,0,2,Gtk::SHRINK);


		if(!no_full)fullscreen(); 
		set_border_width(4);


		//m_bframe.add(m_movie);

		

		//m_bbox.set_child_min_width(1);

		{
			char s[] = "abxyz+-*^.()0123456789D";
			char b[2] = {0,0};
			for(unsigned i = 0; s[i] != 0; i++)
			{
				b[0] = s[i];

				Gtk::Button* B = NULL;

				if(s[i]=='D')
				{
					B = new Gtk::Button("Entf");
					//B -> set_image(*new Gtk::Image(Gtk::Stock::DELETE,Gtk::ICON_SIZE_BUTTON));
				}
				else
				B = new Gtk::Button(b);
				B->signal_clicked().connect( sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_letter_clicked_func) ,b[0]));
				m_bbox.add(*B);


			}}

			
			m_error.set_markup("<span foreground=\"#FF0000\" weight=\"bold\">!</span>");
			m_error.set_selectable(false);

			m_utab.attach(m_error,2,3,3,4,Gtk::SHRINK,Gtk::SHRINK);
			if(!opt.print_cmd.empty())m_fbox.add(m_print);
			if(!opt.save_cmd.empty())m_fbox.add(m_save);
			m_fbox.add(m_full);
			m_fbox.add(m_savefile);
			m_fbox.set_child_min_width(1);


			m_backfor.add(m_prev);
			m_backfor.add(m_next);
			
			m_backfor.set_child_min_width(60);

			m_print.set_image(*new Gtk::Image(Gtk::Stock::PRINT,Gtk::ICON_SIZE_BUTTON));
			m_save.set_image(*new Gtk::Image(Gtk::Stock::SAVE,Gtk::ICON_SIZE_BUTTON));
			
			//m_full.set_image(*new Gtk::Image(Gtk::Stock::FULLSCREEN,Gtk::ICON_SIZE_BUTTON));

			m_left.set_image(*new Gtk::Image(Gtk::Stock::GO_BACK,Gtk::ICON_SIZE_BUTTON));
			m_right.set_image(*new Gtk::Image(Gtk::Stock::GO_FORWARD,Gtk::ICON_SIZE_BUTTON));

			//m_prev.set_image(*new Gtk::Image(Gtk::Stock::GO_BACK,Gtk::ICON_SIZE_BUTTON));
			//m_next.set_image(*new Gtk::Image(Gtk::Stock::GO_FORWARD,Gtk::ICON_SIZE_BUTTON));



			
		

		//m_ltab.attach(m_entryfield,0,2,3,4,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);

			m_entryfield.attach(m_left,0,1,0,1,Gtk::SHRINK,Gtk::SHRINK);
			m_entryfield.attach(m_entry,1,2,0,1,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
			m_entryfield.attach(m_right,2,3,0,1,Gtk::SHRINK,Gtk::SHRINK);

//m_tab.attach(m_colors,0,1,0,1,Gtk::SHRINK,Gtk::SHRINK);
//m_tab.attach(m_colors_inside,2,3,0,1,Gtk::SHRINK,Gtk::SHRINK);
			

			m_entry.set_text(data.public_eq);
			
				
			
			
			

			m_note.append_page(m_ctab,"Farben","x");
			m_ctab.attach(*new Gtk::Image,1,2,0,1);
			m_ctab.attach(m_cframe,1,2,1,2,Gtk::EXPAND,Gtk::SHRINK);//,Gtk::SHRINK|Gtk::FILL,Gtk::SHRINK|Gtk::FILL);
			m_cframe.add(m_colors);

			m_ctab.attach(m_iframe,1,2,2,3,Gtk::EXPAND,Gtk::SHRINK);//,Gtk::SHRINK|Gtk::FILL,Gtk::SHRINK|Gtk::FILL);
			m_iframe.add(m_colors_inside);

			//m_ctab.attach(m_bframe,1,2,2,3,Gtk::SHRINK|Gtk::FILL,Gtk::SHRINK|Gtk::FILL);
			//m_bframe.add(m_colors_back);


			m_note.append_page(m_gtab,"Galerie","y");

			m_note.append_page(m_info,"Info","z");

			m_info.set_size_request(300,600);
			

			m_gtab.attach(*new Gtk::Image,1,2,0,1);
			for(unsigned i = 0; i < gal.size(); i++)
			{
				


				Gtk::Frame* v_gframe = new Gtk::Frame(gal[i].name);

				// white background for gallery widgets
				Gtk::Widget* W = v_gframe->get_label_widget();
				W->modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white")); 
				W->modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white")); 
				v_gframe->modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white")); 
				v_gframe->modify_base(Gtk::STATE_NORMAL,Gdk::Color("white")); 

				m_gtab.attach(*v_gframe,1,2,i+1,i+2,Gtk::EXPAND|Gtk::FILL,Gtk::SHRINK|Gtk::FILL);

				Gtk::EventBox* v_eb = new Gtk::EventBox;
				Gtk::Image* v_im = new Gtk::Image;
				v_im->set(gal[i].image);
				v_eb->modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white")); 
				v_eb->modify_base(Gtk::STATE_NORMAL,Gdk::Color("white")); 
				
				//Gtk::DrawingArea* v_gpic = new Gtk::DrawingArea;
				v_eb->signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this,&SurfBWindow::on_gallery_press_event),i));
				v_gframe->add(*v_eb);
				v_im->set_size_request(100,100);
				v_gframe->set_size_request(120,120);
				v_eb->add(*v_im);
				

				v_eb->add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);
			}


			m_bft.attach(m_backfor,0,1,0,1,Gtk::FILL,Gtk::SHRINK);
			m_bft.attach(m_fbox,0,1,1,2,Gtk::SHRINK,Gtk::SHRINK);
			
			




//m_tab.attach(m_bframe,2,3,0,2,Gtk::SHRINK);

//Add the TreeView, inside a ScrolledWindow, with the button underneath:

	}




	show_all_children();
	
	
	adjust_visibility();
	sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);

// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, MSECS);

}

void SurfBWindow::start()
{

	std::vector<parsepic_out> R;
	add_data(R);
	read_thumbs(R);


	button_down = true;
	pichange=5;
	on_timer_event_func(0);
	button_down = false;

	

	if(!personalized) on_next_clicked();
	

}


bool SurfBWindow::on_expose_event_func(GdkEventExpose*)
{
	
	
	
	refresh_display(TEMP_ROOT_SEP +"surfb.ppm",false);
	return true;
	
	Glib::RefPtr<Gdk::Window> window = m_draw.get_window();
	
	if( pichange>=5||pichange==3)
	{
		
		refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);
		Gtk::Allocation allocation = m_draw.get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();
		
		// coordinates for the center of the window
		
		Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
		Glib::RefPtr<Gdk::Drawable> dr(window);
		
		try{
			//Glib::RefPtr<Gdk::Pixbuf> surface = Gdk::Pixbuf::create_from_file(TEMP_ROOT_SEP +"surfb.ppm");
			//Glib::RefPtr<Gdk::Pixbuf> sface = surface->scale_simple(width-40,height-40,Gdk::INTERP_BILINEAR);

			////dr->draw_rectangle(some_gc,true,0,0,width,height);
			//dr->draw_pixbuf(some_gc,sface,0,0,20,20,-1,-1,Gdk::RGB_DITHER_NONE,0,0);
			
			refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);
		
		}
		catch(...){}

		

		
	}
	else if(pichange == 0)
	{
		
		Gtk::Allocation allocation = m_draw.get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();
		
		// coordinates for the center of the window
		
		Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
		Glib::RefPtr<Gdk::Drawable> dr(window);

		try{
			Glib::RefPtr<Gdk::Pixbuf> surface = Gdk::Pixbuf::create_from_file(TEMP_ROOT_SEP +"surfb_f.ppm");
			Glib::RefPtr<Gdk::Pixbuf> sface = surface->scale_simple(width-40,height-40,Gdk::INTERP_BILINEAR);

			//dr->draw_rectangle(some_gc,true,0,0,width,height);
			dr->draw_pixbuf(some_gc,sface,0,0,20,20,-1,-1,Gdk::RGB_DITHER_NONE,0,0);
		}
		catch(...){}

		
	}

	pichange = 5;
	return true;

}



bool SurfBWindow::on_color_expose_event_func(GdkEventExpose*)
{
	

	Glib::RefPtr<Gdk::Window> window = m_colors.get_window();
		
	Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
	Glib::RefPtr<Gdk::Drawable> dr(window);


	for(double x= 0; x <= 200; x = x+1)
		for(double y= 0; y <= 200; y = y+1)
	{
		
		Gdk::Color nc = colormap(x,y).gdk_colors();
		

		some_gc->set_rgb_fg_color(nc);
		dr->draw_point(some_gc,int(x),int(y));		
		
	}

	return true;
}


color_double colormap(double bx, double by, double Z)
{
	color_double c;
	double x = bx/2;
	double y = 100-by/2;
	
		double l1 = x/100.;
		double l2 = 1 - l1;
		double u1 = y/100.;
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

		//double xy = x;
		//if (x>y) xy = y;

		//double cta = (5000-(x-50)*(x-50)-(y-50)*(y-50))/15000;
		//double ctb = 0;//(x*x-y*y)/10000;

		
		//double ct = (cta-ctb);
		

		//if(y>90) cta=cta*(100-y)/10;
		//if(x>90) cta=cta*(100-x)/10;

		//if(x<10) cta=cta*(x)/10;
		//if(y<10) cta=cta*(y)/10;

		//double ctx =sqrt(81-(x-50)*(x-50)-(y-50)*(y-50));
		//ctx = ctx/500;
		//if(ctx<0) ctx = 0;

		//if((x-50)*(x-50)+(y-50)*(y-50)<4)ct = 1;

		//if(x>97&&y>97) ct = -1;


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
	
// This is where we draw on the window
	//Glib::RefPtr<Gdk::Window> window = m_colors.get_window();
	//if(window)
	{
		//Gtk::Allocation allocation = m_colors.get_allocation();
		//const int width = allocation.get_width();
		//const int height = allocation.get_height();

// coordinates for the center of the window
		data.upside = colormap(event->x,event->y).surf_colors();
		button_down = true;
		pichange = 5;
		
	}
	return false;
}





bool SurfBWindow::on_button_press_event_func(GdkEventButton* event)
{

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



bool SurfBWindow::on_button_release_event_func(GdkEventButton* event)
{

// This is where we draw on the window

	button_down = false;

	/*
	data.initial_x += current_x;
	data.initial_y += current_y;
	data.initial_z += current_z;
	current_x = 0;
	current_y = 0;
	current_z = 0;
	*/
	pichange = 3;

	return false;
}


bool SurfBWindow::on_color_button_release_event_func(GdkEventButton* event)
{

// This is where we draw on the window

	button_down = false;

	/*
	data.initial_x += current_x;
	data.initial_y += current_y;
	data.initial_z += current_z;
	current_x = 0;
	current_y = 0;
	current_z = 0;
	*/
	double x, y;
	x = event->x;
	y = event->y;
	
	data.upside = colormap(x,y).surf_colors();

	pichange = 3;

	return false;
}


bool SurfBWindow::on_motion_notify_event_func(GdkEventMotion* event)
{

// This is where we draw on the window
	Glib::RefPtr<Gdk::Window> window = m_draw.get_window();
	if(window && button_down)
	{
		Gtk::Allocation allocation = m_draw.get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();

// coordinates for the center of the window
		int xc, yc;
		xc = int(event->x);
		yc = int(event->y);
	
		double tx = xc - double(width)/2;
		double ty = yc - double(width)/2;
		double tgx = gui_x - double(width)/2;
		double tgy = gui_y - double(width)/2;



		double fx = -(xc-gui_x)/double(width)*3.1415926;
		double fy = -(yc-gui_y)/double(width)*3.1415926;


		double c_y = 0.0;
		double c_x = 0.0;
		double c_z = 0.0;


		switch(next_action)
		{
			case rotate_around_z:
				c_y = fx;
				c_x = fy;
				break;
			case rotate_around_y:
				c_z = fx;
				c_x = fy;
				break;
			case rotate_around_x:
				c_y = fx;
				c_z = fy;
				break;
			case zoom:
				m_vscale.set_value( tx/double(width)*2);
				data.scale = exp(log(10.0)*m_vscale.get_value());
	
	
				break;
			case change_param:
				m_hscale.set_value(xc/double(width));
				break;
		}

	
	

		if(c_y != 0.0) data.rot = data.rot * roty(c_y);
		if(c_x != 0.0) data.rot = data.rot * rotx(c_x);
		if(c_z != 0.0) data.rot = data.rot * rotz(c_z);

		gui_x = xc;
		gui_y = yc;

		pichange = 5;
	//dr->draw_line(some_gc,gui_x,gui_y,xc,yc);
	
	
		
	}
	return false;
}




bool SurfBWindow::on_color_motion_notify_event_func(GdkEventMotion* event)
{

// This is where we draw on the window

	if(button_down)
	{
		
// coordinates for the center of the window
		double x, y;
		x = event->x;
		y = event->y;

		data.upside = colormap(x,y).surf_colors();
		




	
		pichange = 5;
	//dr->draw_line(some_gc,gui_x,gui_y,xc,yc);
	
	
		
	}
	return false;
}


void SurfBWindow::on_insert_text_func(const Glib::ustring&,int*)
{
	
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
	data.equation = t3;
	data.public_eq = t2;
	}
	else
	data.equation=1;

	//pichange = 5;
	adjust_visibility();
	refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);
	pichange=15;
}


void SurfBWindow::on_delete_text_func(int,int)
{
	
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
	data.equation = t3;
	data.public_eq = t2;
	}

	
	pichange = 15;
	adjust_visibility();
}


void SurfBWindow::on_value_changed_func()
{
	
	data.scale = exp(log(10.0)*m_vscale.get_value());
	
	
	
	pichange = 15;
}


void SurfBWindow::on_para_changed_func()
{
	
	/*std::ostringstream os;
	os.precision(2);
	os.setf(std::ios_base::fixed);
	os<<"a="<<m_hscale.get_value();
	m_avalue.set_text(os.str());*/
	//return "";
	
	
	data.para_a = m_hscale.get_value();
	pichange = 15;
}

void SurfBWindow::on_para2_changed_func()
{
	
	/*std::ostringstream os;
	os.precision(2);
	os.setf(std::ios_base::fixed);
	os<<"a="<<m_hscale.get_value();
	m_avalue.set_text(os.str());*/
	//return "";
	
	
	data.para_b = m_hscale2.get_value();
	pichange = 15;
}


bool SurfBWindow::on_button_changed_func(GdkEventButton* )
{
	return true;
	data.scale = exp(log(10.0)*m_vscale.get_value());
	
	
	refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);
	pichange = 3;

	return true;
}




bool SurfBWindow::on_timer_event_func(int)
{
	//moving_x++;
	//if(moving_x>=total_x) moving_x = 0;

	//on_surface_expose_event_func(0);
	
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

// This is where we connect the slot to the Glib::signal_timeout()
		
		refresh(TEMP_ROOT_SEP +"surfb_f.pic",TEMP_ROOT_SEP +"surfb_f.ppm",data.antialiasing,true);
		conn = Glib::signal_timeout().connect(my_slot, MSECS);

		pichange = 0;
		waiting = true;
	}
	else if( !button_down && pichange == 3)
	{
		
		//conn.disconnect();
		
		//sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);

// This is where we connect the slot to the Glib::signal_timeout()
		
		refresh(TEMP_ROOT_SEP +"surfb_f.pic",TEMP_ROOT_SEP +"surfb_f.ppm",data.antialiasing,true);
		//conn = Glib::signal_timeout().connect(my_slot, MSECS);

		pichange = 0;
		waiting = true;
	}
	else if( !button_down && pichange == 0)
	{
		
		//conn.disconnect();
		
		//sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);

// This is where we connect the slot to the Glib::signal_timeout()
		if(waiting)
		refresh_display(TEMP_ROOT_SEP +"surfb_f.ppm",true);
		//refresh(TEMP_ROOT_SEP "surfb_f.pic",TEMP_ROOT_SEP "surfb_f.ppm",data.antialiasing,true);
		//conn = Glib::signal_timeout().connect(my_slot, MSECS);

		//pichange = 0;
	}
	return true;
}


void SurfBWindow::refresh_image(const std::string& script, const std::string& image, const std::string& aa, bool full, const int n , bool max_res)
{
	if(!valid) return;
	//if(fullscreen_mode)
	{
		//Glib::RefPtr<Gdk::Window> window = get_window();
		//if(window)
		{
		//Gtk::Allocation allocation = get_allocation();
		//const int width = allocation.get_width();
		//const int height = allocation.get_height();
		
		//Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
		//Glib::RefPtr<Gdk::Drawable> dr(window);
		
		
		//Gdk::Color nc;
		//nc.set_rgb(65535*0,65535*0,65535*0);

		//some_gc->set_rgb_fg_color(nc);
		//some_gc->set_rgb_bg_color(nc);
		
		
		//dr->draw_rectangle(some_gc,true,0,0,width,height);
		}
		 ;
	}
		
		
	Gtk::Allocation allocation = m_draw.get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();

	// coordinates for the center of the window
	
	
	
	
	int w = full ? data.hires : data.lores;
	
	if(w>width-40 && !max_res) w = width-40;
	if(w>height-40  && !max_res) w = height-40;
	

	for(int k=1; k<= n; k++)
	{
	std::ostringstream os;
	os<<k;
	std::string sn = os.str();
	if(k==1) sn="";
	std::ofstream f((script+sn).c_str(),FILE_WRITE_MODE);
	
/*	f<<"rot_x="<<data.initial_x+current_x<<";\n";
	f<<"rot_y="<<data.initial_y+current_y<<";\n";
	f<<"rot_z="<<data.initial_z+current_z<<";\n";
*/
	//rot_data t = rot_yxz(data.rot);
	//f<<"rot_x="<<t.rot_x<<";\n";
	//f<<"rot_y="<<t.rot_y<<";\n";
	//f<<"rot_z="<<t.rot_z<<";\n";

	
	
	
	f<<"width="<< w <<";\n";
	f<<"height="<< w <<";\n";


	f<<"double a = "<<data.para_a<<";\n";
	f<<"double b = "<<data.para_b<<";\n";
	f<<"poly u="<<data.rot.el(1,1)<<"*x+("<<data.rot.el(1,2)<<")*y+("<<data.rot.el(1,3)<<")*z;\n";
	f<<"poly v="<<data.rot.el(2,1)<<"*x+("<<data.rot.el(2,2)<<")*y+("<<data.rot.el(2,3)<<")*z;\n";
	f<<"poly w="<<data.rot.el(3,1)<<"*x+("<<data.rot.el(3,2)<<")*y+("<<data.rot.el(3,3)<<")*z;\n";

	f<<data.general_stuff;

	if(!aa.empty())f<<aa<<"\n";
	//f<<"surface="<<data.equation<<";\n";
	f<<"surface="<< fix_input_for_surf(data.equation) <<";\n";
	f<<"scale_x="<<data.scale*data.initial_scale<<";\n";
	f<<"scale_y="<<data.scale*data.initial_scale<<";\n";
	f<<"scale_z="<<data.scale*data.initial_scale<<";\n";

	f<<"surface_red="<<data.upside.red<<";\n";
	f<<"inside_red="<<data.inside.red<<";\n";
	f<<"surface_green="<<data.upside.green<<";\n";
	f<<"inside_green="<<data.inside.green<<";\n";
	f<<"surface_blue="<<data.upside.blue<<";\n";
	f<<"inside_blue="<<data.inside.blue<<";\n";

	f<<"background_red="<<data.background.red<<";\n";
	f<<"background_green="<<data.background.green<<";\n";
	f<<"background_blue="<<data.background.blue<<";\n";
	
	

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
		
	if(max_res)
	{
		system((opt.surf_cmd+" -n "+script+" " REDIRECTION_APEX).c_str());
	}
	else
	if(full)
	{
		
		//my_kill(kill_list);
		
		
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

void SurfBWindow::refresh_display(const std::string& image, bool full)
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

void SurfBWindow::refresh(const std::string& script, const std::string& image, const std::string& aa, bool full)
{
	
	refresh_image(script,image,aa,full);
	refresh_display(image,full);
	

	

	

}




bool SurfBWindow::on_surface_button_press_event_func(GdkEventButton* event)
{
//deprecated, obsolete,dead

// This is where we draw on the window

	gallery G;
	std::vector<parsepic_out>& R = G.file;
	
	R.push_back(data);
	R[0].name = "back";
	R[0].thumbnail = TEMP_ROOT_SEP +"surfb.ppm";
	R[0].rot = data.rot;
	
	add_data(R);

	SelectWindow s(G,opt);

	s.set_modal();
	s.fullscreen();

	
//Shows the window and returns when it is closed.
	Gtk::Main::run(s);
	data = s.res;
	pichange = 5;
	data.rot = data.rot;
	m_entry.set_text(data.public_eq);
	adjust_visibility();
	return false;
}



bool SurfBWindow::on_surface_expose_event_func(GdkEventExpose*)
{
	
	int i = moving_x/100;
	int x = moving_x % 100;


	Glib::RefPtr<Gdk::Window> window = m_movie.get_window();
		
	
	if(window)
	{
		Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
	//some_gc.create(get_window());
	
		Glib::RefPtr<Gdk::Drawable> dr(window);
	
		if(total_x>=100)
		{
			Glib::RefPtr<Gdk::Pixbuf> surface = thumbs[i];
			Glib::RefPtr<Gdk::Pixbuf> next_surface = thumbs[i+1];
	

			dr->draw_pixbuf(some_gc,surface,     x,0,0,0,    100-x,100,Gdk::RGB_DITHER_NONE,0,0);
			dr->draw_pixbuf(some_gc,next_surface,0,0,100-x,0,100,100,Gdk::RGB_DITHER_NONE,0,0);
		}
	
	}
	return true;

}

/*


TODO

report surf errors
make large exponents an error




*/




void read_thumbs(std::vector<parsepic_out>& R)
{
	for(unsigned i = 0; i < R.size(); i++)
	{
		try{
			thumbs.push_back(Gdk::Pixbuf::create_from_file(R[i].thumbnail));
			total_x += 100;
		}
		catch(...){}
		
	}

	try{
		thumbs.push_back(Gdk::Pixbuf::create_from_file(R[0].thumbnail));
	}
	catch(...){}

}



bool SurfBWindow::on_inside_expose_event_func(GdkEventExpose*)
{
	

	Glib::RefPtr<Gdk::Window> window = m_colors_inside.get_window();
		
	Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
		//some_gc.create(get_window());
		
	
	Glib::RefPtr<Gdk::Drawable> dr(window);


	for(double x= 0; x <= 200; x = x+1)
		for(double y= 0; y <= 200; y = y+1)
	{
		Gdk::Color nc = colormap(x,200-y).gdk_colors();
		

		some_gc->set_rgb_fg_color(nc);
		dr->draw_point(some_gc,int(x),int(y));		
		
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
		const int width = allocation.get_width();
		const int height = allocation.get_height();

// coordinates for the center of the window
		double x, y;
		x = event->x;
		y = event->y;
	
	//std::c
		data.inside = colormap(x,200-y).surf_colors();
		
		button_down = true;
		pichange = 5;
		
	}
	return false;
}


bool SurfBWindow::on_inside_button_release_event_func(GdkEventButton* event)
{

// This is where we draw on the window

	button_down = false;

	/*
	data.initial_x += current_x;
	data.initial_y += current_y;
	data.initial_z += current_z;
	current_x = 0;
	current_y = 0;
	current_z = 0;
	*/
	double x, y;
	x = event->x;
	y = event->y;
	
	data.inside = colormap(x,200-y).surf_colors();
	

	pichange = 3;

	return false;
}



bool SurfBWindow::on_inside_motion_notify_event_func(GdkEventMotion* event)
{

// This is where we draw on the window

	if(button_down)
	{
		Gtk::Allocation allocation = m_draw.get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();

// coordinates for the center of the window
		double x, y;
		x = event->x;
		y = event->y;



	data.inside = colormap(x,200-y).surf_colors();
		




	
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
	data.background = colormap(x,y).surf_colors();
	
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
	data.background = colormap(x,y).surf_colors();
	pichange = 3;
	return false;
}



bool SurfBWindow::on_back_motion_notify_event_func(GdkEventMotion* event)
{
	if(button_down)
	{
		Gtk::Allocation allocation = m_draw.get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();
		double x, y;
		x = event->x;
		y = event->y;
	data.background = colormap(x,y).surf_colors();
		pichange = 5;
	}
	return false;
}










void SurfBWindow::on_mode_clicked_func(char s)
{
	next_action = mouse_mode(int(s));
	
	return ;
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
  f.m_entry.set_text(data.public_eq);
  
  //Shows the window and returns when it is closed.
  //hide();
  f.show();
  f.fullscreen();
  f.adjust_visibility();
  f.m_hscale.set_value(data.para_a);
  f.m_hscale2.set_value(data.para_b);
conn.disconnect();
  Gtk::Main::run(f);
	data = f.data;
       
   m_hscale.set_value(data.para_a);
  m_hscale2.set_value(data.para_b);
  	

sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &SurfBWindow::on_timer_event_func),0);

// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, MSECS);
	refresh(TEMP_ROOT_SEP +"surfb.pic",TEMP_ROOT_SEP +"surfb.ppm",fastaa);
}

void SurfBWindow::on_print_clicked()
{
	std::string s1 = fix_file(opt.print_cmd);
	while(s1.find("#")!=-1)
	{
		int i = s1.find("#");
		s1.replace(i,1, TEMP_ROOT_SEP +"surfb_f.ppm");
	}
	
	system((""+s1).c_str());
}

void SurfBWindow::on_save_clicked()
{
	std::string s1 = fix_file(opt.save_cmd);
	while(s1.find("#")!=-1)
	{
		int i = s1.find("#");
		s1.replace(i,1, TEMP_ROOT_SEP +"surfb_f.ppm");
	}
	system((""+s1).c_str());
}


void SurfBWindow::on_save_file_clicked()
{
	Gtk::FileChooserDialog dialog(*this,"Bild speichern",Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);


	Gtk::FileFilter filter_jpeg;
	  filter_jpeg.set_name("JPEG-Bilder (.jpg)");
	  filter_jpeg.add_mime_type("image/jpeg");
	  dialog.add_filter(filter_jpeg);

		/*
	  Gtk::FileFilter filter_ppm;
	  filter_ppm.set_name("Portable Pixmap-Bilder (.ppm)");
	  filter_ppm.add_mime_type("image/x-portable-bitmap");
	  filter_ppm.add_mime_type("image/x-portable-pixmap");
	  dialog.add_filter(filter_ppm);
	*/

	  Gtk::FileFilter filter_any;
	  filter_any.set_name("Alle Dateien");
	  filter_any.add_pattern("*");
	  dialog.add_filter(filter_any);
	  dialog.set_current_name(data.name+".jpg");


        int result = dialog.run();



	if(result == Gtk::RESPONSE_OK)
	{
		std::string of = opt.format;
		opt.format = "jpg";
		
		std::string t = dialog.get_filename();
		if(t.find("\"")==-1L && t.find(".")==-1L)
		t = t+"."+opt.format;
		
		refresh_image(TEMP_ROOT_SEP+"surfb_s.pic",t,data.antialiasing,true,1,true);

		opt.format = of;
	}
}

void SurfBWindow::on_noscreen_clicked()
{
hide();
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


	data =  gal[current_gal].file[current_surf];
	
	pichange=5;
	m_vscale.set_value(0);
	m_entry.set_text(data.public_eq);
	m_hscale.set_value(data.para_a);
	m_hscale2.set_value(data.para_b);
	adjust_visibility();
	try{
	std::ifstream fi(data.desc.c_str());
	if(fi.is_open())
	{
		m_info.set(data.desc);
		m_note.set_current_page(2);
	}
	else
	m_info.clear();}
	catch(...){m_info.clear();}
}

void SurfBWindow::on_prev_clicked()
{
	if(gal.empty()) return;
	if(gal[current_gal].file.empty()) return;
	
	current_surf = (current_surf+gal[current_gal].file.size()-1 )%gal[current_gal].file.size();
	data =  gal[current_gal].file[current_surf];
	

	
	pichange=5;
	m_vscale.set_value(0);
	m_entry.set_text(data.public_eq);
	m_hscale.set_value(data.para_a);
	m_hscale2.set_value(data.para_b);
	adjust_visibility();
	try{
	std::ifstream fi(data.desc.c_str());
	if(fi.is_open())
	{
		m_info.set(data.desc);
		m_note.set_current_page(2);
	}
	else
	m_info.clear();}
	catch(...){m_info.clear();}

}

bool SurfBWindow::on_gallery_press_event(GdkEventButton*,int i)
{
	current_gal = i;
	
	GalleryWindow w(gal[i],opt);
	w.set_modal();
	w.show();
	w.fullscreen();
	Gtk::Main::run(w);
	data = w.ret;
	current_surf = w.isu;
	pichange=5;
	m_vscale.set_value(0);
	m_entry.set_text(data.public_eq);
	m_hscale.set_value(data.para_a);
	m_hscale2.set_value(data.para_b);
	try{
	std::ifstream fi(data.desc.c_str());
	if(fi.is_open())
	{
		m_info.set(data.desc);
		m_note.set_current_page(2);
	}
	else
	m_info.clear();}
	catch(...){m_info.clear();}
	return true;
}

void SurfBWindow::adjust_visibility()
{
	bool fa = (data.public_eq.find('a')!=-1);
	bool fb = (data.public_eq.find('b')!=-1);

	m_hscale.show();
	m_scale_free.set_size_request(1,m_hscale.get_allocation().get_height());
	

	if(fa) m_hscale.show(); else m_hscale.hide();
	if(fb) m_hscale2.show(); else m_hscale2.hide();

	valid = check_input(fix_input(data.public_eq));
	if(valid)
	{
	std::remove(((TEMP_ROOT_SEP+"surfb_x.ppm").c_str()));
	check_image(TEMP_ROOT_SEP+"surfb_x.pic",TEMP_ROOT_SEP+"surfb_x.ppm");
	std::ifstream f((TEMP_ROOT_SEP+"surfb_x.ppm").c_str());
	valid=f.is_open();
	}

	if(valid) m_error.hide();
	else m_error.show();
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
	

	

	f<<"double a = "<<data.para_a<<";\n";
	f<<"double b = "<<data.para_b<<";\n";
	f<<"poly u="<<data.rot.el(1,1)<<"*x+("<<data.rot.el(1,2)<<")*y+("<<data.rot.el(1,3)<<")*z;\n";
	f<<"poly v="<<data.rot.el(2,1)<<"*x+("<<data.rot.el(2,2)<<")*y+("<<data.rot.el(2,3)<<")*z;\n";
	f<<"poly w="<<data.rot.el(3,1)<<"*x+("<<data.rot.el(3,2)<<")*y+("<<data.rot.el(3,3)<<")*z;\n";

	f<<data.general_stuff;

	
	//f<<"surface="<<data.equation<<";\n";
	f<<"surface="<< data.equation <<";\n";
	f<<"scale_x="<<data.scale*data.initial_scale<<";\n";
	f<<"scale_y="<<data.scale*data.initial_scale<<";\n";
	f<<"scale_z="<<data.scale*data.initial_scale<<";\n";

	f<<"surface_red="<<data.upside.red<<";\n";
	f<<"inside_red="<<data.inside.red<<";\n";
	f<<"surface_green="<<data.upside.green<<";\n";
	f<<"inside_green="<<data.inside.green<<";\n";
	f<<"surface_blue="<<data.upside.blue<<";\n";
	f<<"inside_blue="<<data.inside.blue<<";\n";

	f<<"background_red="<<data.background.red<<";\n";
	f<<"background_green="<<data.background.green<<";\n";
	f<<"background_blue="<<data.background.blue<<";\n";
	
	

	//f<<"transparency="<<50<<";\n";
	f<<"filename=\""<<image<<"\";\n";
	f<<"color_file_format="<<opt.format<<";\n";
	f<<"draw_surface;\n";
	f<<"save_color_image;\n";
	f.close();
	

	
    			
		


	
	std::string cmd = opt.surf_cmd+" -n \""+script+"\" " REDIRECTION_APEX;
	
	system(cmd.c_str());
		//if(w)gdk_window_set_cursor(w, NULL);
	
}
