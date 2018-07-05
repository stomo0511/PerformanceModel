//
//  DynamicScheduling
//
//  Created by T. Suzuki on 2018/07/05.
//  Copyright (c) 2013 T. Suzuki. All rights reserved.
//

#include <iostream>
#include <queue>
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

	// Task Queue
	queue<Triplet> Qu;
	Triplet F(0,0,0);
	Qu.push(F);
	
	bool run_flag = true;
	// Definitions and Initialize　END
	//////////////////////////////////////////////////////////////////////

	double tmp;
	static double ttime = 0.0;

	#pragma omp threadprivate(ttime)
	
	//////////////////////////////////////////////////////////////////////
	// Dynamic Scheduling tile QR
	#pragma omp parallel private(F,tmp)
	{
		bool my_flag = true;
		bool my_turn = false;
		bool input = false;
		unsigned char s;
		
		while (my_flag)
		{
			#pragma omp critical (Queue)
			{
				if (!Qu.empty())
				{
					F = Qu.front(); Qu.pop();  // Dequeue task;
					my_turn = true;
				}
				else
					my_turn = false;
			}

			if (my_turn)
			{
				int ti = F.getI();
				int tj = F.getJ();
				int tk = F.getK();
				
				if (tj == tk)
				{
					if (ti == tk)
					{
						// GEQRT
						ttime += T_GEQRT(NB,IB);

						#ifdef DEBUG
						#pragma omp critical
						cout << "GEQRT(" << tk << "," << tk << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
						#endif

						// Enqueue TSQRT task
						if (ti+1 < MT)
						{
							input = false;

							#pragma omp critical (Progress)
							{
								s = Pt.getIJK( ti+1, tj, tk );
								s |= DIR_I;
								Pt.setIJK( ti+1, tj, tk, s );
		
								if (s == DONE)
									input = true;
							}

							if (input)
							{
								F.setIJK( ti+1, tj, tk );
		
								#pragma omp critical (Queue)
								{
									Qu.push(F);
								}
							}
						} // Enqueue TSQRT task End
						else {
							#pragma omp critical (End)
							run_flag = false;  // End flag <- true
						}
						
						// Enqueue LARFB task
						for (int j=tk+1; j<NT; j++)
						{
							input = false;

							#pragma omp critical (Progress)
							{
								s = Pt.getIJK( ti, j, tk );
								s |= DIR_J;
								Pt.setIJK( ti, j, tk, s );
		
								if (s == DONE)
									input = true;
							}

							if (input)
							{
								F.setIJK( ti, j, tk );
		
								#pragma omp critical (Queue)
								Qu.push(F);
							}
						} // Enqueue LARFB task End
					} // GEQRT END
					else
					{
						// TSQRT
						ttime += T_TSQRT(NB,IB);

						#ifdef DEBUG
						#pragma omp critical
						cout << "TSQRT(" << ti << "," << tk << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
						#endif

						// Enqueue TSQRT task
						if (ti+1 < MT)
						{
							input = false;

							#pragma omp critical (Progress)
							{
								s = Pt.getIJK( ti+1, tj, tk );
								s |= DIR_I;
								Pt.setIJK( ti+1, tj, tk, s );
		
								if (s == DONE)
									input = true;
							}

							if (input)
							{
								F.setIJK( ti+1, tj, tk );
		
								#pragma omp critical (Queue)
								Qu.push(F);
							}
						} // Enqueue TSQRT task End
						else if (tj+1 >= NT)
						{
							#pragma omp critical (End)
							run_flag = false;  // End flag <- true
						}
	    
						// Enqueue SSRFB task
						for (int j=tk+1; j<NT; j++)
						{
							input = false;

							#pragma omp critical (Progress)
							{
								s = Pt.getIJK( ti, j, tk );
								s |= DIR_J;
								Pt.setIJK( ti, j, tk, s );
		
								if (s == DONE)
									input = true;
							}

							if (input)
							{
								F.setIJK( ti, j, tk );
		
								#pragma omp critical (Queue)
								Qu.push(F);
							}
						} // Enqueue SSRFB task End
					} // TSQRT END
				}
				else
				{
					if (ti == tk)
					{
						// LARFB
						ttime += T_LARFB(NB,IB);

						#ifdef DEBUG
						#pragma omp critical
						cout << "LARFB(" << tk << "," << tj << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
						#endif

						// Enqueue SSRFB task
						if (ti+1 < MT)
						{
							input = false;

							#pragma omp critical (Progress)
							{
								s = Pt.getIJK( ti+1, tj, tk );
								s |= DIR_I;
								Pt.setIJK( ti+1, tj, tk, s );
		
								if (s == DONE)
									input = true;
							}

							if (input)
							{
								F.setIJK( ti+1, tj, tk );
		
								#pragma omp critical (Queue)
								Qu.push(F);
							}
						} // Enqueue SSRFB task End
					} // LARFB END
					else
					{
						// SSRFB
						ttime += T_SSRFB(NB,IB);

						#ifdef DEBUG
						#pragma omp critical
						cout << "SSRFB(" << ti << "," << tj << "," << tk << ") : " << omp_get_thread_num() << " : " << omp_get_wtime() - ttime << "\n";
						#endif

						// Enqueue GEQRT task
						input = false;

						#pragma omp critical (Progress)
						{
							s = Pt.getIJK( ti, tj, tk+1 );
							s |= DIR_K;
							Pt.setIJK( ti, tj, tk+1, s );
							
							if (s == DONE)
								input = true;
						}

						if (input)
						{
							F.setIJK( ti, tj, tk+1 );
	      
							#pragma omp critical (Queue)
							Qu.push(F);
						}
						// Enqueue GEQRT task End
						
						// Enqueue SSRFB task
						if (ti+1 < MT)
						{
							input = false;

							#pragma omp critical (Progress)
							{
								s = Pt.getIJK( ti+1, tj, tk );
								s |= DIR_I;
								Pt.setIJK( ti+1, tj, tk, s );
								
								if (s == DONE)
									input = true;
							}

							if (input)
							{
								F.setIJK( ti+1, tj, tk );
								
								#pragma omp critical (Queue)
								Qu.push(F);
							}
						} // Enqueue SSRFB task End
					} // SSRFB END
				}
			} // my_turn END
			
			#pragma omp critical (End)
			my_flag = run_flag;
		} // my_flag END
	} // End of outer most loop

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

	// Dynamic Scheduling QR END
	//////////////////////////////////////////////////////////////////////
}
