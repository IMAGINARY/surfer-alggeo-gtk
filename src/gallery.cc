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
#include <sys/types.h>
#include <dirent.h>
#include <fstream>

bool no_modify_gallery = false;

#define MOD if(!no_modify_gallery)


bool ignore_bg_in_gallery = false;

std::string basename(const std::string& ff)
{
	std::string f = ff;

	while(f.find(DIR_SEP)!=std::string::npos)
	{
		long i = f.find(DIR_SEP);
		f=f.substr(i+1);
	}

	unsigned long i = f.find(".");
	if(i==std::string::npos) return f;
	else return f.substr(0,i);
}

std::string extension(const std::string& f)
{
	int i = f.find(".");
	if(i==-1) return "";
	else return f.substr(i);
}

gallery read_gallery(const std::string& path, const std::string& name, double upscaling)
{
	gallery G;

	color global_background = global_defaults().background;

	try
	{
		std::ifstream fi((path+DIR_SEP+name+".txt").c_str());
		if(fi.is_open())
		return read_gallery_file(fi,path,name,upscaling);
	}
	catch(...)
	{}

	DIR* gd = opendir(path.c_str());

	if(gd==NULL) return G;

	dirent* R = readdir(gd);

	while(R)
	{

		
		std::string d (R->d_name);
		if(!d.empty() && d[0]!='.' && extension(d)==".pic")
		{

		std::ifstream f((path+DIR_SEP+R->d_name).c_str());
		parse_result P;
		P.global_data = global_defaults();
		P.data = read_pic( f, P.global_data);
		
		if(!P.data.empty())
		{
			if(ignore_bg_in_gallery)
			{
				P.global_data.background = global_background;
				
			}

			//P.filename = d;
			P.global_data.name = basename(d);
			//P.thumbnail = path+DIR_SEP+basename(d)+".ppm";
			for(unsigned i = 0; i < P.data.size(); i++)
			P.data[i].desc = path+DIR_SEP+basename(d)+".png";
			P.filepath = path+DIR_SEP+basename(d);

			 G.file.push_back(P);

		}

		}
		R = readdir(gd);
	}

	closedir(gd);

	G.name = name;
	G.path = path;
	G.image = path+DIR_SEP+name+".ppm";
	G.desc = path+DIR_SEP+name+".png";
	
	if(!G.file.empty())
	try{
		std::ifstream fi(G.image.c_str());
		if(!fi.is_open())G.image = thumbnail(G.file[0]);
		}catch(...){
	G.image = thumbnail(G.file[0]);
	}
	

	//std::ofstream F((path+DIR_SEP+name+".txt").c_str());
	//write_gallery_file(F,G);
	return G;

}


std::vector<gallery> read_galleries_old(const std::string& path, double upscaling)
{
	std::vector<gallery> V;

	DIR* gd = opendir(path.c_str());

	if(gd==NULL) return V;

	dirent* R = readdir(gd);

	while(R)
	{
		
		std::string d (R->d_name);
		if(!d.empty() && d[0]!='.')
		{
			gallery G = read_gallery(path+R->d_name,R->d_name,upscaling);

			if(!G.path.empty()) V.push_back(G);
		}
		
		R = readdir(gd);
	}

	closedir(gd);

	
	return V;

}



























namespace
{
  class IconEntry
  {
  public:
    IconEntry()
    {}

    IconEntry(const std::string& filename, const Glib::ustring& description)
    : m_filename(filename),
      m_description(description)
    {}


     std::string m_filename;
     Glib::ustring m_description;
  };
}

GalleryWindow::GalleryWindow(const gallery& g, const surfer_options& o)
:
m_Columns(),
m_VBox(),
m_desc(),
m_tab(1,3),
m_ScrolledWindow(),
m_IconView(),
m_refListModel(),
m_frame(),
m_please(),
gal(g),
ret(),
isu(),
opt(o)



