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




void make_thumb(parse_result data,const std::string& script, const std::string& image, const int size, const surfer_options& opt)
{
	
		
		
	
	
	std::ofstream f(script.c_str(),FILE_WRITE_MODE);
	
	
	f<<"width="<<size<<";\n";
	f<<"height="<<size<<";\n";
	


	write_surf(data.global_data,f);
	for(unsigned k = 0; k < data.data.size(); k++)
	write_surf(data.data[k],k,f);
	

	f<<"filename=\""<<image<<"\";\n";
	f<<"color_file_format=ppm;\n";
	f<<"draw_surface;\n";
	f<<"save_color_image;\n";
	f.close();
	system((opt.surf_cmd+" -n \""+script+"\" " +REDIRECTION_APEX).c_str());
	


}



void make_thumbs(const std::vector<parse_result>& R, const surfer_options& opt)
{

	for(unsigned i=0; i<R.size(); i++)
	{
		make_thumb(R[i],TEMP_ROOT_SEP +"surfb_thumb_tmp.pic",thumbnail(R[i]),100,opt);
	}
}





color transform_color(const Gdk::Color& g)
{
color c;
c.red = int(g.get_red_p()*255);
c.blue = int(g.get_blue_p()*255);
c.green = int(g.get_green_p()*255);
return c;
}


Gdk::Color transform_color(const color& c)
{
Gdk::Color g;
g.set_rgb_p(c.red/255.0,c.green/255.0,c.blue/255.0);
return g;
}

void SpecialEffects::on_c1_changed_func()
{
	if(refreshing) return;
	gui.data[gui.data_index].outside = transform_color(m_c1.get_current_color());
	gui.pichange = 15;
}


void SpecialEffects::on_c2_changed_func()
{
	if(refreshing) return;
	gui.data[gui.data_index].inside = transform_color(m_c2.get_current_color());
	gui.pichange = 15;
}


void SpecialEffects::on_cb_changed_func()
{
	if(refreshing) return;
	gui.global_data.background = transform_color(m_cb.get_current_color());
	gui.pichange = 15;
}

void SpecialEffects::on_trans_changed_func()
{
	if(refreshing) return;
	m_transp.update();
	gui.data[gui.data_index].transparence = m_transp.get_value_as_int();
	gui.pichange = 15;
}


void SpecialEffects::on_thick_changed_func()
{
	if(refreshing) return;
	m_thick.update();
	gui.data[gui.data_index].thickness = m_thick.get_value_as_int();
	gui.pichange = 15;
}


void SpecialEffects::on_smooth_changed_func()
{
	if(refreshing) return;
	m_smooth.update();
	gui.data[gui.data_index].smoothness = m_smooth.get_value_as_int();
	gui.pichange = 15;
}


void SpecialEffects::on_ambient_changed_func()
{
	if(refreshing) return;
	m_ambient.update();
	gui.data[gui.data_index].ambient = m_ambient.get_value_as_int();
	gui.pichange = 15;
}


void SpecialEffects::on_diffuse_changed_func()
{
	if(refreshing) return;
	m_diffuse.update();
	gui.data[gui.data_index].diffuse = m_diffuse.get_value_as_int();
	gui.pichange = 15;
}


void SpecialEffects::on_reflected_changed_func()
{
	if(refreshing) return;
	m_reflected.update();
	gui.data[gui.data_index].reflected = m_reflected.get_value_as_int();
	gui.pichange = 15;
}

SpecialEffects::SpecialEffects(SurfBWindow& g)
:gui(g),refreshing(false),

