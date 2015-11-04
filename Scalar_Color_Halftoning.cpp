/*	Scalar Color Halftoning
	Error Diffusion method is applied to each channel separately to study the effect of 
	halftoning using error diffusion on such an application.
*/
#include<iostream>
#include<cmath>
#include<fstream>

using namespace std;

int tonedImage[512][512][3], test;

int main() {
	FILE *fin = fopen("Sailboat.raw","rb");
	unsigned char *input = new unsigned char[512*512*3];
	int inputImage[512][512][3];
	fread(input,1,512*512*3,fin);
	int i,j,k,l,m;
	k = 0;
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			inputImage[i][j][0] = input[k++];
			inputImage[i][j][1] = input[k++];
			inputImage[i][j][2] = input[k++];
			}
		}
	//Converting from RGB to CMY colorspace
	float imageCMY[512][512][3];
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			imageCMY[i][j][0] = 255.0 - float(inputImage[i][j][0]);
			imageCMY[i][j][1] = 255.0 - float(inputImage[i][j][1]);
			imageCMY[i][j][2] = 255.0 - float(inputImage[i][j][2]);
			}
		}
	//Applying Floyd Steinberg error diffusion to this CMY image
	int FS[3][3] = {{0,0,0},{0,0,7},{3,5,1}};	//Error Diffusion Matrix
	float diffusedErrorFS[3][3][3], error[3];
	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			for(m=0; m<3; m++)
				::tonedImage[i][j][m] = 0;
	for(i=1; i<511; i++) {
		for(m=0; m<3; m++)
			error[m] = 0.0;
		for(j=1; j<512; j++) {
			//Computing Diffused error template
			for(k=0; k<3; k++)
				for(l=0; l<3; l++)
					for(m=0; m<3; m++)
						diffusedErrorFS[k][l][m] = error[m]*float(FS[k][l])*(1.0/16.0);
			//Computing the modified input
			for(k=-1; k<2; k++) 
				for(l=-1; l<2; l++) 
					for(m=0; m<3; m++)
						imageCMY[i+k][j+l][m] = imageCMY[i+k][j+l][m] + diffusedErrorFS[k+1][l+1][m];
			//Now thresholding this modified input
			//We shall use threshold = 0.5
				//cout<<i<<"\t"<<j<<"\t"<<m<<endl;
			for(m=0; m<3; m++) {
				::tonedImage[i][j][m] = (imageCMY[i][j][m]>127)?255:0;
				error[m] = (imageCMY[i][j][m] - ::tonedImage[i][j][m]);	
				}
			}
		//Scanning in Serpentine fashion
		for(m=0; m<3; m++)
			error[m] = 0.0;
		++i;
		for(j=510; j>=0; j--) {
			//Computing Diffused error template
			for(k=0; k<3; k++)
				for(l=0; l<3; l++)
					for(m=0; m<3; m++)
						diffusedErrorFS[k][l][m] = error[m]*float(FS[k][l])*(1.0/16.0);
			//Computing the modified input
			for(k=-1; k<2; k++) 
				for(l=-1; l<2; l++) 
					for(m=0; m<3; m++)
						imageCMY[i+k][j+l][m] = imageCMY[i+k][j+l][m] + diffusedErrorFS[k+1][1-l][m];
			//Now thresholding this modified input
			//We shall use threshold = 0.5
			for(m=0; m<3; m++) {
				::tonedImage[i][j][m] = (imageCMY[i][j][m]>127)?255:0;
				error[m] = (imageCMY[i][j][m] - ::tonedImage[i][j][m]);	
				}
			}
		}
	//Computed Scalar halftoned image
	unsigned char *output = new unsigned char[512*512*3];
	k = 0;
	for(i=0; i<512; i++) 
		for(j=0; j<512; j++) 
			for(m=0; m<3; m++)
				output[k++] = 255 - ::tonedImage[i][j][m];	//Converting back to RGB space
	FILE *fout = fopen("Scalar_Halftoned_Sailboat.raw","wb");
	fwrite(output,1,512*512*3,fout);
	return 0;
	}
	
