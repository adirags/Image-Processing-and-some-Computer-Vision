/*	Image Swirling
	Swirling is performed about the center. The degree of swilring is controlled by the scaling coefficient
	To perform swirling, we compute the relative position of a pixel witht the center. The distance and angle of
	the pixel from the center determines where it shall be swirled to.
*/
#include<iostream>
#include<cmath>
#include<fstream>


using namespace std;

#define pi 3.142
int main() {
	FILE *f = fopen("kate.raw","rb");
	unsigned char *Kate = new unsigned char[512*512*3];
	fread(Kate,1,512*512*3,f);
	float angle;	
	int i,j,x,y,k,midX,midY,newX,newY;
	midX = 512/2;
	midY = 512/2;
	k = 0;
	int Image[512][512][3], newImage[512][512][3];
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) { 
			Image[i][j][0] = Kate[k++];
			Image[i][j][1] = Kate[k++];
			Image[i][j][2] = Kate[k++];
			newImage[i][j][0] = 0;
			newImage[i][j][1] = 0;
			newImage[i][j][2] = 0;
			}
		}
	cout<<"Read Image"<<endl;
	double theta, radius;
	int swirlX, swirlY;
	cout<<"Performing Swirling\n";
	for(x=0; x<512; x++) {
		for(y=0; y<512; y++) {	
			newX = x-midX;			
			newY = y-midY;
			theta = pi + pi/4 + atan2(newY,newX);	//Performing Rotating and Swirling
			radius = sqrt(newX*newX + newY*newY);
			swirlX = midX + radius*cos(theta + 0.01*radius);	//0.01 is the scaling coefficient
			swirlY = midY + radius*sin(theta + 0.01*radius);	//0.01 is the scaling coefficient
			if((swirlX >0)&&(swirlX<512)&&(swirlY >0)&&(swirlY<512)) {
				newImage[x][y][0] = Image[swirlX][swirlY][0];
				newImage[x][y][1] = Image[swirlX][swirlY][1];
				newImage[x][y][2] = Image[swirlX][swirlY][2]; 
				}
			}
		}
	cout<<"Swirling Done...that was fast!\n";
	
	unsigned char *SwirlKate = new unsigned char[512*512*3];
	k = 0;
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			SwirlKate[k++] = newImage[i][j][0];
			SwirlKate[k++] = newImage[i][j][1];
			SwirlKate[k++] = newImage[i][j][2];
			}
		}
	FILE *fout = fopen("SwirlKate.raw","wb");
	fwrite(SwirlKate, 1, 512*512*3, fout);
	return 0;
	}
	
	
