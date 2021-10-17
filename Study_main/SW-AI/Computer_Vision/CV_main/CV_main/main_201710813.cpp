#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

#define CLOCKS_PER_SEC ((clock_t)1000)
#define TIME_UTC 1
/*1*/
void InverseImage(BYTE* Img, BYTE *Out, int W, int H) // 이미지 대조
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val) // 밝기 올리기
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
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val) // 대조 값 키우기
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

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) // histogram 생성
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void ObtainAHistogram(int* Histo, int* AHisto) // 누적합 histogram
{
	/*
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}*/
	
	AHisto[0] = Histo[0];
	for (int i=1;i<256:i++){
		AHisto[i] = AHisto[i-1] + Histo[i];
	}
}

void HistogramStretching(BYTE * Img, BYTE * Out, int * Histo, int W, int H) // 스트레칭(min-max scaling)
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
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H) // 이미지 평활화
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
void Binarization(BYTE * Img, BYTE * Out, int W, int H, BYTE Threshold) // 이미지 이진화
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

BYTE GozalezBinThresh(BYTE * Img, int W, int H, int* Histo) // 이진화경계치 검출 이외에도 otsu 있음
{
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
// 합성곱 필터에서 H, W, m n 순으로 4중 for문 사용하여 접근
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // 박스평활화
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

/* 3 */
void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // 가우시안평활화
{
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
										0.125, 0.25, 0.125,
										0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}
// 엣지검출할 때는 합이 0, 그리고 최대로 나올수 있는 수를 확인
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 1.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-2.0, 0.0, 2.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // laplace 경계 검출
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 8.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255 noise
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // laplace 경계검출 합이 1
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 9.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
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
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W*H, fp);
	fclose(fp);
}
void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}
BYTE Median(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}
BYTE MaxPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];
}
BYTE MinPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}

void median_time(BITMAPINFOHEADER hInfo, BYTE* Image, BYTE* Output, int Length) // 6주차 과제
{
	clock_t start, end;
	double result;
	//int Length = 3;  // 마스크의 한 변의 길이
	int Margin = Length / 2; // filtering 후 잘릴 margin의 크기
	int WSize = Length * Length; // filtering 후 width 크기
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // filter 씌울 이미지 구간을 저장하는 임시 배열
	int W = hInfo.biWidth, H = hInfo.biHeight; // 원래 이미지의 width, height
	int i, j, m, n; // 반복문 사용을 위한 변수 선언
	start = clock(); // 실행 시간 측정을 위한 시작 시간
	for (i = Margin; i < H - Margin; i++) { // 이미지 배열 접근을 위한 반복문 선언 filtering 하게 되면 이미지 사이즈가 줄어듬, 
											// height - margin ~ height + margin , Y
		for (j = Margin; j < W - Margin; j++) { // 반복문 선언, width - margin ~ width + margin, X
			for (m = -Margin; m <= Margin; m++) { // filter 사용을 위한 반복문 선언 
				for (n = -Margin; n <= Margin; n++) { // filter 사용을 위한 반복문 선언
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m)*W + j + n]; // filter씌울 이미지 구간을 temp에 저장
				}
			}
			Output[i * W + j] = Median(temp, WSize); // temp를 buuble sort 하여 중간 값을 output에 저장
		}
	}
	end = clock(); // 실행 시간 측정을 위한 종료 시간
	result = (double)(end - start); // 총 실행 시간
	printf("Execution Time is %f", result); // 실행 시간 출력 (ms단위)
}

/* 6 */
int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}


// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width) // 시험전날 보자
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
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
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
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

	// 가장 면적이 넓은 영역을 찾아내기 위함 
	for (i = 1; i <= curColor; i++)
	{
		// BlobArea 면적정보
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장(size flitering)
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);
		//if (BlobArea[coloring[k]] > 500) CutImage = 0; // 특정 면적 이상되는 영역만 출력
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}

void BinaryImageEdgeDetection(BYTE*Bin,BYTE*Out,int W,int H) // 라벨링 이미지 경계 검출, 4방향으로 모두 전경이라면, 경계다
{
	int ImgSize = W * H;
	

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Bin[i*W + j] == 0)//전경화소라면
			{
				if (!(Bin[(i - 1)*W + j] == 0 &&
					Bin[(i + 1)*W + j] == 0 &&
					Bin[i*W + j - 1] == 0 &&
					Bin[i*W + j + 1]==0)) // 4방향 화소중 전경이 아니면
				{
					Out[i*W + j] = 255;
				}
			}
		}
	}
}
void DrawRectOutline(BYTE * Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {

	for (int i = RD_Y; i < LU_Y; i++) //세로줄
	{
		Img[i*W + RD_X] = 255;
		Img[i*W + LU_X] = 255;
	}
	for (int i = LU_X; i < RD_X; i++) // 가로줄
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
	for (int i = H - 1; i > 0; i--) { // get y1
		for (int j = 0; j < W; j++) {
			if (Img[i*W + j] == 255) y1 = i;
			if (y1 != 0) break;
		}
	}
	for (int i = 0; i < W; i++) { // get x1
		for (int j = 0; j < H; j++) {
			if (Img[j*W + i] == 255) x1 = i;
			if (x1 != 0) break;
		}
	}
	for (int i = 0; i < H; i++) { // get y2
		for (int j = 0; j < W; j++) {
			if (Img[i*W + j] == 255) y2 = i;
			if (y2 != 0) break;
		}
	}
	for (int i = W - 1; i > 0; i--) { // get x2
		for (int j = 0; j < H; j++) {
			if (Img[j*W + i] == 255) x2 = i;
			if (x2 != 0) break;
		}
	}
	printf("(%d, %d), (%d, %d)",x1,y1,x2,y2);
	DrawRectOutline(Out, W, H, x1, y1, x2, y2);
	
	DrawCrossLine(Out, W, H, Cx, Cy);

}
int main()
{
	/*
	fileheader
	infoheader
	rgb
	image 받아올 때 infoheader의 w*h만큼 size 받아오고
	image size를 가변배열로 할당
	image(위의 3개 받고 남은거 모두가 image다)
	순으로 read
	filepointer close하기
	*/
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
	FILE* fp;
	fp = fopen("C:\\Users\\user\\Desktop\\3_1\\CV\\CV_main\\CV_main\\pupil2.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image = (BYTE *)malloc(ImgSize);
	BYTE * Output = (BYTE*)malloc(ImgSize);
	BYTE * Temp = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	int H = hInfo.biHeight, W = hInfo.biWidth;

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };
	/*동공 영역 검출*/
	/* image preprocessing */
	Binarization(Image, Temp, W, H,50);
	InverseImage(Temp, Temp, W, H);
	m_BlobColoring(Temp,H,W);
	for (int i = 0; i < ImgSize; i++) Output[i] = 0;
	BinaryImageEdgeDetection(Temp, Output, W, H);
	/*get center*/
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
	/* Draw */
	int Cx = sumx / cnt;
	int Cy = sumy / cnt;
	DrawRectCrossLine(Output,Image ,W, H,Cx,Cy);

	SaveBMPFile(hf, hInfo, hRGB, Image, hInfo.biWidth, hInfo.biHeight, "output_pupil2.bmp");
	free(Image);
	free(Output);
	free(Temp);
	return 0;
}