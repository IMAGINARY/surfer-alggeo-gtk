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


#include "bwindow_gdk.hh"
#include "gdkmm/pixbuf.h"

#include "matrix.hh"



const int x_dim = 7;
const int y_dim = 7;

int sandwich(double x,double l, double u)
{
	if(l<= x && x <= u) return int(x);
	if(x>u) return int(u);
	return int(l);
}

 void add_data(std::vector<parsepic_out>& R)
{
	
	std::vector<std::string> eqs;
	std::vector<std::string> names;

	eqs.push_back("x^2*y+y^2*z+z^2*x-0.1");
        names.push_back("simple cubic sheet");

	eqs.push_back("x^3+y^3+z^3-x-y-z");
        names.push_back("sheet with several holes");

	eqs.push_back("x^2+y^2+z^2+2*x*y*z-1");
        names.push_back("surface with 4 singular points");


	eqs.push_back("x^4+y^4+z^4-(0.5*x+1)^2*(x^2+y^2+z^2)-(x^2*y^2+x^2*z^2+y^2*z^2) +(0.5*x+1)^4");
        names.push_back("a Kummer surface");


	eqs.push_back("4*(2.618*x^2-y^2)*(2.618*y^2-z^2)*(2.618*z^2-x^2)-4.236*(x^2+y^2+z^2-1)^2");
        names.push_back("Bath's sextic");

	std::istringstream is("");
	parsepic_out general = parse_pic( is);


	for(unsigned i = 0; i < eqs.size(); i++)

	{
		parsepic_out P;
		
		P.antialiasing = general.antialiasing;
		P.general_stuff = general.general_stuff;

		//P.initial_x = general.initial_x;
		//P.initial_y = general.initial_y;
		//P.initial_z = general.initial_z;

		P.name = names[i];
		P.public_eq = eqs[i];
		P.equation = trans_eq( eqs[i]);

		P.initial_scale = general.scale;

		//P.size = general.size;

		std::ostringstream os;

		os<<TEMP_ROOT_SEP +"surfb_thumb"<<i<<".ppm";

		P.thumbnail = os.str();

		P.upside.red = sandwich(255 * (i/double(eqs.size()-1))-20,0,255);
		P.upside.green = sandwich(255 * (1 - i/double(eqs.size()-1))-20,0,255);
		P.upside.blue = sandwich(255*sin(i/double(eqs.size()-1)*3)-20,0,255);

		P.inside.red = sandwich(255 * (i/double(eqs.size()-1))+20,0,255);
		P.inside.green = sandwich(255 * (1 - i/double(eqs.size()-1))+20,0,255);
		P.inside.blue = sandwich(255*sin(i/double(eqs.size()-1)*3)+20,0,255);
		
		

		R.push_back(P);

		

	}


	return ;
}


SelectWindow::SelectWindow(const gallery& r, const surfer_options& o)
:m_tab(x_dim,y_dim),R(r.file),opt(o)
 
{

	
	

  
set_title("Auswahl");

  // sets the border width of the window.
  set_border_width(10);

 
  add(m_tab);
  
 

  for(unsigned x = 0; x < x_dim; x++)
  for(unsigned y = 0; y < y_dim+1; y+=2)
  {
  if(vdraw.size() == R.size())
	{x = x_dim; y=y_dim; break;}
  vdraw.push_back(new Gtk::DrawingArea());
  vdraw.back()->set_size_request(100,100);
  m_tab.attach(*vdraw.back(),x, x+1, y, y+1 );
  
  vdraw.back()->signal_expose_event().connect( sigc::bind(sigc::mem_fun(*this, &SelectWindow::on_surface_expose_event_func) ,vdraw.size()-1));

	vdraw.back()->signal_button_press_event().connect( sigc::bind(sigc::mem_fun(*this, &SelectWindow::on_surface_button_press_event_func) ,vdraw.size()-1));

	vdraw.back()->add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);

	
   std::ostringstream os;
   //os<<"Surface "<<vdraw.size();
	

   Gtk::Label* L = new Gtk::Label(R[vdraw.size()-1].name,Gtk::ALIGN_LEFT,Gtk::ALIGN_TOP);


   
  m_tab.attach(*L,x, x+1, y+1, y+2 );

  }
  




show_all_children();

  }



