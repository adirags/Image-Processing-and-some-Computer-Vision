/* This program performs Sobel Edge detection by computing gradient magnitude. Thresholding is performed
  to consider only significant edges. To further improve the result, Non maximal suppression can be implemented.
  This would ensure pixel wide edges only.
*/

#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;

#define pi 3.14159265

int main() {
	FILE *f = fopen("Farm.raw","rb");		//Change to "Cougar.raw" to apply for Cougar image
	unsigned char *inputImage = new unsigned char[481*321*3];
	unsigned char *outputImage = new unsigned char[481*321];
	fread(inputImage,1,481*321*3,f);
	fclose(f);
	f = fopen("Farm_Sobel_Edge_10%_NMS.raw","wb");
	int  grayScaleInput[325][485];		//Considering extra dimension for Mirroring
	int i,j,k,l;		//Indexes
	k = 0;	//Pixel count of color image
	for(i=0; i<321; i++) {
		for(j=0; j<481; j++) {
			if(i%2 == 0) {	//RG row of Bayer Array
				if(j%2 == 0)
					grayScaleInput[i+2][j+2] = inputImage[(3*k)];
				else
					grayScaleInput[i+2][j+2] = inputImage[(3*k)+1];
				}
			else {		//GB row of Bayer Array
				if(j%2 == 0)
					grayScaleInput[i+2][j+2] = inputImage[(3*k)+1];
				else
					grayScaleInput[i+2][j+2] = inputImage[(3*k)+2];
				}
			++k;	//Move to next pixel
			}
		}
	//Now Performing Mirroring
	for(i=0; i<481; i++) {
		grayScaleInput[1][i+2] = grayScaleInput[3][i+2];
		grayScaleInput[0][i+2] = grayScaleInput[4][i+2];
		grayScaleInput[323][i+2] = grayScaleInput[321][i+2];
		grayScaleInput[324][i+2] = grayScaleInput[320][i+2];
		}
	for(i=0; i<325; i++) {
		grayScaleInput[i][1] = grayScaleInput[i][3];
		grayScaleInput[i][0] = grayScaleInput[i][4];
		grayScaleInput[i][483] = grayScaleInput[i][481];
		grayScaleInput[i][484] = grayScaleInput[i][480];
		}
	//Mirroring Done
	//Now we shall perform Sobel first order derivative Edge Detection
	int imageGradient[321][481], Gx[321][481], Gy[321][481], G;
	//Where Gx = Horizontal Gradient
	//	Gy = Vertical Gradient
	//	G = Edge Magnitude
	for(i=2; i<323; i++) {
		for(j=2; j<483; j++) {
			Gx[i][j] = grayScaleInput[i-1][j+1] + 2*grayScaleInput[i][j+1] + grayScaleInput[i+1][j+1];
			Gx[i][j] = Gx[i][j] - grayScaleInput[i-1][j-1] - 2*grayScaleInput[i][j-1] - grayScaleInput[i+1][j-1];
			Gx[i][j] = Gx[i][j]/4;
			Gy[i][j] = grayScaleInput[i-1][j-1] + 2*grayScaleInput[i-1][j] + grayScaleInput[i-1][j+1];
			Gy[i][j] = Gy[i][j] - grayScaleInput[i+1][j-1] - 2*grayScaleInput[i+1][j] - grayScaleInput[i+1][j+1];
			Gy[i][j] = Gy[i][j]/4;
			G = pow(Gx[i][j],2) + pow(Gy[i][j],2);
			G = sqrt(G);
			imageGradient[i-2][j-2] = G;
			}
		}
	/*Outputting Image Gradient
	k = 0;
	for(i=2; i<323; i++) 
		for(j=2; j<483; j++)
			outputImage[k++] = imageGradient[i-2][j-2];
	fwrite(outputImage,1,481*321,f);	*/
	cout<<"Got image gradient"<<endl;
	//Thresholding imageGradient to see if it is an Edge point
	int temp[321*481], t;
	for(i=0; i<321; i++)
		for(j=0; j<481; j++)
			temp[i*321 + j] = imageGradient[i][j];

	for(i=0; i<(321*481)-1; i++) {
		for(j=i+1; j<321*481; j++) {
			if(temp[i]<temp[j]) {	//Getting largest terms on top
		 		t = temp[i];
				temp[i] = temp[j];
				temp[j] = t;
				}
			}	
		}
	int threshold = temp[int((321*421)*0.10)];	//Thresholding so that some %-age of pixels are edge points
	
	for(i=0; i<321; i++) {
		for(j=0; j<481; j++) {
			if(imageGradient[i][j]>threshold)
				imageGradient[i][j] = imageGradient[i][j];
			else
				imageGradient[i][j] = 0;
			}
		}
	cout<<"Applied Threshold"<<endl;
	//Now that Thresholding is done, can implement Non Maximal Suppression
	float thetha; 	//Edge Direction
	int p1,p2;
	int nmsImageGradient[321][481];
	for(i=0; i<321; i++)
		for(j=0;j<481; j++)
			nmsImageGradient[i][j] = imageGradient[i][j];
	for(i=1; i<321; i++) {
		for(j=1; j<481; j++) {
			if((Gx[i][j]==0)&&(Gy[i][j]==0))
				goto P;
			else			
				thetha = atan(Gy[i][j]/(Gx[i][j]+0.00001));	//Within range -pi/2 to pi/2
			thetha = thetha*180/pi;		//Value in degrees
			//We shall now quantize the angles to four possible values
			if( (thetha<22.5)&&(thetha>-22.5) )
				thetha = 0.0;
			else if( (thetha<77.5)&&(thetha>22.5) )
				thetha = 45.0;
			else if( (thetha>-77.5)&&(thetha<-22.5) )
				thetha = -45.0;
			else
				thetha = 90.0;
			//cout<<"Test";
			if(thetha==0.0) {
				p1 = imageGradient[i-1][j];
				p2 = imageGradient[i+1][j];
				nmsImageGradient[i][j] = (imageGradient[i][j]<p1)?0:(imageGradient[i][j]<p2)?0:imageGradient[i][j];
				}
			if(thetha==45.0) {
				p1 = imageGradient[i-1][j-1];
				p2 = imageGradient[i+1][j+1];
				nmsImageGradient[i][j] = (imageGradient[i][j]<p1)?0:(imageGradient[i][j]<p2)?0:imageGradient[i][j];
				}
			if(thetha==-45.0) {
				p1 = imageGradient[i-1][j+1];
				p2 = imageGradient[i+1][j-1];
				nmsImageGradient[i][j] = (imageGradient[i][j]<p1)?0:(imageGradient[i][j]<p2)?0:imageGradient[i][j];
				}
			else {
				p1 = imageGradient[i][j-1];
				p2 = imageGradient[i][j+1];
				nmsImageGradient[i][j] = (imageGradient[i][j]<p1)?0:(imageGradient[i][j]<p2)?0:imageGradient[i][j];
				}
		P:	k=0;
			}
		}	
	k = 0;
	for(i=0; i<321; i++) {
		for(j=0; j<481; j++) {
			if(nmsImageGradient[i][j]==0)
				outputImage[k++] = 255;		//Giving White background and Black edge points
			else
				outputImage[k++] = 0;
			}
		}
	fwrite(outputImage,1,481*321,f);	
	fclose(f);
	return 0;
	}
