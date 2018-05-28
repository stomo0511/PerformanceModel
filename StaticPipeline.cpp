//
//  StaticPipeline
//
//  Created by T. Suzuki on 2018/05/25
//  Copyright (c) 2013 T. Suzuki. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <omp.h>

#include "Progress.hpp"
#include "Kernels.hpp"

using namespace std;

void tileQR( const int MT, const int NT, const int NB, const int IB )
{
	// Progress Table
	Progress_Table Pt( MT, NT, min(MT,NT) );
	Pt.Init();
	Pt.setIJK(0, 0, 0, NYET);

	double time = 0.0;
	double ttime, tmp;
	double max;

	#pragma omp parallel private(ttime,tmp)
	{
		ttime = 0.0;

		int tk = 0;
		int tj = omp_get_thread_num();

		while (tj >= NT)
		{
			tk++;
			tj = tj - NT + tk;
		}

		int ti = tk;

		while (tk < min(MT,NT) && tj < NT)
		{
			int next_i = ti;
			int next_j = tj;
			int next_k = tk;

			next_i++;
			if (next_i == MT)
			{
				next_j += omp_get_num_threads();

				while (next_j >= NT && next_k < min(MT,NT))
				{
					next_k++;
					next_j = next_j - NT + next_k;
				}
				next_i = next_k;
			}

			if (tj == tk)
			{
				if (ti == tk)
				{
					// GEQRT
					if (tk != 0)
					{
						tmp = omp_get_wtime();
						Pt.check_waitIJK( tk, tk, tk-1 );	// Check for SSRFB_(tk,tk,tk-1)
						ttime += omp_get_wtime() - tmp;
					}

					//GEQRT( A(tk,tk), T(tk,tk) );
					ttime += T_GEQRT(NB,IB);

					tmp = omp_get_wtime();
					Pt.setIJK(tk, tk, tk, DONE);			// Progress table update
					ttime += omp_get_wtime() - tmp;
				}   // GEQRT END
				else {
					// TSQRT
					if (tk != 0)
					{
						tmp = omp_get_wtime();
						Pt.check_waitIJK( ti, tk, tk-1 );	// Check for SSRFB_(ti,tk,tk-1)
						ttime += omp_get_wtime() - tmp;
					}

					//TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );
					ttime += T_TSQRT(NB,IB);

					tmp = omp_get_wtime();
					Pt.setIJK(ti, tk, tk, DONE);			// Progress table update
					ttime += omp_get_wtime() - tmp;
				}   // TSQRT END
			} // Decomposition Kernel END
			else // Update Kernel
			{
				if (ti == tk)
				{
					// LARFB
					tmp = omp_get_wtime();
					Pt.check_waitIJK( tk, tk, tk );			// Check for GEQRT_(tk,tk,tk)
					ttime += omp_get_wtime() - tmp;

					if (tk != 0)
					{
						tmp = omp_get_wtime();
						Pt.check_waitIJK( tk, tj, tk-1 );	// Check for SSRFB_(tk,tj,tk-1)
						ttime += omp_get_wtime() - tmp;
					}

					//LARFB( PlasmaLeft, PlasmaTrans, A(tk,tk), T(tk,tk), A(tk,tj) );
					ttime += T_LARFB(NB,IB);

				}   // LARFB END
				else
				{
					// SSRFB
					tmp = omp_get_wtime();
					Pt.check_waitIJK( ti, tk, tk );			// Check for TSQRT_(ti,tk,tk)
					ttime += omp_get_wtime() - tmp;
					if (tk != 0)
					{
						tmp = omp_get_wtime();
						Pt.check_waitIJK( ti, tj, tk-1 );	// Check for SSRFB_(ti,tj,tk-1)
						ttime += omp_get_wtime() - tmp;
					}

					//SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tk), T(ti,tk), A(tk,tj), A(ti,tj) );
					ttime += T_SSRFB(NB,IB);

					tmp = omp_get_wtime();
					Pt.setIJK(ti, tj, tk, DONE);			// Progress table update
					ttime += omp_get_wtime() - tmp;
				}    // SSRFB END
			} // Update Kernel END

			ti = next_i;
			tj = next_j;
			tk = next_k;
		} // while-LOOP end

		max = 0.0;
		#pragma omp critical
		{
		  if (max < ttime)
		    max = ttime;
		}
		#pragma omp master
		cout << NB << ", " << IB << ", " << max << endl;

	} // End of outer most loop
	// Static Pipeline QR END
	////////////////////////////////////////////////////////////////////////////
}
