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

	const double A = 48.68;
	const double B = 7.126;
	const double C = 8.372;
	const double D = 145.3;
	tk = b*b*b / ( A / ( 1.0 + B*s/b + C/s + D/b ) ) / Giga;

	return tk;
}

double T_TSQRT( const int NB, const int IB )
{
	const double b = (double)(NB);
	const double s = (double)(IB);
	double tk = 0.0;

	const double A = 30.05;
	const double B = 9.952;
	const double C = 7.171;
	const double D = -14.73;
	tk = b*b*b / ( A / ( 1.0 + B*s/b + C/s + D/b ) ) / Giga;

	return tk;
}

double T_LARFB( const int NB, const int IB )
{
	const double b = (double)(NB);
	const double s = (double)(IB);
	double tk = 0.0;

	const double A = 21.45;
	const double C = 20.55;
	const double D = 308.7;
	const double E = 1031;
	tk = b*b*b / ( A / ( 1.0 + C/s + D/b + E*s/b/b ) ) / Giga;

	return tk;
}

double T_SSRFB( const int NB, const int IB )
{
	const double b = (double)(NB);
	const double s = (double)(IB);
	double tk = 0.0;

	const double A = 13.99;
	const double C = 4.559;
	const double D = 63.25;
	const double E = 102.9;
	tk = b*b*b / ( A / ( 1.0 + C/s + D/b + E*s/b/b ) ) / Giga;

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
