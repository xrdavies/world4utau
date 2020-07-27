/*
 * *此程序是根据GPL（通用公共许可证）许可的。
 *请检查copying.txt以获取许可证的内容。
 *以下引自WORLD（0.0.4）的readme.txt。
 *>有两点需要注意。
 *>-使用该程序创建的应用程序是
 *>也必须分发源代码。
 *>-该应用程序还必须具有GPL许可证。
 */
//
// world4utau.cpp
//
//此源代码基于WORLD 0.0.4 test.cpp
//论据
// 1个输入文件（确定）
// 2个输出文件（确定）
// 3刻度（确定）
// 4％的速度→速度
// 5个标志t，g，P实现
// 6偏移
// 7个长度：请求长度
// 8个固定的上半部分
// 9最后未使用的部分。如果为负，则偏移使用的部分为空白
// 10卷（OK）卷
// 11调制（OK）调制
// 12〜弯音

/*
 * ※このプログラムはGPL (General Public License)でライセンスされています．
 * ライセンスの内容は，copying.txtをご確認ください．
 * 以下、WORLD(0.0.4)のreadme.txtより引用。
 * > 注意すべきは以下の2点です．
 * > ・本プログラムを利用して作ったアプリケーションは，
 * > 　ソースコードも一緒に配布する必要がある．
 * > ・そのアプリケーションもGPLライセンスにしなければならない．
 */
//
// world4utau.cpp
//
//このソースコードはWORLD0.0.4のtest.cppを元にしています
//引数
// 1 入力ファイル（OK）
// 2 出力ファイル（OK）
// 3 音階（OK）
// 4 タイムパーセント→Velocity
// 5 フラグ　t,g,P 実装
// 6 オフセット
// 7 長さ：要求長
// 8 前半の固定部分 fixed
// 9 最後の使用しない部分。マイナスの場合offsetからの使用部分 blank
// 10 ボリューム (OK) volm
// 11 モジュレーション (OK) modulation
// 12~ ピッチベンド  pitches

#include <stdio.h>
#include <stdlib.h>

#if defined(__WIN32__) || defined(_WIN32) || defined(_WINDOWS)
#include <windows.h>
#endif

#include "world.h"
#include "wavread.h"

#include <math.h>
#include <float.h>
#include <memory.h>

//分析移位量[毫秒]
// 分析シフト量 [msec]
//#define FRAMEPERIOD 2.0
#define FRAMEPERIOD 5.80498866213152

#pragma comment(lib, "winmm.lib")

int get64(int c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0' + 52;
	}
	else if (c >= 'A' && c <= 'Z')
	{
		return c - 'A';
	}
	else if (c >= 'a' && c <= 'z')
	{
		return c - 'a' + 26;
	}
	else if (c == '+')
	{
		return 62;
	}
	else if (c == '/')
	{
		return 63;
	}
	else
	{
		return 0;
	}
}

int decpit(char *str, int *dst, int cnt)
{
	int len = 0;
	int i, n = 0;
	int k = 0, num, ii;
	if (str != NULL)
	{
		len = strlen(str);
		for (i = 0; i < len; i += 2)
		{
			if (str[i] == '#')
			{
				i++;
				sscanf(str + i, "%d", &num);
				for (ii = 0; ii < num && k < cnt; ii++)
				{
					dst[k++] = n;
				}
				while (str[i] != '#' && str[i] != 0)
					i++;
				i--;
			}
			else
			{
				n = get64(str[i]) * 64 + get64(str[i + 1]);
				if (n > 2047)
					n -= 4096;
				if (k < cnt)
				{
					dst[k++] = n;
				}
			}
		}
	}
	return len;
}

double name2freq(char *tname, int trim)
{
	char c;
	int n, m, oct, num;
	//01234567890A
	//C D EF G A B
	c = tname[0];
	if (c >= 'A' && c <= 'G')
	{
		if (c <= 'B')
		{
			n = 9 + (c - 'A') * 2;
		}
		else if (c <= 'E')
		{
			n = (c - 'C') * 2;
		}
		else
		{
			n = 5 + (c - 'F') * 2;
		}

		c = tname[1];

		m = 2;
		if (c == '#')
		{
			n++;
		}
		else if (c == 'b')
		{
			n--;
		}
		else
		{
			m = 1;
		}

		if (tname[m] == 0)
		{
			return 0;
		}

		sscanf(tname + m, "%d", &oct);

		num = (n + oct * 12 - 21) * 10 + trim;
		//return num;
		//0 で 55Hz
		return 55.0 * pow(2, (double)num / 120.0);
	}
	return 0;
}
//
void makeFilename(const char *filename, const char *ext, char *output)
{
	strcpy(output, filename);
	char *cp = strrchr(output, '.');
	if (cp)
		*cp = 0;
	strcat(output, ext);

	printf("%s\n", output);
}

