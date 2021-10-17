#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <math.h>

#define CLOCKS_PER_SEC ((clock_t)1000)
#define TIME_UTC 1
/*1*/
void InverseImage(BYTE* Img, BYTE *Out, int W, int H) // inverse image �̹��� ��� ���� 255-var
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val) // ��� ��ȭ
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else 	Out[i] = Img[i] + Val;
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val) // ����
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) // ������׷� ���� x = 0~255
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void ObtainAHistogram(int* Histo, int* AHisto) // ������ ������׷� ������׷����� H[i-1] + H[i]�� �ϸ� ���� for
{
	/*
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}*/
	AHisto[0] = Histo[0];
	for (int i = 1; i < 256; i++) {
		AHisto[i] = AHisto[i-1] + Histo[i];
	}
}

void HistogramStretching(BYTE * Img, BYTE * Out, int * Histo, int W, int H) // stretching -- min-max scaling ������ �ø� �̻�ġ�� �ΰ�
{
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);
	}
}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H) // ��Ȱȭ ��⸦ ���������� ǥ�� -> ������
{
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];
	}
}
/* 2 */
void Binarization(BYTE * Img, BYTE * Out, int W, int H, BYTE Threshold) // threshold ���� ����ȭ
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

BYTE GozalezBinThresh(BYTE * Img, int W, int H, int* Histo) // ��谪 ����
{
	/*
	psuedo code
	T ���� G1, G2 ����
	G1, G2 ��� ��� mu1, mu2 ���
	mu1, mu2 ����� next T
	abs(T-next T)�� epsilon���� ������ break
	*/
	int ImgSize = W * H;

	/* First Threshold */
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	BYTE T = (Low + High) / 2;


	BYTE NextT;
	int G1, G2, Sum;
	double Mu1, Mu2;
	double epsilon = 3;
	int dif;
	while (true) {
		G1 = 0, G2 = 0, Sum = 0;
		for (int i = 0; i < ImgSize; i++) {
			if (Img[i] < T) {
				G1 += Img[i];
				Sum += 1;
			}
			else {
				G2 += Img[i];
			}
		}
		Mu1 = G1 / Sum;
		Mu2 = G2 / (ImgSize - Sum);
		NextT = (BYTE)((Mu1 + Mu2) / 2);
		dif = (int)(NextT)-(int)(T);
		if ((epsilon > dif) && (-epsilon < dif)) {
			return (NextT);
		}
		T = NextT;
	}


}
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // �ڽ���Ȱȭ avg filter-> blur ȿ�� �ִ�., gaussian noise�� ����
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

/* 3 */
void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // ����þ���Ȱȭ low-pass filter avg filter�� ���� ����� smoothing��,gaussian noise�� ����
{
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
										0.125, 0.25, 0.125,
										0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ Y
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 1.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Sobel ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-2.0, 0.0, 2.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Laplace ��谪 ���� high-pass filter
{													  // but, sigma(filter) == 0 �̱� ���� ��Ⱑ ��������.
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 8.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255 noise
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{														 // Laplace filter sigma(filter) == 1 ��Ⱚ ����
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 9.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//Out[i * W + j] = abs((long)SumProduct) / 8;
			/* Clip */
			if (SumProduct > 255.0) Out[i * W + j] = 255;
			else if (SumProduct < 0.0) Out[i * W + j] = 0;
			else Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

/* 4 */
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE * fp = fopen(FileName, "wb");
	if (hInfo.biBitCount == 24) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W*H*3, fp);
	}
	else if (hInfo.biBitCount == 8) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), W*H, fp);
	}
	fclose(fp);
}
void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}
BYTE Median(BYTE* arr, int size) // pivot sort, get median, impulse ����� ���ϴ�.impulse noise�� ��? ��հ�����ϸ�, ��տ� �̻�ġ ����
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}
BYTE MaxPooling(BYTE* arr, int size) // pivot sort, get Max
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];
}
BYTE MinPooling(BYTE* arr, int size) // pivot sort, get Min
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}

