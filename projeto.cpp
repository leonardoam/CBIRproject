#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

#define ERROR_READING_IMG 1
#define INCORRECT_USAGE 2

using namespace cv;
using namespace std;

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
		        <<  progName << " [image_name]" << endl;
			break;
	}
}

int main(int argc, char ** argv)
{
	/*usage*/
	if (argc != 2){
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

    /*find edges*/
    Mat edges;
    Canny(I, edges, 1, 100);
    imshow("Edges", edges);
    waitKey();

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
