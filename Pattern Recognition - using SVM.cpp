/*Using SVM to claasify an image as either a grass or straw*/

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"

using namespace cv;
using namespace std;

double meanFeature[2];			//Stores mean value of reduced feature	
double featureVariance[2];		//Stors variance of each class

void ComputeClasses() {
	int i,j,k,l,m,n,windowSize,localAverage;
	float filterSum, fsum;
	windowSize = 5;
	FILE *f;
	unsigned char *inputImage = new unsigned char[128*128];
	int mirroredImage[132][132];	//Increased size since we shall be performing mirroring
	int preprocessedImage[132][132];	//To store the zero mean image
	float lawFilteredImage[25][128][128];	//To compute the outputs of the 25 Law's filters
	float energyLawsFilteredImage[25][128][128]; //To store the energy of each pixel after applying filtering
	float features[72][25];		//Stores the computed feature vector for each image
	string fileName;
	ostringstream name;
	//First computing the 25 Law's filter coeffecients
	//We have
	float lawsFilterCoeff[5][5][25];
	float oneDimFilterCoeff[5][5] ={  {1.0,4.0,6.0,4.0,1.0},//Level Filter
			{-1.0,-2.0,0.0,2.0,1.0},	  //Edge Filter
			{-1.0,0.0,2.0,0.0,-1.0},	  //Spot Filter
			{-1.0,2.0,0.0,-2.0,1.0},	  //Wave Filter
			{1.0,-4.0,6.0,-4.0,1.0}  }; 	  //Ripple Filter
	for(i=0; i<25; i++) {
		l = 0;
		for(j=0; j<5; j++) {
			for(k=0; k<5; k++) {
				lawsFilterCoeff[j][k][i] = oneDimFilterCoeff[i/5][k]*oneDimFilterCoeff[i%5][l]/256.0;//Dividing for normalizing
				}
			l++;
			}
		}

//*************EXTRACTING FEATURES OF GRASS IMAGES*******************

	for(i=1; i<37; i++) {		
		name.str("");
		if(i<10)
			name<<"grass_0"<<i<<".raw";
		else
			name<<"grass_"<<i<<".raw";
		fileName = name.str();
		f = fopen(fileName.c_str(),"rb");
		fread(inputImage,1,128*128,f);
		//cout<<"Read "<<fileName<<endl;
		fclose(f);
		l = 0;
		//Performing Pre-processing which includes subtracting local mean
		//so as to make it a zero mean random variable => Variance = Energy
		for(j=windowSize/2; j<128 + windowSize/2; j++) {
			for(k=windowSize/2; k<128 + windowSize/2; k++) {
				mirroredImage[j][k] = inputImage[l++];
				}
			}
		//First performing mirroring
		for(j=0; j<128; j++) {
			for(k=1; k<=windowSize/2; k++) {
				mirroredImage[(windowSize/2)-k][j+(windowSize/2)] = mirroredImage[(windowSize/2)+k][j+(windowSize/2)];
				mirroredImage[128+(windowSize/2)+k-1][j+(windowSize/2)] = mirroredImage[128+(windowSize/2)-k][j+(windowSize/2)];
				}
			}
		for(j=0; j<132; j++) {
			for(k=1; k<=windowSize/2; k++) {
				mirroredImage[j][(windowSize/2)-k] = mirroredImage[j][(windowSize/2)+k];
				mirroredImage[j][128+(windowSize/2)+k-1] = mirroredImage[j][128+(windowSize/2)-k];
				}
			}
		//cout<<"Did mirroring!"<<endl;
		//Now we shall subtract the local mean from the centre pixel intensity
		for(j=windowSize/2; j<128+(windowSize/2); j++) {
			for(k=windowSize/2; k<128+(windowSize/2); k++) {
				localAverage = 0;
				for(l=-1*(windowSize/2); l<=windowSize/2; l++)
					for(m=-1*(windowSize/2); m<=windowSize/2; m++)
						localAverage += mirroredImage[j+l][k+m];
				localAverage = localAverage/25;
				preprocessedImage[j][k] = mirroredImage[j][k] - localAverage;
				}
			}
		//Doing mirroring to this zero mean random variable
		for(j=0; j<128; j++) {
                        for(k=1; k<=windowSize/2; k++) {
                                preprocessedImage[(windowSize/2)-k][j+(windowSize/2)] = preprocessedImage[(windowSize/2)+k][j+(windowSize/2)];
                                preprocessedImage[128+(windowSize/2)+k-1][j+(windowSize/2)] = preprocessedImage[128+(windowSize/2)-k][j+(windowSize/2)];
                                }
                        }
                for(j=0; j<132; j++) {
                        for(k=1; k<=windowSize/2; k++) {
                                preprocessedImage[j][(windowSize/2)-k] = preprocessedImage[j][(windowSize/2)+k];
                                preprocessedImage[j][128+(windowSize/2)+k-1] = preprocessedImage[j][128+(windowSize/2)-k];
                                }
                        }

		//cout<<"Preprocessed the Image!!!"<<endl;
		for(j=0; j<25; j++)
			features[i-1][j] = 0;
		//Applying Law's filters now
		for(j=0; j<25; j++) {
			for(k=windowSize/2; k<(128+windowSize/2); k++) {
				for(l=windowSize/2; l<(128+windowSize/2); l++) {
					filterSum = 0.0;
					for(m=0; m<5; m++)
						for(n=0; n<5; n++)
							filterSum += float(preprocessedImage[k-(windowSize/2)+m][l-(windowSize/2)+n])*lawsFilterCoeff[m][n][j];
					lawFilteredImage[j][k-windowSize/2][l-windowSize/2] = filterSum;
					energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2] = pow(filterSum,2);
					features[i-1][j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
		}

//***********EXTRACTING FEATURES FOR STRAW IMAGES*************************

	//Now for the Straw images	
	for(i=1; i<37; i++) {		
		name.str("");
		if(i<10)
			name<<"straw_0"<<i<<".raw";
		else
			name<<"straw_"<<i<<".raw";
		fileName = name.str();
		f = fopen(fileName.c_str(),"rb");
		fread(inputImage,1,128*128,f);
		//cout<<"Read "<<fileName<<endl;
		fclose(f);
		l = 0;
		//Performing Pre-processing which includes subtracting local mean
		//so as to make it a zero mean random variable => Variance = Energy
		for(j=windowSize/2; j<128 + windowSize/2; j++) {
			for(k=windowSize/2; k<128 + windowSize/2; k++) {
				mirroredImage[j][k] = inputImage[l++];
				}
			}
		//First performing mirroring
		for(j=0; j<128; j++) {
			for(k=1; k<=windowSize/2; k++) {
				mirroredImage[(windowSize/2)-k][j+(windowSize/2)] = mirroredImage[(windowSize/2)+k][j+(windowSize/2)];
				mirroredImage[128+(windowSize/2)+k-1][j+(windowSize/2)] = mirroredImage[128+(windowSize/2)-k][j+(windowSize/2)];
				}
			}
		for(j=0; j<132; j++) {
			for(k=1; k<=windowSize/2; k++) {
				mirroredImage[j][(windowSize/2)-k] = mirroredImage[j][(windowSize/2)+k];
				mirroredImage[j][128+(windowSize/2)+k-1] = mirroredImage[j][128+(windowSize/2)-k];
				}
			}
		//cout<<"Did mirroring!"<<endl;
		//Now we shall subtract the local mean from the centre pixel intensity
		for(j=windowSize/2; j<128+(windowSize/2); j++) {
			for(k=windowSize/2; k<128+(windowSize/2); k++) {
				localAverage = 0;
				for(l=-1*(windowSize/2); l<=windowSize/2; l++)
					for(m=-1*(windowSize/2); m<=windowSize/2; m++)
						localAverage += mirroredImage[j+l][k+m];
				localAverage = localAverage/25;
				preprocessedImage[j][k] = mirroredImage[j][k] - localAverage;
				}
			}
		//Doing mirroring to this zero mean random variable
		for(j=0; j<128; j++) {
                        for(k=1; k<=windowSize/2; k++) {
                                preprocessedImage[(windowSize/2)-k][j+(windowSize/2)] = preprocessedImage[(windowSize/2)+k][j+(windowSize/2)];
                                preprocessedImage[128+(windowSize/2)+k-1][j+(windowSize/2)] = preprocessedImage[128+(windowSize/2)-k][j+(windowSize/2)];
                                }
                        }
                for(j=0; j<132; j++) {
                        for(k=1; k<=windowSize/2; k++) {
                                preprocessedImage[j][(windowSize/2)-k] = preprocessedImage[j][(windowSize/2)+k];
                                preprocessedImage[j][128+(windowSize/2)+k-1] = preprocessedImage[j][128+(windowSize/2)-k];
                                }
                        }

		//cout<<"Preprocessed the Image!!!"<<endl;
		for(j=0; j<25; j++)
			features[36+i-1][j] = 0;
		//Applying Law's filters now
		for(j=0; j<25; j++) {
			for(k=windowSize/2; k<(128+windowSize/2); k++) {
				for(l=windowSize/2; l<(128+windowSize/2); l++) {
					filterSum = 0.0;
					for(m=0; m<5; m++)
						for(n=0; n<5; n++)
							filterSum += float(preprocessedImage[k-(windowSize/2)+m][l-(windowSize/2)+n])*lawsFilterCoeff[m][n][j];
					lawFilteredImage[j][k-windowSize/2][l-windowSize/2] = filterSum;
					energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2] = pow(filterSum,2);
					features[36+i-1][j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
		}
	int labels[72];
	for(j=0; j<36; j++) {
		labels[j] = 0;
		labels[j+36] = 1;
		}
		
	Mat A = Mat(72,25,CV_32FC1,&features);
	//cout<<A;
	Mat Label = Mat(72,1,CV_32SC1,&labels);
//**************SVM TRAINING******************************
	// Set up SVM's parameters
	Ptr<ml::SVM> svm = ml::SVM::create();
	svm->setType(ml::SVM::C_SVC);
	svm->setKernel(ml::SVM::LINEAR);
	svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));
	svm->train(A,ml::ROW_SAMPLE, Label);	
//*******CLASSIFYING UNKNOWN IMAGES*********************

	int decision[24];
	long double  mahalanobisDistance[2];
	unsigned char *testInputImage = new unsigned char[128*128];
	float testFeatures[24][25];		//Stores the computed feature vector for each image
	
	Mat B;
	for(i=1; i<=24; i++) {
		name.str("");
		if(i<10)
			name<<"unknown_0"<<i<<".raw";
		else
			name<<"unknown_"<<i<<".raw";
		fileName = name.str();
		f = fopen(fileName.c_str(),"rb");
		fread(inputImage,1,128*128,f);
		fclose(f);
		//Performing Pre-processing which includes subtracting local mean
		//so as to make it a zero mean random variable => Variance = Energy
		for(j=windowSize/2; j<128 + windowSize/2; j++) {
			for(k=windowSize/2; k<128 + windowSize/2; k++) {
				mirroredImage[j][k] = inputImage[l++];
				}
			}
		//First performing mirroring
		for(j=0; j<128; j++) {
			for(k=1; k<=windowSize/2; k++) {
				mirroredImage[(windowSize/2)-k][j+(windowSize/2)] = mirroredImage[(windowSize/2)+k][j+(windowSize/2)];
				mirroredImage[128+(windowSize/2)+k-1][j+(windowSize/2)] = mirroredImage[128+(windowSize/2)-k][j+(windowSize/2)];
				}
			}
		for(j=0; j<132; j++) {
			for(k=1; k<=windowSize/2; k++) {
				mirroredImage[j][(windowSize/2)-k] = mirroredImage[j][(windowSize/2)+k];
				mirroredImage[j][128+(windowSize/2)+k-1] = mirroredImage[j][128+(windowSize/2)-k];
				}
			}
		//cout<<"Did mirroring!"<<endl;
		//Now we shall subtract the local mean from the centre pixel intensity
		for(j=windowSize/2; j<128+(windowSize/2); j++) {
			for(k=windowSize/2; k<128+(windowSize/2); k++) {
				localAverage = 0;
				for(l=-1*(windowSize/2); l<=windowSize/2; l++)
					for(m=-1*(windowSize/2); m<=windowSize/2; m++)
						localAverage += mirroredImage[j+l][k+m];
				localAverage = localAverage/25;
				preprocessedImage[j][k] = mirroredImage[j][k] - localAverage;
				}
			}
		//Doing mirroring to this zero mean random variable
		for(j=0; j<128; j++) {
                        for(k=1; k<=windowSize/2; k++) {
                                preprocessedImage[(windowSize/2)-k][j+(windowSize/2)] = preprocessedImage[(windowSize/2)+k][j+(windowSize/2)];
                                preprocessedImage[128+(windowSize/2)+k-1][j+(windowSize/2)] = preprocessedImage[128+(windowSize/2)-k][j+(windowSize/2)];
                                }
                        }
                for(j=0; j<132; j++) {
                        for(k=1; k<=windowSize/2; k++) {
                                preprocessedImage[j][(windowSize/2)-k] = preprocessedImage[j][(windowSize/2)+k];
                                preprocessedImage[j][128+(windowSize/2)+k-1] = preprocessedImage[j][128+(windowSize/2)-k];
                                }
                        }

		//cout<<"Preprocessed the Image!!!"<<endl;
		for(j=0; j<25; j++)
			testFeatures[i-1][j] = 0;
		//Applying Law's filters now
		for(j=0; j<25; j++) {
			for(k=windowSize/2; k<(128+windowSize/2); k++) {
				for(l=windowSize/2; l<(128+windowSize/2); l++) {
					filterSum = 0.0;
					for(m=0; m<5; m++)
						for(n=0; n<5; n++)
							filterSum += float(preprocessedImage[k-(windowSize/2)+m][l-(windowSize/2)+n])*lawsFilterCoeff[m][n][j];
					lawFilteredImage[j][k-windowSize/2][l-windowSize/2] = filterSum;		
					//Now we have the 25 Law's Filtered Images, we can compute the Output energy of each pixel
					energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2] = pow(filterSum,2);
					testFeatures[i-1][j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
		}
	B = Mat(24,25,CV_32FC1,&testFeatures);
	//************SVM TEST**************************************
	Mat result;
	svm->predict(B,result);
	cout<<result<<endl;
	}
	
int main() {
	ComputeClasses();
	cout<<"Done!!"<<endl;
	return 0;
	}
