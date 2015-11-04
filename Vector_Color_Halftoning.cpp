/*	Vector Color Halftoning - by performing MBVQ and CED
	MBVQ - Minimum Brightness Variation Quadruples
	CED - Color Error Diffusion
*/
#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;

int findMBVQ(int C, int M, int Y) {
	int result;
	if((C+M)<255) {
		if((M+Y)<255) {
			if((C+M+Y)<255) {
				result = 1;	//CMYW
				}
			else {
				result = 2;	//MYGC
				}
			}
		else {
			result = 3;		//RGMY
			}
		}
	else {
		if((M+Y)>255) {
			if((C+M+Y)>510) {
				result = 4;	//KRGB
				}
			else {
				result = 5;	//RGBM
				}
			}
		else {
			result = 6;		//CMGB
			}
		}
	return result;
	}

int* findClosestVertex(int tetrahedron, int imagePointX, int imagePointY, int imagePointZ) {
	int i, v;
	/*The value of v = 1 => Vertex C
			 = 2 => Vertex M
			 = 3 => Vertex Y
			 = 4 => Vertex W
			 = 5 => Vertex R
			 = 6 => Vertex G
			 = 7 => Vertex B
			 = 8 => Vertex K 
			*/
	float dist, minDist;
	switch(tetrahedron) {
		case 1:{//CMYW
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ,2));	//Dist from C
			v = 1;		//Assume initially
			minDist = dist;	//Initially
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));	//Dist from M
			v = (dist<minDist)?2:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));	//Dist from Y
			v = (dist<minDist)?3:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY,2) + pow(imagePointZ,2));	//Dist from W
			v = (dist<minDist)?4:v;	
			minDist = (dist<minDist)?dist:minDist;
			break;
			}
		case 2:{//MYGC
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));	//Dist from M
			v = 2;	//Assume Initially	
			minDist = dist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));	//Dist from Y
			v = (dist<minDist)?3:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));//Dist from G
			v = (dist<minDist)?6:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ,2));	//Dist from C
			v = (dist<minDist)?1:v;	
			minDist = (dist<minDist)?dist:minDist;
			break;
			}
		case 3:{//RGMY
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ-255,2));//Dist from R
			v = 5;	//Assume Initially	
			minDist = dist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));//Dist from G
			v = (dist<minDist)?6:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));//Dist from M
			v = (dist<minDist)?2:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));	//Dist from Y
			v = (dist<minDist)?3:v;	
			minDist = (dist<minDist)?dist:minDist;
			break;
			}
		case 4:{//KRGB
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY-255,2) + pow(imagePointZ-255,2));//Dist from K
			v = 8;	//Assume Initially	
			minDist = dist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ-255,2));//Dist from R
			v = (dist<minDist)?5:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));//Dist from G
			v = (dist<minDist)?6:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));//Dist from B
			v = (dist<minDist)?7:v;	
			minDist = (dist<minDist)?dist:minDist;
			break;
			}
		case 5:{//RGBM
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ-255,2));//Dist from R
			v = 5;	//Assume Initially	
			minDist = dist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));//Dist from G
			v = (dist<minDist)?6:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));//Dist from B
			v = (dist<minDist)?7:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));	//Dist from M
			v = (dist<minDist)?2:v;	
			minDist = (dist<minDist)?dist:minDist;
			break;
			}
		case 6:{//CMGB
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ,2));	//Dist from C
			v = 1;	//Assume Initially	
			minDist = dist;
			dist = sqrt(pow(imagePointX,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));	//Dist from M
			v = (dist<minDist)?2:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY,2) + pow(imagePointZ-255,2));//Dist from G
			v = (dist<minDist)?6:v;	
			minDist = (dist<minDist)?dist:minDist;
			dist = sqrt(pow(imagePointX-255,2) + pow(imagePointY-255,2) + pow(imagePointZ,2));//Dist from B
			v = (dist<minDist)?7:v;	
			minDist = (dist<minDist)?dist:minDist;
			break;
			}
		}
	int vertex[3];
	switch(v) {
		case 1:{
			int V[3] = {255,0,0};	//Vertex C
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		case 2:{
			int V[3] = {0,255,0};	//Vertex M
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		case 3:{
			int V[3] = {0,0,255};	//Vertex Y
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		case 4:{
			int V[3] = {0,0,0};	//Vertex W
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		case 5:{
			int V[3] = {0,255,255};//Vertex R
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		case 6:{
			int V[3] = {255,0,255};//Vertex G
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		case 7:{
			int V[3] = {255,255,0};//Vertex B
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		case 8:{
			int V[3] = {255,255,255};//Vertex K
			for(i=0; i<3; i++)
				vertex[i] = V[i];
			break;
			}
		}
	//cout<<vertex[0]<<"\t"<<vertex[1]<<"\t"<<vertex[2]<<endl;
	return vertex;
	}
			

int main() {
	FILE *fin = fopen("Sailboat.raw","rb");
	unsigned char *input = new unsigned char[512*512*3];
	int imageCMY[512][512][3], MBVQ[512][512], outputImageCMY[512][512][3];
	int i,j,k,l,m;
	int *V;
	k = 0;
	fread(input,1,512*512*3,fin);	
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			for(m=0; m<3; m++) {
				imageCMY[i][j][m] = 255 - input[k++];
				}
			}
		}

	int FS[3][3] = {{0,0,0},{0,0,7},{3,5,1}};
	float diffusedErrorFS[3][3][3], error[3];
	error[0] = error[1] = error[2] = 0.0;
	for(i=1; i<511; i++) {
		error[0] = error[1] = error[2] = 0.0;
		for(j=1; j<512; j++) {
			//Finding MBVQ vertex for (i,j)-th pixel
			MBVQ[i][j] = findMBVQ(imageCMY[i][j][0], imageCMY[i][j][1], imageCMY[i][j][2]);	
			//Obtaining Diffused Error Template
			for(k=0; k<3; k++)
				for(l=0; l<3; l++)
					for(m=0; m<3; m++)
						diffusedErrorFS[k][l][m] = error[m]*float(FS[k][l])*(1./16.);
			//Computing Modified Input input
			for(k=-1; k<2; k++) 
				for(l=-1; l<3; l++) 
					for(m=0; m<3; m++) 
						imageCMY[i+k][j+l][m] += diffusedErrorFS[k+1][l+1][m];	
			V = findClosestVertex(MBVQ[i][j], imageCMY[i][j][0], imageCMY[i][j][1], imageCMY[i][j][2]);	
			for(m=0; m<3; m++) {
				outputImageCMY[i][j][m] = V[m];//(imageCMY[i][j][m]>127)?255:0;
				error[m] = imageCMY[i][j][m] - V[m];
				}
			}
		++i;	//Performing Serpentine Scanning
		for(m=0; m<3; m++)
			error[m] = 0.0;
		for(j=510; j>=0; j--) {
			//Finding MBVQ vertex for (i,j)-th pixel
			MBVQ[i][j] = findMBVQ(imageCMY[i][j][0], imageCMY[i][j][1], imageCMY[i][j][2]);	
			//Obtaining Diffused Error Template
			for(k=0; k<3; k++)
				for(l=0; l<3; l++)
					for(m=0; m<3; m++)
						diffusedErrorFS[k][l][m] = error[m]*float(FS[k][l])*(1./16.);
			//Computing Modified Input input
			for(k=-1; k<2; k++) 
				for(l=-1; l<3; l++) 
					for(m=0; m<3; m++) 
						imageCMY[i+k][j+l][m] += diffusedErrorFS[k+1][l+1][m];	
			V = findClosestVertex(MBVQ[i][j], imageCMY[i][j][0], imageCMY[i][j][1], imageCMY[i][j][2]);	
			for(m=0; m<3; m++) {
				outputImageCMY[i][j][m] = V[m];//(imageCMY[i][j][m]>127)?255:0;
				error[m] = imageCMY[i][j][m] - V[m];
				}
			}

		}
	FILE *fout = fopen("Sailboat_VectorHalftoned.raw","wb");
	unsigned char *output = new unsigned char[512*512*3];
	k = 0;
	for(i=0; i<512; i++)
		for(j=0; j<512; j++)
			for(m=0; m<3; m++)
				output[k++] = 255 - outputImageCMY[i][j][m];
	fwrite(output,1,512*512*3,fout);
	return 0;
	}
	