//TODO 分析ファイルが揃ってたらwaveは読まなくて良い。（今は必ず読んでる）
// 如果您拥有所有分析文件，则无需阅读wave。 （我现在肯定阅读过）
int readDIOParam(const char *filename, double *p_t[], double *p_f0[], int *p_fs, int *p_siglen) //return tLen
{
	char fname1[512];
	int n = 0;
	int len = 0, sps = 0;
	int i;
	int siglen = 0, fs = 0, tLen = 0;
	double *t = 0;
	double *f0 = 0;

	makeFilename(filename, ".dio", fname1);

	// DWORD elapsedTime;

	printf("read .dio:\n");
	// elapsedTime = timeGetTime();

	FILE *fp = fopen(fname1, "rb");
	if (fp)
	{
		char d[9];
		fread(d, 8, 1, fp);
		if (strncmp(d, "wrld-dio", 8) != 0)
		{
			fclose(fp);
			printf(" bad file.\n");
			return 0;
		}
		fread(&siglen, sizeof(int), 1, fp);
		fread(&fs, sizeof(int), 1, fp);
		fread(&tLen, sizeof(int), 1, fp);
		if (tLen > 0)
		{
			t = (double *)malloc(tLen * sizeof(double));
			f0 = (double *)malloc(tLen * sizeof(double));
			if (t && f0)
			{
				for (i = 0; i < tLen; i++)
				{
					fread(&(t[i]), sizeof(double), 1, fp);
					fread(&(f0[i]), sizeof(double), 1, fp);
				}
			}
			else
			{
				SAFE_FREE(t);
				SAFE_FREE(f0);
				t = 0;
				f0 = 0;
				tLen = 0;
				fprintf(stderr, " メモリーが確保できません。\n");
			}
		}
		fclose(fp);
	}
	*p_t = t;
	*p_f0 = f0;
	*p_fs = fs;
	*p_siglen = siglen;
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
	return tLen;
}

