#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "lbp.hpp"

#include <iostream>
#include <cstring>
#include <climits>

#define ERROR_READING_IMG 1
#define INCORRECT_USAGE 2
#define INVALID_METHOD 3

using namespace cv;
using namespace std;
using namespace libfacerec;

static void error(int errorCode, char* progName = NULL)
{
	switch(errorCode){
		case ERROR_READING_IMG:
			cout
        		<<  "ERROR: file could not be opened." << endl;
			break;
		case INCORRECT_USAGE:
			cout << endl
		        <<  "Program description. Options." << endl
		        <<  "Usage:" << endl
		        <<  progName << " <image_name> <k-value> ( lbp <radius> <n> | fourier <n>)" << endl;
			break;
		case INVALID_METHOD:
			cout << endl
		        <<  "ERROR: Invalid method or value for k." << endl;
			break;
	}
}

Mat lbpHist(Mat I, int radius, int n);
void findKNearest(int k, const char* filename, const char* method, int radius, int n);
Mat prepareFile(const char* filename);
long long calc_distance(Mat hist1, Mat hist2);

int main(int argc, char ** argv)
{
	/*usage*/
	if (argc < 4){
		error(INCORRECT_USAGE, argv[0]);
		return 0;
	}

	/*read file*/
    const char* filename = argv[1];
    Mat I = prepareFile(filename);
    if(I.empty()) return 0;

    const int k = atoi(argv[2]);
    const char* method = argv[3];

    /*check what the chosen method is*/
    if(strcmp(method, "lbp")==0 && k > 0){
        const int radius = atoi(argv[4]);
        const int n = atoi(argv[5]);
        Mat hist = lbpHist(I, radius, n);    
        findKNearest(k, filename, method, radius, n);
    }else if(strcmp(method, "fourier")==0 && k > 0){
        /*find edges*/
        Mat edges;
        Canny(I, edges, 1, 100);
        imshow("Edges", edges);
        waitKey();    
    }else{ 
    	error(INVALID_METHOD);
    }

    /*Mat padded;                            //expand input image to optimal size
    int m = getOptimalDFTSize( I.rows );
    int n = getOptimalDFTSize( I.cols ); // on the border add zero values
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

    dft(complexI, complexI);            // this way the result may fit in the source matrix

    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat magI = planes[0];

    magI += Scalar::all(1);                    // switch to logarithmic scale
    log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;

    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
                                            // viewable image form (float between values 0 and 1).

    imshow("Input Image"       , I   );    // Show the result
    imshow("spectrum magnitude", magI);
    waitKey();*/

    return 0;
}

Mat prepareFile(const char* filename){
    Mat I = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
    if(I.empty()){
    	error(ERROR_READING_IMG);
        return I;
    }

    /*pre-processing: transpose all images to 400x300*/
    if(I.size().height != 300)
    	I = I.t();

    return I;
}

long long calc_distance(Mat hist1, Mat hist2){
    long long distance = 0;
    for(int i = 0; i < hist1.total(); i++){
        long long diff = cvRound(hist1.at<float>(i))-cvRound(hist2.at<float>(i));
        distance += diff*diff;
    }

    return distance;
}

void findKNearest(int k, const char* filename, const char* method, int radius, int n){
    /*getting query image number*/
    string file(filename);
    file = file.substr(0, file.length()-4);
    string needle("/");
    string::iterator found = search(file.rbegin(), file.rend(), needle.rbegin(), needle.rend()).base();
    string number = file.substr(distance(file.begin(), found), file.length());
    string filepath = file.substr(0, distance(file.begin(), found));
    int filenumber = atoi(number.c_str());
    cout << "File number: " << filenumber << endl;

    Mat img = prepareFile(filename);
    if(img.empty()) return;
    
    //calculates the histogram of the query image
    Mat hist = lbpHist(img, radius, n);

    long distances[k];
    int indexes[k];
    for(int i = 0; i < k; i++){
        distances[i] = LONG_MAX;
    }

    char* cur_number = (char*) malloc(sizeof(char)*6);
    char* path = (char*) malloc(sizeof(char)*100);
    for(int i = 1; i <= 10000; i++){
        if(i != filenumber){
            strcpy(path, filepath.c_str());
            sprintf(cur_number, "%d", i);
            char* cur_file = strcat(path, cur_number);
            strcat(cur_file, ".jpg");
            
            Mat cur_img = prepareFile(cur_file);
            if(cur_img.empty()) continue;

            Mat cur_hist = lbpHist(cur_img, radius, n);
            long long cur_dist = calc_distance(hist, cur_hist);
            cout << "distance from image "<< i << ": " << cur_dist << endl;

            int found_lower = 1;
            int cur_index = i;
            while(found_lower){
                found_lower = 0;
                for(int j = 0; j < k; j++){
                    if(cur_dist < distances[j]){
                        cout << cur_dist << " < " << distances[j] << endl;
                        long long aux = distances[j];
                        distances[j] = cur_dist;
                        int aux_i = indexes[j];
                        indexes[j] = cur_index;
                        cur_dist = aux;
                        cur_index = aux_i;
                        found_lower = 1;
                        break;
                    }
                }
            }
            
           //imshow("image", cur_img);
           // waitKey();
        }
    }
    free(cur_number);
    free(path);

    cout << "Closest images to " << filename << endl;
    for(int i = 0; i < k; i++){
        cout << indexes[i] << endl;
    }
}

Mat lbpHist(Mat I, int radius, int n){
    Mat lbpImage = elbp(I, radius, n);    
      
    int histSize[] = {256};
  
    float range[] = { 0, 255 } ;
    const float* histRange[] = { range };

    Mat hist;
    int c[] = {0}; 
    lbpImage.convertTo(lbpImage, CV_8U);
    //imshow("LBP image", lbpImage);
    //waitKey();

    calcHist(&lbpImage, 1, c, Mat(), hist, 1, histSize, histRange);
    /*Mat histImage = Mat::ones(200, 320, CV_8U)*255;
  
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, CV_32F);
    histImage = Scalar::all(255);
    int binW = cvRound((double)histImage.cols/histSize[0]);
   
    for( int i = 0; i < histSize[0]; i++ ){
        rectangle( histImage, Point(i*binW, histImage.rows),
        Point((i+1)*binW, histImage.rows - cvRound(hist.at<float>(i))),
        Scalar::all(0), -1, 8, 0 );
    }
    
    imshow("histogram", histImage);
    waitKey();*/
    return hist;
}
