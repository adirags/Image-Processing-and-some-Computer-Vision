/*This program uses the bilinear interpolation method to perform demosaicing
  on a CFA image to produce an image containing all color components.
  This program does not require any input, all inputs have been configured for the 
  desired output as per the problem statement
  The variables used are :
	P = Pointer to the CFA image 
	newP = Pointer to the resultant image after bilinear interpolation
	oldPic[][] = 2D matrix storing the intensity values of the CFA image
	newPicRed[] = 2D matrix storing intensity values of Red Channel
	newPicGreen[] = 2D matrix storing intensity values of Green Channel
	newPicBlue[] = 2D matrix storing intensity values of Blue Channel
  The image "parrot_CFA.raw" and "parrot_color_bl.raw" have a dimension of 424 x 636	
*/
#include<iostream>
#include<fstream>
#include<cstdlib>

using namespace std;

int main(int argc, char **argv) {
	unsigned char *P = new unsigned char[424*636];
	unsigned char *newP = new unsigned char[424*636*3];
	FILE *readptr = fopen("parrot_CFA.raw","rb");
	FILE *writeptr = fopen("parrot_color_bl.raw","wb");
	fread(P,1,424*636,readptr);
	int i,j,k=0;
	int oldPic[638][426];	//Size has been increased to accomodate mirroring that will be performed
	int newPicRed[636][424],newPicGreen[636][424],newPicBlue[636][424];
	for(i=1;i<637;i++)
		for(j=1;j<425;j++)
			oldPic[i][j] = P[k++];
	//Performing Mirroring	
	for(i=0;i<424;i++) {
		oldPic[0][i+1] = oldPic[2][i+1];
		oldPic[637][i+1] = oldPic[635][i+1];
		}
	for(i=0;i<638;i++) {
		oldPic[i][0] = oldPic[i][2];
		oldPic[i][425] = oldPic[i][423];
		}
	k=0;
	for(i=1;i<637;i++) {
		for(j=1;j<425;j++) {
			if((i+j)%2==1) {		//Green Tile from Bayer Array
				newPicGreen[i-1][j-1] = oldPic[i][j];
				if(i%2==1) {	//If Green Tile is in an odd row => RG row
					newPicRed[i-1][j-1] = (oldPic[i][j-1] + oldPic[i][j+1])/2;
					newPicBlue[i-1][j-1] = (oldPic[i-1][j] + oldPic[i+1][j])/2;
						}
				else {		//If Green Tile is in an even row => GB row
					newPicRed[i-1][j-1] = (oldPic[i-1][j] + oldPic[i+1][j])/2;
					newPicBlue[i-1][j-1] = (oldPic[i][j-1] + oldPic[i][j+1])/2;
					}
				}
			if(((i+j)%2==0)&&(i%2==1)) {	//Red Tile from Bayer Array
				newPicRed[i-1][j-1] = oldPic[i][j];
				newPicGreen[i-1][j-1] = (oldPic[i][j+1]+oldPic[i][j-1]+oldPic[i+1][j]+oldPic[i-1][j])/4;
				newPicBlue[i-1][j-1] = (oldPic[i-1][j-1]+oldPic[i-1][j+1]+oldPic[i+1][j-1]+oldPic[i+1][j+1])/4;
				}
			if(((i+j)%2==0)&&(i%2==0)) {	//Blue Tile from Bayer Array
				newPicBlue[i-1][j-1] = oldPic[i][j];
				newPicGreen[i-1][j-1] = (oldPic[i][j+1]+oldPic[i][j-1]+oldPic[i+1][j]+oldPic[i-1][j])/4;
                                newPicRed[i-1][j-1] = (oldPic[i-1][j-1]+oldPic[i-1][j+1]+oldPic[i+1][j-1]+oldPic[i+1][j+1])/4;
				}
			}
		}
	k = 0;
	for(i=0;i<636;i++) {
		for(j=0;j<424;j++) {
			newP[k++] = newPicRed[i][j];
			newP[k++] = newPicGreen[i][j];
			newP[k++] = newPicBlue[i][j];
			}
		}
	fwrite(newP,1,424*636*3,writeptr);
	return 0;
}
