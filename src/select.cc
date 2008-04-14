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
	log_system((opt.surf_cmd+QUIET_SURF+" -n \""+script+"\" " +REDIRECTION_APEX).c_str());
	


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

	if(m_apply.get_active())
	gui.pichange = 15;
}


void SpecialEffects::on_c2_changed_func()
{
	if(refreshing) return;
	gui.data[gui.data_index].inside = transform_color(m_c2.get_current_color());
	
	if(m_apply.get_active())
	gui.pichange = 15;
}


void SpecialEffects::on_cb_changed_func()
{
	if(refreshing) return;
	gui.global_data.background = transform_color(m_cb.get_current_color());

	if(m_apply.get_active())
	gui.pichange = 15;
}

void SpecialEffects::on_trans_changed_func()
{
	if(refreshing) return;
	m_transp.update();
	gui.data[gui.data_index].transparence = m_transp.get_value_as_int();

	if(m_apply.get_active())
	gui.pichange = 15;
}


void SpecialEffects::on_thick_changed_func()
{
	if(refreshing) return;
	m_thick.update();
	gui.data[gui.data_index].thickness = m_thick.get_value_as_int();

	if(m_apply.get_active())
	gui.pichange = 15;
}


void SpecialEffects::on_smooth_changed_func()
{
	if(refreshing) return;
	m_smooth.update();
	gui.data[gui.data_index].smoothness = m_smooth.get_value_as_int();

	if(m_apply.get_active())
	gui.pichange = 15;
}


void SpecialEffects::on_ambient_changed_func()
{
	if(refreshing) return;
	m_ambient.update();
	gui.data[gui.data_index].ambient = m_ambient.get_value_as_int();

	if(m_apply.get_active())
	gui.pichange = 15;
}


void SpecialEffects::on_diffuse_changed_func()
{
	if(refreshing) return;
	m_diffuse.update();
	gui.data[gui.data_index].diffuse = m_diffuse.get_value_as_int();

	if(m_apply.get_active())
	gui.pichange = 15;
}


void SpecialEffects::on_reflected_changed_func()
{
	if(refreshing) return;
	m_reflected.update();
	gui.data[gui.data_index].reflected = m_reflected.get_value_as_int();

	if(m_apply.get_active())
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
m_lrotx(_("rotate around x-axis")),
m_lroty(_("rotate around y-axis")),
m_lrotz(_("rotate around z-axis")),
m_lres_video(_("Video resolution")),
m_lframe_video(_("Video frame rate")),
m_lbitrate_video(_("Video bit rate")),
m_apply(_("apply changes immediately"))

{
	add(m_tab);
	m_tab.attach(m_apply,0,1,0,1);
	m_tab.attach(m_note,0,1,1,2);
	m_apply.set_active(true);
	set_title(_("Scene Properties - Surfer"));

	m_note.append_page(m_c1,_("Color 1"));
	m_note.append_page(m_c2,_("Color 2"));
	m_note.append_page(m_cb,_("Background color"));
	m_note.append_page(m_light,_("Illumination"));
	m_note.append_page(m_mat,_("Material"));
	m_note.append_page(m_pos,_("Position"));
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


	m_res.attach(m_lres_video,0,1,4,5);
	m_res.attach(m_res_video,1,2,4,5);

	m_res.attach(m_lbitrate_video,0,1,5,6);
	m_res.attach(m_bitrate_video,1,2,5,6);

	m_res.attach(m_lframe_video,0,1,6,7);
	m_res.attach(m_frame_video,1,2,6,7);


	m_res_save.set_range(1,3000);
	m_res_video.set_range(1,3000);

	m_bitrate_video.set_range(1,32000);
	m_frame_video.set_range(1,100);
	
	m_res_save.set_increments(10,100);
	m_res_video.set_increments(10,100);
	m_bitrate_video.set_increments(100,1000);
	m_frame_video.set_increments(1,10);

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
	m_res_video.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_res_changed));
	m_frame_video.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_res_changed));
	m_bitrate_video.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_res_changed));
	
	m_c1.signal_color_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_c1_changed_func) );
	m_c2.signal_color_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_c2_changed_func) );
	m_cb.signal_color_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_cb_changed_func) );

	m_code.get_buffer()->signal_changed().connect( sigc::mem_fun(*this, &SpecialEffects::on_code_changed_func) );
	
	m_apply.signal_toggled().connect(sigc::mem_fun(*this,&SpecialEffects::on_apply_toggle));
	
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

	
	m_pos.attach(m_lrotx,0,1,0,1);
	m_pos.attach(m_rotx,1,2,0,1);

	m_pos.attach(m_lroty,0,1,1,2);
	m_pos.attach(m_roty,1,2,1,2);

	m_pos.attach(m_lrotz,0,1,2,3);
	m_pos.attach(m_rotz,1,2,2,3);

	m_rotx.set_range(-360,360);
	m_rotx.set_increments(1,1);

	m_roty.set_range(-360,360);
	m_roty.set_increments(1,1);

	m_rotz.set_range(-360,360);
	m_rotz.set_increments(1,1);



	m_rotx.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_rot_change));
	m_roty.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_rot_change));
	m_rotz.signal_changed().connect(sigc::mem_fun(*this,&SpecialEffects::on_rot_change));

	show_all_children();
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

	if(m_apply.get_active())
	gui.pichange = 5;
}

