/*This program first quantizes the color sample space of the input image and outputs it.
  It then works on this reduced color set image to generate an image with an oil painting 
  effect. The variables used are :
	I0 = Initial input image
	I1 = Output image after reducing the color set of I0
	I2 = Final output with an oil painting effect
	Red_Count[], Green_Count[], Blue_Count[] = Used to compute histogram of each channel
	Rbinx, Gbinx, Bbinx = Variables to store the bin length, where x = 1,2,3,4
	avgrbx, avggbx, avgbbx = Average value of bin x of Red, Green and Blue channels respectively
	newImgR[][], newImgG[][], newImgB[][] = 2D matrices to store the reduced color set image
	N = Window size used to obtain the Oil Painting effect
  We find the most common color in the N*N window and use it to represent the center pixel of the window
	Rneighbors[], Gneighbors[], Bneighbors[] = Stores the neighbors from each channel in a windowed region
	commonColorCount[] = Keep track of how frequently each color in the N*N window appears
	finalImageR[][], finalImageG[][], finalImageB[][] = Stores resultant image with oilpainting effect
  The image dimension is 380 x 275 for The Barn
  and 580 x 247 for The Coliseum
  
  The below code is for the "barn.raw" image. To apply it for the "coliseum.raw" image,
  you would need to change the size of the above variables to match the dimensionality of the coliseum.
  Need to change every occurence of 380 to 580, and 275 to 247 

  You will need to vary the Window size, N, to view the results of the oil painting effect for different window sizes
*/
#include<iostream>
#include<fstream>

using namespace std;

