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

#include "bwindow_gdk.hh"
#include "matrix.hh"

#include <sys/stat.h>


#include "vecmath.h"
/*

rotation in surf


y before x before z

*/

matrix<double> rotx(const double a)
{
	matrix<double> M(3,0.0);
	M.el(1,1) = 1.0;
	M.el(2,2) = cos(a);
	M.el(2,3) = -sin(a);
	M.el(3,2) = sin(a);
	M.el(3,3) = cos(a);

	return M;
}


matrix<double> rotz(const double a)
{
	matrix<double> M(3,0.0);
	M.el(3,3) = 1.0;
	M.el(1,1) = cos(a);
	M.el(1,2) = -sin(a);
	M.el(2,1) = sin(a);
	M.el(2,2) = cos(a);

	return M;
}


matrix<double> roty(const double a)
{
	matrix<double> M(3,0.0);
	M.el(2,2) = 1.0;
	M.el(1,1) = cos(a);
	M.el(1,3) = sin(a);
	M.el(3,1) = -sin(a);
	M.el(3,3) = cos(a);

	return M;
}




double arcus_sin_cos(double s, double c)
{
	double a = acos(c);
	if(fabs(sin(a) - s)>=EPS) a = -a;
	return a;
}

rot_data rot_yxz(const matrix<double> M)
{

	rot_data R;

	double sb = M.el(3,2);
	
	double cb = sqrt(1-sb*sb);

	if(cb>=EPS)
	{
		double ca = M.el(2,2)/cb;
		double sa = -M.el(2,1)/cb;

		double cg = M.el(3,3)/cb;
		double sg = -M.el(3,1)/cb;

		//check sign consistency - maybe cb := -cb

		//I think this works
		R.rot_x = arcus_sin_cos(sb,cb);
		R.rot_y = arcus_sin_cos(sg,cg);
		R.rot_z = arcus_sin_cos(sa,ca);
	}
	else
	{
		if(fabs(sb +1)>=EPS) //sb == 1
		{
			R.rot_x = acos(0.0);
			R.rot_y = 0.0;
			R.rot_z = arcus_sin_cos(M.el(1,3),M.el(1,1));
		}
		else //sb == -1
		{
			R.rot_x = -acos(0.0);
			R.rot_y = 0.0;
			R.rot_z = arcus_sin_cos(M.el(1,3),-M.el(1,1));
		}
	}

	return R;
}

matrix<double> interpolate(const matrix<double>& P, const matrix<double>& Q, double t)
{

	Matrix3<double> P3(P.el(1,1),P.el(1,2),P.el(1,3),P.el(2,1),P.el(2,2),P.el(2,3),P.el(3,1),P.el(3,2),P.el(3,3));

	Matrix3<double> Q3(Q.el(1,1),Q.el(1,2),Q.el(1,3),Q.el(2,1),Q.el(2,2),Q.el(2,3),Q.el(3,1),Q.el(3,2),Q.el(3,3));

	Quat4<double> p;
	p.set(P3);
	Quat4<double> q;
	q.set(Q3);

	Quat4<double> qt;
	qt.interpolate(p,q,t);

	Matrix3<double> Qt3;
	Qt3.set(qt);

	matrix<double> Qt(3);

	Qt.el(1,1)	 = Qt3.m00;
	Qt.el(1,2)	 = Qt3.m01;
	Qt.el(1,3)	 = Qt3.m02;

	Qt.el(2,1)	 = Qt3.m10;
	Qt.el(2,2)	 = Qt3.m11;
	Qt.el(2,3)	 = Qt3.m12;

	Qt.el(3,1)	 = Qt3.m20;
	Qt.el(3,2)	 = Qt3.m21;
	Qt.el(3,3)	 = Qt3.m22;

	return Qt;
}


double interpolate(double P, double Q, double t)
{
	double Qt = t*P+(1-t)*Q;
	return Qt;
}


int interpolate(int P, int Q, double t)
{
	int Qt = int(t*P+(1-t)*Q);
	return Qt;
}


double exp_interpolate(double P, double Q, double t)
{
	return exp(interpolate(log(P),log(Q),t));
}

color interpolate(const color& P, const color& Q, double t)
{
	color Qt;

	Qt.red = interpolate(P.red,Q.red,t);
	Qt.green = interpolate(P.green,Q.green,t);
	Qt.blue = interpolate(P.blue,Q.blue,t);

	return Qt;
}

surf_light interpolate(const surf_light& P, const surf_light& Q, double t)
{
	surf_light Qt;

	Qt.color = interpolate(P.color,Q.color,t);
	Qt.x = interpolate(P.x,Q.x,t);
	Qt.y = interpolate(P.y,Q.y,t);
	Qt.z = interpolate(P.z,Q.z,t);

	Qt.volume = interpolate(P.volume,Q.volume,t);

	return Qt;
}