m_ltransp(_("Transparency")),
m_lthick(_("Thickness")),
m_lsmooth(_("Smoothness")),
m_lambient(_("Ambient light")),
m_ldiffuse(_("Diffuse light")),
m_lreflected(_("Reflected light")),
m_res_lsave(_("Resolution of saved image")),
m_res_lfast(_("Resolution while rotating")),
m_res_lfine(_("Maximal resolution when idle")),
m_res_laa(_("Anti-aliasing factor")),
m_pos_langle(_("angle")),
m_pos_rot_object_x(_("rotate around x-axis of surface")),
m_pos_rot_object_y(_("rotate around y-axis of surface")),
m_pos_rot_object_z(_("rotate around z-axis of surface")),
m_pos_rot_camera_x(_("rotate around x-axis of camera")),
m_pos_rot_camera_y(_("rotate around y-axis of camera")),
m_pos_rot_camera_z(_("rotate around z-axis of camera")),
m_pos_default(_("rotate into default position"))
{
	add(m_note);
	
	set_title(_("Scene Properties - Surfer"));

	m_note.append_page(m_c1,_("Color 1"));
	m_note.append_page(m_c2,_("Color 2"));
	m_note.append_page(m_cb,_("Background color"));
	m_note.append_page(m_light,_("Illumination"));
	m_note.append_page(m_mat,_("Material"));
	if(false) m_note.append_page(m_pos,_("Position"));
        m_note.append_page(m_res,_("Resolution"));
	m_note.append_page(m_surf,_("Special Surf-Code"));

	m_surf.attach(m_code,0,1,0,1);
	

	m_res.attach(m_res_lsave,0,1,0,1);
	m_res.attach(m_res_save,1,2,0,1);

	m_res.attach(m_res_lfast,0,1,1,2);
	m_res.attach(m_res_fast,1,2,1,2);

	m_res.attach(m_res_lfine,0,1,2,3);
	m_res.attach(m_res_fine,1,2,2,3);

	m_res.attach(m_res_laa,0,1,3,4);
	m_res.attach(m_res_aa,1,2,3,4);


	m_res_save.set_range(1,3000);
	
	m_res_save.set_increments(10,100);
	

	m_res_fast.set_range(1,1000);
	
	m_res_fast.set_increments(1,10);

	m_res_fine.set_range(1,3000);
	
	m_res_fine.set_increments(10,100);

	m_res_aa.set_range(1,6);
	m_res_aa.set_increments(1,1);

	m_res_save.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_res_changed));
	m_res_fast.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_res_changed));
	m_res_fine.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_res_changed));
	m_res_aa.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_res_changed));

	
	m_c1.signal_color_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_c1_changed_func) );
	m_c2.signal_color_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_c2_changed_func) );
	m_cb.signal_color_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_cb_changed_func) );

	m_code.get_buffer()->signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_code_changed_func) );
	
	
	m_mat.attach(m_ltransp,0,1,0,1);
	m_mat.attach(m_transp,1,2,0,1);

	
	m_mat.attach(m_lthick,0,1,1,2);
	m_mat.attach(m_thick,1,2,1,2);

	
	m_mat.attach(m_lsmooth,0,1,2,3);
	m_mat.attach(m_smooth,1,2,2,3);

	
	m_mat.attach(m_lambient,0,1,3,4);
	m_mat.attach(m_ambient,1,2,3,4);

	
	m_mat.attach(m_ldiffuse,0,1,4,5);
	m_mat.attach(m_diffuse,1,2,4,5);

	
	m_mat.attach(m_lreflected,0,1,5,6);
	m_mat.attach(m_reflected,1,2,5,6);






	m_transp.set_range(0,100);
	
	m_transp.set_increments(1,10);

	m_thick.set_range(0,100);
	
	m_thick.set_increments(1,10);

	m_smooth.set_range(0,100);
	
	m_smooth.set_increments(1,10);

	m_ambient.set_range(0,100);
	
	m_ambient.set_increments(1,10);

	m_diffuse.set_range(0,100);
	
	m_diffuse.set_increments(1,10);

	m_reflected.set_range(0,100);
	m_reflected.set_increments(1,10);


	m_lights.set_tab_pos(Gtk::POS_LEFT);

	m_light.attach(m_lights,0,1,0,1);
	for(unsigned i = 0; i < 9; i++)
	{
		std::ostringstream s;
		s<<(i+1);
		m_lights.append_page(m_alight[i],_("Light")+(" "+s.str()));
		

		m_alight[i].signal_light_changed().connect(sigc::bind(sigc::mem_fun(*this,&SpecialEffects::on_light_changed),i));
	}

	m_transp.signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_trans_changed_func) );
	m_thick.signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_thick_changed_func) );
	m_smooth.signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_smooth_changed_func) );
	m_ambient.signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_ambient_changed_func) );
	m_diffuse.signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_diffuse_changed_func) );
	m_reflected.signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_reflected_changed_func) );

	

	show_all_children();
}

