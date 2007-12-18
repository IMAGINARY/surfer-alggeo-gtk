
/***************************************************************************
 *   Copyright (C) 2006 by Henning Meyer                                   *
 *   hmeyer@mathematik.uni-kl.de                                           *
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

#ifndef SB_MATRIX_H

#define SB_MATRIX_H

#include <valarray>
#include <vector>
#include <deque>
#include <sstream>

#include<math.h>
#include<float.h>


template<typename Z, int n>
struct c_array
{
Z data[n];
unsigned siz;

c_array(): data(), siz(0) {for(int i = 0; i < n; i++)data[i]=0;}
c_array(const unsigned N): data(), siz(N) {for(int i = 0; i < n; i++)data[i]=0;}
~c_array() {}

unsigned size() const {return siz;}
void push_back(const Z& x) {data[siz++]=x;}
void resize(const unsigned N) {siz=N;}
Z operator[](const unsigned i) const {return data[i];}
Z& operator[](const unsigned i)  {return data[i];}
};

template< typename T>
class matrix
{
	typedef std::valarray<T> data;
	public:
		matrix(const int n);
		matrix(const int n, const int m);
		matrix(const matrix& M);
		matrix(const int n, const int m, const data& V);
		matrix& operator=(const matrix& M);
		~matrix();

		T& el(const int i, const int j);
		const T el(const int i, const int j) const;
		
		T& operator[](const int x);
		const T operator[](const int x) const;

		

		matrix operator+(const matrix& Y) const;
		

		matrix operator-(const matrix& Y) const;
		
		

		int nrows() const;
		int ncols() const;
	private:
		int n;
		int m;
		data A;

};

template <typename T>
matrix<T>::matrix(const int N): n(N), m(N), A(N*N)
{
	T v;
	for( unsigned i = 0; i < A.size(); i++ )
		A[i] = v;
}

template <typename T>
matrix<T>::matrix(const int N, const int M): n(N), m(M), A(N*M)
{
	T v;
	for( unsigned i = 0; i < A.size(); i++ )
		A[i] = v;
}

template <typename T>
matrix<T>::matrix(const matrix& M): n(M.n), m(M.m), A(M.A.size())
{	for(unsigned i = 0; i < M.A.size(); i++)
	A[i] = M.A[i];
}

template <typename T>
matrix<T>::matrix(const int N, const int M, const data& V) : n(N), m(M), A(V)
{
	T v;
	for( unsigned i = 0; i < A.size(); i++ )
		A[i] = v;
}

template <typename T>
matrix<T>& matrix<T>::operator=(const matrix& M)
{
	n=M.n;
	m=M.m;
	A.resize(M.A.size());
	for(unsigned i = 0; i < M.A.size(); i++)
	A[i] = M.A[i];
	return *this;
}

template <typename T>
matrix<T>::~matrix() {}

template <typename T>
T& matrix<T>::el(const int i, const int j)
{
return A[(i-1)*m+(j-1)];}

template <typename T>
const T matrix<T>::el(const int i, const int j) const
{

	return A[(i-1)*m+(j-1)];
}

template <typename T>
T& matrix<T>::operator[](const int i) {return A[i];}

template <typename T>
const T matrix<T>::operator[](const int i) const {return A[i];}


template <typename T>
int matrix<T>::nrows() const {return n;}

template <typename T>
int matrix<T>::ncols() const {return m;}

template <typename T>
matrix<T> matrix<T>::operator+(const matrix<T>& Y) const
{


	matrix<T> R(n,m);
	for(unsigned i = 0; i< A.size(); i++)
	R.A[i] = A[i]+Y.A[i];
	return R;
}


template <typename T>
matrix<T> matrix<T>::operator-(const matrix<T>& Y) const
{
	matrix<T> R(n,m);
	for(unsigned i = 0; i< A.size(); i++)
	R.A[i] = A[i]-Y.A[i];
	return R;
}
		


template <typename T>
matrix<T> operator*(const matrix<T>& X, const matrix<T>& Y);

template <typename T>
matrix<T> transpose(const matrix<T>& X);

template <typename T>
matrix<T> operator~(const matrix<T>& X);

template <typename T>
matrix<T> hcat(const matrix<T>& X, const matrix<T>& Y, const int v = 1);

template <typename T>
matrix<T> vcat(const matrix<T>& X, const matrix<T>& Y, const int v = 1);

template <typename T>
matrix<T> unitmat(const int n, const T d = T(1));


template <typename T>
matrix<T> operator*(const matrix<T>& X, const matrix<T>& Y)
{

	matrix<T> R(X.nrows(), Y.ncols());
	for(int i = 1; i <= X.nrows(); i++)
	for(int j = 1; j <= Y.ncols(); j++)
	{
	T r(0);
	for(int k = 1; k <= X.ncols(); k++)
	r += X.el(i,k)*Y.el(k,j);
	R.el(i,j) = r;
	}
	return R;
}

template <typename T>
matrix<T> operator-(const matrix<T>& X)
{
	matrix<T> R(X.nrows(), X.ncols());
	for(int i = 1; i <= X.nrows(); i++)
	for(int j = 1; j <= X.ncols(); j++)
	{
	
	R.el(i,j) = -X.el(i,j);
	}
	return R;
}


template <typename T>
matrix<T> operator*(const T& x, const matrix<T>& Y)
{
	matrix<T> R(Y.nrows(), Y.ncols());
	for(int i = 1; i <= Y.nrows(); i++)
	for(int j = 1; j <= Y.ncols(); j++)
	{
	
	R.el(i,j) = x*Y.el(i,j);
	}
	return R;
}

template <typename T>
matrix<T> transpose(const matrix<T>& X)
{
	matrix<T> R(X.ncols(), X.nrows());
	for(int i = 1; i <= X.nrows(); i++)
	for(int j = 1; j <= X.ncols(); j++)
	R.el(j,i) = X.el(i,j);
	return R;
}

template <typename T>
matrix<T> operator~(const matrix<T>& X)
{return transpose(X);}



template <typename T>
matrix<T> hcat(const matrix<T>& X, const matrix<T>& Y, const int v)
{

	matrix<T> R(X.nrows(),X.ncols()+v*Y.ncols());
	for(int i = 1; i <= X.nrows(); i++)
	{
	for(int j = 1; j <= X.ncols(); j++)
	R.el(i,j) = X.el(i,j);
	
	for(int w = 0; w < v; w++)
	for(int j = 1; j <= Y.ncols(); j++)
	R.el(i,j+X.ncols()+w*Y.ncols()) = Y.el(i,j);
	}
	return R;
}

template <typename T>
matrix<T> vcat(const matrix<T>& X, const matrix<T>& Y, const int v)
{

	matrix<T> R(X.nrows()+v*Y.nrows(),X.ncols());
	for(int j = 1; j <= X.ncols(); j++)
	{
	for(int i = 1; i <= X.nrows(); i++)
	R.el(i,j) = X.el(i,j);
	
	for(int w = 0; w < v; w++)
	for(int i = 1; i <= Y.nrows(); i++)
	R.el(i+X.nrows()+w*Y.nrows(),j) = Y.el(i,j);
	}
	return R;
}

template <typename T>
bool operator==(const matrix<T>& A, const matrix<T>& B)
{
	if(A.nrows() != B.nrows() || A.ncols() != B.ncols())
	return false;
	for(int i = 1; i <= A.nrows(); i++)
	for(int j = 1; j <= A.ncols(); j++)
	if(A.el(i,j) != B.el(i,j))
	return false;

	return true;
}


template <typename T>
bool operator!=(const matrix<T>& A, const matrix<T>& B)
{
	return !(A==B);
}

template <typename T>
matrix<T> unitmat(const int n, const T  d)
{
	matrix<T> R(n);
	for(int i = 1; i <= n; i++)
	R.el(i,i) = d;
	return R;
}




template< typename T>
matrix<T> submat(const matrix<T>& X, const int di, const int dj, const int x, const int y)
{

	matrix<T> R(x,y);
	for(int i = 0; i < x; i++)
	for(int j = 0; j < y; j++)
	R.el(1+i,1+j) = X.el(i+di,j+dj);
	return R;
	
}



template<typename T>
std::ostream& operator<<(std::ostream& o, const matrix<T>& m)
{
	o << "[";
	for(int i = 1; i <= m.nrows(); i++)
	{
		if(i>1) o << ";";
		for(int j = 1; j <= m.ncols(); j++)
		{
		if(j>1) o << ",";
			o << m.el(i,j);
		}
	}
	o << "]";
	return o;
}




template<typename T>
void assign(matrix<T>& X,const int di, const int dj,const matrix<T> Y)
{
	

	for(int i = 0; i < Y.nrows(); i++)
	for(int j = 0; j < Y.ncols(); j++)
	X.el(di+i,dj+j) = Y.el(1+i,1+j);
}


const double EPS = 0.000001;
struct rot_data
{
	double rot_x;
	double rot_y;
	double rot_z;
};


rot_data rot_yxz(const matrix<double> M);



matrix<double> rotx(const double a);

matrix<double> rotz(const double a);


matrix<double> roty(const double a);


template<typename T>
T det(const matrix<T>& M)
{
	if(M.nrows() == 1) return M.el(1,1);
	T r;
	int f = 1;
	for(int i = 1; i <= M.nrows(); i++)
	{
		matrix<T> X(M.nrows()-1);
		if(i == 1)
		X = submat(M,i+1,2,M.nrows()-i,M.nrows()-1);
		else if (i == M.nrows())
		X = submat(M,1,2,i-1,M.nrows()-1);
		else
		X = vcat(submat(M,1,2,i-1,M.nrows()-1),submat(M,i+1,2,M.nrows()-i,M.nrows()-1));

		r = r + M.el(i,1)*T(f) * det(X);
		f = -f;
	}
	return r;
}


template<typename T>
matrix<T> inv(const matrix<T>& M)
{
	//std::cout<<"M "<<M<<std::endl;
	matrix<T> R(M.nrows());
	for(int i = 1; i <= M.nrows(); i++)
	for(int j = 1; j <= M.ncols(); j++)
	{
		matrix<T> A = submat(M,1,1,i-1,j-1);
		matrix<T> B = submat(M,1,j+1,i-1,M.nrows()-j);
		matrix<T> C = submat(M,i+1,1,M.nrows()-i,j-1);
		matrix<T> D = submat(M,i+1,j+1,M.nrows()-i,M.nrows()-j);

		matrix<T> X = vcat(hcat(A,B),hcat(C,D));
		//std::cout<<"("<<i<<","<<j<<") "<<X<<std::endl;
		R.el(j,i) = det(X);
		if(i+j%2==1) R.el(j,i) = -R.el(j,i);
	}
	//std::cout<<"R "<<R<<std::endl;
	R = (1/det(M))*R;
	
	return R;
}

#endif