int getDIOParam(double x[], int signalLen, int fs, double framePeriod, double *p_t[], double *p_f0[])
{
	// DWORD elapsedTime;
	printf("DIO:");
	// elapsedTime = timeGetTime();
	int tLen = getSamplesForDIO(fs, signalLen, framePeriod);
	double *t = (double *)malloc(tLen * sizeof(double));
	double *f0 = (double *)malloc(tLen * sizeof(double));
	if (t && f0)
	{
		dio(x, signalLen, fs, framePeriod, t, f0);
	}
	else
	{
		fprintf(stderr, " メモリーが確保できません。\n");
		// fprintf(stderr, "无法保护内存。 %d\n");
		SAFE_FREE(t);
		SAFE_FREE(f0);
		t = 0;
		f0 = 0;
		tLen = 0;
	}
	*p_t = t;
	*p_f0 = f0;
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
	return tLen;
}
int writeDIOParam(int signalLen, int fs, int tLen, const char *filename, double t[], double f0[])
{
	char fname1[512];
	makeFilename(filename, ".dio", fname1);

	// DWORD elapsedTime;

	printf("write .dio\n");
	// elapsedTime = timeGetTime();

	//FILE *ft = fopen("dio0.txt", "wt");
	FILE *f = fopen(fname1, "wb");
	if (f)
	{
		fwrite("wrld-dio", 1, 8, f);
		fwrite(&signalLen, sizeof(int), 1, f);
		fwrite(&fs, sizeof(int), 1, f);
		fwrite(&tLen, sizeof(int), 1, f);
		int i;
		for (i = 0; i < tLen; i++)
		{
			int un;
			FP_SUBNORMAL;
			// if ((un = _fpclass(f0[i]) & 0x0087) != 0) //NaN,+Inf,-Inf,denormalを除外する // NaN,+Inf,-Inf,denormal排除
			// FIXME: 这里需要看看_fpclass 和 fpclassify的返回值上有什么不同之处 [ruix]
			// if ((un = fpclassify(f0[i]) & 0x0087) != 0)
			un = fpclassify(f0[i]);
			if (un == FP_NAN || un == FP_INFINITE || un == FP_SUBNORMAL)
			{
#ifdef _DEBUG
				printf("un[%d]=%04x!\n", i, un);
#endif
				f0[i] = 0;
			}
			fwrite(&(t[i]), sizeof(double), 1, f);
			fwrite(&(f0[i]), sizeof(double), 1, f);
			//fprintf(ft, "%lf\t%lf\n", t[i], f0[i]);
		}
		fclose(f);
	}

	// printf(": %d [msec]\n", timeGetTime() - elapsedTime);
	return 0;
}
//void getSTARParam(double x[], int signalLen, int fs, const char *filename, double t[], double f0[], double *specgram[], int tLen, int fftl, int flag_G)
double **readSTARParam(int signalLen, int fs, const char *filename, int tLen, int fftl)
{
	int i, j;
	char fname2[512];
	// DWORD elapsedTime;
	unsigned short tn = 0;
	unsigned short us = 0;
	int siglen = 0;
	int rate = 0;
	double **specgram = 0;

	makeFilename(filename, ".star", fname2);

	printf("read .star:\n");
	// elapsedTime = timeGetTime();

	FILE *fp = fopen(fname2, "rb");
	if (fp)
	{
		char st[9];
		fread(st, 1, 8, fp);
		if (strncmp(st, "wrldstar", 8) != 0)
		{
			fclose(fp);
			printf(" bad file.\n");
			return 0;
		}
		fread(&siglen, sizeof(int), 1, fp);
		fread(&rate, sizeof(int), 1, fp);
		fread(&tn, sizeof(unsigned short), 1, fp);
		fread(&us, sizeof(unsigned short), 1, fp);
		if (tn == tLen && us == (fftl / 2 + 1) && signalLen == siglen && fs == rate)
		{
			specgram = (double **)malloc(tLen * sizeof(double *));
			if (specgram)
			{
				for (i = 0; i < tLen; i++)
				{
					specgram[i] = (double *)malloc((fftl / 2 + 1) * sizeof(double));
					memset(specgram[i], 0, (fftl / 2 + 1) * sizeof(double));
					if (specgram[i])
					{
						for (j = 0; j <= fftl / 2; j++)
						{
							unsigned short v;
							fread(&v, sizeof(unsigned short), 1, fp);
							//= (unsigned short)(log(specgram[i][j]*(2048.0*2048*2048)+1) * 512.0 + 0.5);
							specgram[i][j] = (exp(v / 1024.0) - 1) * 1.16415321826935E-10; // /(2048.0*2048*2048);
						}
					}
					else
					{
						break;
					}
				}
				if (i < tLen)
				{
					for (j = 0; j < i; j++)
					{
						free(specgram[i]);
					}
					free(specgram);
					specgram = 0;
					fprintf(stderr, " メモリーが確保できません。%d\n", i);
				}
			}
			else
			{
				fprintf(stderr, " メモリーが確保できません。\n");
			}
		}
		else
		{
			tn = 0;
		}
		fclose(fp);
	}
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
	return specgram;
}
double **getSTARParam(double x[], int signalLen, int fs, double t[], double f0[], int tLen, int fftl)
{
	printf("STAR:");
	// DWORD elapsedTime = timeGetTime();

	double **specgram = (double **)malloc(sizeof(double *) * tLen);
	if (specgram)
	{
		int i, j;
		for (i = 0; i < tLen; i++)
		{
			specgram[i] = (double *)malloc(sizeof(double) * (fftl / 2 + 1));
			memset(specgram[i], 0, sizeof(double) * (fftl / 2 + 1));
			if (specgram[i])
			{
				memset(specgram[i], 0, sizeof(double) * (fftl / 2 + 1));
			}
			else
			{
				break;
			}
		}
		if (i == tLen)
		{
			star(x, signalLen, fs, t, f0, specgram);
		}
		else
		{
			for (j = 0; j < i; j++)
			{
				free(specgram[i]);
			}
			free(specgram);
			specgram = 0;
			fprintf(stderr, " メモリーが確保できません。%d\n", i);
		}
	}
	else
	{
		fprintf(stderr, " メモリーが確保できません。\n");
	}
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
	return specgram;
}
void writeSTARParam(int signalLen, int fs, const char *filename, double *specgram[], int tLen, int fftl)
{
	unsigned short tn;
	unsigned short us;
	int i, j;
	char fname2[512];
	makeFilename(filename, ".star", fname2);

	printf("write .star:");
	// DWORD elapsedTime = timeGetTime();

	short max = -32767, min = 32767;
	//FILE *ft = fopen("star0.txt", "wt");
	FILE *f1 = fopen(fname2, "wb");
	if (f1)
	{
		fwrite("wrldstar", 1, 8, f1);
		tn = (unsigned short)tLen;
		us = (unsigned short)fftl / 2 + 1;
		fwrite(&signalLen, sizeof(int), 1, f1);
		fwrite(&fs, sizeof(int), 1, f1);
		fwrite(&tn, sizeof(unsigned short), 1, f1);
		fwrite(&us, sizeof(unsigned short), 1, f1);
		for (i = 0; i < tLen; i++)
		{
			for (j = 0; j <= fftl / 2; j++)
			{
				int un;
				// if ((un = _fpclass(specgram[i][j]) & 0x0087) != 0)
				un = fpclassify(specgram[i][j]);
				if (un == FP_NAN || un == FP_INFINITE || un == FP_SUBNORMAL)
				{
					specgram[i][j] = 0;
#ifdef _DEBUG
					printf("un[%d][%d]=%04x!\n", i, j, un);
#endif
				}
				unsigned short v = (unsigned short)(log(specgram[i][j] * (2048.0 * 2048 * 2048) + 1) * 1024.0 + 0.5);
				fwrite(&v, sizeof(unsigned short), 1, f1);
				//fprintf(ft, "%0.9lf\t", specgram[i][j]*1000000.0);
				if (max < v)
				{
					max = v;
				}
				if (min > v)
				{
					min = v;
				}
			}
			//fprintf(ft, "\n");
		}
		fclose(f1);
	}
	//fclose(ft);
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
	printf("max = %d, min = %d\n", max, min);
}
double **readPlatinumParam(int signalLen, int fs, const char *filename, int tLen, int fftl)
{
	int i, j;

	// DWORD elapsedTime;
	unsigned short tn = 0;
	unsigned short us = 0;
	int siglen = 0;
	int rate = 0;
	double **residualSpecgram = 0;

	char fname3[512];
	makeFilename(filename, ".platinum", fname3);

	printf("read .platinum:\n");
	// elapsedTime = timeGetTime();

	FILE *fp = fopen(fname3, "rb");
	if (fp)
	{
		char b[9];
		fread(b, 1, 8, fp);
		if (strncmp(b, "platinum", 8) != 0)
		{
			fclose(fp);
			printf(" bad file.\n");
			return 0;
		}
		fread(&siglen, sizeof(int), 1, fp);
		fread(&rate, sizeof(int), 1, fp);
		fread(&tn, sizeof(unsigned short), 1, fp);
		fread(&us, sizeof(unsigned short), 1, fp);
		if (tn == tLen && (fftl + 1) == us && signalLen == siglen && fs == rate)
		{
			residualSpecgram = (double **)malloc(tLen * sizeof(double *));
			if (residualSpecgram)
			{
				for (i = 0; i < tLen; i++)
				{
					residualSpecgram[i] = (double *)malloc((fftl + 1) * sizeof(double));
					if (residualSpecgram[i])
					{
						memset(residualSpecgram[i], 0, (fftl + 1) * sizeof(double));
						for (j = 0; j <= fftl; j++)
						{
							short v;
							fread(&v, sizeof(short), 1, fp);
							residualSpecgram[i][j] = v * 3.90625E-03; // /256.0;
						}
					}
					else
					{
						break;
					}
				}
				if (i < tLen)
				{
					for (j = 0; j < i; j++)
					{
						free(residualSpecgram[i]);
					}
					free(residualSpecgram);
					residualSpecgram = 0;
					fprintf(stderr, " メモリーが確保できません。%d\n", i);
				}
			}
			else
			{
				fprintf(stderr, " メモリーが確保できません。\n");
			}
		}
		fclose(fp);
	}
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
	return residualSpecgram;
}
double **getPlatinumParam(double x[], int signalLen, int fs, double t[], double f0[], double *specgram[], int tLen, int fftl)
{
	printf("PLATINUM:");
	// DWORD elapsedTime = timeGetTime();

	double **residualSpecgram = (double **)malloc(sizeof(double *) * tLen);
	if (residualSpecgram)
	{
		int i, j;
		for (i = 0; i < tLen; i++)
		{
			residualSpecgram[i] = (double *)malloc(sizeof(double) * (fftl + 1));
			memset(residualSpecgram[i], 0, sizeof(double) * (fftl + 1));
			if (residualSpecgram[i])
			{
				memset(residualSpecgram[i], 0, sizeof(double) * (fftl + 1));
			}
			else
			{
				break;
			}
		}
		if (i == tLen)
		{
			platinum(x, signalLen, fs, t, f0, specgram, residualSpecgram);
		}
		else
		{
			for (j = 0; j < i; j++)
			{
				free(residualSpecgram[i]);
			}
			free(residualSpecgram);
			residualSpecgram = 0;
			fprintf(stderr, " メモリーが確保できません。%d\n", i);
		}
	}
	else
	{
		fprintf(stderr, " メモリーが確保できません。\n");
	}
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
	return residualSpecgram;
}
void writePlatinumParam(int signalLen, int fs, const char *filename, double *residualSpecgram[], int tLen, int fftl)
{
	int i, j;
	// DWORD elapsedTime;
	printf("write .platinum:");
	// elapsedTime = timeGetTime();

	unsigned short tn = 0;
	unsigned short us = 0;
	short max = -32767, min = 32767;

	char fname3[512];
	makeFilename(filename, ".platinum", fname3);

	FILE *f1 = fopen(fname3, "wb");
	if (f1)
	{
		fwrite("platinum", 1, 8, f1);
		tn = (unsigned short)tLen;
		us = (unsigned short)(fftl + 1);
		fwrite(&signalLen, sizeof(int), 1, f1);
		fwrite(&fs, sizeof(int), 1, f1);
		fwrite(&tn, sizeof(unsigned short), 1, f1);
		fwrite(&us, sizeof(unsigned short), 1, f1);
		for (i = 0; i < tLen; i++)
		{
			for (j = 0; j <= fftl; j++)
			{
				int un;
				// if ((un = _fpclass(residualSpecgram[i][j]) & 0x0087) != 0)
				un = fpclassify(residualSpecgram[i][j]);
				if (un == FP_NAN || un == FP_INFINITE || un == FP_SUBNORMAL)
				{
					residualSpecgram[i][j] = 0;
#ifdef _DEBUG
					printf("unr[%d][%d]=%04x!\n", i, j, un);
#endif
				}
				short v = (short)(residualSpecgram[i][j] * 256.0);
				//v = log(v * (2048.0*2048.0*2048.0) + 1) * 1024.0;
				if (v > 32767)
				{
					v = 32767;
				}
				else if (v < -32768)
				{
					v = -32768; //一応飽和計算しておく（したら不味いけど） //暂时计算饱和度（不好吃）
				}
				fwrite(&v, sizeof(short), 1, f1);
				if (max < v)
				{
					max = v;
				}
				if (min > v)
				{
					min = v;
				}
			}
		}
		fclose(f1);
	}
	printf("max = %d, min = %d\n", max, min);
	// printf(" %d [msec]\n", timeGetTime() - elapsedTime);
}
double getFreqAvg(double f0[], int tLen)
{
	int i, j;
	double value = 0, r;
	double p[6], q;
	double freq_avg = 0;
	double base_value = 0;
	for (i = 0; i < tLen; i++)
	{
		value = f0[i];
		if (value < 1000.0 && value > 55.0)
		{
			r = 1.0;
			//連続して近い値の場合のウエイトを重くする
			//当值连续关闭时称重
			for (j = 0; j <= 5; j++)
			{
				if (i > j)
				{
					q = f0[i - j - 1] - value;
					p[j] = value / (value + q * q);
				}
				else
				{
					p[j] = 1 / (1 + value);
				}
				r *= p[j];
			}
			freq_avg += value * r;
			base_value += r;
		}
	}
	if (base_value > 0)
	{
		freq_avg /= base_value;
	}
	return freq_avg;
}
void freeSpecgram(double **spec, int n)
{
	int i;
	if (spec && n)
	{
		for (i = 0; i < n; i++)
		{
			free(spec[i]);
		}
		free(spec);
	}
}

