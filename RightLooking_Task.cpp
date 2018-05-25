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

#include <CoreBlasTile.hpp>
#include <TMatrix.hpp>

using namespace std;

//#define COUT
//#define ANIM

void tileQR( const int MT, const int NT, TMatrix& A, TMatrix& T )
{
	// Progress table
	int **Ap, **Tp;

	Ap = (int **)malloc( sizeof(int*) * MT);
	for (int i=0; i<MT; i++)
		Ap[i] = (int *)malloc( sizeof(int) * NT);

	Tp = (int **)malloc( sizeof(int*) * MT);
	for (int i=0; i<MT; i++)
		Tp[i] = (int *)malloc( sizeof(int) * NT);

	#ifdef ANIM
	cout << "Kernel,Ii,Ij,Ik,Time\n";
	#endif

	double ttime = omp_get_wtime();

	//////////////////////////////////////////////////////////////////////
	// Right Looking tile QR Task version
	#pragma omp parallel firstprivate(ttime)
	{
		#pragma omp single
		{
			for (int tk=0; tk < min(MT,NT); tk++ )
			{
				#pragma omp task depend(inout:Ap[tk][tk]) depend(out:Tp[tk][tk])
				{
					GEQRT( A(tk,tk), T(tk,tk) );

					#ifdef COUT
					#pragma omp critical
					cout << "GEQRT(" << tk << "," << tk << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
					#endif
					#ifdef ANIM
					cout << "GL," << tk << "," << tk << "," << tk << "," << omp_get_wtime() - ttime << endl;
					#endif
				}

				for (int tj=tk+1; tj < NT; tj++)
				{
					#pragma omp task depend(in:Tp[tk][tk]) depend(inout:Ap[tk][tj])
					{
						LARFB( PlasmaLeft, PlasmaTrans, A(tk,tk), T(tk,tk), A(tk,tj) );

						#ifdef COUT
						#pragma omp critical
						cout << "LARFB(" << tk << "," << tj << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
						#endif
						#ifdef ANIM
						#pragma omp critical
						cout << "LL," << tk << "," << tj << "," << tk << "," << omp_get_wtime() - ttime << endl;
						#endif
					}
				}

				for (int ti=tk+1; ti < MT; ti++)
				{
					#pragma omp task depend(inout:Ap[tk][tk]) depend(out:Ap[ti][tk], Tp[ti][tk])
					{
						TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );

						#ifdef COUT
						#pragma omp critical
						cout << "TSQRT(" << ti << "," << tk << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
						#endif
						#ifdef ANIM
						#pragma omp critical
						cout << "TL," << ti << "," << tk << "," << tk << "," << omp_get_wtime() - ttime << endl;
						#endif
					}

					for (int tj=tk+1; tj < NT; tj++)
					{
						#pragma omp task depend(in:Tp[ti][tk]) depend(inout:Ap[tk][tj], Ap[ti][tj])
						{
							SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tk), T(ti,tk), A(tk,tj), A(ti,tj) );

							#ifdef COUT
							#pragma omp critical
							cout << "SSRFB(" << ti << "," << tj << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
							#endif
							#ifdef ANIM
							#pragma omp critical
							cout << "SL," << ti << "," << tj << "," << tk << "," << omp_get_wtime() - ttime << endl;
							#endif
						}
					} // j-LOOP END
				} // i-LOOP END
			} // k-LOOP END
		} // parallel section END
	}
	// Right Looking tile QR task END
	//////////////////////////////////////////////////////////////////////
}
