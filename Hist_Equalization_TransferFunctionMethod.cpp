/*Program to equalize an image and therby improve its contrast. Equalization of the
  image is done by computing a transfer function and using it to either strectch or
  shrink the histogram of the image. The variables used are :
	P, eq_P = Pointers to the input and output image respectively
	R_count[], G_count[], B_count[] = Used to compute histogram of input image
	newIntensityR[], newIntensityG[], newIntensityB[] = Stores the histogram distribution
	after applying the transfer function	
  Image Dimension = 512 x 512
*/
#include<iostream>
#include<fstream>

using namespace std;

int main() {
	FILE *readptr = fopen("jet.raw","rb");
	FILE *writeptr = fopen("tf_eq_jet.raw","wb");
	unsigned char *P = new unsigned char[512*512*3]; 
	unsigned char *eq_P = new unsigned char[512*512*3];
	fread(P,1,512*512*3,readptr);
	int R_count[256], G_count[256], B_count[256],i,j;
	for(i=0; i<256; i++) {				//Initializing counts to zero
		R_count[i] = G_count[i] = B_count[i] = 0;
		}
	for(i=0; i<512*512*3; i=i+3) {
		R_count[P[i]]++;
		G_count[P[i+1]]++;
		B_count[P[i+2]]++;
		}
	int R_tot[256], G_tot[256], B_tot[256];
	for(i=1; i<256; i++)	
		R_tot[i] = G_tot[i] = B_tot[i] = 0;
	R_tot[0] = R_count[0];
	G_tot[0] = G_count[0];
	B_tot[0] = B_count[0];
	for(i=1; i<256; i++) {
		R_tot[i] = R_tot[i-1] + R_count[i];
		G_tot[i] = G_tot[i-1] + G_count[i];
		B_tot[i] = B_tot[i-1] + B_count[i];
		}
	cout<<"Histogram Computed"<<endl;
	//We now have the number of occurences for each RGB component.
	//Can now use the transfer function formula to compute new pixel intensities
	int newIntensityR[256], newIntensityG[256], newIntensityB[256], R_intensity, G_intensity, B_intensity;
	for(i=0; i<256; i++) {
		newIntensityR[i] = int((float(R_tot[i])/float(R_tot[255]))*255.0);
		newIntensityG[i] = int((float(G_tot[i])/float(G_tot[255]))*255.0);
		newIntensityB[i] = int((float(B_tot[i])/float(B_tot[255]))*255.0);
		}
	for(i=0; i<512*512; i++) {
		R_intensity = P[3*i];
		G_intensity = P[(3*i)+1];
		B_intensity = P[(3*i)+2];
		eq_P[3*i] = newIntensityR[R_intensity];
		eq_P[(3*i)+1] = newIntensityG[G_intensity];
		eq_P[(3*i)+2] = newIntensityB[B_intensity];
		}
	fwrite(eq_P,1,512*512*3,writeptr);
	return 0;
}
