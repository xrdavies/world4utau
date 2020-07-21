#ifndef _H_SYNTHESIS_
#define _H_SYNTHESIS_

#include "fftw3.h"
#include <stdlib.h>

void getMinimumPhaseSpectrum(double *inputSpec, fftw_complex *spectrum, fftw_complex *cepstrum, int fftl);

void synthesis(double *f0, int tLen, double **specgram, double **residualSpecgram, int fftl, double framePeriod, int fs, double *synthesisOut, int xLen);

#endif