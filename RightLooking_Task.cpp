/*
 * RightLooking_Task.cpp
 *
 *  Created on: 2018/05/25
 *      Author: stomo
 */

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <omp.h>

#include "Kernels.hpp"

using namespace std;

void tileQR( const int MT, const int NT, const int NB, const int IB )
{
	// Progress table
	int **Ap, **Tp;

	Ap = (int **)malloc( sizeof(int*) * MT);
	for (int i=0; i<MT; i++)
		Ap[i] = (int *)malloc( sizeof(int) * NT);

	Tp = (int **)malloc( sizeof(int*) * MT);
	for (int i=0; i<MT; i++)
		Tp[i] = (int *)malloc( sizeof(int) * NT);

	static double ttime = 0.0;

	#pragma omp threadprivate(ttime)

	#pragma omp parallel
	{
		#pragma omp single
		{
			for (int tk=0; tk < min(MT,NT); tk++ )
			{
				#pragma omp task depend(inout:Ap[tk][tk]) depend(out:Tp[tk][tk])
				{
					//GEQRT( A(tk,tk), T(tk,tk) );
					ttime += T_GEQRT(NB,IB);
				}

				for (int tj=tk+1; tj < NT; tj++)
				{
					#pragma omp task depend(in:Tp[tk][tk]) depend(inout:Ap[tk][tj])
					{
						//LARFB( PlasmaLeft, PlasmaTrans, A(tk,tk), T(tk,tk), A(tk,tj) );
						ttime += T_LARFB(NB,IB);
					}
				}

				for (int ti=tk+1; ti < MT; ti++)
				{
					#pragma omp task depend(inout:Ap[tk][tk]) depend(out:Ap[ti][tk], Tp[ti][tk])
					{
						//TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );
						ttime += T_TSQRT(NB,IB);
					}

					for (int tj=tk+1; tj < NT; tj++)
					{
						#pragma omp task depend(in:Tp[ti][tk]) depend(inout:Ap[tk][tj], Ap[ti][tj])
						{
							//SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tk), T(ti,tk), A(tk,tj), A(ti,tj) );
							ttime += T_SSRFB(NB,IB);
						}
					} // j-LOOP END
				} // i-LOOP END
			} // k-LOOP END
		} // single section END

	}
	// Right Looking tile QR task END
	//////////////////////////////////////////////////////////////////////

	double time = 0.0;
	#pragma omp parallel
	{
		#pragma omp critical
		{
			if (time < ttime)
				time = ttime;
		}
		#pragma barrier
	}

	cout << NB << ", " << IB << ", " << time << endl;

}