void median_time(BITMAPINFOHEADER hInfo, BYTE* Image, BYTE* Output, int Length) // 6���� ����
{
	clock_t start, end;
	double result;
	//int Length = 3;  // ����ũ�� �� ���� ����
	int Margin = Length / 2; // filtering �� �߸� margin�� ũ��
	int WSize = Length * Length; // filtering �� width ũ��
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // filter ���� �̹��� ������ �����ϴ� �ӽ� �迭
	int W = hInfo.biWidth, H = hInfo.biHeight; // ���� �̹����� width, height
	int i, j, m, n; // �ݺ��� ����� ���� ���� ����
	start = clock(); // ���� �ð� ������ ���� ���� �ð�
	for (i = Margin; i < H - Margin; i++) { // �̹��� �迭 ������ ���� �ݺ��� ���� filtering �ϰ� �Ǹ� �̹��� ����� �پ��, 
											// height - margin ~ height + margin , Y
		for (j = Margin; j < W - Margin; j++) { // �ݺ��� ����, width - margin ~ width + margin, X
			for (m = -Margin; m <= Margin; m++) { // filter ����� ���� �ݺ��� ���� 
				for (n = -Margin; n <= Margin; n++) { // filter ����� ���� �ݺ��� ����
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m)*W + j + n]; // filter���� �̹��� ������ temp�� ����
				}
			}
			Output[i * W + j] = Median(temp, WSize); // temp�� buuble sort �Ͽ� �߰� ���� output�� ����
		}
	}
	end = clock(); // ���� �ð� ������ ���� ���� �ð�
	result = (double)(end - start); // �� ���� �ð�
	printf("Execution Time is %f", result); // ���� �ð� ��� (ms����)
}

/* 6 */
int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top) // stack ����� ���� push
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top) // stack ����� ���� pop
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}


// GlassFire �˰����� �̿��� �󺧸� �Լ�
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// �������� ����� �޸� �Ҵ�
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// �󺧸��� �ȼ��� �����ϱ� ���� �޸� �Ҵ�

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // �޸� �ʱ�ȭ

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// �̹� �湮�� ���̰ų� �ȼ����� 255�� �ƴ϶�� ó�� ����
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//���� �ȼ��� �����踦 ����� ó�� ����
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // ���� �󺧷� ��ũ
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor;

	// ���� ������ ���� ������ ã�Ƴ��� ���� 
	for (i = 1; i <= curColor; i++)
	{
		// BlobArea ��������
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage �迭 Ŭ����~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring�� ����� �󺧸� ����� (Out_Area�� �����) ������ ���� ū �͸� CutImage�� ����
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // ���� ū �͸� ����(size flitering)
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);
		//if (BlobArea[coloring[k]] > 500) CutImage = 0; // Ư�� ���� �̻�Ǵ� ������ ���
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}