global_parse interpolate(const global_parse& P, const global_parse& Q, double t)
{
	global_parse Qt;
	Qt.background = interpolate(P.background,Q.background,t);
	Qt.spec_z = interpolate(P.spec_z,Q.spec_z,t);

	Qt.initial_scale = exp_interpolate(P.initial_scale,Q.initial_scale,t);
	Qt.scale = exp_interpolate(P.scale,Q.scale,t);

	Qt.normalize_factor = interpolate(P.normalize_factor,Q.normalize_factor,t);

	Qt.normalize_brightness = P.normalize_brightness || Q.normalize_brightness;
	Qt.depth_cueing = P.depth_cueing || Q.depth_cueing;
	
	Qt.depth_value = interpolate(P.depth_value,Q.depth_value,t);

	for(unsigned i = 0; i < 4; i++)
	Qt.para[i] = interpolate(P.para[i],Q.para[i],t);

	Qt.illumination = P.illumination;
	
	for(unsigned i = 0; i < 9; i++)
	Qt.light[i] = interpolate(P.light[i],Q.light[i],t);
	
	Qt.antialiasing = P.antialiasing;

	Qt.rot = interpolate(P.rot,Q.rot,t);
	
	Qt.lores = P.lores;
	Qt.hires = P.hires;
	Qt.saveres = P.saveres;
	
	Qt.general_stuff = P.general_stuff; //seperately add clock
	
	return Qt;
}

surface_data interpolate(const surface_data& P, const surface_data& Q, double t)
{
	surface_data Qt;

	Qt.outside = interpolate(P.outside,Q.outside,t);
	Qt.inside = interpolate(P.inside,Q.inside,t);

	Qt.ambient = interpolate(P.ambient,Q.ambient,t);
	Qt.diffuse = interpolate(P.diffuse,Q.diffuse,t);
	Qt.transmitted = interpolate(P.transmitted,Q.transmitted,t);
	Qt.reflected = interpolate(P.reflected,Q.reflected,t);

	Qt.smoothness = interpolate(P.smoothness,Q.smoothness,t);
	Qt.transparence = interpolate(P.transparence,Q.transparence,t);
	Qt.thickness = interpolate(P.thickness,Q.thickness,t);

	Qt.equation = P.equation;
	Qt.public_eq = P.public_eq;

	return Qt;
}

parse_result interpolate(const parse_result& P, const parse_result& Q, double t)
{

	parse_result Qt;

	Qt.global_data = interpolate(P.global_data,Q.global_data,t);

	for(unsigned i = 0; i < P.data.size() && i < P.data.size(); i++)
	Qt.data.push_back(interpolate(P.data[i],Q.data[i],t));

	return Qt;
}






AniWindow::AniWindow(SurfBWindow& g)
:gui(g),fcount(0),m_record(Gtk::Stock::MEDIA_PLAY),m_lres(_("Resolution")),m_movie_frame(-1)
{
m_refListStore = Gtk::ListStore::create(m_Columns);

m_TreeView.set_model(m_refListStore);

m_TreeView.append_column(_("Position"),m_Columns.m_col_number);
m_TreeView.append_column(_("Preview"),m_Columns.m_col_pic);
m_TreeView.append_column_editable(_("Duration"),m_Columns.m_col_duration);



/*
parse_result p;
p.data = gui.data;
p.global_data = gui.global_data;

add_entry(p,10,1);
*/

set_title(_("Create Animation"));

add(m_tab);

m_tab.attach(m_TreeView,0,3,2,3);

m_tab.attach(m_record,0,1,0,1,Gtk::SHRINK,Gtk::SHRINK);
m_tab.attach(m_lres,1,2,0,1,Gtk::SHRINK,Gtk::SHRINK);
m_tab.attach(m_res,2,3,0,1,Gtk::SHRINK,Gtk::SHRINK);

m_tab.attach(m_preview,0,3,1,2,Gtk::SHRINK,Gtk::SHRINK);

m_record.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::on_record));

m_preview.set_size_request(100,100);

	m_res.set_range(1,3000);
	
	m_res.set_increments(10,100);
	m_res.set_value(100);

m_TreeView.set_size_request(400,500);

sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &AniWindow::on_timer),0);

// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, 1000/25);


show_all_children();

}

const int ANI_PREVIEW_SIZE = 48;

Glib::RefPtr<Gdk::Pixbuf> create_ani_thumb(const parse_result& P,const surfer_options& opt)
{


	
		std::string ti = TEMP_ROOT_SEP+"surfb_at.ppm";

		std::string ts = TEMP_ROOT_SEP+"surfb_at.pic";

		
		std::ofstream f(ts.c_str(),FILE_WRITE_MODE);
	
	
	f<<"// This file is intended for use with surf (http://surf.sf.net/)"<<"\n";
	f<<"// or Surfer (http://surfer.imaginary2008.de/)"<<"\n";
	f<<"// This file was generated by Surfer."<<"\n\n";

	f<<"width="<< ANI_PREVIEW_SIZE <<";\n";
	f<<"height="<< ANI_PREVIEW_SIZE <<";\n";
	
	

	write_surf(P.global_data,f,0);
	for(unsigned k = 0; k < P.data.size(); k++)
	{
		write_surf(P.data[k],k,f);
	
	}

	
	
	

	//f<<"transparency="<<50<<";\n";
	f<<"filename=\""<<ti<<"\";\n";
	f<<"color_file_format="+opt.format+";\n";
	f<<"draw_surface;\n";
	f<<"save_color_image;\n";
	f.close();
	

	
    			
    //} else {
        
        //gtk_widget_set_sensitive(this->window, true);
    //}
		
	
	
	log_system((opt.surf_cmd+" -n \""+ts+"\" " +REDIRECTION_APEX).c_str());
	Glib::RefPtr<Gdk::Pixbuf> img;
		try{
		
		img = Gdk::Pixbuf::create_from_file(ti);
		}
		catch(...)
		{}
	return img;
}

