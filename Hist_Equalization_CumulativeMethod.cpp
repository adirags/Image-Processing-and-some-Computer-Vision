/*Histogram Equalization by Cumulative Method
  This program does NOT accept any input argument. 
  The variables used are :
	P = Pointer to input image
	eq_P = Pointer to equalized image
	R_count[], G_count[], B_count[] = Used to compute histogram of each channel
	Red_Coord[], Green_Coord[], Blue_Coord[] = Arrays to map each coordinate
        with the new intensity value it was assigned in each channel
  Dimension of image is 512 x 512
	
*/

#include<iostream>
#include<cmath>
#include<fstream>

using namespace std;

int main() {
	FILE *readptr = fopen("jet.raw","rb");
	FILE *writeptr = fopen("hist_eq_cm_jet.raw","wb");
	unsigned char *P = new unsigned char[512*512*3];
	unsigned char *eq_P = new unsigned char[512*512*3];
	fread(P,1,512*512*3,readptr);
	int R_count[256], G_count[256], B_count[256],i,j;
	for(i=0; i<256; i++) {				//Initializing counts to zero
		R_count[i] = G_count[i] = B_count[i] = 0;
		}
	for(i =0; i<512*512*3; i=i+3) {
		R_count[P[i]]++;
		G_count[P[i+1]]++;
		B_count[P[i+2]]++;
		}
	cout<<"Histogram Computed"<<endl;
	//We now have the number of occurences for each RGB component.
	int Red_Coord[512*512], Blue_Coord[512*512], Green_Coord[512*512], temp;
	for(i=0; i<512*512; i++) {
		Red_Coord[i] = i;
		Green_Coord[i] = i;
		Blue_Coord[i] = i;
		}
	int Red_Val[512*512], Green_Val[512*512], Blue_Val[512*512]; 
	for(i=0; i<512*512*3; i=i+3) {	//Getting the R,G,B components
		Red_Val[i/3] = P[i];
		Green_Val[i/3] = P[i+1];
		Blue_Val[i/3] = P[i+2];
		}
	cout<<"Computed Brightness and Identified their Coordinates"<<endl;
	//Sorting Pixel value in ascending value
	for(i=0; i<512*512; i++) {
		for(j=i+1; j<512*512; j++) {
			if(Red_Val[i]>Red_Val[j]) {
				temp = Red_Val[i];		//Swapping Pixel values 
				Red_Val[i] = Red_Val[j];
				Red_Val[j] = temp;
				temp = Red_Coord[i];		//Reflecting swap so that we know positions 
				Red_Coord[i] = Red_Coord[j];
				Red_Coord[j] = temp;
				}
			if(Green_Val[i]>Green_Val[j]) {
				temp = Green_Val[i];		//Swapping Pixel values 
				Green_Val[i] = Green_Val[j];
				Green_Val[j] = temp;
				temp = Green_Coord[i];		//Reflecting swap so that we know positions 
				Green_Coord[i] = Green_Coord[j];
				Green_Coord[j] = temp;
				}
			if(Blue_Val[i]>Blue_Val[j]) {
				temp = Blue_Val[i];		//Swapping Pixel values 
				Blue_Val[i] = Blue_Val[j];
				Blue_Val[j] = temp;
				temp = Blue_Coord[i];		//Reflecting swap so that we know positions 
				Blue_Coord[i] = Blue_Coord[j];
				Blue_Coord[j] = temp;
				}
			}
		}
	cout<<"Sorted!!"<<endl;
	//We have now sorted the coordinates in increasing order of their brightness
	int EquiLength;
	EquiLength = (512*512)/256;
	//Now assigning new brightness values using EquiLength as bin size
	for(i=0;i<256;i++) {
		for(j=0; j<1024; j++) {
			Red_Val[(i*1024)+j] = i;
			Green_Val[(i*1024)+j] = i;
			Blue_Val[(i*1024)+j] = i;
			}
		}
	cout<<"Assigned new brightness"<<endl;
	for(i=0; i<512*512; i++) {
		eq_P[3*Red_Coord[i]] = Red_Val[i];
		eq_P[(3*Green_Coord[i])+1] = Green_Val[i];
		eq_P[(3*Blue_Coord[i])+2] = Blue_Val[i];
		}
	cout<<"Wrote to eq_P"<<endl;
	fwrite(eq_P,1,512*512*3,writeptr);	
	return 0;
	}
