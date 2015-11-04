/*	Implementing Halftoning using the Error Diffusion method
	This program shall implement the Floyd-Steinberg's, JJN, and Stucki Error Diffusion methods
	A modification is also suggested by me at the end. (Performance Measure??) 
*/

#include<iostream>
#include<cmath>
#include<fstream>

using namespace std;

int main() {
	FILE *fin = fopen("mandrill.raw","rb");
	unsigned char *input = new unsigned char[512*512];
	float inputImage[512][512], temp[512][512];
	fread(input,1,512*512,fin);
	int i,j,k,l;
	k = 0;
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			inputImage[i][j] = float(input[k++]);	//input image
			temp[i][j] = inputImage[i][j];
			}
		}

	int FS[3][3] = {{0,0,0},{0,0,7},{3,5,1}};	//Floyd Steingberg method
	int JJN[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,7,5},{3,5,7,5,3},{1,3,5,3,1}};	//JJN method
	int Stucki[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,8,4},{2,4,8,4,2},{1,2,4,2,1}};//Stucki method
	int test[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,9,3},{1,3,9,3,1},{1,3,6,3,1}};	//Proposed method
	//We shall consider sepentine scanning instead of row wise scanning as this would ensure we would 
	//not accumulate error diffusion in the bottom right corner only
	//Since most objects of interest would be in the center of the image instead of the boundaries, 
	//image extension need not be performed
	float error = 0.0;	//Initially
	float diffusedErrorFS[3][3], diffusedErrorJJN[5][5], diffusedErrorStucki[5][5], diffusedErrorTest[5][5];//Diffused error pattern
	int outputFS[512][512], outputJJN[512][512], outputStucki[512][512], outputTest[512][512];	//Halftoned image

	//USING FLOYD-STEINBERG'S DIFFUSION MATRIX

	for(i=1; i<511; i++) {
		error = 0.0;
		for(j=1; j<512; j++) {
			//Computing Diffused error template
			for(k=0; k<3; k++)
				for(l=0; l<3; l++)
					diffusedErrorFS[k][l] = error*float(FS[k][l])*(1.0/16.0);
			//Computing the modified input
			for(k=-1; k<2; k++) 
				for(l=-1; l<2; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorFS[k+1][l+1];
			//Now thresholding this modified input
			//We shall use threshold = 127
			outputFS[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputFS[i][j]);	
			}
			//Scanning in Serpentine fashion
		error = 0.0;
		++i;
		for(j=510; j>=0; j--) {
			//Computing Diffused error template
			for(k=0; k<3; k++)
				for(l=0; l<3; l++)
					diffusedErrorFS[k][l] = error*float(FS[k][l])*(1.0/16.0);
			//Computing the modified input
			for(k=-1; k<2; k++) 
				for(l=-1; l<2; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorFS[k+1][1-l];
			//Now thresholding this modified input
			//We shall use threshold = 127
			outputFS[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputFS[i][j]);	
	
			}
		}
	FILE *fout1 = fopen("madrill_FS_Dithered_Serpentine_Scan.raw","wb");
	unsigned char *output1 = new unsigned char[512*512];
	k = 0;
	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			output1[k++] = outputFS[i][j];
	fwrite(output1,1,512*512,fout1);
	
	//USING JJN DIFFUSION MATRIX

	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			inputImage[i][j] = temp[i][j];

	for(i=2; i<511; i++) {
		error = 0.0;
		for(j=2; j<512; j++) {
			//Computing Diffused error template
			for(k=0; k<5; k++)
				for(l=0; l<5; l++)
					diffusedErrorJJN[k][l] = error*float(JJN[k][l])*(1.0/48.0);
			//Computing the modified input
			for(k=-2; k<3; k++) 
				for(l=-2; l<3; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorJJN[k+2][l+2];
			//Now thresholding this modified input
			//We shall use threshold = 127
			outputJJN[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputJJN[i][j]);	
			}
			//Scanning in Serpentine fashion
		error = 0.0;
		++i;
		for(j=510; j>=0; j--) {
			//Computing Diffused error template
			for(k=0; k<5; k++)
				for(l=0; l<5; l++)
					diffusedErrorJJN[k][l] = error*float(JJN[k][l])*(1.0/48.0);
			//Computing the modified input
			for(k=-2; k<3; k++) 
				for(l=-2; l<3; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorJJN[k+2][2-l];
			//Now thresholding this modified input
			//We shall use threshold = 127
			outputJJN[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputJJN[i][j]);	
	
			}
		}
	FILE *fout2 = fopen("madrill_JJN_Dithered_Serpentine_Scan.raw","wb");
	unsigned char *output2 = new unsigned char[512*512];
	k = 0;
	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			output2[k++] = outputJJN[i][j];
	fwrite(output2,1,512*512,fout2);

//	USING STUCKI'S DIFFUSION MATRIX

	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			inputImage[i][j] = temp[i][j];

	for(i=2; i<511; i++) {
		error = 0.0;
		for(j=2; j<512; j++) {
			//Computing Diffused error template
			for(k=0; k<5; k++)
				for(l=0; l<5; l++)
					diffusedErrorStucki[k][l] = error*float(Stucki[k][l])*(1.0/42.0);
			//Computing the modified input
			for(k=-2; k<3; k++) 
				for(l=-2; l<3; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorStucki[k+2][l+2];
			//Now thresholding this modified input
			//We shall use threshold = 127
			outputStucki[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputStucki[i][j]);	
			}
			//Scanning in Serpentine fashion
		error = 0.0;
		++i;
		for(j=510; j>=0; j--) {
			//Computing Diffused error template
			for(k=0; k<5; k++)
				for(l=0; l<5; l++)
					diffusedErrorStucki[k][l] = error*float(Stucki[k][l])*(1.0/42.0);
			//Computing the modified input
			for(k=-2; k<3; k++) 
				for(l=-2; l<3; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorJJN[k+2][2-l];
			//Now thresholding this modified input
			//We shall use threshold = 127
			//cout<<i<<"\t"<<j<<endl;
			outputStucki[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputStucki[i][j]);	
	
			}
		}
	FILE *fout3 = fopen("madrill_Stucki_Dithered_Serpentine_Scan.raw","wb");
	unsigned char *output3 = new unsigned char[512*512];
	k = 0;
	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			output3[k++] = outputStucki[i][j];
	fwrite(output3,1,512*512,fout3);

//	Proposed Idea

	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			inputImage[i][j] = temp[i][j];
	for(i=2; i<510; i++) {
		error = 0.0;
		for(j=2; j<511; j++) {
			//Computing Diffused error template
			for(k=0; k<5; k++)
				for(l=0; l<5; l++)
					diffusedErrorTest[k][l] = error*float(test[k][l])*(1.0/43.0);
			//Computing the modified input
			for(k=-2; k<3; k++) 
				for(l=-2; l<3; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorTest[k+1][l+1];
			//Now thresholding this modified input
			//We shall use threshold = 127
			outputTest[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputTest[i][j]);	
			}
			//Scanning in Serpentine fashion
		error = 0.0;
		++i;
		for(j=510; j>=0; j--) {
			//Computing Diffused error template
			for(k=0; k<5; k++)
				for(l=0; l<5; l++)
					diffusedErrorTest[k][l] = error*float(test[k][l])*(1.0/43.0);
			//Computing the modified input
			for(k=-2; k<3; k++) 
				for(l=-2; l<3; l++) 
					inputImage[i+k][j+l] = inputImage[i+k][j+l] + diffusedErrorTest[k+1][1-l];
			//Now thresholding this modified input
			//We shall use threshold = 127
			outputTest[i][j] = (inputImage[i][j]>127)?255:0;
			error = (inputImage[i][j] - outputTest[i][j]);	
	
			}
		}
	FILE *fout4 = fopen("madrill_test_Dithered_Serpentine_Scan.raw","wb");
	unsigned char *output4 = new unsigned char[512*512];
	k = 0;
	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			output4[k++] = outputTest[i][j];
	fwrite(output4,1,512*512,fout4);
	 

	
	return 0;
	}
