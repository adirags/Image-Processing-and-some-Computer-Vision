/*Using a feature vector of Dimensionality = 25, to classify image texture. The features
  were calculated using Law's Filters. Decisions were made by considering Mahalanobis Distance
*/
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>

using namespace std;

float avgFeatures[2][25];		//Stores the average feature vector for a particular class
float sigma[2][25];			//Store the standard deviation of each feature

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
	float features[36][25];		//Stores the computed feature vector for each image
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
					//Can try scaling the output too...TRY IT!! 			
					//Now we have the 25 Law's Filtered Images, we can compute the Output energy of each pixel
					energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2] = pow(filterSum,2);
					features[i-1][j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
		//We now have the feature vector for the image.
		}
	//We now have the feature vector for all Grass images. Let's compute the average now :)
	cout<<"Feature Vector for Grass : ";
	for(i=0; i<25; i++) {
		::avgFeatures[0][i] = 0.0;
		::sigma[0][i] = 0.0;
		for(j=0; j<36; j++) {
			::avgFeatures[0][i] += features[j][i];
			::sigma[0][i] += pow(features[j][i],2);
			}
		::avgFeatures[0][i] = ::avgFeatures[0][i]/25.0;
		::sigma[0][i] = sqrt(::sigma[0][i]/25.0);
		cout<<::avgFeatures[0][i]<<",";
		//cout<<::sigma[1][i]<<",";
		}
	cout<<endl;

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
					//Can try scaling the output too...TRY IT!! 			
					//Now we have the 25 Law's Filtered Images, we can compute the Output energy of each pixel
					energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2] = pow(filterSum,2);
					features[i-1][j] += energyLawsFilteredImage[j][k-windowSize/2][l-windowSize/2];
					}
				}
			}
		//We now have the feature vector for the image.
		}
	//We now have the feature vector for all Grass images. Let's compute the average now :)
	cout<<"Feature Vector for Straw : ";
	for(i=0; i<25; i++) {
		::avgFeatures[1][i] = 0.0;
		::sigma[1][i] = 0.0;
		for(j=0; j<36; j++) {
			::avgFeatures[1][i] += features[j][i];
			::sigma[1][i] += pow(features[j][i],2);
			}
		::avgFeatures[1][i] = ::avgFeatures[1][i]/25.0;
		::sigma[1][i] = sqrt(::sigma[1][i]/25.0);
		cout<<::avgFeatures[1][i]<<",";
		//cout<<::sigma[1][i]<<",";
		}
	cout<<endl;
	}

void IdentifyUnknownClasses() {
	int i,j,k,l,m,n;	//indexes
	int windowSize,localAverage, decision[24];
	float filterSum;
	long double  mahalanobisDistance[2];
	windowSize = 5;
	FILE *f;
	unsigned char *inputImage = new unsigned char[128*128];
	int mirroredImage[132][132];	//Increased size since we shall be performing mirroring
	int preprocessedImage[132][132];	//To store the zero mean image
	float lawFilteredImage[25][128][128];	//To compute the outputs of the 25 Law's filters
	float energyLawsFilteredImage[25][128][128]; //To store the energy of each pixel after applying filtering
	float features[24][25];		//Stores the computed feature vector for each image
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
	ofstream fout("Min_Mean_square.txt");
	for(i=0; i<2; i++) {
		for(j=0; j<25; j++) 
			fout<<::avgFeatures[i][j]<<", ";
		fout<<"\n";
		}
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
			fout<<features[i-1][j]<<", ";
			}
		fout<<"\n";
		//We now have the feature vector for the image.
		//Now we shall see which class it can belong to
		//We shall use Mahalanobis Distance to compute which class the computed feature can belong to
		for(j=0;j<2;j++) {	//Checking distance with both classes
			mahalanobisDistance[j] = 0.0;
			for(k=0; k<25; k++) {
				mahalanobisDistance[j] += pow(features[i-1][k] - ::avgFeatures[j][k],2)/::sigma[j][k];
				}
			}		
		mahalanobisDistance[0] = sqrt(mahalanobisDistance[0]);
		mahalanobisDistance[1] = sqrt(mahalanobisDistance[1]);
		//cout<<i<<" MD0 = "<<mahalanobisDistance[0]<<" ; MD1 = "<<mahalanobisDistance[1]<<endl;
		decision[i-1] = (mahalanobisDistance[0]<mahalanobisDistance[1])?0:1;
		cout<<i<<". Image is of class "<<decision[i-1]<<endl;
		}	

	}
	
int main() {
	ComputeClasses();
	//Computed the Feature vectors for each class
	//Now to process the unkown images to see in which class it should belong to
	IdentifyUnknownClasses();
	return 0;
	}
