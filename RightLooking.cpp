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
	double time = 0.0;
	double ttime;
	double max;

	for (int tk=0; tk < min(MT,NT); tk++ )
	{
		#pragma omp parallel private(ttime)
		{
			#pragma omp master
			{
				//GEQRT( A(tk,tk), T(tk,tk) );
				time += T_GEQRT(NB,IB);
			}

			ttime = 0.0;
			#pragma omp for
			for (int tj=tk+1; tj < NT; tj++)
			{
				//LARFB( PlasmaLeft, PlasmaTrans, A(tk,tk), T(tk,tk), A(tk,tj) );
				ttime += T_LARFB(NB,IB);
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

			for (int ti=tk+1; ti < MT; ti++)
			{
				#pragma omp master
				{
					//TSQRT( A(tk,tk), A(ti,tk), T(ti,tk) );
					time += T_TSQRT(NB,IB);
				}

				ttime = 0.0;
				#pragma omp for
				for (int tj=tk+1; tj < NT; tj++)
				{
					//SSRFB( PlasmaLeft, PlasmaTrans, A(ti,tk), T(ti,tk), A(tk,tj), A(ti,tj) );
					ttime += T_SSRFB(NB,IB);
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
		} // parallel section END
	} // k-LOOP END
	// Right Looking tile QR END
	//////////////////////////////////////////////////////////////////////
	cout << "# of threads = " << omp_get_max_threads() << endl;
	cout << "time = " << time << endl;
}