{
	// change gallery background to white
	MOD{ modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
	m_desc.modify_bg(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 

	m_tab.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
	m_ScrolledWindow.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
	m_IconView.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK); 
	m_VBox.modify_base(Gtk::STATE_NORMAL,MAIN_COLOR_GDK);

	m_IconView.modify_fg(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);
	m_IconView.modify_text(Gtk::STATE_NORMAL,CONTRAST_COLOR_GDK);  }
 

	set_title(_("Select surface - Surfer"));
  set_border_width(5);
  set_default_size(400, 400);
  add(m_tab);
  
 m_please.set_markup(std::string("<span foreground=\"#FF0000\" weight=\"bold\">")+_("Click on a surface, please.")+"</span>");

  m_frame.add(m_desc);
  m_tab.attach(m_frame,0,1,0,1,Gtk::EXPAND|Gtk::FILL,Gtk::SHRINK);
  m_tab.attach( m_VBox,0,1,2,3 );
  m_tab.attach( m_please,0,1,1,2,Gtk::EXPAND|Gtk::FILL,Gtk::SHRINK );

m_desc.set_size_request(1000,200);
try{
	std::ifstream fi(g.desc.c_str());
	if(fi.is_open())
	{
		m_desc.set(g.desc);
		m_desc.set_size_request(1000,300);		
	}
	else
	m_desc.clear();
	}
	catch(...){m_desc.clear();}

  // Add the TreeView inside a ScrolledWindow, with the button underneath:
  m_ScrolledWindow.add(m_IconView);

  // Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_VBox.pack_start(m_ScrolledWindow);
  //m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  //m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
  //m_ButtonBox.set_border_width(6);
  //m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
  //m_Button_Quit.signal_clicked().connect(sigc::mem_fun(*this,
   //           &GalleryWindow::on_button_quit) );

  // Create the Tree model:
  m_refListModel = Gtk::ListStore::create( m_Columns );
  //m_refListModel->set_sort_column( m_Columns.m_col_description, Gtk::SORT_ASCENDING );

  m_IconView.set_model(m_refListModel);
  m_IconView.set_markup_column(m_Columns.m_col_description);
  m_IconView.set_pixbuf_column(m_Columns.m_col_pixbuf);
  m_IconView.signal_item_activated().connect(sigc::mem_fun(*this,
              &GalleryWindow::on_item_activated) );
  m_IconView.signal_selection_changed().connect(sigc::mem_fun(*this,
              &GalleryWindow::on_selection_changed) );

  std::vector<IconEntry> entries;

  m_IconView.set_size_request(1000,400);
  for(unsigned i = 0; i < gal.file.size(); i++)
  {
	
	IconEntry e(thumbnail(gal.file[i]),gal.file[i].global_data.name);

	entries.push_back(e);
  }
const int count = entries.size();
  for( int idx = 0; idx < count; ++idx )
  {

    add_entry( entries[idx].m_filename, entries[idx].m_description ,idx);
  }


		maximize();
		fullscreen();

  show_all_children();
  present();
}

GalleryWindow::~GalleryWindow()
{
}





void GalleryWindow::on_item_activated(const Gtk::TreeModel::Path& )
{
  

 
}


void GalleryWindow::on_selection_changed()
{
  typedef std::list<Gtk::TreeModel::Path> type_list_paths;
  type_list_paths selected = m_IconView.get_selected_items();
  if(!selected.empty())
  {
    const Gtk::TreeModel::Path& path = *selected.begin();
    Gtk::TreeModel::iterator iter = m_refListModel->get_iter(path);
    Gtk::TreeModel::Row row = *iter;

    const std::string filename = row[m_Columns.m_col_filename];
    const Glib::ustring description = row[m_Columns.m_col_description];
    int i = row[m_Columns.m_col_index];
  
	isu = i;
        ret = gal.file[i];
	hide();
  }
}

void GalleryWindow::add_entry(const std::string& filename,
        const Glib::ustring& description , int idx)
{
  Gtk::TreeModel::Row row = *(m_refListModel->append());
  row[m_Columns.m_col_filename] = filename;
  row[m_Columns.m_col_description] = description;
try{
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  row[m_Columns.m_col_pixbuf] = Gdk::Pixbuf::create_from_file(filename);
  #else
  std::auto_ptr<Glib::Error> error;
  row[m_Columns.m_col_pixbuf] = Gdk::Pixbuf::create_from_file(filename, error);
  #endif //GLIBMM_EXCEPTIONS_ENABLED
}catch(...){

make_thumbs(gal.file,opt);

}
	row[m_Columns.m_col_index] = idx;

}











std::string thumbnail(const parse_result& P)
{
	return P.filepath+".ppm";
}

