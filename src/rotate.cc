/***************************************************************************
 *   Copyright (C) 2008 by Henning Meyer                                   *
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

#include "bwindow_gdk.hh"
#include "matrix.hh"

#include <glib/gstdio.h>



#include <sys/types.h>
#include <dirent.h>

#include "vecmath.h"

bool use_mencoder = false;
bool use_ffmpeg = false;
extern bool no_modify;
#define MOD if(!no_modify)

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
	double Qt = (1-t)*P+t*Q;
	return Qt;
}


int interpolate(int P, int Q, double t)
{
	int Qt = int(interpolate(double(P),double(Q),t));
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


bool AniWindow::on_view_key_press(GdkEventKey* event)
{
	if(!event)	return true;

	if(event->keyval == GDK_Delete)
	{
		on_delete_frame();
	}

	return true;
}

AniWindow::AniWindow(SurfBWindow& g):
	gui(g),
	fcount(0),
	m_record(Gtk::Stock::MEDIA_PLAY),
	m_movie_frame(-1),
	m_fpreview(_("Video Preview")),
	m_pad_to(0),
	m_stop(Gtk::Stock::MEDIA_STOP),
	m_pause_cont(Gtk::Stock::MEDIA_PAUSE),
	m_add_frame(Gtk::Stock::ADD),
	m_delete_frame(Gtk::Stock::REMOVE),
	m_edit(Gtk::Stock::JUMP_TO),
	m_save(Gtk::Stock::SAVE),
	invalidated(true)
{

set_up_the_ani_menu();


if(use_mencoder == false && use_ffmpeg == false)
{
	use_mencoder = !gui.opt.mencoder_cmd.empty();
	use_ffmpeg = !gui.opt.ffmpeg_cmd.empty();

	if(use_mencoder && use_ffmpeg) use_mencoder = false;
}

m_refListStore = Gtk::ListStore::create(m_Columns);

m_TreeView.set_model(m_refListStore);

//m_TreeView.append_column(_("Position"),m_Columns.m_col_number);
m_TreeView.append_column(_("Key Frame"),m_Columns.m_col_pic);
m_TreeView.append_column_editable(_("Duration"),m_Columns.m_col_duration);

Gtk::Table& m_tab = *this;

m_scroll.add(m_TreeView);


m_TreeView.signal_key_press_event().connect( sigc::mem_fun(*this, &AniWindow::on_view_key_press) );

m_TreeView.add_events(Gdk::POINTER_MOTION_MASK|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK);

m_TreeView.signal_button_press_event().connect( sigc::mem_fun(*this, &AniWindow::on_button_press_event_func) );

m_refListStore->signal_row_changed().connect( sigc::mem_fun(*this, &AniWindow::on_my_row_changed) );

m_scroll.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);

m_tab.attach(m_scroll,0,3,8,9);

//m_tab.attach(m_lres,1,2,0,1,Gtk::SHRINK,Gtk::SHRINK);
//m_tab.attach(m_res,2,3,0,1,Gtk::SHRINK,Gtk::SHRINK);

m_fpreview.add(m_preview);
m_tab.attach(m_fpreview,2,3,1,7,Gtk::SHRINK,Gtk::SHRINK);



m_tab.attach(m_add_frame,0,1,1,2,Gtk::FILL,Gtk::SHRINK);
m_tab.attach(m_delete_frame,1,2,1,2,Gtk::FILL,Gtk::SHRINK);
m_tab.attach(m_record,0,1,3,4,Gtk::FILL,Gtk::SHRINK);
//m_tab.attach(m_pause_cont,0,1,4,5,Gtk::SHRINK,Gtk::SHRINK);
m_tab.attach(m_stop,1,2,3,4,Gtk::FILL,Gtk::SHRINK);
m_tab.attach(m_save,0,1,6,7,Gtk::FILL,Gtk::SHRINK);
m_tab.attach(m_edit,1,2,6,7,Gtk::FILL,Gtk::SHRINK);



m_tab.attach(m_prog,0,2,7,8,Gtk::FILL,Gtk::SHRINK);

m_record.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::on_record));
m_stop.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::on_pause));
m_save.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::on_save));
m_add_frame.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::ani_add));
m_edit.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::on_edit));

m_delete_frame.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::on_delete_frame));
m_pause_cont.signal_clicked().connect(sigc::mem_fun(*this,&AniWindow::on_pause_cont));

m_preview.set_size_request(100,100);


m_TreeView.set_reorderable();

sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &AniWindow::on_timer),0);

// This is where we connect the slot to the Glib::signal_timeout()
//const int frame_rate = 10;

	conn = Glib::signal_timeout().connect(my_slot, 1000/gui.opt.video_frame_rate);


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

	log_system((opt.surf_cmd+QUIET_SURF+" -n \""+ts+"\" " +REDIRECTION_APEX).c_str());
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
invalidated = true;
parse_result p;
p.data = gui.data;
p.global_data = gui.global_data;
add_entry(p,25,++fcount);
}

bool AniWindow::on_timer(int)
{
	if(!is_visible()) return true;
	if(m_movie_frame==m_movie_file.size()) m_movie_frame = 0;
	if ((m_movie_frame<0) || (m_movie_file.size()==0)) {return true;}

	//out<<"now playing "<<m_movie_frame<<": "<<m_movie_file[m_movie_frame]<<std::endl;

	Glib::RefPtr<Gdk::Pixbuf> g = Gdk::Pixbuf::create_from_file(m_movie_file[m_movie_frame]);

	m_preview.set(g->scale_simple(100,100,Gdk::INTERP_BILINEAR));

	conn.disconnect();
	sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &AniWindow::on_timer),0);

// This is where we connect the slot to the Glib::signal_timeout()
//const int frame_rate = 10;

	conn = Glib::signal_timeout().connect(my_slot, 1000/gui.opt.video_frame_rate);

	m_movie_frame++;

	return true;
}


/*
Gtk::Image m_preview;

std::vector<Glib::RefPtr<Gdk::Pixbuf> > m_movie;
int m_movie_frame;

Gtk::Button m_record;
*/