//频谱拉伸
// スペクトル伸縮
void stretchSpectrum(double **specgram, int oLen, double ratio, int fs, int fftl)
{
	int i, j;
	double w = 0;
	//ratio = atof(argv[4]);
	if (ratio != 1.0)
	{
		double *freqAxis1, *freqAxis2;
		double *spec1, *spec2;
		freqAxis1 = (double *)malloc(sizeof(double) * fftl);
		freqAxis2 = (double *)malloc(sizeof(double) * fftl);
		spec1 = (double *)malloc(sizeof(double) * fftl);
		spec2 = (double *)malloc(sizeof(double) * fftl);

		// 周波数伸縮の前処理
		//频率扩展/收缩的预处理
		for (i = 0; i <= fftl / 2; i++)
		{
			freqAxis1[i] = (double)i / (double)fftl * (double)fs * ratio;
			freqAxis2[i] = (double)i / (double)fftl * (double)fs;
		}
		for (i = 0; i < oLen; i++)
		{
			for (j = 0; j <= fftl / 2; j++)
				spec1[j] = log(specgram[i][j]);
			interp1(freqAxis1, spec1, fftl / 2 + 1, freqAxis2, fftl / 2 + 1, spec2);
			for (j = 0; j <= fftl / 2; j++)
				specgram[i][j] = exp(spec2[j]);
			if (ratio < 1.0)
			{
				for (j = (int)((double)fftl / 2 * ratio); j <= fftl / 2; j++)
				{
					specgram[i][j] = specgram[i][(int)((double)fftl / 2 * ratio) - 1];
				}
			}
		}

		free(spec1);
		free(spec2);
		free(freqAxis1);
		free(freqAxis2);
	}
}
void makeHeader(char *header, int samples, int fs, int nbit)
{
	memcpy(header, "RIFF", 4);
	*(long *)(header + 4) = samples * 2 + 36;
	memcpy(header + 8, "WAVE", 4);
	memcpy(header + 12, "fmt ", 4);
	*(long *)(header + 16) = 16;

	*(short *)(header + 20) = 1;
	*(unsigned short *)(header + 22) = 1;
	*(unsigned long *)(header + 24) = fs;
	*(unsigned long *)(header + 28) = fs * nbit / 8;
	*(unsigned short *)(header + 32) = nbit / 8;
	*(unsigned short *)(header + 34) = nbit;
	memcpy(header + 36, "data", 4);
	*(long *)(header + 40) = samples * 2;
}

