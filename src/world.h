// 音声分析合成法 WORLD by M. Morise
//
// FFTWを使うので，別途インストールが必要です．
//

// 現状で分かっているバグ
// decimateForF0 : 開始直後・終了間際4サンプルくらいに誤差が入ります．

#include "fftw3.h"
#include <stdlib.h>
#include <windows.h>
#include <math.h>

//メモリリークチェック：
//* VS2008とかだと通らないかもしれないのでその場合はコメントにすること。
#ifdef WIN32
 #ifndef _CRTDBG_MAP_ALLOC
  #define _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
 #endif
#endif
//*/
#define PI 3.1415926535897932384

// windowsならでは
#pragma warning( disable : 4996 )

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")
#define MAX_FFT_LENGTH 2048
#define FLOOR_F0 71.0
#define DEFAULT_F0 150.0
#define LOW_LIMIT 65.0
// 71は，fs: 44100においてFFT長を2048にできる下限．
// 70 Hzにすると4096点必要になる．
// DEFAULT_F0は，0.0.4での新機能．調整の余地はあるが，暫定的に決定する．

// F0推定法 DIO : Distributed Inline-filter Operation
void dio(double *x, int xLen, int fs, double framePeriod, 
		 double *timeAxis, double *f0);
int getSamplesForDIO(int fs, int xLen, double framePeriod);

// スペクトル包絡推定法 STAR : Synchronous Technique and Adroit Restoration
int getFFTLengthForStar(int fs);
void star(double *x, int xLen, int fs, double *timeAxis, double *f0,
		  double **specgram);
void getMinimumPhaseSpectrum(double *inputSpec, fftw_complex *spectrum, fftw_complex *cepstrum, int fftl);

// 非周期性指標推定法 PLATINUM : 名称未定
void platinum(double *x, int xLen, int fs, double *timeAxis, double *f0, double **specgram, 
		 double **residualSpecgram);


// WORLD Synthesis
void synthesis(double *f0, int tLen, double **specgram, double **residualSpecgram, int fftl, double framePeriod, int fs, 
			   double *synthesisOut, int xLen);
void getMinimumPhaseSpectrum(double *inputSpec, fftw_complex *spectrum, fftw_complex *cepstrum, int fftl);

//------------------------------------------------------------------------------------
// Matlab 関数の移植
double std(double *x, int xLen);
void inv(double **r, int n, double **invr);
void fftfilt(double *x, int xlen, double *h, int hlen, int fftl, double *y);
float randn(void);
void histc(double *x, int xLen, double *y, int yLen, int *index);
void interp1(double *t, double *y, int iLen, double *t1, int oLen, double *y1);
long decimateForF0(double *x, int xLen, double *y, int r);
void filterForDecimate(double *x, int xLen, double *y, int r);
int round(double x);
void diff(double *x, int xLength, double *ans);
void interp1Q(double x, double shift, double *y, int xLength, double *xi, int xiLength, double *ans);