void clear_dir(const std::string& path)
{
	DIR* gd = opendir(path.c_str());
	if(gd==NULL) return ;
	dirent* R = readdir(gd);
	while(R)
	{
		std::string d (R->d_name);
		if(!d.empty() && d[0]!='.' )
		std::remove((path+DIR_SEP+R->d_name).c_str());
		R = readdir(gd);
	}
	closedir(gd);
}

void AniWindow::on_record()
{
	compute();
	m_movie_frame = 0;
}



void AniWindow::compute()
{
	if(!invalidated) return;

	m_movie_frame = -1;
	//m_save.set_sensitive(false);
	m_prog.set_fraction(0);

	const bool do_pad = use_mencoder;
	const bool do_jpeg = true;

	std::vector<std::string> v_script;
	std::vector<std::string> v_image;
	std::vector<std::string> v_jpeg;

	std::vector<unsigned> v_dur;
	std::vector<parse_result> v_sta;

	Gtk::TreeModel::iterator irow = m_refListStore->children().begin();

	for(;irow != m_refListStore->children().end(); ++irow)
	{
		v_dur.push_back((*irow)[m_Columns.m_col_duration]);
		v_sta.push_back((*irow)[m_Columns.m_col_state]);
	}

	clear_dir(TEMP_ROOT_SEP+"surfer_ani");
	m_movie_frame = -1;
	m_movie.clear();
	m_movie_file.clear();

	if(v_dur.empty()) {
		m_preview.clear();
		return;
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

	for(unsigned i = 0; i < intermediate.size(); i++)
	{
		std::ostringstream clocks;
		clocks<<"double clock = "<<i<<";";

		std::ostringstream clocki;
		clocki<<"int clocki = "<<i<<";";

		if(intermediate[i].global_data.general_stuff.find("double clock = 0;")!=std::string::npos)
		intermediate[i].global_data.general_stuff.replace(intermediate[i].global_data.general_stuff.find("double clock = 0;"),std::string("double clock = 0;").size(),clocks.str());

		if(intermediate[i].global_data.general_stuff.find("int clocki = 0;")!=std::string::npos)
		intermediate[i].global_data.general_stuff.replace(intermediate[i].global_data.general_stuff.find("int clocki = 0;"),std::string("int clocki = 0;").size(),clocki.str());
	}

	std::ostringstream osx;
	osx<<intermediate.size();
	std::string six = osx.str();

	std::vector<std::string> optionk;
	optionk.push_back("quality");

	std::vector<std::string> optionv;
	optionv.push_back("100");

	if(do_pad) m_pad_to = six.size();

	for(unsigned i = 0; i < intermediate.size(); i++)
	{
		g_mkdir((TEMP_ROOT_SEP+"surfer_ani").c_str(), 0774);

		std::ostringstream os;
		os<<i;

		std::string si = os.str();


		if(do_pad)
		while (si.size()<six.size())
		si = "0"+si;

		v_script.push_back(TEMP_ROOT_SEP+"surfer_ani"+DIR_SEP+"frame"+si+".pic");
		v_image.push_back(TEMP_ROOT_SEP+"surfer_ani"+DIR_SEP+"frame"+si+".ppm");
		v_jpeg.push_back(TEMP_ROOT_SEP+"surfer_ani"+DIR_SEP+"frame"+si+".jpg");

		std::ofstream f(v_script[i].c_str(),FILE_WRITE_MODE);

		f<<"width="<<gui.opt.video_resolution<<";\n";
		f<<"height="<<gui.opt.video_resolution<<";\n";

		write_surf(intermediate[i].global_data,f,0);
		for(unsigned k = 0; k < intermediate[i].data.size(); k++)
		{
			write_surf(intermediate[i].data[k],k,f);
		}

		//f<<"transparency="<<50<<";\n";
		f<<"filename=\""<<(v_image[i])<<"\";\n";
		f<<"color_file_format="+gui.opt.format+";\n";
		f<<"draw_surface;\n";
		f<<"save_color_image;\n";
		f.close();

		//out<<"finished file "<<i<<std::endl;
	}

	for(unsigned i = 0; i < intermediate.size(); i++)
	{
		m_prog.set_fraction(double(i)/intermediate.size());

		while( Gtk::Main::events_pending() )
   		Gtk::Main::iteration();

		log_system(gui.opt.surf_cmd+QUIET_SURF+" -n \""+v_script[i]+"\" " + REDIRECTION_APEX);
	}

	for(unsigned i = 0; i < intermediate.size(); i++)
	{
		if(do_jpeg)
		{m_movie_file.push_back((v_jpeg[i]));
		//try{
		 Gdk::Pixbuf::create_from_file(v_image[i])->save(v_jpeg[i],"jpeg",optionk,optionv);
		}
		else
		{m_movie_file.push_back((v_image[i]));}
		//}
		//catch(...){}
	}
	if(m_movie_file.size() == intermediate.size())
	{
		invalidated = false;
		m_prog.set_fraction(1);
	}
	else
	m_prog.set_fraction(0);

}


void make_movie_mencoder(const std::string& outfile, const std::string& indir, const surfer_options& opt, unsigned bitrate = 1800, unsigned fps = 10,  const std::string format ="msmpeg4v2", const std::string container="avi")
{
	std::ostringstream os;
	os<<opt.mencoder_cmd;
	os<<" \"mf://"<<indir<<DIR_SEP<<"*.jpg\" -mf fps="<<fps<<" -o \""<<outfile<<"\"";
	if(container != "avi") os<<" -of lavf ";
	os<<" -ovc lavc -lavcopts vcodec="<<format<<":vbitrate="<<bitrate;
	if(container == "gif") os<<":pix_fmt=rgb24";
	if(container != "avi") os<<" -lavfopts format="<<container;

	log_system(os.str()+" "+REDIRECTION_APEX);
}


void make_movie_ffmpeg(const std::string& outfile, const std::string& indir, const surfer_options& opt, unsigned bitrate = 1800, unsigned fps = 10, unsigned pad = 0, const std::string format ="msmpeg4v2")
{
	std::ostringstream os;

	os<<opt.ffmpeg_cmd<<" -y -r "<<fps<<" -i \""<<indir<<DIR_SEP<<"frame%";
	if(pad)os<<"0"<<pad;
	os<<"d.jpg\"";
	if(format=="flv")os<<"-f flv";
	else if(format =="mp4")os<<"";
	else os<<" -vcodec "<<format;

	os<<" \""<<outfile<<"\"";
	if(format != "flv")
	os<<" -b "<<bitrate;

	log_system(os.str()+" "+REDIRECTION_APEX);
}


void AniWindow::on_save()
{
	compute();

	std::ostringstream title;

	title << _("Save video");

	Gtk::FileChooserDialog dialog(gui,title.str(),Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	Gtk::FileFilter filter_msmpeg;
	  filter_msmpeg.set_name(_("AVI Video Container (*.avi)"));
	  filter_msmpeg.add_mime_type("video");
	  filter_msmpeg.add_pattern("*.avi");
	dialog.add_filter(filter_msmpeg);


	Gtk::FileFilter filter_mp4;
	  filter_mp4.set_name(_("MPEG-4 Video (.mp4)"));
	  filter_mp4.add_mime_type("video/mp4");
	dialog.add_filter(filter_mp4);

	Gtk::FileFilter filter_gif;
	  filter_gif.set_name(_("GIF Animation (.gif)"));
	  filter_gif.add_mime_type("image/gif");
if(false) dialog.add_filter(filter_gif);

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
	  dialog.set_current_name("surfer_video.avi");

       dialog.show();
       dialog.present();
       dialog.raise();

	//Gtk::Main::run(dialog);

	int result = Gtk::RESPONSE_CANCEL;

	bool do_gif = false;
	bool do_msmpeg = false;
	bool do_mp4 = false;

	choose_again:
        result = dialog.run();

	if(result == Gtk::RESPONSE_OK)
	{
		//std::string of = opt.format;
		//opt.format = "jpg";
		if(!no_log)
		{
		std::cout<<"video save: "<<dialog.get_filename()<<std::endl;
		std::cout<<dialog.get_filter()->get_name()<<std::endl;
		}

		do_gif = dialog.get_filter()->get_name() == filter_gif.get_name();
		do_mp4 = dialog.get_filter()->get_name() == filter_mp4.get_name();
		do_msmpeg = dialog.get_filter()->get_name() == filter_msmpeg.get_name();

		if(dialog.get_filter()->get_name() == filter_any.get_name())
		{do_msmpeg = true; }

		std::string t = dialog.get_filename();

		if(do_msmpeg)
		{
			if(t.find("\"")==-1L && t.find(".")==-1L) t = t+".avi";
		}

		if(do_mp4)
		{

			if(t.find("\"")==-1L && t.find(".")==-1L) t = t+".mp4";
			if(t.find("\"")==-1L && t.find(".avi")!=-1L) t = t.replace(t.find(".avi"),4,".mp4");
		}

		if(do_gif)
		{

			if(t.find("\"")==-1L && t.find(".")==-1L) t = t+".gif";
			if(t.find("\"")==-1L && t.find(".avi")!=-1L) t = t.replace(t.find(".avi"),4,".gif");
		}

		if(use_mencoder)
		{
			if(do_msmpeg)
			make_movie_mencoder(t,TEMP_ROOT_SEP+"surfer_ani",gui.opt);
			else if(do_mp4)
			{
			make_movie_mencoder(t,TEMP_ROOT_SEP+"surfer_ani",gui.opt,gui.opt.video_bitrate,gui.opt.video_frame_rate,"mpeg4","mp4");
			}
			else if(do_gif)
			{
			make_movie_mencoder(t,TEMP_ROOT_SEP+"surfer_ani",gui.opt,gui.opt.video_bitrate,gui.opt.video_frame_rate,"gif","gif");
			}

		} else
		if(use_ffmpeg)
		{
			{
				std::cout<<t<<std::endl;
				std::ifstream fe(t.c_str());
				if(fe.is_open())
				{
					Gtk::MessageDialog md(gui,_("File exists. Overwrite?"),false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_YES_NO,true);
					int result = md.run();
					if(result != Gtk::RESPONSE_YES)
					goto choose_again;
				}
			}

			if(do_msmpeg)
			make_movie_ffmpeg(t,TEMP_ROOT_SEP+"surfer_ani",gui.opt,gui.opt.video_bitrate,gui.opt.video_frame_rate);
			else if(do_mp4)
			{
			make_movie_ffmpeg(t,TEMP_ROOT_SEP+"surfer_ani",gui.opt,gui.opt.video_bitrate,gui.opt.video_frame_rate,m_pad_to,"mpeg4");
			}
			else if(do_gif)
			{
			make_movie_ffmpeg(t,TEMP_ROOT_SEP+"surfer_ani",gui.opt,gui.opt.video_bitrate,gui.opt.video_frame_rate,m_pad_to,"gif");
			}

		}
	}

}



void AniWindow::on_pause()
{
m_movie_frame = -1;
}

void AniWindow::on_delete_frame()
{
invalidated = true;
Gtk::TreePath T;
		Gtk::TreeViewColumn* C;
		m_TreeView.get_cursor(T,C);

		if(!T.empty() && m_refListStore->iter_is_valid(m_refListStore->get_iter(T))) m_refListStore->erase(m_refListStore->get_iter(T));
}

void AniWindow::on_pause_cont()
{
m_movie_frame = -1;
}

bool AniWindow::on_button_press_event_func(GdkEventButton* event)
{
	if(event)
	{
		if(event->type == GDK_2BUTTON_PRESS)
		{
			Gtk::TreePath T;
			Gtk::TreeViewColumn* C;
			m_TreeView.get_cursor(T,C);
			Gtk::TreeModel::Row row = *(m_refListStore->get_iter(T));
			std::cout<<"button press"<<std::endl;
			if(!T.empty() && m_refListStore->iter_is_valid(m_refListStore->get_iter(T)))
			{
			parse_result P = row[m_Columns.m_col_state];
			gui.data = P.data;
			gui.global_data = P.global_data;
			gui.data_index = 0;
			gui.update_visuals();
			}
		}
	}
	return true;
}

void AniWindow::on_my_row_changed(Gtk::TreePath const&,Gtk::TreeIter const&)
{
	invalidated = true;
}

void AniWindow::on_edit()
{

	{
			Gtk::TreePath T;
			Gtk::TreeViewColumn* C;
			m_TreeView.get_cursor(T,C);

			if(!T.empty() && m_refListStore->iter_is_valid(m_refListStore->get_iter(T)))
			{
			Gtk::TreeModel::Row row = *(m_refListStore->get_iter(T));
			parse_result P = row[m_Columns.m_col_state];
			gui.data = P.data;
			gui.global_data = P.global_data;
			gui.data_index = 0;
			gui.update_visuals(false);
			}
		}
}



void AniWindow::set_up_the_ani_menu()
{

std::string amp = "&";

 mr_AG = Gtk::ActionGroup::create();

mr_AG->add(Gtk::Action::create("GotoFrame",Gtk::Stock::JUMP_TO), sigc::mem_fun(*this, &AniWindow::on_edit));

mr_AG->add(Gtk::Action::create("AddFrame",Gtk::Stock::ADD), sigc::mem_fun(*this, &AniWindow::ani_add));
mr_AG->add(Gtk::Action::create("KillFrame",Gtk::Stock::REMOVE), sigc::mem_fun(*this, &AniWindow::on_delete_frame));


mr_AG->add(Gtk::Action::create("Preview",Gtk::Stock::MEDIA_PLAY), sigc::mem_fun(*this, &AniWindow::on_record) );


mr_AG->add(Gtk::Action::create("Stop",Gtk::Stock::MEDIA_STOP),sigc::mem_fun(*this, &AniWindow::on_pause));

mr_AG->add(Gtk::Action::create("Save",Gtk::Stock::SAVE),sigc::mem_fun(*this, &AniWindow::on_save));
//FIXME: Gtk::Stock::DELETE does not work under windows (with Gtkmm 2.10)

mr_UIM = Gtk::UIManager::create();

mr_UIM->insert_action_group(mr_AG);

//add_accel_group(mr_UIM->get_accel_group());

Glib::ustring ui_info =
    "<ui>"
    "  <toolbar  name='AniBar'>"
    "    <toolitem action='AddFrame'/>"
    "    <toolitem action='KillFrame'/>"
    "      <separator/>"
    "    <toolitem action='Preview'/>"
    "    <toolitem action='Stop'/>"
    "      <separator/>"
//    "      <toolitem action='HelpManual'/>"
    "      <toolitem action='GotoFrame'/>"
    "      <separator/>"
//    "      <toolitem action='HelpManual'/>"
    "      <toolitem action='Save'/>"
    "  </toolbar>"
    "</ui>";

mr_UIM->add_ui_from_string(ui_info);



Gtk::Toolbar* pPropbar = dynamic_cast<Gtk::Toolbar*>(mr_UIM->get_widget("/AniBar")) ;
//if(pPropbar)
{
pPropbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
//pPropbar->set_icon_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);

    //m_tab.attach(*pPropbar,0,1,0,1);

//pPropbar->set_size_request(500,400);
//pPropbar->show();

    //pPropbar->show();
pPropbar->set_show_arrow(false);
//pToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH_HORIZ);
MOD{pPropbar->modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);}
}

}
