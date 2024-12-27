#include "image.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

using namespace std;
using namespace ComputerVisionProjects;

int main(int argc, char* argv[]){ // Define the main function with command-line argument count and values.
    if (argc != 6) { // Check if the number of command-line arguments is exactly 6.
        printf("Usage: %s {input parameters filename} {input sphere image 1 filename} {input sphere image 2 filename} {input sphere image 3 filename} {output directions filename} \n", argv[0]);
        return 0; // Exit the program if the required arguments are not provided.
    }

    const string in_param(argv[1]); // Store the first argument as the input parameters filename.
    const string in_sph1(argv[2]); // Store the second argument as the first sphere image filename.
    const string in_sph2(argv[3]); // Store the third argument as the second sphere image filename.
    const string in_sph3(argv[4]); // Store the fourth argument as the third sphere image filename.
    const string out_file(argv[5]); // Store the fifth argument as the output directions filename.

    int ctr_x, ctr_y, rad; // Declare variables for the center coordinates and radius of the sphere.
    ifstream param_file(in_param); // Open the input parameters file for reading.
    if(!param_file.is_open()){ // Check if the parameters file opened successfully.
        cout << "Can't open parameter file " << in_param << endl;
        return 0; // Exit the program if the file cannot be opened.
    }

    param_file >> ctr_x >> ctr_y >> rad; // Read the sphere center coordinates and radius from the file.
    param_file.close(); // Close the input parameters file.

    vector<string> mult_imgs = {in_sph1, in_sph2, in_sph3}; // Create a vector containing the filenames of the sphere images.
    ofstream out(out_file); // Open the output file for writing.
    if(!out.is_open()){ // Check if the output file opened successfully.
        cout << "Can't open output file " << out_file << endl;
        return 0; // Exit the program if the file cannot be opened.
    }

    for(const auto& img_file : mult_imgs){ // Loop through each sphere image file in the vector.
        Image sph_img; // Declare an Image object to hold each sphere image.
        if(!ReadImage(img_file, &sph_img)){ // Read the image file into the sph_img object.
            cout << "Can't open image file " << img_file << endl;
            return 0; // Exit the program if the image file cannot be opened.
        }

        int mx_x = 0; // Initialize the x-coordinate of the pixel with the maximum intensity.
        int mx_y = 0; // Initialize the y-coordinate of the pixel with the maximum intensity.
        int mx_int = 0; // Initialize the maximum pixel intensity value.

        for(int r = 0; r < sph_img.num_rows(); ++r){ // Loop through each row of the image.
            for(int c = 0; c < sph_img.num_columns(); ++c){ // Loop through each column of the image.
                int pix_val = sph_img.GetPixel(r, c); // Get the pixel intensity at the current position.

                if(pix_val > mx_int){ // Check if the current pixel intensity is greater than the max intensity.
                    mx_int = pix_val; // Update max intensity to current pixel intensity.
                    mx_x = c; // Update max intensity pixel x-coordinate.
                    mx_y = r; // Update max intensity pixel y-coordinate.
                }
            }
        }

        double dz_sq = static_cast<double>(rad*rad) - ((mx_x - ctr_x) * (mx_x - ctr_x)) - ((mx_y - ctr_y) * (mx_y - ctr_y));
        // Calculate the squared value of the z-component (dz) based on the distance formula.
        
        if(dz_sq < 0){ // Check if the squared z-component is negative.
            cout << "brightest point is out of bound" << endl; // Notify if the point is out of bounds.
            return 0; // Exit the program if the point is out of bounds.
        }

        double dz = sqrt(dz_sq); // Calculate the z-component by taking the square root of dz_sq.

        cout << "dz: " << dz << endl; // Print the value of dz to the console.

        double N1 = mx_x - ctr_x; // Calculate the x-component of the normal vector.
        double N2 = mx_y - ctr_y; // Calculate the y-component of the normal vector.
        double N3 = dz; // Assign dz as the z-component of the normal vector.

        double norm_vec = sqrt(N1*N1 + N2*N2 + N3*N3); // Calculate the magnitude of the normal vector.
        double norm_scl = mx_int / norm_vec; // Calculate the scaling factor based on the maximum intensity and vector magnitude.

        N1 = N1*norm_scl; // Scale the x-component of the normal vector.
        N2 = N2*norm_scl; // Scale the y-component of the normal vector.
        N3 = N3*norm_scl; // Scale the z-component of the normal vector.

        out << N1 << " " << N2 << " " << N3 << endl; // Write the scaled normal vector components to the output file.

    }

    out.close(); // Close the output file.
    cout << "Output was saved to " << out_file << endl; // Confirm that output was saved to the file.
    return 0; // End the program.
}
