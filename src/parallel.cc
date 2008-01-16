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


#define SURF_OPT "-n"

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

return n > 2 ? n - 1 : n;
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
	const int n = num_threads();
	

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

		std::string cmd = (opt.surf_cmd + ((n!=1)?(" --clip_to 0 "+min_buff.str()+" "+fbuff+" "+" "+max_buff.str()):"")+" "SURF_OPT +" \""+script+buf.str()+"\" " REDIRECTION_APEX +((!sync) ?"":DAEMONIZE));


		//std::cout<<cmd<<std::endl;
		system(cmd.c_str());
	}


}




/*
PID_type system_async(const std::string& s, const std::string& ulimit, const surfer_options& opt)
{
	const std::string s2 = " " REDIRECTION_APEX " " DAEMONIZE;
	switch(CM)
	{
	case dual_core:

#pragma omp parallel sections
{
#pragma omp section
{
	//std::cout << (PARASURF "surf_1_2/surf_1_2 -n "+s+s2).c_str() << std::endl;
	system(("ulimit -t "+ulimit+"; "+PARASURF "surf_1_2/surf_1_2 -n "+s+s2).c_str());
}
#pragma omp section
{
	//std::cout << (PARASURF "surf_2_2/surf_2_2 -n "+s+"2"+s2).c_str() << std::endl;
	system(("ulimit -t "+ulimit+"; "+PARASURF "surf_2_2/surf_2_2 -n "+s+"2"+s2).c_str());
}
}

			break;
		case quad_core:

#pragma omp parallel sections
{
#pragma omp section
{
	//std::cout << (PARASURF "surf_1_2/surf_1_2 -n "+s+s2).c_str() << std::endl;
	system(("ulimit -t "+ulimit+"; "+PARASURF "surf_1_4/surf_1_4 -n "+s+s2).c_str());
}
#pragma omp section
{
	//std::cout << (PARASURF "surf_2_2/surf_2_2 -n "+s+"2"+s2).c_str() << std::endl;
	system(("ulimit -t "+ulimit+"; "+PARASURF "surf_2_4/surf_2_4 -n "+s+"2"+s2).c_str());
}
#pragma omp section
{
	//std::cout << (PARASURF "surf_2_2/surf_2_2 -n "+s+"2"+s2).c_str() << std::endl;
	system(("ulimit -t "+ulimit+"; "+PARASURF "surf_3_4/surf_3_4 -n "+s+"3"+s2).c_str());
}
#pragma omp section
{
	//std::cout << (PARASURF "surf_2_2/surf_2_2 -n "+s+"2"+s2).c_str() << std::endl;
	system(("ulimit -t "+ulimit+"; "+PARASURF "surf_4_4/surf_4_4 -n "+s+"4"+s2).c_str());
}

}

			break;
		case single_core:
		default:
			//#ifdef WIN32
			//system((opt.surf_cmd+" -n "+s+"" REDIRECTION_APEX).c_str());
			return execute_async(opt.surf_cmd+" "SURF_OPT" "+s+" "REDIRECTION_APEX);
			//#else
			//system(("ulimit -t "+ulimit+"; "+opt.surf_cmd+" -n "+s+"" REDIRECTION_APEX).c_str());
			//#endif
	}
	


	return 0;
}







PID_type execute_async(const std::string& s)
{
	#ifdef WIN32
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof( si );

		const char *orig = s.c_str();

		// Convert to a wchar_t*
		size_t origsize = strlen(orig) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t wcstring[newsize];
		mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);

		CreateProcess(NULL,wcstring,NULL,NULL,false,CREATE_DEFAULT_ERROR_MODE,NULL,NULL, &si, &pi );
		return pi.dwProcessId;
	#else

		pid_t pid = fork();
		if(pid!=0) return pid;
		system(s.c_str());
		exit(0);
		return 9999;
	#endif
}

void kill_pid(const PID_type pid)
{
	if(pid==0) return;
	#ifdef WIN32
		//HANDLE hid = OpenProcess(PROCESS_ALL_ACCESS,false,pid);TerminateProcess(hid,17);CloseHandle(hid);
	#else
		kill(pid, SIGTERM);
		wait();
	#endif
}


*/