bool SelectWindow::on_surface_expose_event_func(GdkEventExpose*, int index)
{
	

  Glib::RefPtr<Gdk::Window> window = vdraw[index]->get_window();
		
	
	if(window)
	{
	Glib::RefPtr<Gdk::GC> some_gc = Gdk::GC::create(get_window());
	//some_gc.create(get_window());
	
	Glib::RefPtr<Gdk::Drawable> dr(window);
	
	try{
	Glib::RefPtr<Gdk::Pixbuf> surface = Gdk::Pixbuf::create_from_file(R[index].thumbnail);
	Glib::RefPtr<Gdk::Pixbuf> sface = surface->scale_simple(100,100,Gdk::INTERP_BILINEAR);
	dr->draw_pixbuf(some_gc,sface,0,0,20,20,-1,-1,Gdk::RGB_DITHER_NONE,0,0);
	}
	catch(...){}	

	}

	return true;
}



void make_thumb(parsepic_out data,const std::string& script, const std::string& image, const int size, const surfer_options& opt)
{
	
		
		
	
	
	// coordinates for the center of the window
	
	//std::cout<<"Thumbing "<<data.name<<" to "<<image<<std::endl;
	
	std::ofstream f(script.c_str(),FILE_WRITE_MODE);
	
/*	f<<"rot_x="<<data.initial_x+current_x<<";\n";
	f<<"rot_y="<<data.initial_y+current_y<<";\n";
	f<<"rot_z="<<data.initial_z+current_z<<";\n";
*/
	//rot_data t = rot_yxz(rotmat);
	//f<<"rot_x="<<t.rot_x<<";\n";
	//f<<"rot_y="<<t.rot_y<<";\n";
	//f<<"rot_z="<<t.rot_z<<";\n";

	
	f<<"width="<<size<<";\n";
	f<<"height="<<size<<";\n";
	

	f<<"double a = "<<data.para_a<<";\n";
	f<<"double b = "<<data.para_b<<";\n";

	f<<"poly u="<<data.rot.el(1,1)<<"*x+("<<data.rot.el(1,2)<<")*y+("<<data.rot.el(1,3)<<")*z;\n";
	f<<"poly v="<<data.rot.el(2,1)<<"*x+("<<data.rot.el(2,2)<<")*y+("<<data.rot.el(2,3)<<")*z;\n";
	f<<"poly w="<<data.rot.el(3,1)<<"*x+("<<data.rot.el(3,2)<<")*y+("<<data.rot.el(3,3)<<")*z;\n";

	f<<data.general_stuff;

	f<<data.antialiasing<<"\n";
	//f<<"surface="<<data.equation<<";\n";
	f<<"surface="<< data.equation <<";\n";
	f<<"scale_x="<<data.initial_scale<<";\n";
	f<<"scale_y="<<data.initial_scale<<";\n";
	f<<"scale_z="<<data.initial_scale<<";\n";

	f<<"surface_red="<<data.upside.red<<";\n";
	f<<"inside_red="<<data.inside.red<<";\n";
	f<<"surface_green="<<data.upside.green<<";\n";
	f<<"inside_green="<<data.inside.green<<";\n";
	f<<"surface_blue="<<data.upside.blue<<";\n";
	f<<"inside_blue="<<data.inside.blue<<";\n";
	//f<<"transparency="<<50<<";\n";


	f<<"background_red="<<255<<";\n";
	f<<"background_green="<<255<<";\n";
	f<<"background_blue="<<255<<";\n";


	f<<"filename=\""<<image<<"\";\n";
	f<<"color_file_format=ppm;\n";
	f<<"draw_surface;\n";
	f<<"save_color_image;\n";
	f.close();
	system((opt.surf_cmd+" -n "+script+" " REDIRECTION_APEX).c_str());
	


}



void make_thumbs(const std::vector<parsepic_out>& R, const surfer_options& opt)
{

	for(unsigned i=0; i<R.size(); i++)
	{
		make_thumb(R[i],TEMP_ROOT_SEP +"surfb_thumb_tmp.pic",R[i].thumbnail,100,opt);
	}
}




bool SelectWindow::on_surface_button_press_event_func(GdkEventButton* event, int index)
{

  // This is where we draw on the window

	res = R[index];
	hide();

 return true;
}