void BinaryImageEdgeDetection(BYTE*Bin,BYTE*Out,int W,int H) // �󺧸� edge ����, 4�������α��� 8���� ����ϸ� edge �β�������.
{
	int ImgSize = W * H;
	

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Bin[i*W + j] == 0)//����ȭ�Ҷ��
			{
				if (!(Bin[(i - 1)*W + j] == 0 &&
					Bin[(i + 1)*W + j] == 0 &&
					Bin[i*W + j - 1] == 0 &&
					Bin[i*W + j + 1]==0)) // 4���� ȭ���� ������ �ƴϸ�
				{
					Out[i*W + j] = 255;
				}
			}
		}
	}
}
void DrawRectOutline(BYTE * Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {

	for (int i = RD_Y; i < LU_Y; i++) //������
	{
		Img[i*W + RD_X] = 255;
		Img[i*W + LU_X] = 255;
	}
	for (int i = LU_X; i < RD_X; i++) // ������
	{
		Img[RD_Y*W + i] = 255;
		Img[LU_Y*W + i] = 255;
	}
}

void DrawCrossLine(BYTE * Img, int W, int H, int Cx, int Cy) {
	for (int i = 0; i < H; i++)
	{
		Img[i*W + Cx] = 255;
	}
	for (int i = 0; i < W; i++)
	{
		Img[Cy*W + i] = 255;
	}
}

void DrawRectCrossLine(BYTE* Img,BYTE* Out ,int W,int H,int Cx, int Cy) {
	/* get round */
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	bool chk = false;
	for (int i = H - 1; i > 0; i--) { // get y1
		for (int j = 0; j < W; j++) {
			if (Img[i*W + j] == 255) y1 = i;
			if (y1 != 0) {
				chk = true;
				break;
			}
		}
		if (chk) break;
	}
	chk = false;
	for (int i = 0; i < W; i++) { // get x1
		for (int j = 0; j < H; j++) {
			if (Img[j*W + i] == 255) x1 = i;
			if (x1 != 0) { 
				chk = true;
				break; 
			}
		}
		if (chk) break;
	}
	chk = false;
	for (int i = 0; i < H; i++) { // get y2
		for (int j = 0; j < W; j++) {
			if (Img[i*W + j] == 255) y2 = i;
			if (y2 != 0) {
				chk = true;
				break;
			}
		}
		if (chk) break;
	}
	chk = false;
	for (int i = W - 1; i > 0; i--) { // get x2
		for (int j = 0; j < H; j++) {
			if (Img[j*W + i] == 255) x2 = i;
			if (x2 != 0) {
				chk = true;
				break;
			}
		}
		if (chk) break;
	}
	printf("(%d, %d), (%d, %d)",x1,y1,x2,y2);
	DrawRectOutline(Out, W, H, x1, y1, x2, y2);
	
	DrawCrossLine(Out, W, H, Cx, Cy);

}
/* ���� ���� ���� ����
Binarization(Image, Temp, W, H, 50);
InverseImage(Temp, Temp, W, H);
m_BlobColoring(Temp, H, W);
for (int i = 0; i < ImgSize; i++) Output[i] = 0;
BinaryImageEdgeDetection(Temp, Output, W, H);

int sumx = 0, sumy = 0, cnt = 0;
for (int i = 0; i < H; i++)
{
	for (int j = 0; j < W; j++)
	{
		if (Temp[i*W + j] == 0) {
			sumx += j;
			sumy += i;
			cnt++;
		}
	}
}
int Cx = sumx / cnt;
int Cy = sumy / cnt;
DrawRectCrossLine(Output, Image, W, H, Cx, Cy);
*/
/* 7���� */
// ������ ������ ����� ���ؼ� ��������
void VerticalFlip(BYTE* Img, int W, int H) // ���Ʒ� swap�Ͽ� vertical flip
{
	for (int i = 0; i < H/2; i++){
		for (int j = 0; j < W; j++){
			swap(&Img[i*W+j], &Img[(H-1-i)*W+j]);
		}
	}
}

void HorizontalFlip(BYTE* Img, int W, int H)
{
	for (int i = 0; i < W / 2; i++) { // x
		for (int j = 0; j < H; j++) { // y
			swap(&Img[j*W + i], &Img[j*W + (W-1-i)]);
		}
	}
}

void Translation(BYTE* Image, BYTE* Output, int W, int H, int Tx, int Ty) 
{
	Ty *= -1;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Tx >= 0)) {
				Output[(i + Ty)*W + j + Tx] = Image[i*W + j]; // ������ ���
			}
		}
	}
}
// excel���� mmult ��İ�
void Scalig(BYTE* Image, BYTE* Output, int W, int H, double SF_X, double SF_Y) // ������ mapping �Ͽ��� ������ ¦����°�鸸 ���ù޴´�.
{
	int tmpx, tmpy;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpx = (int)(j / SF_X); // ������ ����� ���� tmpx, tmpy ���⼭ ��ȯ����� ������� ������.
			tmpy = (int)(i / SF_Y);
			if (tmpy < H && tmpx < W) {
				Output[i*W + j] = Image[tmpy*W + tmpx];
			}
		}
	}
}

// Roatation ladian ����� ����
void Rotation_zero(BYTE* Image, BYTE* Output, int W, int H, int Angle) 
{
	int tmpx, tmpy;\
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpx = (int)(cos(-Radian)*j - sin(-Radian)*i);
			tmpy = (int)(sin(-Radian)*j + cos(-Radian)*i);
			if ((tmpy < H &&tmpy >= 0) && (tmpx < W && tmpx >= 0)) {
				Output[i*W + j] = Image[tmpy*W + tmpx];
			}
		}
	}
}

