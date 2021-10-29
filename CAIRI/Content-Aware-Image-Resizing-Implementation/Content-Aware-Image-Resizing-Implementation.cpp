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

int resize_cols;	//!< Default max number of columns to show
int resize_rows;	//!< Default max number of rows to show

int main(int argc, const char** argv) {
    try {
		// parse the command line arguments

		cv::CommandLineParser parser(argc, argv, keys);
		std::string image_path = parser.get<std::string>(2);

		parser.about("Content Aware Image Resizing Implementation v1.0");
		if (parser.has("help") || image_path.empty())
		{
			parser.printMessage();
			return (1);
		}

		resize_rows = parser.get<int>(0);
		resize_cols = parser.get<int>(1);

		cv::Mat img = cv::imread(image_path);
		assert(!img.empty());

		// Get eHOG version of image

		// Get Seams eHOG version of image

		// Add/Delete Seams

		// Output ImaGE

    }
	catch (std::string& str)	// handle string exception
	{
		std::cerr << "Error: " << argv[0] << ": " << str << std::endl;
		return (1);
	}
	catch (cv::Exception& e)	// handle OpenCV exception
	{
		std::cerr << "Error: " << argv[0] << ": " << e.msg << std::endl;
		return (1);
	}

	return (0);
}