int main() {
	FILE *readptr = fopen("barn.raw"/*"coliseum.raw"*/,"rb");
	FILE *ReducedColorptr = fopen("Reduced_color_set_64_barn.raw"/*"Reduced_color_set_64_coliseum.raw"*/,"wb");
	FILE *OilPaintptr = fopen("Paintified_barn_N3.raw"/*"Paintified_coliseum_N3"*/,"wb");
	unsigned char *I0 = new unsigned char[380*275*3];
	unsigned char *I1 = new unsigned char[380*275*3];
	unsigned char *I2 = new unsigned char[380*275*3];
	fread(I0,1,380*275*3,readptr);
	int i,j,Red_Count[256],Green_Count[256],Blue_Count[256],r,g,b;
	for(i=0; i<256; i++) 
		 Red_Count[i] = Green_Count[i] = Blue_Count[i] = 0;
	for(i=0; i<380*275*3; i=i+3) {
		r = I0[i];
		g = I0[i+1];
		b = I0[i+2];
		++Red_Count[r];
		++Green_Count[g];
		++Blue_Count[b];
		}
	cout<<"Got Counts"<<endl;
	//Now we have the occurrences of each pixel
	//Need to identify suitable bin size which we can use. One way to do this is to integrate histogram and//
	//split integral value into 4 equal parts and project the boundaries of the parts down to find bin size

	int Red_tot, Blue_tot, Green_tot, running_sum_R, running_sum_G, running_sum_B;
	Red_tot = Green_tot = Blue_tot = 0;
	//Variable to store bin size for each color
	int Rbin1, Rbin2, Rbin3, Rbin4, Gbin1, Gbin2, Gbin3, Gbin4, Bbin1, Bbin2, Bbin3, Bbin4;
	Rbin1 = Rbin2 = Rbin3 = Rbin4 = 0;
	Gbin1 = Gbin2 = Gbin3 = Gbin4 = 0;
	Bbin1 = Bbin2 = Bbin3 = Bbin4 = 0;
	running_sum_R = running_sum_G = running_sum_B = 0;
	//Integrating
	for(i=0; i<256; i++) {
		Red_tot = Red_tot + Red_Count[i];
		Green_tot = Green_tot + Green_Count[i];
		Blue_tot = Blue_tot + Blue_Count[i];
		}
	//Identifying Bins
	for(i=0; i<256; i++) {
		running_sum_R = running_sum_R + Red_Count[i];
		running_sum_G = running_sum_G + Green_Count[i];
		running_sum_B = running_sum_B + Blue_Count[i];
		if((running_sum_R>(Red_tot/4))&&(Rbin1==0)) 
			Rbin1 = i;
		if((running_sum_G>(Green_tot/4))&&(Gbin1==0)) 
			Gbin1 = i;
		if((running_sum_B>(Blue_tot/4))&&(Bbin1==0)) 
			Bbin1 = i;

		if((running_sum_R>(Red_tot/2))&&(Rbin2==0)) 
			Rbin2 = i;
		if((running_sum_G>(Green_tot/2))&&(Gbin2==0)) 
			Gbin2 = i;
		if((running_sum_B>(Blue_tot/2))&&(Bbin2==0)) 
			Bbin2 = i;

		if((running_sum_R>(3*Red_tot/4))&&(Rbin3==0)) 
			Rbin3 = i;
		if((running_sum_G>(3*Green_tot/4))&&(Gbin3==0)) 
			Gbin3 = i;
		if((running_sum_B>(3*Blue_tot/4))&&(Bbin3==0)) 
			Bbin3 = i;

		if((running_sum_R==Red_tot)&&(Rbin4==0)) 
			Rbin4 = i;
		if((running_sum_G==Green_tot)&&(Gbin4==0)) 
			Gbin4 = i;
		if((running_sum_B==Blue_tot)&&(Bbin4==0)) 
			Bbin4 = i;
		}
	cout<<"Identified Bins"<<endl;
	cout<<"Red : "<<Rbin1<<", "<<Rbin2<<", "<<Rbin3<<", "<<Rbin4<<endl;
	cout<<"Green : "<<Gbin1<<", "<<Gbin2<<", "<<Gbin3<<", "<<Gbin4<<endl;
	cout<<"Blue : "<<Bbin1<<", "<<Bbin2<<", "<<Bbin3<<", "<<Bbin4<<endl;
	//Now Quantizing values within each bin size to the average bin value
	int avgrb1, avgrb2, avgrb3, avgrb4, avggb1, avggb2, avggb3, avggb4, avgbb1, avgbb2, avgbb3, avgbb4;
	//Red	
	avgrb1 = Rbin1/2;
	avgrb2 = ((Rbin2-Rbin1)/2) + Rbin1;
	avgrb3 = ((Rbin3-Rbin2)/2) + Rbin2;
	avgrb4 = ((Rbin4-Rbin3)/2) + Rbin3;
	//Green
	avggb1 = Gbin1/2;
	avggb2 = ((Gbin2-Gbin1)/2) + Gbin1;
	avggb3 = ((Gbin3-Gbin2)/2) + Gbin2;
	avggb4 = ((Gbin4-Gbin3)/2) + Gbin3;
	//Blue
	avgbb1 = Bbin1/2;
	avgbb2 = ((Bbin2-Bbin1)/2) + Bbin1;
	avgbb3 = ((Bbin3-Bbin2)/2) + Bbin2;
	avgbb4 = ((Bbin4-Bbin3)/2) + Bbin3;

	//cout<<"Avg Red : "<<avgrb1<<", "<<avgrb2<<", "<<avgrb3<<", "<<avgrb4<<endl;
	//cout<<"Avg Green : "<<avggb1<<", "<<avggb2<<", "<<avggb3<<", "<<avggb4<<endl;
	//cout<<"Avg Blue : "<<avgbb1<<", "<<avgbb2<<", "<<avgbb3<<", "<<avgbb4<<endl;
	for(i=0; i<380*275; i++) {
		I1[3*i] = (I0[3*i]<Rbin1)?avgrb1:(I0[3*i]<Rbin2)?avgrb2:(I0[3*i]<Rbin3)?avgrb3:avgrb4;
		I1[(3*i)+1] = (I0[(3*i)+1]<Gbin1)?avggb1:(I0[(3*i)+1]<Gbin2)?avggb2:(I0[(3*i)+1]<Gbin3)?avggb3:avggb4;
		I1[(3*i)+2] = (I0[(3*i)+2]<Bbin1)?avgbb1:(I0[(3*i)+2]<Bbin2)?avgbb2:(I0[(3*i)+2]<Bbin3)?avgbb3:avgbb4;
		}
	cout<<"Quantized and Writing..."<<endl;
	//Writing image with reduced color set
	fwrite(I1,1,380*275*3,ReducedColorptr);
//************************************************************************************************************//
	//Now Working on "oil painting" effect
	cout<<"Working on 'Oil Painting' Effect..."<<endl;
	int N; 	//Block size considered for neighborhood
	N = 3;
	int newImgR[275+(2*N)][380+(2*N)], newImgG[275+(2*N)][380+(2*N)], newImgB[275+(2*N)][380+(2*N)], k, l, m;
	k = 0;
	for(i=N; i<275+N; i++) {
		for(j=N; j<380+N; j++) {
			newImgR[i][j] = I1[k++];
			newImgG[i][j] = I1[k++];
			newImgB[i][j] = I1[k++];
			}
		}
	//Performing Mirroring
	for(i=0; i<380; i++) {
		for(j=0; j<N; j++) {
			newImgR[N-1-j][i+N] = newImgR[N+j+1][i+N]; 
			newImgG[N-1-j][i+N] = newImgG[N+j+1][i+N];
			newImgB[N-1-j][i+N] = newImgB[N+j+1][i+N];
			newImgR[275+N+j][i+N] = newImgR[275-j][i+N]; 
			newImgG[275+N+j][i+N] = newImgG[275-j][i+N];
			newImgB[275+N+j][i+N] = newImgB[275-j][i+N];
			}
		}
	for(i=0; i<275+(2*N); i++) {
		for(j=0; j<N; j++) {
			newImgR[i][N-j-1] = newImgR[i][N+j+1]; 
			newImgG[i][N-j-1] = newImgG[i][N+j+1];
			newImgB[i][N-j-1] = newImgB[i][N+j+1];
			newImgR[i][380+N+j] = newImgR[i][380-j]; 
			newImgG[i][380+N+j] = newImgG[i][380-j];
			newImgB[i][380+N+j] = newImgB[i][380-j];
			}
		}
	cout<<"Mirroring done!"<<endl;
	//Now to find the neighbors for each point
	m =0;
	int RCount[256], GCount[256], BCount[256], max, freq;
	int FinalImageR[275][380], FinalImageG[275][380], FinalImageB[275][380];
	int Rneighbors[N*N], Gneighbors[N*N], Bneighbors[N*N], commonColorCount[N*N];
	for(i=N; i<275+N; i++) {		
		for(j=N; j<380+N; j++) {		//For every coordinate
			m = 0;
			for(k=-1*(N-1)/2; k<(N+1)/2; k++) {	//finding the N*N neighborhood
				for(l=-1*(N-1)/2; l<(N+1)/2; l++) {
					Rneighbors[m] = newImgR[i+k][j+l];
					Gneighbors[m] = newImgG[i+k][j+l];
					Bneighbors[m] = newImgB[i+k][j+l];
					++m;
					}
				}
			//cout<<"Neighbors found"<<endl;
			//We now have the N*N nearest neighbors
			//Got to find the most frequently occuring number
			for(k=0; k<N*N; k++) 
				commonColorCount[k] = 0;
			for(m=0; m<N*N; m++) {
				for(l=m+1; l<N*N; l++) {
					if((Rneighbors[l]==Rneighbors[m])&&(Gneighbors[l]==Gneighbors[m])&&(Bneighbors[l]==Bneighbors[m]))
						commonColorCount[l]++;
					}
				}
			max = 0;	
			for(int k=0; k<N*N; k++) {
				if(max<commonColorCount[k]) {
    					max = commonColorCount[k];
					freq = k;
					}					
				}
			FinalImageR[i-N][j-N] = Rneighbors[freq];
			FinalImageG[i-N][j-N] = Gneighbors[freq];
			FinalImageB[i-N][j-N] = Bneighbors[freq];
			}
		}
	k = 0;
	for(i=0; i<275; i++) {
		for(j=0; j<380; j++) {
			I2[k++] = FinalImageR[i][j];
			I2[k++] = FinalImageG[i][j];
			I2[k++] = FinalImageB[i][j]; 
			}
		}

	fwrite(I2,1,380*275*3,OilPaintptr);
	return 0;
	}	