void Rotation_center(BYTE* Image, BYTE* Output, int W, int H, int Angle) // ȸ�� ��, �� ��ǥ �����̵�, ���� ȸ����ȯ��� ���Ѵ�. �̸� ����ķ� ���.
{
	int tmpx, tmpy;
	int cenX = W / 2, cenY = H / 2;
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpx = (int)(cos(-Radian)*(j-cenX) - sin(-Radian)*(i-cenY) + cenX);
			tmpy = (int)(sin(-Radian)*(j-cenX) + cos(-Radian)*(i-cenY) + cenY);
			if ((tmpy < H &&tmpy >= 0) && (tmpx < W && tmpx >= 0)) {
				Output[i*W + j] = Image[tmpy*W + tmpx];
			}
		}
	}
}
// Shearing
void Shearing() {}

// Cartesian 3x3 ���(3�� ���� ���)�� ǥ��, (x,y,1)�� �� ����

/* 10���� */

void FillColor(BYTE*Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
	Image[(Y * W + X) * 3] = B; // B
	Image[(Y * W + X) * 3 + 1] = G; // G
	Image[(Y * W + X) * 3 + 2] = R; // R

}

void init(BYTE*Image, int W, int H, int N) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			FillColor(Image, j, i, W, H, N, N, N);
		}
	}
}
// 10���� ���� : 3�� �׶��̼� ��� ���α׷�
void GetGradation(BYTE*Image, int W, int H) {
	double wt;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			if (i < H / 3) {
				FillColor(Image, j, i, W, H, (BYTE)(255 * (wt)), (BYTE)(255 * (wt)), (BYTE)(255 * (1.0 - wt)));
			}
			else if (i < H / 3 * 2) {
				FillColor(Image, j, i, W, H, (BYTE)(255 * (wt)), (BYTE)(255 * (1.0 - wt)), (BYTE)(255 * (wt)));
			}
			else {
				FillColor(Image, j, i, W, H, (BYTE)(255 * (1.0 - wt)), (BYTE)(255 * (wt)), (BYTE)(255 * (wt)));
			}
		}
	}
}

// 11����
/*
rgb color model�� �� ��Ⱑ correlate�ϴ�
YCbCr ## Y = ���� 1 �� �ܴ� 0(max = 0.5) +128
�� B�� ���� ũ�� (2��������) �Ķ��� ���� ���ؼ�
y = ��������� ���� ���� 0.587�� ���� ����
Cb �󸶳� Ǫ��
Cr �󸶳� ����

*/
// 11���� ���� func
void DrawRectOutline_R(BYTE * Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {

	for (int i = RD_Y; i < LU_Y; i++) //������
	{
		Img[(i*W + RD_X)*3+2] = 255;
		Img[(i*W + LU_X)*3+2] = 255;
		Img[(i*W + RD_X) * 3 + 1] = 0;
		Img[(i*W + LU_X) * 3 + 1] = 0;
		Img[(i*W + RD_X) * 3] = 0;
		Img[(i*W + LU_X) * 3] = 0;
	}
	for (int i = LU_X; i < RD_X; i++) // ������
	{
		Img[(RD_Y*W + i)*3+2] = 255;
		Img[(LU_Y*W + i)*3+2] = 255;
		Img[(RD_Y*W + i) * 3 + 1] = 0;
		Img[(LU_Y*W + i) * 3 + 1] = 0;
		Img[(RD_Y*W + i) * 3] = 0;
		Img[(LU_Y*W + i) * 3] = 0;
	}
}
void RGB2YCbCr(BYTE* Image, BYTE* Y, BYTE* Cb, BYTE* Cr, int W, int H)
{
	for (int i = 0; i < H; i++) { // Y��ǥ
		for (int j = 0; j < W; j++) { // X��ǥ
			Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
			Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] - 0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0);
			Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
		}
	}
}
// �÷� ���� ���� ����
//RGB2YCbCr(Image, Y, Cb, Cr, W, H);
//for (int i = 0; i < H; i++)
//{
//	for (int j = 0; j < W; j++)
//	{
//		if ((80 < Cb[i*W + j] && Cb[i*W + j] < 125) &&// Cb
//			(150 < Cr[i*W + j] && Cr[i*W + j] < 190)// Cr
//			/*
//			(Image[(i * W + j) * 3 + 2] > 80) && //R
//			(Image[(i * W + j) * 3 + 1] < 50) && //G
//			(Image[(i * W + j) * 3 + 0] < 30) && //B
//			(Image[(i * W + j) * 3 + 2] > Image[(i * W + j) * 3 + 1]) && //R>G
//			(Image[(i * W + j) * 3 + 2] > Image[(i * W + j) * 3 + 0]) //R>B*/
//			)
//		{
//			Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
//			Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
//			Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
//			tmp[i*W + j] = 255;
//		}
//		else {
//			Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
//			tmp[i*W + j] = 0;
//		}
//	}
//}

