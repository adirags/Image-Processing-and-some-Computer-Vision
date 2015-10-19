/*This program applies Median Filtering and Bilateral Filtering on the noisy peppers image
  to get a filtered output. The variables used are :
	P, newP = Pointers to input and output images
	N = Window Dimension used
	RedChannel[][], GreenChannel[][], BlueChannel[][] = 2D matrices storing channel intensities of input image
	newRedChannel[][], newGreenChannel[][], newBlueChannel[][] = 2D matrices storing intensities of image after median filtering
	finalRedChannel[][], finalGreenChannel[][], finalBlueChannel[][] = 2D matrices storing intensities of image after bilateral filtering
	Rneighbors[], Gneighbors[], Bneighbors[] = Stores neighboring intensity values of the windows pixels
	cWeight[], sWeight[] = Closeness and Similarity function respectively
	sumR, sumG, sumB = Used to store computed pixel after bilateral filtering	
  The dimension of the image is 512 x 512 x 3
*/
#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;

int main() {
	FILE *readptr = fopen("peppers_noisy.raw","rb");
	FILE *writeptr = fopen("median_bilateral_denoised_peppers.raw","wb");
	unsigned char *P = new unsigned char[512*512*3];
	unsigned char *newP = new unsigned char[512*512*3];
	int i,j,k,N,m,n;
	N = 5;		//Window Dimension
	int RedChannel[512+2*N][512+2*N], GreenChannel[512+2*N][512+2*N], BlueChannel[512+2*N][512+2*N];
	static int newRedChannel[522][522], newGreenChannel[522][522], newBlueChannel[522][522];
	int finalRedChannel[512][512], finalGreenChannel[512][512], finalBlueChannel[512][512];
	k = 0;	
	fread(P,1,512*512*3,readptr);
	for(i=N; i<512+N; i++) {
		for(j=N; j<512+N; j++) {
			RedChannel[i][j] = P[k++];
			GreenChannel[i][j] = P[k++];
			BlueChannel[i][j] = P[k++];
			}
		}
	cout<<"Read Channels"<<endl;
	//Performing mirroring!!	
	for(i=0; i<512; i++) {
		for(j=0; j<N; j++) {
			RedChannel[N-1-j][i+N] = RedChannel[N+j+1][i+N]; 
			RedChannel[512+N+j][i+N] = RedChannel[512-j][i+N];
			GreenChannel[N-1-j][i+N] = GreenChannel[N+j+1][i+N]; 
			GreenChannel[512+N+j][i+N] = GreenChannel[512-j][i+N];
			BlueChannel[N-1-j][i+N] = BlueChannel[N+j+1][i+N]; 
			BlueChannel[512+N+j][i+N] = BlueChannel[512-j][i+N]; 
			}
		}
	for(i=0; i<512+(2*N); i++) {
		for(j=0; j<N; j++) {
			RedChannel[i][N-j-1] = RedChannel[i][N+j+1]; 
			RedChannel[i][512+N+j] = RedChannel[i][512-j];
			GreenChannel[i][N-j-1] = GreenChannel[i][N+j+1]; 
			GreenChannel[i][512+N+j] = GreenChannel[i][512-j];
			BlueChannel[i][N-j-1] = BlueChannel[i][N+j+1]; 
			BlueChannel[i][512+N+j] = BlueChannel[i][512-j];
			}
		}
	//Mirroring Done!!
	cout<<"Mirroring Done"<<endl;
	// First removing Salt and Pepper noise
	int Rneighbors[N*N], Gneighbors[N*N], Bneighbors[N*N], temp;
	for(i=N; i<512+N; i++) {
		for(j=N; j<512+N; j++) {
			k = 0;
			for(m=0; m<N; m++) {
				for(n=0; n<N; n++) {
					Rneighbors[k] = RedChannel[i+m-(N/2)][j+n-(N/2)];
					Gneighbors[k] = GreenChannel[i+m-(N/2)][j+n-(N/2)];
					Bneighbors[k] = BlueChannel[i+m-(N/2)][j+n-(N/2)];
					++k;
					}
				}
		//	cout<<"found neighbors"<<endl;
			for(m=0; m<(N*N)-1; m++) { 		//Sorting
				for(n=m+1; n<N*N; n++) {
					if(Rneighbors[m]>Rneighbors[n]) {
						temp = Rneighbors[m];
						Rneighbors[m] = Rneighbors[n];
						Rneighbors[n] = temp;
						}
					if(Gneighbors[m]>Gneighbors[n]) {
						temp = Gneighbors[m];
						Gneighbors[m] = Gneighbors[n];
						Gneighbors[n] = temp;
						}
					if(Bneighbors[m]>Bneighbors[n]) {
						temp = Bneighbors[m];
						Bneighbors[m] = Bneighbors[n];
						Bneighbors[n] = temp;
						}
					}
				}
			//cout<<i<<":"<<j<<endl;
			newRedChannel[i][j] = Rneighbors[((N*N)/2)+1];		//Assigning Median Value
			newGreenChannel[i][j] = Gneighbors[((N*N)/2)+1];
			newBlueChannel[i][j] = Bneighbors[((N*N)/2)+1];
			}
		}
	cout<<"Done with Median filtering"<<endl;
	k = 0;
	//Done with Median filtering!!
	//Now time for Bilateral filtering
	//First performing mirroring on new Channels	
	for(i=0; i<512; i++) {
		for(j=0; j<N; j++) {
			newRedChannel[N-1-j][i+N] = newRedChannel[N+j+1][i+N]; 
			newRedChannel[512+N+j][i+N] = newRedChannel[512-j][i+N];
			newGreenChannel[N-1-j][i+N] = newGreenChannel[N+j+1][i+N]; 
			newGreenChannel[512+N+j][i+N] = newGreenChannel[512-j][i+N];
			newBlueChannel[N-1-j][i+N] = newBlueChannel[N+j+1][i+N]; 
			newBlueChannel[512+N+j][i+N] = newBlueChannel[512-j][i+N]; 
			}
		}
	for(i=0; i<512+(2*N); i++) {
		for(j=0; j<N; j++) {
			newRedChannel[i][N-j-1] = newRedChannel[i][N+j+1]; 
			newRedChannel[i][512+N+j] = newRedChannel[i][512-j];
			newGreenChannel[i][N-j-1] = newGreenChannel[i][N+j+1]; 
			newGreenChannel[i][512+N+j] = newGreenChannel[i][512-j];
			newBlueChannel[i][N-j-1] = newBlueChannel[i][N+j+1]; 
			newBlueChannel[i][512+N+j] = newBlueChannel[i][512-j];
			}
		}
	//Mirroring Done!!
	float cWeight[N*N], sWeight[N*N]; //Weights for closeness and similarity functions
	float sigmaD2, sigmaR2, num, weight; //Geometric and Photometric spread respectively
	sigmaD2 = 1.0;
	sigmaR2 = 300.0;
	float sumR, sumG, sumB, test, Wsum;

	for(i=N; i<512+N; i++) {
		for(j=N; j<512+N; j++) {
			k = 0;
			sumR = sumG = sumB = 0.0; test = 0.0;
			Wsum = 0.0;
			for(m=0; m<N; m++) {
				for(n=0; n<N; n++) {
					Rneighbors[k] = newRedChannel[i+m-(N/2)][j+n-(N/2)];
					Gneighbors[k] = newGreenChannel[i+m-(N/2)][j+n-(N/2)];
					Bneighbors[k] = newBlueChannel[i+m-(N/2)][j+n-(N/2)];
					num = sqrt(pow(m-N/2,2)+pow(n-N/2,2)); //for calculating closeness function
					cWeight[k] = exp(-0.5*(pow(num,2)/sigmaD2))/(2*3.14*sigmaD2);
					num = sqrt(pow(Rneighbors[k]-RedChannel[i][j],2) + pow(Gneighbors[k]-GreenChannel[i][j],2) + pow(Bneighbors[k]-BlueChannel[i][j],2));	//for calculating similarity function
					sWeight[k] = exp(-0.5*(pow(num,2)/sigmaR2))/(2*3.14*sigmaR2);
					weight = (cWeight[k]*sWeight[k]);
					Wsum = Wsum + weight;										
					k++;
					}
				}
			k = 0;
			for(m=0; m<N; m++) {
				for(n=0; n<N; n++) {			
					//Scaling weights
					weight = (cWeight[k]*sWeight[k]);
					weight = weight/Wsum;
					//test = test + weight;
					//cout<<weight<<endl;
					sumR = sumR + weight*newRedChannel[i+m-(N/2)][j+n-(N/2)];
					sumG = sumG + weight*newGreenChannel[i+m-(N/2)][j+n-(N/2)];
					sumB = sumB + weight*newBlueChannel[i+m-(N/2)][j+n-(N/2)];
					k++;
					}
				}
			finalRedChannel[i-N][j-N] = int(sumR);
			finalGreenChannel[i-N][j-N] = int(sumG);
			finalBlueChannel[i-N][j-N] = int(sumB);
			}
		}
	cout<<"Done with Bilateral Filtering"<<endl;
	k = 0;
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) {
			newP[k++] = finalRedChannel[i][j];
			newP[k++] = finalGreenChannel[i][j];
			newP[k++] = finalBlueChannel[i][j];
		}
	}
	fwrite(newP,1,512*512*3,writeptr);
	return 0;
	}
