/*Using PCA to reduce Feature dimension from 25 to 1, and then using this
  reduced feature set to perform classification of the unlabeled images
  Need openCV to run this
*/

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

int meanFeature[2];			//Stores mean value of reduced feature	
float featureVariance[2];		//Stors variance of each class
Mat EV;					//Stores Eigenvector

void ComputeClasses() {
	int i,j,k,l,m,n,windowSize,localAverage;
	float filterSum;
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
				//cout<<lawsFilterCoeff[j][k][i]<<",";
				}
			//cout<<endl;
			l++;
			}
		//cout<<endl;
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
					//Now we have the 25 Law's Filtered Images, we can compute the Output energy of each pixel
					energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2] = pow(filterSum,2);
					features[i-1][j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
		//We now have the feature vector for the image.
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
					//Now we have the 25 Law's Filtered Images, we can compute the Output energy of each pixel
					energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2] = pow(filterSum,2);
					features[36+i-1][j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
		//We now have the feature vector for the image.
		}


	Mat A = Mat(72,25,CV_32FC1,&features);		//Assigning the 72 feature vectors computed to a Mat data type

	//*****USING PCA******************************************

	//features[][] has the unlabled vectors for the 72 datasets
	PCA pca(A, Mat(), CV_PCA_DATA_AS_ROW, 1);	//Retaining 1 feature (last parameter)
	Mat mean = pca.mean.clone();
	Mat eigenvalues = pca.eigenvalues.clone();
	Mat eigenvectors = pca.eigenvectors.clone();
	EV = eigenvectors;
	//cout<<"Eigenvector = "<<eigenvectors<<endl;
	
	Mat point; 
	float *p;
	::meanFeature[0] = ::meanFeature[1] = 0;
	::featureVariance[0] = ::featureVariance[1] = 0.0;
	ofstream fout("PCA.txt");
	for(i=0;i<72;i++) {
		point = pca.project(A.row(i));
		//cout<<i+1<<" Vector : "<<point<<endl;
		p = (float*)point.data;
		if(i<36) {
			::meanFeature[0] += *p;
			::featureVariance[0] += pow(*p,2);
			}
		else {
			::meanFeature[1] += *p;
			::featureVariance[1] += pow(*p,2);
			}
		}		
	::meanFeature[0] = ::meanFeature[0]/36;
	::meanFeature[1] = ::meanFeature[1]/36;
	::featureVariance[0] = ::featureVariance[0]/36.0;
	::featureVariance[1] = ::featureVariance[1]/36.0;
	::featureVariance[0] = ::featureVariance[0] - pow(::meanFeature[0],2);
	::featureVariance[1] = ::featureVariance[1] - pow(::meanFeature[1],2);
	cout<<"Feature of Grass : "<<::meanFeature[0]<<endl;
	cout<<"Feature of Straw : "<<::meanFeature[1]<<endl;
	fout<<::meanFeature[0]<<"\n"<<::meanFeature[1]<<"\n";
	cout<<"Variance of Grass : "<<::featureVariance[0]<<endl;
	cout<<"Variance of Straw : "<<::featureVariance[1]<<endl;
	
	//*********CLASSIFYING UNKNOWN IMAGES**********************

	int decision[24];
	long double  mahalanobisDistance[2];
	unsigned char *testInputImage = new unsigned char[128*128];
	float testFeatures[25];		//Stores the computed feature vector for each image
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
			testFeatures[j] = 0;
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
					testFeatures[j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
	
		B = Mat(1,25,CV_32FC1,&testFeatures);
		point = pca.project(B.row(0));
		p = (float*)point.data;
		fout<<*p<<"\n";
		//We now have the Reduced feature vector for the image.
		//Now we shall see which class it can belong to
		//We shall use Mahalanobis Distance to compute which class the computed feature can belong to
		for(j=0;j<2;j++) {	//Checking distance with both classes
				mahalanobisDistance[j] = pow(*p - (float)::meanFeature[j],2)/::featureVariance[j];
			}		
		
		//cout<<mahalanobisDistance[0]<<", "<<mahalanobisDistance[1]<<endl;
		mahalanobisDistance[0] = sqrt(mahalanobisDistance[0]);
		mahalanobisDistance[1] = sqrt(mahalanobisDistance[1]);
		//cout<<i<<" MD0 = "<<mahalanobisDistance[0]<<" ; MD1 = "<<mahalanobisDistance[1]<<endl;
		decision[i-1] = (mahalanobisDistance[0]<mahalanobisDistance[1])?0:1;
		cout<<i<<". Image is of class "<<decision[i-1]<<endl;
		
		}		

	}
	
int main() {
	ComputeClasses();
	return 0;
	}