void SpecialEffects::on_code_changed_func()
{
	if(refreshing) return;
	gui.global_data.general_stuff = m_code.get_buffer()->get_text();

	if(m_apply.get_active())
	gui.pichange = 15;
}

void SpecialEffects::on_res_changed()
{
	if(refreshing) return;
	m_res_save.update();
	m_res_fast.update();
	m_res_fine.update();
	m_res_aa.update();
	m_res_video.update();
	m_bitrate_video.update();
	m_frame_video.update();

	gui.global_data.hires = m_res_fine.get_value_as_int();
	gui.global_data.lores = m_res_fast.get_value_as_int();
	gui.global_data.saveres=m_res_save.get_value_as_int();
	gui.global_data.antialiasing=m_res_aa.get_value_as_int();

	gui.opt.video_resolution=m_res_video.get_value_as_int();
	gui.opt.video_frame_rate=m_frame_video.get_value_as_int();
	gui.opt.video_bitrate=m_bitrate_video.get_value_as_int();
	
	if(m_apply.get_active())
	gui.pichange=5;
}

void SpecialEffects::on_apply_toggle()
{
	if(refreshing) return;
	
	
	if(m_apply.get_active())
	gui.pichange=5;
}

void mat2ang(const matrix<double>& M, double& w1, double& w2, double& w3)
{
/*
xyz
M = c2c3, c3s1s2-c1s3, c1c3s2+s1s3
    c2s3, c1c3+s1s2s3, c1s2s3-c3s1
    -s2 , c2s1,        c1c2

*/

w3 = atan2(M.el(2,1),M.el(1,1));
w2 = asin(-M.el(3,1));
w1 = atan2(M.el(3,2),M.el(3,3));


}

matrix<double> ang2mat(const double w1, const double w2, const double w3)
{
	
	return rotz(w3)*roty(w2)*rotx(w1);
}

const double PI = 3.14159265358979323846264338327950288419716939937510;

void SpecialEffects::on_rot_change()
{
	if(refreshing) return;

	m_rotx.update();
	m_roty.update();
	m_rotz.update();

	gui.global_data.rot = ang2mat(m_rotx.get_value()*PI/180,m_roty.get_value()*PI/180, m_rotz.get_value()*PI/180);

	if(m_apply.get_active()) gui.pichange = 15;
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

	m_res_video.set_value(gui.opt.video_resolution);
	m_frame_video.set_value(gui.opt.video_frame_rate);
	m_bitrate_video.set_value(gui.opt.video_bitrate);

	double w1 = 0;
	double w2 = 0;
	double w3 = 0;

	mat2ang(gui.global_data.rot,w1,w2,w3);

	w1 *= 180/PI;
	w2 *= 180/PI;
	w3 *= 180/PI;

	m_rotx.set_value(w1);
	m_roty.set_value(w2);
	m_rotz.set_value(w3);
	

refreshing = false;
}
