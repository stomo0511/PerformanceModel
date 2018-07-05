/*
 * Kernels.cpp
 *
 *  Created on: 2018/05/25
 *      Author: stomo
 */

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cmath>

#include "Kernels.hpp"

double T_GEQRT( const int NB, const int IB )
{
	const double b = (double)(NB);
	const double s = (double)(IB);
	double tk = 0.0;

	const double A = 11.75;
	const double B = 8.458;
	const double C = 16.33;
	const double D = 408.1;
	tk = b*b*b / ( A / ( 1.0 + B*s/b + C/s + D/b ) ) / Giga;

	return tk;
}

double T_TSQRT( const int NB, const int IB )
{
	const double b = (double)(NB);
	const double s = (double)(IB);
	double tk = 0.0;

	const double A = 7.780;
	const double B = 8.855;
	const double C = 23.52;
	const double D = 204.6;
	tk = b*b*b / ( A / ( 1.0 + B*s/b + C/s + D/b ) ) / Giga;

	return tk;
}

double T_LARFB( const int NB, const int IB )
{
	const double b = (double)(NB);
	const double s = (double)(IB);
	double tk = 0.0;

	const double A = 6.452;
	const double B = 1.076;
	const double C = 25.18;
	const double D = 271.2;
	tk = b*b*b / ( A / ( 1.0 + B*s/b + C/s + D/b ) ) / Giga;

	return tk;
}

double T_SSRFB( const int NB, const int IB )
{
	const double b = (double)(NB);
	const double s = (double)(IB);
	double tk = 0.0;

	const double A = 3.014;
	const double B = 0.3322;
	const double C = 31.17;
	const double D = 24.86;
	tk = b*b*b / ( A / ( 1.0 + B*s/b + C/s + D/b ) ) / Giga;

	return tk;
}

double T_SSMem( const double b, const int T )
{
	const double BandWidth = 76.8*Giga;  // Memory bandwidth
	const double L3size = 20*Mega;       // L3 cache size
	double D = (2.0 + 3.0*T)*b*b*8.0;    // Working memory

	if (D > L3size)
//		return (D - L3size) / BandWidth;
		return (D - L3size) / BandWidth *10;
	else
		return 0.0;
}

double T_LAMem( const double b, const int T )
{
	const double BandWidth = 76.8*Giga;  // Memory bandwidth
	const double L3size = 20*Mega;       // L3 cache size
	double D = (2.0 + 2.0*T)*b*b*8.0;    // Working memory

	if (D > L3size)
//		return (D - L3size) / BandWidth;
		return (D - L3size) / BandWidth *10;
	else
		return 0.0;
}
