#if defined(__WIN32__) || defined(_WIN32) || defined(_WINDOWS)
#include <windows.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavread.h"
#include "common.h"

#pragma warning(disable : 4996)

/* wavread関数の移植 */
double *wavread(char *filename, int *fs, int *Nbit, int *waveLength)
{
	FILE *fp;
	char dataCheck[5]; // 少し多めに
	unsigned char forIntNumber[4];
	double tmp, signBias, zeroLine;
	int quantizationByte;
	double *waveForm;
	int i;
	dataCheck[4] = '\0'; // 文字列照合のため，最後に終了文字を入れる．
						 //	fp = fopen(filename, "rb");
	fp = fopen(filename, "rb");
	if (NULL == fp)
	{
		printf("ファイルのロードに失敗\n");
		return NULL;
	}
	//ヘッダのチェック
	fread(dataCheck, sizeof(char), 4, fp); // "RIFF"
	if (0 != strcmp(dataCheck, "RIFF"))
	{
		fclose(fp);
		printf("ヘッダRIFFが不正\n");
		return NULL;
	}
	fseek(fp, 4, SEEK_CUR);				   // 4バイト飛ばす
	fread(dataCheck, sizeof(char), 4, fp); // "WAVE"
	if (0 != strcmp(dataCheck, "WAVE"))
	{
		fclose(fp);
		printf("ヘッダWAVEが不正\n");
		return NULL;
	}
	fread(dataCheck, sizeof(char), 4, fp); // "fmt "
	if (0 != strcmp(dataCheck, "fmt "))
	{
		fclose(fp);
		printf("ヘッダfmt が不正\n");
		return NULL;
	}
	fread(dataCheck, sizeof(char), 4, fp); //1 0 0 0
	if (!(16 == dataCheck[0] && 0 == dataCheck[1] && 0 == dataCheck[2] && 0 == dataCheck[3]))
	{
		fclose(fp);
		printf("ヘッダfmt (2)が不正\n");
		return NULL;
	}
	fread(dataCheck, sizeof(char), 2, fp); //1 0
	if (!(1 == dataCheck[0] && 0 == dataCheck[1]))
	{
		fclose(fp);
		printf("フォーマットIDが不正\n");
		return NULL;
	}
	fread(dataCheck, sizeof(char), 2, fp); //1 0
	if (!(1 == dataCheck[0] && 0 == dataCheck[1]))
	{
		fclose(fp);
		printf("ステレオには対応していません\n");
		return NULL;
	}

	// サンプリング周波数
	fread(forIntNumber, sizeof(char), 4, fp);
	*fs = 0;
	for (i = 3; i >= 0; i--)
	{
		*fs = *fs * 256 + forIntNumber[i];
	}
	// 量子化ビット数
	fseek(fp, 6, SEEK_CUR); // 6バイト飛ばす
	fread(forIntNumber, sizeof(char), 2, fp);
	*Nbit = forIntNumber[0];
	// ヘッダ
	fread(dataCheck, sizeof(char), 4, fp); // "data"
	if (0 != strcmp(dataCheck, "data"))
	{
		fclose(fp);
		printf("ヘッダdataが不正\n");
		return NULL;
	}
	// サンプル点の数
	fread(forIntNumber, sizeof(char), 4, fp); // "data"
	*waveLength = 0;
	for (i = 3; i >= 0; i--)
	{
		*waveLength = *waveLength * 256 + forIntNumber[i];
	}
	*waveLength /= (*Nbit / 8);

	// 波形を取り出す
	waveForm = (double *)malloc(sizeof(double) * *waveLength);
	if (waveForm == NULL)
		return NULL;

	quantizationByte = *Nbit / 8;
	zeroLine = pow(2.0, *Nbit - 1);
	for (i = 0; i < *waveLength; i++)
	{
		signBias = 0.0;
		tmp = 0.0;
		fread(forIntNumber, sizeof(char), quantizationByte, fp); // "data"
		// 符号の確認
		if (forIntNumber[quantizationByte - 1] >= 128)
		{
			signBias = pow(2.0, *Nbit - 1);
			forIntNumber[quantizationByte - 1] = forIntNumber[quantizationByte - 1] & 0x7F;
		}
		// データの読み込み
		for (int j = quantizationByte - 1; j >= 0; j--)
		{
			tmp = tmp * 256.0 + (double)(forIntNumber[j]);
		}
		waveForm[i] = (double)((tmp - signBias) / zeroLine);
	}
	// 成功
	fclose(fp);
	return waveForm;
}
