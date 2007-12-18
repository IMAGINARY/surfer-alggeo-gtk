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


/*

rotation in surf


y before x before z

*/

matrix<double> rotx(const double a)
{
	matrix<double> M(3);
	M.el(1,1) = 1.0;
	M.el(2,2) = cos(a);
	M.el(2,3) = -sin(a);
	M.el(3,2) = sin(a);
	M.el(3,3) = cos(a);

	return M;
}


matrix<double> rotz(const double a)
{
	matrix<double> M(3);
	M.el(3,3) = 1.0;
	M.el(1,1) = cos(a);
	M.el(1,2) = -sin(a);
	M.el(2,1) = sin(a);
	M.el(2,2) = cos(a);

	return M;
}


matrix<double> roty(const double a)
{
	matrix<double> M(3);
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
