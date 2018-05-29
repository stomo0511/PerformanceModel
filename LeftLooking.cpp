//
//  LeftLooking
//
//  Created by T. Suzuki on 2018/05/25
//  Copyright (c) 2018 T. Suzuki. All rights reserved.
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

	double tmp;
	static double ttime = 0.0;

	#pragma omp threadprivate(ttime)

	#pragma omp parallel
	{
		#pragma omp for schedule(static,1) private(tmp)
		for (int tk = 0; tk < NT; tk++)
		{
			for (int tl = 0; tl < min(MT,tk); tl++)
			{
				tmp = omp_get_wtime();
				Pt.check_waitIJK(tl, tl, tl);	// Check for GEQRT_(tl,tl,tl)
				ttime += omp_get_wtime() - tmp;

				//LARFB( PlasmaLeft, PlasmaTrans, A(tl,tl), T(tl,tl), A(tl,tk) );
				ttime += T_LARFB(NB,IB);

				for (int ti = tl+1; ti < MT; ti++)
				{
					tmp = omp_get_wtime();
					Pt.check_waitIJK(ti, tl, tl);	// Check for TSQRT_(ti,tl,tl)
					ttime += omp_get_wtime() - tmp;

					//SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tl), T(ti,tl), A(tl,tk), A(ti,tk) );
					ttime += T_SSRFB(NB,IB);
				} // End of i-loop
			} // End of l-loop

			if ( tk < MT )
			{
				//GEQRT( A(tk,tk), T(tk,tk) );
				ttime += T_GEQRT(NB,IB);

				tmp = omp_get_wtime();
				Pt.setIJK(tk, tk, tk, DONE);  // Progress table update
				ttime += omp_get_wtime() - tmp;

				for (int ti = tk+1; ti < MT; ti++)
				{
					//TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );
					ttime += T_TSQRT(NB,IB);

					tmp = omp_get_wtime();
					Pt.setIJK(ti, tk, tk, DONE);  // Progress table update
					ttime += omp_get_wtime() - tmp;
				} // End of i-loop
			} // End if ( k < MT )

		} // End of k-loop

	}  // End of parallel section


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

	// Left Looking tile QR END
	////////////////////////////////////////////////////////////////////////////
}
