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
						Pt.check_waitIJK( tk, tk, tk-1 );	// Check for SSRFB_(tk,tk,tk-1)

					GEQRT( A(tk,tk), T(tk,tk) );

					#ifdef DEBUG
					#pragma omp critical
					cout << "GEQRT(" << tk << "," << tk << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
					#endif

					Pt.setIJK(tk, tk, tk, DONE);			// Progress table update
				}   // GEQRT END
				else {
					// TSQRT
					if (tk != 0)
						Pt.check_waitIJK( ti, tk, tk-1 );	// Check for SSRFB_(ti,tk,tk-1)

					TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );

					#ifdef DEBUG
					#pragma omp critical
					cout << "TSQRT(" << ti << "," << tk << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
					#endif

					Pt.setIJK(ti, tk, tk, DONE);			// Progress table update
				}   // TSQRT END
			} // Decomposition Kernel END
			else // Update Kernel
			{
				if (ti == tk)
				{
					// LARFB
					Pt.check_waitIJK( tk, tk, tk );			// Check for GEQRT_(tk,tk,tk)
					if (tk != 0)
						Pt.check_waitIJK( tk, tj, tk-1 );	// Check for SSRFB_(tk,tj,tk-1)

					LARFB( PlasmaLeft, PlasmaTrans, A(tk,tk), T(tk,tk), A(tk,tj) );

					#ifdef DEBUG
					#pragma omp critical
					cout << "LARFB(" << tk << "," << tj << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
					#endif

				}   // LARFB END
				else
				{
					// SSRFB
					Pt.check_waitIJK( ti, tk, tk );			// Check for TSQRT_(ti,tk,tk)
					if (tk != 0)
						Pt.check_waitIJK( ti, tj, tk-1 );	// Check for SSRFB_(ti,tj,tk-1)

					SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tk), T(ti,tk), A(tk,tj), A(ti,tj) );

					#ifdef DEBUG
					#pragma omp critical
					cout << "SSRFB(" << ti << "," << tj << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
					#endif

					Pt.setIJK(ti, tj, tk, DONE);			// Progress table update
				}    // SSRFB END
			} // Update Kernel END

			ti = next_i;
			tj = next_j;
			tk = next_k;
		} // while-LOOP end
	} // End of outer most loop
	// Static Pipeline QR END
	////////////////////////////////////////////////////////////////////////////
}
