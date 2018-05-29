//
//  RightLooking
//
//  Created by T. Suzuki on 2018/05/25
//  Copyright (c) 2013 T. Suzuki. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <omp.h>

#include "Kernels.hpp"

using namespace std;

void tileQR( const int MT, const int NT, const int NB, const int IB )
{
	static double ttime = 0.0;

	#pragma omp threadprivate(ttime)

	for (int tk=0; tk < min(MT,NT); tk++ )
	{
		#pragma omp parallel
		{
			#pragma omp single
			{
				//GEQRT( A(tk,tk), T(tk,tk) );
				ttime += T_GEQRT(NB,IB);
			}

			#pragma omp for
			for (int tj=tk+1; tj < NT; tj++)
			{
				//LARFB( PlasmaLeft, PlasmaTrans, A(tk,tk), T(tk,tk), A(tk,tj) );
				ttime += T_LARFB(NB,IB);
			}

			for (int ti=tk+1; ti < MT; ti++)
			{
				#pragma omp single
				{
					//TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );
					ttime += T_TSQRT(NB,IB);
				}

				#pragma omp for
				for (int tj=tk+1; tj < NT; tj++)
				{
					//SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tk), T(ti,tk), A(tk,tj), A(ti,tj) );
					ttime += T_SSRFB(NB,IB);
				} // j-LOOP END
			} // i-LOOP END
		} // parallel section END
	} // k-LOOP END
	// Right Looking tile QR END
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
