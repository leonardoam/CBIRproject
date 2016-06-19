#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "lbp.hpp"

#include <iostream>
#include <string.h>

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
		        <<  progName << " <image_name> <k-value> <descriptor>" << endl;
			break;
		case INVALID_METHOD:
			cout << endl
		        <<  "ERROR: Invalid method or value for k." << endl;
			break;
	}
}

Mat lbpHist(Mat I);

int main(int argc, char ** argv)
{
	/*usage*/
	if (argc < 4){
		error(INCORRECT_USAGE, argv[0]);
		return 0;
	}

	/*read file*/
    const char* filename = argv[1];
    Mat I = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
    if(I.empty()){
    	error(ERROR_READING_IMG);
        return 0;
    }

    /*pre-processing: transpose all images to 400x300*/
    if(I.size().height != 300)
    	I = I.t();
    imshow("Transposed image", I);
    waitKey();
    
    /*check what the chosen method is*/
    const int k = atoi(argv[2]);
    const char* method = argv[3];
    if(strcmp(method, "lbp") && k > 0){
        Mat hist = lbpHist(I);
    }else if(strcmp(method, "fourier") && k > 0){
        
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

Mat lbpHist(Mat I){
    Mat lbpImage = elbp(I, 1, 8);    
    imshow("LBP image", lbpImage);
    waitKey();
      
    int histSize[] = {256};
  
    float range[] = { 0, 255 } ;
    const float* histRange[] = { range };

    Mat hist;
    int c[] = {0}; 
    lbpImage.convertTo(lbpImage, CV_8U);
    calcHist(&lbpImage, 1, c, Mat(), hist, 1, histSize, histRange);
    Mat histImage = Mat::ones(200, 320, CV_8U)*255;
  
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, CV_32F);
    histImage = Scalar::all(255);
    int binW = cvRound((double)histImage.cols/histSize[0]);
   
    for( int i = 0; i < histSize[0]; i++ ){
        rectangle( histImage, Point(i*binW, histImage.rows),
        Point((i+1)*binW, histImage.rows - cvRound(hist.at<float>(i))),
        Scalar::all(0), -1, 8, 0 );
    }
    
    imshow("histogram", histImage);
    waitKey();
}
