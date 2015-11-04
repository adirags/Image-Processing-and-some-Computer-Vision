/*	Implementation of Digital Halftoning using Ordered Index Matrices to get a two tone image. 
	Index matrices of order 4 amd 8 were tried.
	Extended implementation by assuming the possibility of printing 4 levels instead of just 2
	to study the effect of color levels on the dithered output image
*/
#include<iostream>
#include<cmath>
#include<fstream>

using namespace std;

int main() {
	FILE *fin = fopen("mandrill.raw","rb");
	unsigned char *input = new unsigned char[512*512];
	fread(input,1,512*512,fin);
	int I4[4][4], I8[8][8], i, j, k;
	int I2[2][2] = {{1,2},{3,0}};
	float inputImage[512][512];
	k = 0;
	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			inputImage[i][j] = float(input[k++])/255.0;

	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			k = ((i<2)&&(j<2))?1:(i<2)?2:(j<2)?3:0;
			I4[i][j] = 4*I2[i%2][j%2] + k; 
			}
		}
	for(i=0; i<8; i++) {
		for(j=0; j<8; j++) {
			k = ((i<4)&&(j<4))?1:(i<4)?2:(j<4)?3:0;
			I8[i][j] = 4*I4[i%4][j%4] + k;
			}
		}
	//Computed the Index Matrices!
	float T4[4][4], T8[8][8];
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			T4[i][j] = (float(I4[i][j]) + 0.5)/16.0;
	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
			T8[i][j] = (float(I8[i][j]) + 0.5)/64.0;
	//Computed the Thresholds!!
	int imageDitheredWithI4[512][512], imageDitheredWithI8[512][512];
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {	
			imageDitheredWithI4[i][j] = (inputImage[i][j]>T4[i%4][j%4])?255:0;
			imageDitheredWithI8[i][j] = (inputImage[i][j]>T8[i%8][j%8])?255:0;
			}
		}
	FILE *fout1 = fopen("mandrillDitheredI4.raw","wb");
	FILE *fout2 = fopen("mandrillDitheredI8.raw","wb");
	unsigned char *outputImage1 = new unsigned char[512*512];
	unsigned char *outputImage2 = new unsigned char[512*512];
	k = 0;
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			outputImage1[k] = imageDitheredWithI4[i][j];
			outputImage2[k++] = imageDitheredWithI8[i][j];
			}
		}
	fwrite(outputImage1,1,512*512,fout1);
	fwrite(outputImage2,1,512*512,fout2);
	fclose(fout1);
	fclose(fout2);
	//Now suppose we are able to display four levels instead of just two...
	//We would need to come up with some alternative thresholding
	//Naively, we could use T4 and T8 and choose ratios of it which could result in one of four possible output levels
	int imageDitheredI4_4Levels[512][512], imageDitheredI8_4Levels[512][512];
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			imageDitheredI4_4Levels[i][j] = (inputImage[i][j]>(255+T4[i%4][j%4])/2)?255:(inputImage[i][j]>T4[i%4][j%4])?170:(inputImage[i][j]>T4[i%4][j%4]*0.5)?85:0;
			imageDitheredI8_4Levels[i][j] = (inputImage[i][j]>(255+T8[i%8][j%8])/2)?255:(inputImage[i][j]>T8[i%8][j%8])?170:(inputImage[i][j]>T8[i%8][j%8]*0.5)?85:0;
			}
		}
	fout1 = fopen("mandrill_4Levels_DitheredI4.raw","wb");
	fout2 = fopen("mandrill_4Levels_DitheredI8.raw","wb");
	k = 0;
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			outputImage1[k] = imageDitheredI4_4Levels[i][j];
			outputImage2[k++] = imageDitheredI8_4Levels[i][j];
			}
		}
	fwrite(outputImage1,1,512*512,fout1);
	fwrite(outputImage2,1,512*512,fout2);
	fclose(fout1);
	fclose(fout2);
	return 0;
	}
