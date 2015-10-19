/***************Convert From Gray Scale to Color*******************
  *****************************************************************
  This program uses the MHC method to demosaic a CFA image. The variables used are :
	width = width of input image specified in main()
	height = height of input image specified in main()
	Image = Pointer to input CFA image
	NewImage = Pointer to demosaiced image
	GrayPic[][] = 2D matrix to store intensity values of input image
	ColorPicRed[][] = 2D matrix to store intensity of Red channel of output
	ColorPicGreen[][] = 2D matrix to store intensity of Green channel of output
	ColorPicBlue[][] = 2D matrix to store intensity of Blue channel of output
  The dimension of the input CFA image is 424 x 636
*/

#include<iostream>
#include<fstream>
#include<cstdlib>

using namespace std;

int main(int argc, char **argv) {
	int width, height;
	width = 424;
	height = 636;
	int i,j,k = 0;
	unsigned char *Image = new unsigned char[width*height];
	unsigned char *NewImage = new unsigned char[width*height*3];
	FILE *readptr = fopen("parrot_CFA.raw","rb");		//Specify input file
	FILE *writeptr = fopen("parrot_color_MHC.raw","wb");
	fread(Image,1,width*height,readptr);
	int GrayPic[height+4][width+4];	//Extra dimensionality to accomodate mirroring
	int ColorPicRed[height][width], ColorPicGreen[height][width], ColorPicBlue[height][width];
	for(i=2; i<height+2; i++)
		for(j=2; j<width+2; j++)
			GrayPic[i][j] = Image[k++];

	//Performing Mirroring
	for(i=0; i<width; i++) {
		GrayPic[1][i+2] = GrayPic[3][i+2];
		GrayPic[0][i+2] = GrayPic[4][i+2];
		GrayPic[height+2][i+2] = GrayPic[height][i+2];
		GrayPic[height+3][i+2] = GrayPic[height-1][i+2];
		}
	for(i=0; i<height+4; i++) {
		GrayPic[i][1] = GrayPic[i][3];
		GrayPic[i][0] = GrayPic[i][4];
		GrayPic[i][width+2] = GrayPic[i][width];
		GrayPic[i][width+3] = GrayPic[i][width-1];
		}

	k = 0;
	for(i=2; i<height+2; i++) {
		for(j=2; j<width+2; j++) {
			if((i+j)%2==1) {			//Green Tile in Bayer Array
				ColorPicGreen[i-2][j-2] = GrayPic[i][j];
				if(i%2==0) {		//Green Tile in RG row
					ColorPicRed[i-2][j-2] = ( 5*GrayPic[i][j]+0.5*(GrayPic[i-2][j]+GrayPic[i+2][j])+4*(GrayPic[i][j-1]+GrayPic[i][j+1])-GrayPic[i-1][j-1]-GrayPic[i-1][j+1]-GrayPic[i+1][j-1]-GrayPic[i+1][j+1]-GrayPic[i][j-2]-GrayPic[i][j+2] )/8;

					ColorPicBlue[i-2][j-2] = ( 5*GrayPic[i][j]+0.5*(GrayPic[i][j-2]+GrayPic[i][j+2])+4*(GrayPic[i-1][j]+GrayPic[i+1][j])-GrayPic[i-1][j-1]-GrayPic[i-1][j+1]-GrayPic[i+1][j-1]-GrayPic[i+1][j+1]-GrayPic[i-2][j]-GrayPic[i+2][j] )/8;
					}
				else {			//Green tile in GB row
					ColorPicRed[i-2][j-2] = ( 5*GrayPic[i][j]+0.5*(GrayPic[i][j-2]+GrayPic[i][j+2])+4*(GrayPic[i-1][j]+GrayPic[i+1][j])-GrayPic[i-1][j-1]-GrayPic[i-1][j+1]-GrayPic[i+1][j-1]-GrayPic[i+1][j+1]-GrayPic[i-2][j]-GrayPic[i+2][j] )/8;
	
					ColorPicBlue[i-2][j-2] = ( 5*GrayPic[i][j]+0.5*(GrayPic[i-2][j]+GrayPic[i+2][j])+4*(GrayPic[i][j-1]+GrayPic[i][j+1])-GrayPic[i-1][j-1]-GrayPic[i-1][j+1]-GrayPic[i+1][j-1]-GrayPic[i+1][j+1]-GrayPic[i][j-2]-GrayPic[i][j+2] )/8;
					}
			}

			if((i%2==0)&&(j%2==0)) {		//Red Tile in Bayer Array
				ColorPicRed[i-2][j-2] = GrayPic[i][j];
				ColorPicGreen[i-2][j-2] = ( 4*GrayPic[i][j]+2*(GrayPic[i-1][j]+GrayPic[i+1][j]+GrayPic[i][j-1]+GrayPic[i][j+1])-GrayPic[i-2][j]-GrayPic[i+2][j]-GrayPic[i][j-2]-GrayPic[i][j+2])/8;

				ColorPicBlue[i-2][j-2] = ( 6*GrayPic[i][j]+2*(GrayPic[i-1][j-1]+GrayPic[i-1][j+1]+GrayPic[i+1][j-1]+GrayPic[i+1][j+1])-1.5*(GrayPic[i-2][j]+GrayPic[i+2][j]+GrayPic[i][j-2]+GrayPic[i][j+2]))/8;
				}

			if((i%2==1)&&(j%2==1)) {		//Blue Tile in Bayer Array
				ColorPicBlue[i-2][j-2] = GrayPic[i][j];
				ColorPicRed[i-2][j-2] = ( 6*GrayPic[i][j]+2*(GrayPic[i-1][j-1]+GrayPic[i-1][j+1]+GrayPic[i+1][j-1]+GrayPic[i+1][j+1])-1.5*(GrayPic[i-2][j]+GrayPic[i+2][j]+GrayPic[i][j-2]+GrayPic[i][j+2]) )/8;

				ColorPicGreen[i-2][j-2] = ( 4*GrayPic[i][j]+2*(GrayPic[i-1][j]+GrayPic[i+1][j]+GrayPic[i][j-1]+GrayPic[i][j+1])-GrayPic[i-2][j]-GrayPic[i+2][j]-GrayPic[i][j-2]-GrayPic[i][j+2])/8;
				}
			}
		}
	
	//Placing a cap on pixel values
	for(i=0; i<height;i++) {
		for(j=0; j<width; j++) {
			if(ColorPicRed[i][j]<0)	
				ColorPicRed[i][j] = 0;
			if(ColorPicGreen[i][j]<0)	
				ColorPicGreen[i][j] = 0;
			if(ColorPicBlue[i][j]<0)	
				ColorPicBlue[i][j] = 0;
			if(ColorPicRed[i][j]>255)	
				ColorPicRed[i][j] = 255;
			if(ColorPicGreen[i][j]>255)	
				ColorPicGreen[i][j] = 255;
			if(ColorPicBlue[i][j]>255)	
				ColorPicBlue[i][j] = 255;
			}
		}

	k = 0;
	//Writing RGB components on to result image
	for(i=0; i<height;i++) {
		for(j=0; j<width;j++) {
			NewImage[k++] = ColorPicRed[i][j];
			NewImage[k++] = ColorPicGreen[i][j];
			NewImage[k++] = ColorPicBlue[i][j];
			}
		}
	
	fwrite(NewImage,1,width*height*3,writeptr);
	return 0;
	}
