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

extern  char * BMBF_RGB_Gef_M_xpm[];
extern char * mfo_logo_xpm[] ;
extern char * surfer_small_xpm[] ;
extern char * logo_wissenschaftsjahr08_xpm[] ;
extern char * BMBF_JM_Alles_xpm[];

AboutWindow::AboutWindow(const surfer_options& so)
:
opt(so),
l_surfer1("Surfer"),
l_surfer2(""),
l_surfer3("Surfer basiert auf dem Programm surf von Stehpen Endrass et al.\n(http://surf.sourceforge.net)"),
l_surfer4("Surfer wurde für die Ausstellung IMAGINARY entwickelt, die vom Mathematischen Forschungsinstitut Oberwolfach zum Jahr der Mathematik 2008 in Deutschland konzipiert wurde. Die Ausstellung ist vom Bundesministerium von Bildung und Forschung gefördert."),
m_close(Gtk::Stock::OK),
l_credits(
"Gesamtleitung: Gert-Martin Greuel\n"
"Programmierung: Henning Meyer und Christian Stussak\n"
"Konzept/Galerien: Oliver Labs\n"
"Unterstützung: Felix Riemann\n"
"Konzept/Koordination"),
l_copy("(c) Mathematisches Forschungsinstitut Oberwolfach und Technische Universität Kaiserslautern, 2008"),
l_info("Die aktuelle Version und mehr Informationen zu Surfer finden Sie auf der Webseite http://surfer.imaginary2008.de/ und http://www.imaginary2008.de/"),
l_contact("Kontakt: surfer@imaginary2008.de"),
m_guide(Gtk::Stock::HELP),
a1(0.0,0.5,0.0,0.0),
a2(0.0,0.5,0.0,0.0),
a3(0.0,0.5,0.0,0.0),
a4(0.0,0.5,0.0,0.0),
a5(0.0,0.5,0.0,0.0),
a7(0.0,0.5,0.0,0.0),
a8(0.0,0.5,0.0,0.0),
a6(0.0,0.5,0.0,0.0)
{

l_surfer1.set_markup("<span weight=\"bold\">Surfer</span>");
m_close.signal_clicked().connect(sigc::mem_fun(*this, &AboutWindow::on_close_click));
m_guide.signal_clicked().connect(sigc::mem_fun(*this, &AboutWindow::on_help_click));

set_border_width(10);

add(m_tab);

//add(m_b);

m_tab.attach(a1,0,2,0,1);

a1.add(l_surfer1);
//m_b.pack_start(a1,false,true);
//m_b.pack_start(l_surfer2,false,true);
//m_b.pack_start(l_surfer3,true,true);
//m_b.pack_start(l_surfer4,true,false);
//m_b.pack_start(i_bmbf,false,true);

//l_surfer1.set_justify(Gtk::JUSTIFY_LEFT);
//l_surfer3.set_justify(Gtk::JUSTIFY_LEFT);
//l_surfer4.set_justify(Gtk::JUSTIFY_LEFT);


//m_tab.attach(l_surfer2,1,2,0,1,Gtk::SHRINK,Gtk::SHRINK);

a8.add(i_surfer);
m_tab.attach(a8,0,2,1,2);

a3.add(l_surfer3);
m_tab.attach(a3,0,2,2,3);
a4.add(l_surfer4);
m_tab.attach(a4,0,2,3,4);

m_tab.attach(i_bmbf,0,1,4,5);

Glib::RefPtr<Gdk::Pixbuf> surfer = Gdk::Pixbuf::create_from_xpm_data(surfer_small_xpm);
i_surfer.set(surfer);


Glib::RefPtr<Gdk::Pixbuf> bmbf = Gdk::Pixbuf::create_from_xpm_data(BMBF_RGB_Gef_M_xpm);
i_bmbf.set(bmbf);

m_tab.attach(i_math,1,2,4,5);

Glib::RefPtr<Gdk::Pixbuf> mfo = Gdk::Pixbuf::create_from_xpm_data(mfo_logo_xpm);
i_mfo.set(mfo);

a2.add(l_credits);
m_tab.attach(a2,0,2,5,6);
a5.add(l_copy);
m_tab.attach(a5,0,2,6,7);

a7.add(i_mfo);
m_tab.attach(a7,0,2,7,8);


a6.add(l_contact);
m_tab.attach(a6,0,2,8,9);

tm mittwoch = {0,0,0,24,0,108,0,0,-1};
time_t m2 =mktime(&mittwoch);

time_t now;
time(&now);


Glib::RefPtr<Gdk::Pixbuf> math = Gdk::Pixbuf::create_from_xpm_data(

(now>m2)?
BMBF_JM_Alles_xpm:
logo_wissenschaftsjahr08_xpm

);
i_math.set(math);


modify_bg(Gtk::STATE_NORMAL,Gdk::Color("white")); 
set_title("Über Surfer");
set_size_request(500,600);

m_tab.attach(m_close,1,2,9,10,Gtk::SHRINK,Gtk::SHRINK);
l_surfer4.set_line_wrap();
l_copy.set_line_wrap();
l_surfer1.set_line_wrap();

#ifdef WIN32
if(!opt.helpfile.empty())
{
m_tab.attach(m_guide,0,1,9,10,Gtk::SHRINK,Gtk::SHRINK);
}

#endif

show_all_children();
}

void AboutWindow::on_close_click()
{
hide();
}

void AboutWindow::on_help_click()
{
#ifdef WIN32
	switch(int(ShellExecuteA(NULL,"open",opt.helpfile.c_str(),NULL,NULL,SW_SHOW)))
	{
		case ERROR_FILE_NOT_FOUND:
		case SE_ERR_ACCESSDENIED:
		{
			Gtk::MessageDialog m(*this,"Auf die Hilfe-Datei kann nicht zugegriffen werden.",Gtk::MESSAGE_ERROR,Gtk::BUTTONS_OK,true);
			m.run()
			break;
		}
		case SE_ERR_NO_ASSOC:
		case SE_ERR_ASSOCINCOMPLETE:
		{
			Gtk::MessageDialog m(*this,"Die Hilfe-Datei kann nicht angezeigt werden. Installieren Sie den Adobe Reader.",Gtk::MESSAGE_ERROR,Gtk::BUTTONS_OK,true);
			m.run()
			break;
		}
		
	}
#endif
}


