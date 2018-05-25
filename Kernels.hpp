/*
 * Kernels.hpp
 *
 *  Created on: 2018/05/25
 *      Author: stomo
 */

#ifndef KERNELS_HPP_
#define KERNELS_HPP_

#define Giga 1000000000;
#define Mega 1000000;

double T_GEQRT( const int NB, const int IB );
double T_TSQRT( const int NB, const int IB );
double T_LARFB( const int NB, const int IB );
double T_SSRFB( const int NB, const int IB );
double T_SSMem( const double b, const int T );
double T_LAMem( const double b, const int T );

#endif /* KERNELS_HPP_ */
