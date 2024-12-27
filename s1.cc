#include "image.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;            
using namespace ComputerVisionProjects; 

int main(int argc, char* argv[]) {  // Main function with arguments for command line input

    // Check if the program received exactly 4 arguments (program name + 3 arguments)
    if (argc != 4) {
        printf("Usage: %s {input gray–level sphere image} {input threshold value} {output parameters file}\n", argv[0]);
        return 0;  // Exit if the argument count is incorrect
    }

    const string in_file(argv[1]); // Get the input image file path from the first argument
    int thresh = stoi(argv[2]);    // Convert the second argument to an integer threshold value
    const string out_file(argv[3]); // Get the output file path from the third argument

    Image img;  // Create an Image object to store the loaded image data

    // Attempt to read the image file into the Image object
    if (!ReadImage(in_file, &img)) {
        cout << "Can't open file " << in_file << endl;  // Print error if file can't be opened
        return 0;  // Exit if image loading fails
    }

    // Loop through each pixel in the image and apply thresholding
    for (int r = 0; r < img.num_rows(); ++r) { // Loop through each row in the image
        for (int c = 0; c < img.num_columns(); ++c) { // Loop through each column in the row
            const int pix_val = img.GetPixel(r, c); // Get the pixel value at (r, c)
            if (pix_val < thresh) { // If pixel value is less than threshold
                img.SetPixel(r, c, 0); // Set pixel to black (0)
            } else {
                img.SetPixel(r, c, 255); // Set pixel to white (255)
            }
        }
    }

    // Initialize variables for calculating the center and radius of the white region
    int x_cnt = 0;  // Sum of x-coordinates of white pixels
    int y_cnt = 0;  // Sum of y-coordinates of white pixels
    int pix_cnt = 0; // Count of white pixels
    int x_left = img.num_columns(); // Leftmost x-coordinate for the white region
    int x_right = 0; // Rightmost x-coordinate for the white region
    int y_up = img.num_rows(); // Topmost y-coordinate for the white region
    int y_low = 0; // Bottommost y-coordinate for the white region

    // Loop through the image again to find white pixels and calculate bounding box and center
    for (int r = 0; r < img.num_rows(); ++r) { // Loop through each row
        for (int c = 0; c < img.num_columns(); ++c) { // Loop through each column
            if (img.GetPixel(r, c) == 255) { // Check if the pixel is white
                x_cnt = x_cnt + c; // Add x-coordinate to x-coordinate sum
                y_cnt = y_cnt + r; // Add y-coordinate to y-coordinate sum
                pix_cnt++; // Increment the white pixel count

                if (c < x_left) x_left = c; // Update leftmost x-coordinate if necessary
                if (c > x_right) x_right = c; // Update rightmost x-coordinate if necessary
                if (r < y_up) y_up = r; // Update topmost y-coordinate if necessary
                if (r > y_low) y_low = r; // Update bottommost y-coordinate if necessary
            }
        }
    }

    // Calculate the center (x, y) of the white region
    int ctr_x = x_cnt / pix_cnt; // Calculate average x-coordinate (center x)
    int ctr_y = y_cnt / pix_cnt; // Calculate average y-coordinate (center y)

    // Estimate the radius of the white region based on bounding box
    int rad = ((x_right - x_left) / 2 + (y_low - y_up) / 2) / 2;

    // Open the output file for writing
    ofstream out(out_file);
    if (!out.is_open()) { // Check if file opened successfully
        cout << "Can't write to file " << out_file << endl; // Print error message
        return 0; // Exit if file opening fails
    }
    out << ctr_x << " " << ctr_y << " " << rad << endl; // Write center and radius to file
    out.close(); // Close the output file

    // Print the center and radius information to the console
    cout << "Center: (" << ctr_x << ", " << ctr_y << "), Radius: " << rad << endl;

    return 0; // End of program
}
