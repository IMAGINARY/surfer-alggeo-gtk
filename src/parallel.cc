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


#define PARASURF "/home/ubuntu/totalsurf/"

void my_system(const std::string& s, const std::string& s2, const std::string& ulimit, const surfer_options& opt)
{
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
			#ifdef WIN32
			system((opt.surf_cmd+" -n "+s+"" REDIRECTION_APEX).c_str());
			#else
			system(("ulimit -t "+ulimit+"; "+opt.surf_cmd+" -n "+s+"" REDIRECTION_APEX).c_str());
			#endif
	}
	


	
}









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
		PROCESS_INFORMATION PI;
		std::string t=s;
		CreateProcess(NULL,(char*)t.c_str(),NULL,NULL,false,0,NULL,NULL,NULL,&PI);
		return PI.dwProcessId;

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
		/*HANDLE hid = OpenProcess(PROCESS_ALL_ACCESS,false,pid);TerminateProcess(hid,17);CloseHandle(hid);*/
	#else
		kill(pid, SIGTERM);
		wait();
	#endif
}




void my_kill(PID_type pid)
{
//#ifndef WIN32
		if(CM == single_core)
		kill_pid(pid);
		else		
		system("killall -9 -q -r surf._. ");

		
//#endif

}