// 12����
/*
��â -> ħ�� = ���� ����
ħ�� -> ��â = ���� ����

1. ���ȭ Skeletonization -> ���� �߽��� ã�� ����, ���� ��� ���������� ����
2. ����ȭ Thining -> �� �������� �������� �뷫���� ���(�β��� 1�ȼ�) �� ���� ����
	�����νĿ� ���� ����, 
	�б���, �����̶�?
	���� �б����� ������ ã�� ���� ��������
	[111]
	[111]
	[111]
	���� �Ѱ�
	�б��� 3�� �̻��� ����
	��ó�� grey�� �ڽ�ǥ��
*/
void Erosion(BYTE*Image, BYTE*Output,int W, int H)   // ħ��
{
	for (int i = 1; i < H-1; i++) { // 1= margin
		for (int j = 1; j < W-1; j++) {
			if (Image[i*W + j] == 255) // ����ȭ�Ҷ��
			{
				if (!((Image[(i - 1)*W + j] == 255) &&
					(Image[(i + 1)*W + j] == 255) &&
					(Image[i*W + j - 1] == 255) &&
					(Image[i*W + j + 1] == 255))
					) // 4�ֺ� ȭ�Ұ� ��� ����ȭ�Ұ� �ƴ϶��
					Output[i*W + j] = 0;
				else Output[i*W + j] = 255;
			}
			else Output[i*W + j] = 0;
		}
	}
}
// 2���� �迭 �����Ҵ� ����
unsigned char** imageMatrix;
// �������󿡼� 
unsigned char blankPixel = 255, imagePixel = 0;

typedef struct {
	int row, col;
}pixel;

int getBlackNeighbours(int row, int col) {

	int i, j, sum = 0;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if (i != 0 || j != 0)
				sum += (imageMatrix[row + i][col + j] == imagePixel);
		}
	}

	return sum;
}

int getBWTransitions(int row, int col) {
	return 	((imageMatrix[row - 1][col] == blankPixel && imageMatrix[row - 1][col + 1] == imagePixel)
		+ (imageMatrix[row - 1][col + 1] == blankPixel && imageMatrix[row][col + 1] == imagePixel)
		+ (imageMatrix[row][col + 1] == blankPixel && imageMatrix[row + 1][col + 1] == imagePixel)
		+ (imageMatrix[row + 1][col + 1] == blankPixel && imageMatrix[row + 1][col] == imagePixel)
		+ (imageMatrix[row + 1][col] == blankPixel && imageMatrix[row + 1][col - 1] == imagePixel)
		+ (imageMatrix[row + 1][col - 1] == blankPixel && imageMatrix[row][col - 1] == imagePixel)
		+ (imageMatrix[row][col - 1] == blankPixel && imageMatrix[row - 1][col - 1] == imagePixel)
		+ (imageMatrix[row - 1][col - 1] == blankPixel && imageMatrix[row - 1][col] == imagePixel));
}

int zhangSuenTest1(int row, int col) {
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel)
		&& (imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col - 1] == blankPixel));
}

int zhangSuenTest2(int row, int col) {
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row][col - 1] == blankPixel)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel));
}

void zhangSuen(unsigned char* image, unsigned char* output, int rows, int cols) { // ��� ����� thining

	int startRow = 1, startCol = 1, endRow, endCol, i, j, count, processed;

	pixel* markers;

	imageMatrix = (unsigned char**)malloc(rows * sizeof(unsigned char*));

	for (i = 0; i < rows; i++) {
		imageMatrix[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));
		for (int k = 0; k < cols; k++) imageMatrix[i][k] = image[i * cols + k];
	}

	endRow = rows - 2;
	endCol = cols - 2;
	do {
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest1(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest2(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		if (processed == 0)
			processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
	} while (processed == 1);


	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			output[i * cols + j] = imageMatrix[i][j];
		}
	}
}

