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

	double time = 0.0;
	double ttime;
	double max;

	#pragma omp parallel private(ttime)
	{
		#pragma omp single
		{
			for (int tk=0; tk < min(MT,NT); tk++ )
			{
				#pragma omp task depend(inout:Ap[tk][tk]) depend(out:Tp[tk][tk])
				{
					//GEQRT( A(tk,tk), T(tk,tk) );
					time += T_GEQRT(NB,IB);
				}

				ttime = 0.0;
				for (int tj=tk+1; tj < NT; tj++)
				{
					#pragma omp task depend(in:Tp[tk][tk]) depend(inout:Ap[tk][tj])
					{
						//LARFB( PlasmaLeft, PlasmaTrans, A(tk,tk), T(tk,tk), A(tk,tj) );
						ttime += T_LARFB(NB,IB);
					}
				}

				max = 0.0;
				#pragma omp critical
				{
					if (max < ttime)
						max = ttime;
				}
				#pragma omp master
				{
					time += ttime;
				}

				for (int ti=tk+1; ti < MT; ti++)
				{
					#pragma omp task depend(inout:Ap[tk][tk]) depend(out:Ap[ti][tk], Tp[ti][tk])
					{
						//TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );
						time += T_TSQRT(NB,IB);
					}

					ttime = 0.0;
					for (int tj=tk+1; tj < NT; tj++)
					{
						#pragma omp task depend(in:Tp[ti][tk]) depend(inout:Ap[tk][tj], Ap[ti][tj])
						{
							//SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tk), T(ti,tk), A(tk,tj), A(ti,tj) );
							ttime += T_SSRFB(NB,IB);
						}
					} // j-LOOP END

					max = 0.0;
					#pragma omp critical
					{
						if (max < ttime)
							max = ttime;
					}
					#pragma omp master
					{
						time += ttime;
					}

				} // i-LOOP END
			} // k-LOOP END
		} // parallel section END
	}
	// Right Looking tile QR task END
	//////////////////////////////////////////////////////////////////////
	cout << "# of threads = " << omp_get_max_threads() << endl;
	cout << "time = " << time << endl;
}
