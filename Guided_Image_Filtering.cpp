/*This program performs Guided Image Filtering on the noisy peppers image. The variables used are :
	InputImage, GuidanceImage, OutputImage = Pointers to input, guiding and output images
	MirroredInput[][], MirroredGuidance[][] = 2D arrays to store input image and guding image. Its dimension
		is greater than image size to accomodate mirroring
	WindowSize = Size of Window being used
	abcount[] = Array storing how many 'a' and 'b' parameters have been computed for each pixel
	aVals[][], bVals[][] = 2D array storing the abcount[] parameters of each pixel
	MeanP = Mean of the windowed input image
	MeanI = Mean of the windowed guiding image
	CorrI = Correlation of windowed elements of the guiding image
	CorrIP = Cross-correlation of windowed elements of guiding and input image
	VarI = Variance of windowed elements of guiding image
	CovarIP = CoVariance of windowed elements of guiding image and input image
	epsilon = Regularization factor
	aMean[] = Stores the average of aVals[][] once all parameters have been computed
	bMean[] = Stores the average of bVals[][] once all parameters have been computed
  The dimension of the input image used is 512 x 512
*/
#include<iostream>
#include<fstream>
#include<cmath>	
int abcount[512*512];
float aVals[512*512][81], bVals[512*512][81]; //The dimension "81" is because window size of 9 is used
					      // => Each pixel will have 9^2 a and b values
float MirroredInput[530][530], MirroredGuidance[530][530]; //Increased size considering window size = 9

using namespace std;

float aMean[512*512],bMean[512*512];
unsigned char *InputImage = new unsigned char[512*512*3];		//Referenced as P
unsigned char *GuidanceImage = new unsigned char[512*512*3];		//Referenced as I
unsigned char *OutputImage = new unsigned char[512*512*3];		//Referenced as Q

void ComputeParameters(int channel, int N) {
	int i,j,k,l,index;
	k = channel;
	for(i=0; i<512; i++) {	
		for(j=0; j<512; j++) {	//Considering only the k-th channel
			//cout<<i<<":"<<j<<endl;	
			::MirroredInput[i+N][j+N] = float(::InputImage[k])/255.0;	//normalizing
			::MirroredGuidance[i+N][j+N] = float(::GuidanceImage[k])/255.0;
			k = k+3;
			::abcount[(i*512)+j] = 0;
			}
		}
//	cout<<"Copied "<<channel<<"-th channel"<<endl;
//Performing mirroring!!	
	for(i=0; i<512; i++) {
		for(j=0; j<N; j++) {
			::MirroredInput[N-1-j][i+N] = ::MirroredInput[N+j+1][i+N]; 
			::MirroredInput[512+N+j][i+N] = ::MirroredInput[512-j][i+N]; 
			::MirroredGuidance[N-1-j][i+N] = ::MirroredGuidance[N+j+1][i+N]; 
			::MirroredGuidance[512+N+j][i+N] = ::MirroredGuidance[512-j][i+N]; 
			}
		}
	for(i=0; i<512+(2*N); i++) {
		for(j=0; j<N; j++) {
			::MirroredInput[i][N-j-1] = ::MirroredInput[i][N+j+1]; 
			::MirroredInput[i][512+N+j] = ::MirroredInput[i][512-j];
			::MirroredGuidance[i][N-j-1] = ::MirroredGuidance[i][N+j+1]; 
			::MirroredGuidance[i][512+N+j] = ::MirroredGuidance[i][512-j];
			}
		}
//Mirroring Done!!
//	cout<<"Mirroring Done"<<endl;
	float MeanI, MeanP, CorrI, CorrIP, VarI, CovarIP, epsilon,a,b;
	epsilon = 0.01;
	for(i=N; i<512+N; i++) {
		for(j=N; j<512+N; j++) {
			MeanI = MeanP = CorrI = CorrIP = 0.0; 
			for(k=0; k<N; k++) {
				for(l=0; l<N; l++) {
					MeanI = MeanI + ::MirroredGuidance[i+k-(N/2)][j+l-(N/2)];
					MeanP = MeanP + ::MirroredInput[i+k-(N/2)][j+l-(N/2)];
					CorrI = CorrI + (::MirroredGuidance[i+k-(N/2)][j+l-(N/2)]*::MirroredGuidance[i+k-(N/2)][j+l-(N/2)]);
					CorrIP = CorrIP + (::MirroredGuidance[i+k-(N/2)][j+l-(N/2)]*::MirroredInput[i+k-(N/2)][j+l-(N/2)]);
					}
				}
			MeanI = MeanI/pow(N,2);
			MeanP = MeanP/pow(N,2);
			CorrI = CorrI/pow(N,2);
			CorrIP = CorrIP/pow(N,2);
			VarI = CorrI - pow(MeanI,2);
			CovarIP = CorrIP - (MeanI*MeanP);
			a = CovarIP/(VarI+epsilon);			
			b = MeanP - a*MeanI;		
			//cout<<"a = "<<a<<", b = "<<b<<", MeanP = "<<MeanP<<", VarI = "<<VarI<<endl;			
			for(k=0; k<N; k++) {
				for(l=0; l<N; l++) {
					index = (i-N)*512 + (j-N);
					::aVals[index][::abcount[index]] = a;
					::bVals[index][::abcount[index]] = b;
					::abcount[index]++;
					}
				}
			}
		}
	//Now Computing the Average 'a' and 'b' parameters
//	cout<<"Found aVals and bVals"<<endl;
	float aSum, bSum;
	for(i=0; i<512*512; i++) {
		aSum = bSum = 0.0;
		for(j=0; j<pow(N,2); j++) {
			aSum = aSum + ::aVals[i][j];
			bSum = bSum + ::bVals[i][j];
			}
		::aMean[i] = aSum/pow(N,2);
		//cout<<::aMean[i]<<endl;
		::bMean[i] = bSum/pow(N,2);
		}
//	cout<<"Found mean a and b"<<endl;
	}		