int main(int argc, char *argv[])
{
	// メモリリーク検出
	//内存泄漏检测
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	printf("./world4utau");
	int ii = 1;
	while (ii < argc)
	{
		printf(" '%s'", argv[ii]);
		ii++;
	}
	printf("\n");

	int i, j;

	if (argc <= 4)
	{
		fprintf(stderr, "error: missing params．\n");
		return 0;
	}

	FILE *fp;
	int fs, nbit = 16;
	int flag_G = 0;
	if (argc > 5)
	{
		flag_G = strchr(argv[5], 'G') != 0;
	}

	double *x = 0, *f0 = NULL, *t = NULL, *y = NULL;
	double **specgram = NULL;
	double **residualSpecgram = NULL;
	int fftl = 0;

	int signalLen = 0;
	int tLen = 0;

	tLen = readDIOParam(argv[1], &t, &f0, &fs, &signalLen);
	if (tLen != 0)
	{
		fftl = getFFTLengthForStar(fs);
		specgram = readSTARParam(signalLen, fs, argv[1], tLen, fftl);
		if (specgram)
		{
			residualSpecgram = readPlatinumParam(signalLen, fs, argv[1], tLen, fftl);
			if (!residualSpecgram)
			{
				tLen = 0;
			}
		}
		else
		{
			tLen = 0;
		}
	}
	if (tLen == 0)
	{
		freeSpecgram(specgram, tLen);
		freeSpecgram(residualSpecgram, tLen);

		x = wavread(argv[1], &fs, &nbit, &signalLen);
		if (x == NULL)
		{
			fprintf(stderr, "error: 指定されたファイルは存在しません．\n"); // 指定的文件不存在
			return 0;
		}
		tLen = getDIOParam(x, signalLen, fs, FRAMEPERIOD, &t, &f0);
		if (tLen != 0)
		{
			writeDIOParam(signalLen, fs, tLen, argv[1], t, f0);
		}
		else
		{
			SAFE_FREE(x);
			fprintf(stderr, "error: DIO initialization failed．\n"); // DIO参数创建失败
			return 0;
		}
		fftl = getFFTLengthForStar(fs);
		specgram = getSTARParam(x, signalLen, fs, t, f0, tLen, fftl);
		if (!specgram)
		{
			SAFE_FREE(x);
			SAFE_FREE(t);
			SAFE_FREE(f0);
			fprintf(stderr, "error: STAR initialization failed．\n"); // STAR参数创建失败。
			return 0;
		}
		else
		{
			writeSTARParam(signalLen, fs, argv[1], specgram, tLen, fftl);
		}
		residualSpecgram = getPlatinumParam(x, signalLen, fs, t, f0, specgram, tLen, fftl);
		if (!residualSpecgram)
		{
			SAFE_FREE(x);
			SAFE_FREE(t);
			SAFE_FREE(f0);
			SAFE_FREE(specgram);
			fprintf(stderr, "error: Platinum initialization failed．\n"); // STAR参数创建失败
			return 0;
		}
		else
		{
			writePlatinumParam(signalLen, fs, argv[1], residualSpecgram, tLen, fftl);
		}
	}

	// 注意！
	// 全ての分析が完了するまで，F0は操作しないこと．
	//在所有分析完成之前，请勿操作F0。

	//引数を取り込む
	//接受参数
	double offset = 0;
	double wavelength = (double)signalLen / (double)fs * 1000; //音源の長さをmsecにする  将声源长度设置为毫秒
	double length_req = wavelength;							   //初期値いれとく  初始值
	double fixed = 0;
	double blank = 0;
	double *target_freqs = NULL;
	double velocity = 1.0;
	double value = 100;
	if (argc > 4)
	{
		sscanf(argv[4], "%lf", &value);
	}
	velocity = pow(2, value / 100 - 1.0);

	if (argc > 6)
	{
		sscanf(argv[6], "%lf", &offset);
	}

	if (argc > 7)
	{
		sscanf(argv[7], "%lf", &length_req);
	}

	if (argc > 8)
	{
		sscanf(argv[8], "%lf", &fixed);
	}

	if (argc > 9)
	{
		sscanf(argv[9], "%lf", &blank);
	}

#ifdef _DEBBUG
	printf("Parameters\n");
	printf("velocity      :%lf\n", velocity);
	printf("offset        :%lf\n", offset);
	printf("request length:%lf\n", length_req);
	printf("fixed         :%lf\n", fixed);
	printf("blank         :%lf\n", blank);
#endif
	//伸縮の概念図
	//  offset    fixed      m2      blank
	//|--------|--------|---------|---------| 原音
	//         |        |          |
	//         |   l1   |    l2     |
	//         |--------|------------|  出力
	// l1  = fixed / velocity
	// l2  = m2    / stretch
	// l1 + l2  = 要求長＝argv[7]
	if (blank < 0)
	{
		blank = wavelength - offset + blank;
		if (blank < 0)
			blank = 0;
	}
	if (offset + blank >= wavelength)
	{
		fprintf(stderr, "error: パラメータ異常．\n"); // 参数错误
		free(x);
		return 0;
	}
	if (offset + blank + fixed >= wavelength)
	{
		fixed = wavelength - offset + blank;
	}
	double l1, l2;
	double m2 = wavelength - offset - fixed - blank;

	l1 = fixed / velocity;
	l2 = length_req - l1;
	if (m2 <= 0 && l2 > 0)
	{
		fprintf(stderr, "error: パラメータ異常2．\n"); // 参数错误2
		free(x);
		return 0;
	}

	double stretch = m2 / l2;
	if (stretch > 1.0)
	{
		stretch = 1.0;
	}

	int outSamples = (int)(length_req * 0.001 * fs + 1);
	int oLen = getSamplesForDIO(fs, outSamples, FRAMEPERIOD);

	printf("File information\n");
	printf("Sampling : %d Hz %d Bit\n", fs, nbit);
	printf("Input:\n");
	printf("Length %d [sample]\n", signalLen);
	printf("Length %f [sec]\n", (double)signalLen / (double)fs);
	printf("Output:\n");
	printf("Length %d [sample]\n", outSamples);
	printf("Length %f [sec]\n", (double)outSamples / (double)fs);

	// FIXME: what does this mean?? [ruix]
	int flag_t = 0;
	char *cp;
	if (argc > 5 && (cp = strchr(argv[5], 't')) != 0)
	{
		sscanf(cp + 1, "%d", &flag_t);
	}

	double modulation = 100;
	if (argc > 11)
	{
		sscanf(argv[11], "%lf", &modulation);
	}

	double volume = 1.0;
	if (argc > 10)
	{
		volume = 100;
		sscanf(argv[10], "%lf", &volume);
		volume *= 0.01;
	}

	double target_freq = name2freq(argv[3], flag_t);
	double freq_avg = getFreqAvg(f0, tLen);

#ifdef _DEBUG
	printf("volume        :%lf\n", volume);
	printf("modulation    :%lf\n", modulation);
	printf("target frequency     :%lf\n", target_freq);
	printf("input frequency(avg.):%lf\n", freq_avg);
#endif

	double *f0out = (double *)malloc(oLen * sizeof(double));
	memset(f0out, 0, sizeof(double) * oLen);
	//double *tout = (double*)malloc(oLen * sizeof(double));
	int *pit = NULL;
	double tempo = 120;
	int pLen = oLen;
	int pStep = 256;
	if (argc > 13)
	{
		cp = argv[12];
		sscanf(cp + 1, "%lf", &tempo);
		pStep = (int)(60.0 / 96.0 / tempo * fs + 0.5);
		pLen = outSamples / pStep + 1;
		pit = (int *)malloc((pLen + 1) * sizeof(int));
		memset(pit, 0, (pLen + 1) * sizeof(int));
		decpit(argv[13], pit, pLen);
	}
	else
	{
		pit = (int *)malloc((pLen + 1) * sizeof(int));
		memset(pit, 0, (pLen + 1) * sizeof(int));
	}

	double **specgram_out = (double **)malloc(sizeof(double *) * oLen);
	double **residualSpecgram_out = (double **)malloc(sizeof(double *) * oLen);
	for (i = 0; i < oLen; i++)
	{
		specgram_out[i] = (double *)malloc(sizeof(double) * (fftl / 2 + 1));
		memset(specgram_out[i], 0, sizeof(double) * (fftl / 2 + 1));
		residualSpecgram_out[i] = (double *)malloc(sizeof(double) * (fftl + 1));
		memset(residualSpecgram_out[i], 0, sizeof(double) * (fftl + 1));
	}
	//出力f0数列
	double tmo, tmi;
	// DWORD elapsedTime = timeGetTime();
	printf("\nTransform\n");
#ifdef _DEBUG
	FILE *fp0 = fopen("time.txt", "wt");
	FILE *fp1 = fopen("dio.txt", "wt");
	FILE *fp2 = fopen("star.txt", "wt");
	FILE *fp3 = fopen("plat.txt", "wt");
#endif
	double v, u;
	int n, m;
	for (i = 0; i < oLen; i++)
	{
		tmo = FRAMEPERIOD * i;
		if (tmo < l1)
		{
			tmi = offset + tmo * velocity;
		}
		else
		{
			tmi = offset + fixed + (tmo - l1) * stretch;
		}
#ifdef _DEBUG
		fprintf(fp0, "%0.6lf\t%0.6lf\n", tmi, tmo);
#endif
		v = tmi / FRAMEPERIOD;
		n = (int)floor(v);
		v -= n;

		double f0i = f0[n];
		if (n < tLen - 1)
		{
			double f0j = f0[n + 1];
			if (f0i != 0 || f0j != 0)
			{
				if (f0i == 0)
					f0i = freq_avg;
				if (f0j == 0)
					f0j = freq_avg;
				f0i = f0i * (1.0 - v) + f0j * v;
			}
		}

		u = tmo * 0.001 * fs / pStep;
		m = (int)floor(u);
		u -= m;
		if (m >= pLen)
		{
			m = pLen - 1;
			v = 0.0;
		}
		f0out[i] = target_freq * pow(2, (pit[m] * (1.0 - u) + pit[m + 1] * u) / 1200.0);
		f0out[i] *= pow(f0i / freq_avg, modulation * 0.01);
#ifdef _DEBUG
		fprintf(fp1, "%lf\n", f0out[i]);
#endif
		for (j = 0; j <= fftl / 2; j++)
		{
			if (n < tLen - 1)
			{
				specgram_out[i][j] = specgram[n][j] * (1.0 - v) + specgram[n + 1][j] * v;
			}
			else
			{
				specgram_out[i][j] = specgram[tLen - 1][j];
			}
#ifdef _DEBUG
			fprintf(fp2, "%lf\t", specgram_out[i][j] * 1000000);
#endif
			/*if (_isnan(specgram_out[i][j]))
			{
				printf("nan!\n");
			}
			else if (specgram_out[i][j] == 0)
			{
				printf("(%d)(%d)zero!\n", i, j);
			}*/
		}
#ifdef _DEBUG
		fprintf(fp2, "\n");
#endif
		int m = n;
		if (v > 0.5)
			m++;
		for (j = 0; j <= fftl; j++)
		{
			if (m < tLen) //(n < tLen - 1)
			{
				//residualSpecgram_out[i][j] = residualSpecgram[n][j] * (1.0 - v) + residualSpecgram[n + 1][j] * v;
				residualSpecgram_out[i][j] = residualSpecgram[m][j];
			}
			else
			{
				residualSpecgram_out[i][j] = residualSpecgram[tLen - 1][j];
			}
			/*if (_isnan(residualSpecgram_out[i][j]))
			{
				printf("nan!\n");
			}
			else if (residualSpecgram_out[i][j] == 0)
			{
				printf("(%d)(%d)zero!\n", i, j);
			}*/
		}
#ifdef _DEBUG
		for (j = 0; j < fftl; j += 8)
		{
			fprintf(fp3, "%lf\t", residualSpecgram_out[i][j]);
		}
		fprintf(fp3, "\n");
		for (j = 0; j < fftl; j += 8)
		{
			fprintf(fp3, "%lf\t", residualSpecgram_out[i][j + 1]);
		}
		fprintf(fp3, "\n");
#endif
	}

#ifdef _DEBUG
	fclose(fp0);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
#endif
	// スペクトル伸縮
	// 光谱拉伸
	//stretchSpectrum(double **specgram, double ratio)
	if (argc > 5 && (cp = strchr(argv[5], 'g')) != 0)
	{
		double w = 0;
		double ratio = 1.0;
		sscanf(cp + 1, "%lf", &w);
		if (w > 100)
			w = 100;
		if (w < -100)
			w = -100;
		ratio = pow(10, -w / 200);
		stretchSpectrum(specgram_out, oLen, ratio, fs, fftl);
	}
	// printf("TRANSFORM: %d [msec]\n", timeGetTime() - elapsedTime);

	// 合成
	y = (double *)malloc(sizeof(double) * outSamples);
	memset(y, 0, sizeof(double) * outSamples);

	printf("\nSynthesis\n");
	// elapsedTime = timeGetTime();
	//synthesis(f0out, oLen, specgram_out, residualSpecgram_out, fftl, FRAMEPERIOD, fs, y, outSamples);
	synthesis(f0out, oLen, specgram_out, residualSpecgram_out, fftl, FRAMEPERIOD, fs, y, outSamples);
	// printf("WORLD: %d [msec]\n", timeGetTime() - elapsedTime);

	// ファイルの書き出し (内容には関係ないよ)
	// 文件导出（与内容无关）
	char header[44];
	short *output;
	double maxAmp;

	output = (short *)malloc(sizeof(short) * outSamples);
	// 振幅の正規化
	maxAmp = 0.0;
#ifdef _DEBUG
	{
		FILE *f = fopen("synthesis.txt", "wt");
		for (i = 0; i < outSamples; i++)
		{
			fprintf(f, "%f\n", y[i]);
		}
		fclose(f);
	}
#endif
	for (i = 0; i < outSamples; i++)
	{
		// if (!_isnan(y[i]))
		if (!isnan(y[i]))
		{
			maxAmp = maxAmp < fabs(y[i]) ? fabs(y[i]) : maxAmp;
		}
	}
	value = 0.86;
	if (argc > 5)
	{
		cp = strchr(argv[5], 'P');
		if (cp)
		{
			sscanf(cp + 1, "%lf", &value);
			if (value < 0)
				value = 0;
			else if (value > 100)
				value = 100;
			value *= 0.01;
		}
	}
	double peekcomp = 32.0 * pow(512.0 / maxAmp, value);
	//double peekcomp = pow( 16384.0 / maxAmp, value);
	for (i = 0; i < outSamples; i++)
	{
		//****** 空白部分でsynthesis()がnanを吐くので対策した*********
		// 由于Synthesis()在空白部分吐出了nan，因此我采取了措施。
		// value = _isnan(y[i]) ? 0 : y[i] * peekcomp * volume;
		value = isnan(y[i]) ? 0 : y[i] * peekcomp * volume;
		if (value > 32767.0)
			value = 32767.0;
		else if (value < -32767.0)
			value = -32767.0;
		output[i] = (short)value;
	}

	fp = fopen(argv[1], "rb");
	makeHeader(header, outSamples, fs, nbit);

	fp = fopen(argv[2], "wb");
	fwrite(header, sizeof(char), 44, fp);
	fwrite(output, sizeof(short), outSamples, fp);
	fclose(fp);

	printf("complete.\n");

	// メモリの解放
	free(output);
	free(x);
	free(t);
	free(f0);
	free(y);
	for (i = 0; i < tLen; i++)
	{
		free(specgram[i]);
		free(residualSpecgram[i]);
	}
	free(specgram);
	free(residualSpecgram);

	free(f0out);
	for (i = 0; i < oLen; i++)
	{
		free(specgram_out[i]);
		free(residualSpecgram_out[i]);
	}
	free(specgram_out);
	free(residualSpecgram_out);
	free(pit);

	return 0;
}