void Dilation(BYTE*Image, BYTE*Output, int W, int H) 
{
	for (int i = 1; i < H - 1; i++) { // 1= margin
		for (int j = 1; j < W - 1; j++) {
			if (Image[i*W + j] == 0) // ����ȭ�Ҷ��
			{
				if (!((Image[(i - 1)*W + j] == 0) &&
					(Image[(i + 1)*W + j] == 0) &&
					(Image[i*W + j - 1] == 0) &&
					(Image[i*W + j + 1] == 0))
					) // 4�ֺ� ȭ�Ұ� ��� ��� ȭ�Ұ� �ƴ϶��
					Output[i*W + j] = 255;
				else Output[i*W + j] = 0;
			}
			else Output[i*W + j] = 255;
		}
	}
}
void draw(BYTE* Image, int i , int j, int W, int H) 
{
	for (int m = -1; m <= 1; m++) { // Kernel ��
		for (int n = -1; n <= 1; n++) { // Kernel ��
			if (!(m == 0 && n == 0) ){
				Image[(i+m)*W + j+n] = 128;
			}
		}
	}
	
}
void FeatureExtractThinImage(BYTE * Image, BYTE * Output, int W, int H)
{
	int stk = 0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			stk = 0;
			if (Image[i*W + j] == 0) {
				for (int m = -1; m <= 1; m++) { // Kernel ��
					for (int n = -1; n <= 1; n++) { // Kernel ��
						if (Image[(i + m)*W + j + n] == 0) {
							stk++;
						}
					}
				}
			}
			if (stk >=4 || stk == 2) {
				draw(Output, i, j, W, H);
			}
		}
	}
}

int main()  // Ʈ���÷� �̹��� ������ ��� ����
{
	BITMAPFILEHEADER hf; // 14����Ʈ
	BITMAPINFOHEADER hInfo; // 40����Ʈ
	RGBQUAD hRGB[256]; // 1024����Ʈ
	FILE* fp;
	fp = fopen("C:\\Users\\user\\Desktop\\3_1\\CV\\CV_main\\CV_main\\12\\dilation.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);

	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	int H = hInfo.biHeight, W = hInfo.biWidth;
	BYTE*Image;
	
	BYTE*Output;
	if (hInfo.biBitCount == 24) { // Ʈ�� �÷�
		Image = (BYTE *)malloc(ImgSize*3);
		Output = (BYTE*)malloc(ImgSize*3);
		fread(Image, sizeof(BYTE), ImgSize * 3, fp);
	}
	else{ // �ε���(�׷���)
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE *)malloc(ImgSize);
		Output = (BYTE*)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);
	}
	fclose(fp);

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };
	
	// YCbCr ## Y = ���� 1 �� �ܴ� 0(max = 0.5) +128  
	// Cb < 130 Cr > 200 ���� ����
	BYTE* Y = (BYTE*)malloc(ImgSize);
	BYTE* Cb = (BYTE*)malloc(ImgSize);
	BYTE* Cr = (BYTE*)malloc(ImgSize);
	BYTE*tmp = (BYTE*)malloc(ImgSize); // ������ �׸��� ���� ����ȭ �迭
	
	//	main

	Dilation(Image, Output, W, H);
	Dilation(Output, Image, W, H);
	Dilation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	InverseImage(Image, Image, W, H);

	zhangSuen(Image, tmp, W, H); // input image
	zhangSuen(Image, Output, H, W); // output image

	FeatureExtractThinImage(tmp, Output, W, H);
	
	SaveBMPFile(hf, hInfo, hRGB, tmp, hInfo.biWidth, hInfo.biHeight, "12_output_thin.bmp"); 
	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "12_output_thin_write.bmp");


	free(Image);
	free(Output);
	free(Y);
	free(Cb);
	free(Cr);
	free(tmp);
	return 0;
}