void SpecialEffects::recapture()
{
refreshing = true;
m_code.get_buffer()->set_text(gui.global_data.general_stuff);

for(unsigned i = 0; i < 9; i++)
	{
		m_alight[i].set_light(gui.global_data.light[i]);

		
	}



	m_transp.set_value(gui.data[gui.data_index].transparence);
	m_thick.set_value(gui.data[gui.data_index].thickness);
	m_smooth.set_value(gui.data[gui.data_index].smoothness);
	m_ambient.set_value(gui.data[gui.data_index].ambient);
	m_diffuse.set_value(gui.data[gui.data_index].diffuse);
	m_reflected.set_value(gui.data[gui.data_index].reflected);
	

	m_c1.set_current_color(transform_color(gui.data[gui.data_index].outside));
	m_c2.set_current_color(transform_color(gui.data[gui.data_index].inside));
	m_cb.set_current_color(transform_color(gui.global_data.background));

	m_res_save.set_value(gui.global_data.saveres);
	m_res_fast.set_value(gui.global_data.lores);
	m_res_fine.set_value(gui.global_data.hires);
	m_res_aa.set_value(gui.global_data.antialiasing);
	

refreshing = false;
}


void LightWidget::set_light(const surf_light& L)
{
	m_vol.set_value(L.volume);
	m_x.set_value(L.x);
	m_y.set_value(L.y);
	m_z.set_value(L.z);
	
	m_color.set_current_color(transform_color(L.color));
	
}

surf_light LightWidget::get_light() const
{
	
	surf_light L;

	L.volume = m_vol.get_value_as_int();
	L.x = m_x.get_value();
	L.y = m_y.get_value();
	L.z = m_z.get_value();

	L.color = transform_color(m_color.get_current_color());

	return L;

}

LightWidget::LightWidget()
:m_lcolor(_("color of light")),
m_lvol(_("intensity of light")),
m_lx(_("x coordinate")),
m_ly(_("y coordinate")),
m_lz(_("z coordinate"))
{

	m_x.set_range(-1000,1000);
	m_y.set_range(-1000,1000);
	m_z.set_range(-1000,1000);
	m_vol.set_range(0,100);

	m_x.set_increments(1,10);
	m_y.set_increments(1,10);
	m_z.set_increments(1,10);
	m_vol.set_increments(1,10);

	set_light(global_defaults().light[0]);

	attach(m_lcolor,0,1,0,1);
	attach(m_color,0,1,1,9);

	attach(m_sep,1,2,1,9);

	attach(m_lvol,2,3,0,1);
	attach(m_vol,2,3,1,2);

	attach(m_lx,2,3,2,3);
	attach(m_x,2,3,3,4);

	attach(m_ly,2,3,5,6);
	attach(m_y,2,3,6,7);

	attach(m_lz,2,3,7,8);
	attach(m_z,2,3,8,9);

	m_color.signal_color_changed().connect(sigc::mem_fun(*this,&LightWidget::on_light_changed));
	m_vol.signal_changed().connect(sigc::mem_fun(*this,&LightWidget::on_light_changed));
	m_x.signal_changed().connect(sigc::mem_fun(*this,&LightWidget::on_light_changed));
	m_y.signal_changed().connect(sigc::mem_fun(*this,&LightWidget::on_light_changed));
	m_z.signal_changed().connect(sigc::mem_fun(*this,&LightWidget::on_light_changed));

	show_all_children();

}

std::ostream& operator<<(std::ostream& o, const surf_light& L)
{
	return o<<"[("<<L.color.red<<","<<L.color.green<<","<<L.color.blue<<"),"<<L.volume<<",("<<L.x<<","<<L.y<<","<<L.z<<")]";
}

void SpecialEffects::on_light_changed(unsigned i)
{
	if(refreshing) return;	
	gui.global_data.light[i] = m_alight[i].get_light();

	gui.pichange = 5;
}

void SpecialEffects::on_code_changed_func()
{
	if(refreshing) return;
	gui.global_data.general_stuff = m_code.get_buffer()->get_text();

	gui.pichange = 15;
}

void SpecialEffects::on_res_changed()
{
	if(refreshing) return;
	m_res_save.update();
	m_res_fast.update();
	m_res_fine.update();
	m_res_aa.update();

	gui.global_data.hires = m_res_fine.get_value_as_int();
	gui.global_data.lores = m_res_fast.get_value_as_int();
	gui.global_data.saveres=m_res_save.get_value_as_int();
	gui.global_data.antialiasing=m_res_aa.get_value_as_int();
	
	gui.pichange=5;
}
