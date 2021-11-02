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

int resize_rows;
int resize_cols;

// Variables for storing the max vector size and index
int max_vect_size = 0;
int vect_index = 0;

enum SeamDirection { VERTICAL, HORIZONTAL };

// Create Vectors for Storing Data for Slider Preview
vector<Mat> e_img_vect, red_img_vect, seam_prev_vect;

// Create Vectors for Storing Seam Direction for Slider Preview
vector<SeamDirection> vect_seam_dirs;

Mat createEnergyImage(Mat& image) 
{
    Mat image_blur, image_gray;
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Mat grad, energy_image;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    // Apply a gaussian blur to reduce noise in image
    GaussianBlur(image, image_blur, Size(3, 3), 0, 0, BORDER_DEFAULT);

    // Convert image to grayscale
    cvtColor(image_blur, image_gray, COLOR_BGR2GRAY);

    // Use Sobel to calculate the gradient of the image in the x and y direction
    Sobel(image_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    Sobel(image_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

    // Convert gradients to their absolute values
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);

    // Sum the gradient
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    // Convert the default values to double precision
    grad.convertTo(energy_image, CV_64F, 1.0 / 255.0);

 
	// Create and show the energy image
	namedWindow("Energy Image", WINDOW_AUTOSIZE);
	imshow("Energy Image", energy_image);
    waitKey(1);
 

    return energy_image;
}

Mat createCumulativeEnergyMap(Mat& energy_image, SeamDirection seam_direction) 
{
    double a, b, c;

    // get the numbers of rows and columns in the image
    int rowsize = energy_image.rows;
    int colsize = energy_image.cols;

    // initialize the map with zeros
    Mat cumulative_energy_map = Mat(rowsize, colsize, CV_64F, double(0));

    // copy the first row
    if (seam_direction == VERTICAL) energy_image.row(0).copyTo(cumulative_energy_map.row(0));
    else if (seam_direction == HORIZONTAL) energy_image.col(0).copyTo(cumulative_energy_map.col(0));

    // take the minimum of the three neighbors and add to total, this creates a running sum which is used to determine the lowest energy path
    if (seam_direction == VERTICAL) 
    {
        for (int row = 1; row < rowsize; row++) 
        {
            for (int col = 0; col < colsize; col++) 
            {
                a = cumulative_energy_map.at<double>(row - 1, max(col - 1, 0));
                b = cumulative_energy_map.at<double>(row - 1, col);
                c = cumulative_energy_map.at<double>(row - 1, min(col + 1, colsize - 1));

                cumulative_energy_map.at<double>(row, col) = energy_image.at<double>(row, col) + std::min(a, min(b, c));
            }
        }
    }
    else if (seam_direction == HORIZONTAL) 
    {
        for (int col = 1; col < colsize; col++) 
        {
            for (int row = 0; row < rowsize; row++) 
            {
                a = cumulative_energy_map.at<double>(max(row - 1, 0), col - 1);
                b = cumulative_energy_map.at<double>(row, col - 1);
                c = cumulative_energy_map.at<double>(min(row + 1, rowsize - 1), col - 1);

                cumulative_energy_map.at<double>(row, col) = energy_image.at<double>(row, col) + std::min(a, min(b, c));
            }
        }
    }

    Mat color_cumulative_energy_map;
    
    double Cmin;
    double Cmax;
    minMaxLoc(cumulative_energy_map, &Cmin, &Cmax);
    float scale = 255.0 / (Cmax - Cmin);
    cumulative_energy_map.convertTo(color_cumulative_energy_map, CV_8UC1, scale);
    applyColorMap(color_cumulative_energy_map, color_cumulative_energy_map, COLORMAP_JET);

    namedWindow("Cumulative Energy Map", WINDOW_AUTOSIZE); 
    imshow("Cumulative Energy Map", color_cumulative_energy_map);
    waitKey(1);

    return cumulative_energy_map;
}

vector<int> findOptimalSeam(Mat& cumulative_energy_map, SeamDirection seam_direction) 
{
    double a, b, c;
    int offset = 0;
    vector<int> path;
    double min_val, max_val;
    Point min_pt, max_pt;

    // get the number of rows and columns in the cumulative energy map
    int rowsize = cumulative_energy_map.rows;
    int colsize = cumulative_energy_map.cols;

    if (seam_direction == VERTICAL) 
    {
        // copy the data from the last row of the cumulative energy map
        Mat row = cumulative_energy_map.row(rowsize - 1);

        // get min and max values and locations
        minMaxLoc(row, &min_val, &max_val, &min_pt, &max_pt);

        // initialize the path vector
        path.resize(rowsize);
        int min_index = min_pt.x;
        path[--rowsize] = min_index;

        // starting from the bottom, look at the three adjacent pixels above current pixel, choose the minimum of those and add to the path
        for (int i = rowsize - 2; i >= 0; i--) 
        {
            a = cumulative_energy_map.at<double>(i, max(min_index - 1, 0));
            b = cumulative_energy_map.at<double>(i, min_index);
            c = cumulative_energy_map.at<double>(i, min(min_index + 1, colsize - 1));

            if (min(a, b) > c) 
            {
                offset = 1;
            }
            else if (min(a, c) > b) 
            {
                offset = 0;
            }
            else if (min(b, c) > a) 
            {
                offset = -1;
            }

            min_index += offset;
            min_index = min(max(min_index, 0), colsize - 1); // take care of edge cases
            path[i] = min_index;
        }

    }
    else if (seam_direction == HORIZONTAL) 
    {
        // copy the data from the last column of the cumulative energy map
        Mat col = cumulative_energy_map.col(colsize - 1);

        // get min and max values and locations
        minMaxLoc(col, &min_val, &max_val, &min_pt, &max_pt);

        // initialize the path vector
        path.resize(colsize);
        int min_index = min_pt.y;
        path[--colsize] = min_index;

        // starting from the right, look at the three adjacent pixels to the left of current pixel, choose the minimum of those and add to the path
        for (int i = colsize - 2; i >= 0; i--) 
        {
            a = cumulative_energy_map.at<double>(max(min_index - 1, 0), i);
            b = cumulative_energy_map.at<double>(min_index, i);
            c = cumulative_energy_map.at<double>(min(min_index + 1, rowsize - 1), i);

            if (min(a, b) > c) 
            {
                offset = 1;
            }
            else if (min(a, c) > b) 
            {
                offset = 0;
            }
            else if (min(b, c) > a) 
            {
                offset = -1;
            }

            min_index += offset;
            min_index = min(max(min_index, 0), rowsize - 1); // take care of edge cases
            path[i] = min_index;
        }
    }
    return path;
}

Mat reduce(Mat& image, vector<int> path, SeamDirection seam_direction) 
{
    // get the number of rows and columns in the image
    int rowsize = image.rows;
    int colsize = image.cols;

    // create a 1x1x3 dummy matrix to add onto the tail of a new row to maintain image dimensions and mark for deletion
    Mat dummy(1, 1, CV_8UC3, Vec3b(0, 0, 0));

    if (seam_direction == VERTICAL)  // reduce the width
    {
        for (int i = 0; i < rowsize; i++) 
        {
            // take all pixels to the left and right of marked pixel and store them in appropriate subrow variables
            Mat new_row;
            Mat lower = image.rowRange(i, i + 1).colRange(0, path[i]);
            Mat upper = image.rowRange(i, i + 1).colRange(path[i] + 1, colsize);

            // merge the two subrows and dummy matrix/pixel into a full row
            if (!lower.empty() && !upper.empty()) 
            {
                hconcat(lower, upper, new_row);
                hconcat(new_row, dummy, new_row);
            }
            else 
            {
                if (lower.empty()) 
                {
                    hconcat(upper, dummy, new_row);
                }
                else if (upper.empty()) 
                {
                    hconcat(lower, dummy, new_row);
                }
            }
            // take the newly formed row and place it into the original image
            new_row.copyTo(image.row(i));
        }
        // clip the right-most side of the image
        image = image.colRange(0, colsize - 1);
    }
    else if (seam_direction == HORIZONTAL) // reduce the height
    { 
        for (int i = 0; i < colsize; i++) 
        {
            // take all pixels to the top and bottom of marked pixel and store the in appropriate subcolumn variables
            Mat new_col;
            Mat lower = image.colRange(i, i + 1).rowRange(0, path[i]);
            Mat upper = image.colRange(i, i + 1).rowRange(path[i] + 1, rowsize);

            // merge the two subcolumns and dummy matrix/pixel into a full row
            if (!lower.empty() && !upper.empty()) 
            {
                vconcat(lower, upper, new_col);
                vconcat(new_col, dummy, new_col);
            }
            else 
            {
                if (lower.empty()) 
                {
                    vconcat(upper, dummy, new_col);
                }
                else if (upper.empty()) 
                {
                    vconcat(lower, dummy, new_col);
                }
            }
            // take the newly formed column and place it into the original image
            new_col.copyTo(image.col(i));
        }
        // clip the bottom-most side of the image
        image = image.rowRange(0, rowsize - 1);
    }

        // Display Reduced Image
        namedWindow("Reduced Image", WINDOW_AUTOSIZE); 
        imshow("Reduced Image", image);
        waitKey(1);
 
        return image;
}

Mat showPath(Mat& energy_image, vector<int> path, SeamDirection seam_direction) 
{
    // loop through the image and change all pixels in the path to white
    if (seam_direction == VERTICAL) 
    {
        for (int i = 0; i < energy_image.rows; i++) 
        {
            energy_image.at<double>(i, path[i]) = 1;
        }
    }
    else if (seam_direction == HORIZONTAL) 
    {
        for (int i = 0; i < energy_image.cols; i++) 
        {
            energy_image.at<double>(path[i], i) = 1;
        }
    }

    // Display the seam on top of the energy image
    namedWindow("Seam on Energy Image", WINDOW_AUTOSIZE); 
    imshow("Seam on Energy Image", energy_image);
    waitKey(1);

    return energy_image;
}

int main(int argc, const char** argv) {
    try 
    {
		CommandLineParser parser(argc, argv, keys);
		parser.about("Content Aware Image Resizing Implementation v1.0");
		

		if (parser.has("help"))
		{
			parser.printMessage();
			return (0);
		}

        resize_cols = parser.get<int>(0);
		resize_rows = parser.get<int>(1);
		string image_path = parser.get<string>(2);

		Mat img = imread(image_path);
		assert(!img.empty());

        if (img.size().width < resize_cols || img.size().height < resize_rows ) 
        {
            cout << "ERROR: You are trying to enlarge the input image.\n";
            cout << "In version 1.0, Seam Adding is not supported yet.\n";
            cout << "Only Seam Carving is supported.\n";
            return 0;
        }
        else if (img.size().width == resize_cols && img.size().height == resize_rows) 
        {
            cout << "ERROR: Resize dimensions are the same as the input image dimensions.\n";
            cout << "No change is needed.\n";
            return 0;
        }
        else 
        {
            namedWindow("Original Image", WINDOW_AUTOSIZE); 
            imshow("Original Image", img);
            waitKey(1);

            // Setting up vector of Seam Directions for two-way seam carving
            vector<SeamDirection> seam_dir_vect;

            if (img.size().width >= img.size().height) 
            {
                seam_dir_vect.push_back(VERTICAL);
                seam_dir_vect.push_back(HORIZONTAL);
            }
            else if (img.size().width <= img.size().height) 
            {
                seam_dir_vect.push_back(HORIZONTAL);
                seam_dir_vect.push_back(VERTICAL);
            }


            if (img.size().width == resize_cols) 
            {
                seam_dir_vect.erase(remove(seam_dir_vect.begin(), seam_dir_vect.end(), HORIZONTAL), seam_dir_vect.end());
            }

            if (img.size().height == resize_rows) 
            {
                seam_dir_vect.erase(remove(seam_dir_vect.begin(), seam_dir_vect.end(), VERTICAL), seam_dir_vect.end());
            }

            // Loop for doing two-direction seam carving
            Mat current_img = img;
            for (int i = 0; i < seam_dir_vect.size(); i++) 
            {
                int iterations = 1;

                if (seam_dir_vect[i] == VERTICAL) 
                {
                    iterations = current_img.size().width - resize_cols;
                    max_vect_size += iterations;
                }
                else if (seam_dir_vect[i] == HORIZONTAL) 
                {
                    iterations = current_img.size().height - resize_rows;
                    max_vect_size += iterations;
                }

                for (int j = 0; j < iterations; j++) 
                {
                    vect_seam_dirs.push_back(seam_dir_vect[i]);

                    // Get energy version of image
                    Mat e_img = createEnergyImage(current_img);
                    e_img_vect.push_back(e_img);

                    // Get energy cumululation map of image
                    Mat cum_e_map = createCumulativeEnergyMap(e_img, seam_dir_vect[i]);

                    // Get Low Energy Seam
                    vector<int> path = findOptimalSeam(cum_e_map, seam_dir_vect[i]);

                    // Delete Seams
                    Mat reduce_img = reduce(current_img, path, seam_dir_vect[i]);
                    red_img_vect.push_back(current_img);

                    // Show seam paths
                    Mat reduce_img_path = showPath(e_img, path, seam_dir_vect[i]);
                    seam_prev_vect.push_back(reduce_img_path);

                    vect_index++;
                }
            }
            vect_index--;
            namedWindow("Frame Slider", WINDOW_AUTOSIZE);
            createTrackbar("Index", "Frame Slider", NULL, (max_vect_size - 1), 0);
            setTrackbarPos("Index", "Frame Slider", vect_index);
            cout << "\nPress ESC to end program.\n";
            while (true) {
                int ind = getTrackbarPos("Index", "Frame Slider");
                imshow("Energy Image", e_img_vect[ind]);
                createCumulativeEnergyMap(e_img_vect[ind], vect_seam_dirs[ind]);
                imshow("Reduced Image", red_img_vect[ind]);
                imshow("Seam on Energy Image", seam_prev_vect[ind]); 

                int response = waitKey(1);
                if (response == 27)
                {
                    cout << "Program closed.\n";
                    break;
                }
            }
        }
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
