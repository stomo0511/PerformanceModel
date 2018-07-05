//
//  main.cpp
//
//  Created by T. Suzuki on 2018/05/25
//  Copyright (c) 2013 T. Suzuki. All rights reserved.
//

#include <iostream>
#include <omp.h>
#include <cassert>
#include <cstdlib>

//#define DEBUG

using namespace std;

void tileQR( const int MT, const int NT, const int NB, const int IB );

int main(int argc, const char * argv[])
{
	if (argc < 5)
	{
		cerr << "Usage: a.out [M] [N] [NB] [IB]\n";
		exit (1);
	}

	const int M =  atoi(argv[1]);  // n. of rows of the matrix
	const int N =  atoi(argv[2]);  // n. of columns of the matrix
	const int NB = atoi(argv[3]);  // tile size
	const int IB = atoi(argv[4]);  // inner blocking size

	assert( M >= N );
	assert( NB >= IB );

	#ifdef DEBUG
	cout << "M = " << M << ", N = " << N << ", NB = " << NB << ", IB = " << IB << endl;
	cout << "Max threads = " << omp_get_max_threads() << endl;
	cout << "clock precision = " << omp_get_wtick() << endl;
	#endif

	cout << M << ", ";

	const int MT = ( M % NB == 0 ? M / NB : M / NB +1 );
	const int NT = ( N % NB == 0 ? N / NB : N / NB +1 );

	tileQR(MT,NT,NB,IB);

	return EXIT_SUCCESS;
}