void ComputeNewImage(int channel, int N) {
	int i,j,k,l;
	float Output;
	k = channel;
	//Now to compute the output image
	for(i=0; i<512; i++) {
		for(j=0; j<512; j++) { 
			Output = float(::aMean[(i*512)+j]*(float(::InputImage[k])/255.0) + ::bMean[(i*512)+j]);
			::InputImage[k] = Output*255.0;
			k = k+3;
			}
		}
//	cout<<"Found output of "<<channel<<"-th channel"<<endl;
	}


int main() {
	FILE *f1 = fopen("peppers.raw","rb");
	FILE *f2 = fopen("peppers.raw","rb");
	FILE *f3 = fopen("filtered_peppers.raw","wb");
	int i,j,k, WindowSize;
	k = 4;		//(k+1) is the number of iteration of the guided filter
	WindowSize = 9;
	fread(::InputImage,1,512*512*3,f1);
	fread(::GuidanceImage,1,512*512*3,f2);    //Using input image as guiding image as well
//	cout<<"Read Input and Guidance Images"<<endl;	
	//Computing for Red Channel
P:	ComputeParameters(0,WindowSize);
//	cout<<"done 1"<<endl;
	ComputeNewImage(0,WindowSize);
	//Computing for Green Channel
	ComputeParameters(1,WindowSize);
	ComputeNewImage(1,WindowSize);
	//Computing for Blue Channel
	ComputeParameters(2,WindowSize);
	ComputeNewImage(2,WindowSize);
	
	while(k>0) {
		--k;
		goto P;
		}
	k = 0;
	for(i=0; i<512*512*3; i++) {
		OutputImage[k] = int(InputImage[k]);
		k++;
		}
	fwrite(OutputImage,1,512*512*3,f3);
	fclose(f1);
	fclose(f2);
	fclose(f3);
		
  return 0;
	}

