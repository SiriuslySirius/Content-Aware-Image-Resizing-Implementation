#include <iostream>
#include <iomanip>
#include <conio.h>
#include <vector>
#include <assert.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include "Content-Aware-Image-Resizing-Implementation.hpp"

using namespace cv;
using namespace std;

int resize_cols;	//!< Default max number of columns to show
int resize_rows;	//!< Default max number of rows to show

enum SeamDirection { VERTICAL, HORIZONTAL };

Mat createEnergyImage(Mat& image) {
    Mat image_blur, image_gray;
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Mat grad, energy_image;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    // Apply a gaussian blur to reduce noise
    GaussianBlur(image, image_blur, Size(3, 3), 0, 0, BORDER_DEFAULT);

    // convert to grayscale
    cvtColor(image_blur, image_gray, COLOR_BGR2GRAY);

    // use Sobel to calculate the gradient of the image in the x and y direction
    Sobel(image_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    Sobel(image_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

    // convert gradients to their absolut values
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);

    // total gradient
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    // convert the default values to double precision
    grad.convertTo(energy_image, CV_64F, 1.0 / 255.0);

    // create and show the newly created energy image
    namedWindow("Energy Image", WINDOW_AUTOSIZE); imshow("Energy Image", energy_image);

    return energy_image;
}

int main(int argc, const char** argv) {
    try {
		// parse the command line arguments

		CommandLineParser parser(argc, argv, keys);
		string image_path = parser.get<string>(2);

		parser.about("Content Aware Image Resizing Implementation v1.0");
		if (parser.has("help") || image_path.empty())
		{
			parser.printMessage();
			return (1);
		}

		resize_rows = parser.get<int>(0);
		resize_cols = parser.get<int>(1);

		Mat img = imread(image_path);
		assert(!img.empty());

		// Get eHOG version of image

		// Get Seams eHOG version of image

		// Add/Delete Seams

		// Output ImaGE

    }
	catch (string& str)	// handle string exception
	{
		cerr << "Error: " << argv[0] << ": " << str << endl;
		return (1);
	}
	catch (Exception& e)	// handle OpenCV exception
	{
		cerr << "Error: " << argv[0] << ": " << e.msg << endl;
		return (1);
	}

	return (0);
}
