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

#include "bwindow_gdk.hh"




#ifdef _OPENMP
#include <omp.h>
#endif

int global_num_threads = 1;



int num_threads_via_omp()
{
int n = 1;

#pragma omp parallel shared(n)
{
#ifdef _OPENMP
n = ( int ) ( omp_get_num_threads() );
#endif

}

return n;//n > 2 ? n - 1 : n;
}

int num_threads()
{
	return global_num_threads;
}

void init_threads()
{
	global_num_threads = num_threads_via_omp();
}

void parallel_surf(const std::string& script,bool sync, int width, const surfer_options& opt)
{
	if(width<1)return;
	int n = num_threads();
	
	if(n>1 && no_new_surf_features)
	{
		std::cerr<<_("surfer: multithreaded rendering not supported with this version of surf.")<<std::endl;
		std::cerr<<_("use version of surf shipped with surfer and activate \"modified_surf=1\" in ")+fix_file("~/.surfer")<<std::endl;
		n = 1;
	}

	std::string fbuff;
	{
		std::ostringstream o;
		o<<width;
		fbuff = o.str();
	}

	#pragma omp parallel for
	for(int i = 0; i < n; i++)
	{
		std::ostringstream buf;
		std::ostringstream min_buff;
		std::ostringstream max_buff;
		
		min_buff<<(i*width/n - ((i==0) ? 0 : 1) );
		max_buff<<( (i+1) * width/n + ((i==n-1)?0:1) );
		if(i)
		buf<<(i+1);

		std::string cmd = (opt.surf_cmd +QUIET_SURF+ ((n!=1)?(" --clip_to 0 "+min_buff.str()+" "+fbuff+" "+" "+max_buff.str()):"")+" -n \""+script+buf.str()+"\" " + REDIRECTION_APEX +((!sync) ?"":DAEMONIZE));



		log_system(cmd);
	}


}

void parallel_clear()
{
	
	int n = num_threads();
	
	

	#pragma omp parallel for
	for(int i = 0; i < n; i++)
	{
		std::ostringstream buf;
		if(i)
		buf<<(i+1);

                std::remove( (TEMP_ROOT_SEP+"surfb.ppm"+buf.str()).c_str() );



                std::remove(((TEMP_ROOT_SEP+std::string("surfb.pic")+buf.str()).c_str()));

                std::remove(((TEMP_ROOT_SEP+std::string("surfb_f.ppm")+buf.str()).c_str()));
                std::remove(((TEMP_ROOT_SEP+std::string("surfb_f.pic")+buf.str()).c_str()));


		
	}


}