void AniWindow::add_entry(const parse_result& P,
        unsigned duration , int idx)
{
  Gtk::TreeModel::Row row = *(m_refListStore->append());
  row[m_Columns.m_col_duration] = duration;
  row[m_Columns.m_col_state] = P;
  row[m_Columns.m_col_pic] = create_ani_thumb(P,gui.opt);
  

  row[m_Columns.m_col_number] = idx;

}

void AniWindow::ani_add()
{

parse_result p;
p.data = gui.data;
p.global_data = gui.global_data;

add_entry(p,25,++fcount);


}


bool AniWindow::on_timer(int)
{
	if(m_movie_frame==m_movie_file.size()) m_movie_frame = 0;
	if (m_movie_frame<0) {return true;}

	
	std::cout<<"now playing "<<m_movie_frame<<": "<<m_movie_file[m_movie_frame]<<std::endl;
	m_preview.set(Gdk::Pixbuf::create_from_file(m_movie_file[m_movie_frame]));

	m_movie_frame++;

	return true;
}


/*
Gtk::Image m_preview;

std::vector<Glib::RefPtr<Gdk::Pixbuf> > m_movie;
int m_movie_frame;

Gtk::Button m_record;
*/


void AniWindow::on_record()
{

	std::vector<std::string> v_script;
	std::vector<std::string> v_image;

	std::vector<unsigned> v_dur;
	std::vector<parse_result> v_sta;

	
	Gtk::TreeModel::iterator irow = m_refListStore->children().begin();

	for(;irow != m_refListStore->children().end(); ++irow)
	{
		v_dur.push_back((*irow)[m_Columns.m_col_duration]);
		v_sta.push_back((*irow)[m_Columns.m_col_state]);
	}


	std::vector<parse_result> intermediate;
	
	for(unsigned i = 0; i+1 < v_sta.size(); i++)
	{
		intermediate.push_back(v_sta[i]);

		for(unsigned j = 1; j < v_dur[i]; j++)
		{
			intermediate.push_back(interpolate(v_sta[i],v_sta[i+1],double(j)/v_dur[i]));
		}
	}
	intermediate.push_back(v_sta.back());

	m_movie_frame = -1;
	m_movie.clear();

	for(unsigned i = 0; i < intermediate.size(); i++)
	{
		
		mkdir((TEMP_ROOT_SEP+"ani").c_str(), S_IRWXU | S_IRWXG | S_IROTH );
		
		
		
		std::ostringstream os;
		os<<i;

		v_script.push_back(TEMP_ROOT_SEP+"ani"+DIR_SEP+"frame"+os.str()+".pic");
		v_image.push_back(TEMP_ROOT_SEP+"ani"+DIR_SEP+"frame"+os.str()+".ppm");

		std::ofstream f(v_script[i].c_str());

		f<<"width="<<m_res.get_text()<<";\n";
		f<<"height="<<m_res.get_text()<<";\n";

			
		write_surf(intermediate[i].global_data,f,0);
		for(unsigned k = 0; k < intermediate[i].data.size(); k++)
		{
			write_surf(intermediate[i].data[k],k,f);
		
		}
	
		
		
		
	
		//f<<"transparency="<<50<<";\n";
		f<<"filename=\""<<(TEMP_ROOT_SEP+"ani"+DIR_SEP+"frame"+os.str()+".ppm")<<"\";\n";
		f<<"color_file_format="+gui.opt.format+";\n";
		f<<"draw_surface;\n";
		f<<"save_color_image;\n";
		f.close();		

		std::cout<<"finished file "<<i<<std::endl;
	}

        //#pragma omp parallel for
	for(unsigned i = 0; i < intermediate.size(); i++)
	{
		log_system(gui.opt.surf_cmd+" -n \""+v_script[i]+"\" " + REDIRECTION_APEX);
	}

	for(unsigned i = 0; i < intermediate.size(); i++)
	{
		m_movie_file.push_back((v_image[i]));
		//try{
		m_movie.push_back(Gdk::Pixbuf::create_from_file(v_image[i]));
		//}
		//catch(...){}
	}
	if(m_movie.size() == intermediate.size()) m_movie_frame = 0;

}
