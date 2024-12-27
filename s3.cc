

#include <iostream>   
#include <fstream>     
#include <string>     
#include <cmath>       
#include <vector>      
#include "image.h"    

using namespace std;                       
using namespace ComputerVisionProjects;    


int main(int argc, char *argv[]){          
    if (argc != 9) {                       
        printf("Usage: %s {input directions filename} {input object image 1 filename} {input object image 2 filename} {input object image 3 filename} {input step parameter (integer greater than 0)} {input threshold parameter (integer greater than 0)} {output normals image filename} {output albedo image filename \n", argv[0]);
        return 0;                           
    }

    const string in_dir(argv[1]);           // Input direction filename
    const string in_obj1(argv[2]);          // Input object image 1 filename
    const string in_obj2(argv[3]);          // Input object image 2 filename
    const string in_obj3(argv[4]);          // Input object image 3 filename
    int step_sz = stoi(argv[5]);            // Step size parameter, converting from string to integer
    int thresh = stoi(argv[6]);             // Threshold parameter, converting from string to integer
    const string out_norm_img(argv[7]);     // Output filename for normals image
    const string out_alb_img(argv[8]);      // Output filename for albedo image

    vector<vector<double>> dir;             // Vector to hold light direction vectors
    ifstream dir_file(in_dir);              // Open direction file
    if(!dir_file.is_open()){                // Check if direction file opened successfully
        cout << "Can't open direction file " << in_dir << endl;
        return 0;                           // Exit if file cannot be opened
    }

    // Reading 3 light direction vectors from the file
    for(int i = 0; i < 3; ++i){
        double N1, N2, N3;
        dir_file >> N1 >> N2 >> N3;         // Read a light direction vector
        dir.push_back({N1, N2, N3});        // Store in the direction vector
    }
    dir_file.close();                       // Close the direction file

   cout << "After reading I" << endl;

    // Load the object images
    vector<Image> obj_img(3);               // Vector to hold three input object images
    vector<string> obj_fnames = {in_obj1, in_obj2, in_obj3}; // File names of object images
    for(int i = 0; i < 3; ++i){
        if(!ReadImage(obj_fnames[i], &obj_img[i])){ // Load each image
            cout << "Can't open object file " << obj_fnames[i] << endl;
            return 0;                       // Exit if an image file cannot be loaded
        }
    }

    // Allocate space for the normals and albedo images
    Image norm_img = obj_img[0];
    Image alb_img;
    alb_img.AllocateSpaceAndSetSize(obj_img[0].num_rows(), obj_img[0].num_columns()); // Allocate space for albedo map
    alb_img.SetNumberGrayLevels(255);
    double max_alb = 0;                        // Variable to keep track of the maximum albedo value

    vector<double> alb_values;                 // Vector to store all albedo values

    // Create matrix S with light directions
    vector<vector<double>> StS(3, vector<double>(3));  // Initialize a 3x3 matrix for light directions
    for(int k = 0; k < 3; ++k) {  // Fill in light direction values from dir
        StS[k][0] = dir[k][0];
        StS[k][1] = dir[k][1];
        StS[k][2] = dir[k][2];
    }

    double det = StS[0][0] * (StS[1][1] * StS[2][2] - StS[1][2] * StS[2][1]) - StS[0][1] * (StS[1][0] * StS[2][2] - StS[1][2] * StS[2][0]) +
                 StS[0][2] * (StS[1][0] * StS[2][1] - StS[1][1] * StS[2][0]);  // Calculate determinant of StS

    if (det == 0) return 0;  // Skip pixel if matrix is singular (no unique solution)

    // Compute inverse of StS using its determinant
    vector<vector<double>> invStS(3, vector<double>(3));  // Initialize inverse matrix of StS
    invStS[0][0] = (StS[1][1] * StS[2][2] - StS[1][2] * StS[2][1]) / det;
    invStS[0][1] = (StS[0][2] * StS[2][1] - StS[0][1] * StS[2][2]) / det;
    invStS[0][2] = (StS[0][1] * StS[1][2] - StS[0][2] * StS[1][1]) / det;
    invStS[1][0] = (StS[1][2] * StS[2][0] - StS[1][0] * StS[2][2]) / det;
    invStS[1][1] = (StS[0][0] * StS[2][2] - StS[0][2] * StS[2][0]) / det;
    invStS[1][2] = (StS[0][2] * StS[1][0] - StS[0][0] * StS[1][2]) / det;
    invStS[2][0] = (StS[1][0] * StS[2][1] - StS[1][1] * StS[2][0]) / det;
    invStS[2][1] = (StS[0][1] * StS[2][0] - StS[0][0] * StS[2][1]) / det;
    invStS[2][2] = (StS[0][0] * StS[1][1] - StS[0][1] * StS[1][0]) / det;

    cout << "Calculated inverse" << endl;
    // Iterate over each pixel in the images
    for(int i = 0; i < obj_img[0].num_rows(); ++i){  // Loop through each row of the image
        for(int j = 0; j < obj_img[0].num_columns(); ++j){  // Loop through each column of the image
            
            // Retrieve intensity values from each image at pixel (i, j)
            int I1 = obj_img[0].GetPixel(i, j);  // Get pixel intensity from the first image
            int I2 = obj_img[1].GetPixel(i, j);  // Get pixel intensity from the second image
            int I3 = obj_img[2].GetPixel(i, j);  // Get pixel intensity from the third image

            // Check if any intensity is below threshold
            if (I1 < thresh || I2 < thresh || I3 < thresh) {  // If any intensity is below the threshold
                alb_img.SetPixel(i, j, 0);  // Set albedo pixel to black if below threshold
                alb_values.push_back(0);    // Push zero albedo into the vector
                continue;  // Skip further processing for this pixel
            }

            vector<double> I = {static_cast<double>(I1), static_cast<double>(I2), static_cast<double>(I3)};  // Intensity vector

            // Calculate the normal vector N by solving for N in StS * N = StI
            vector<double> N(3, 0);  // Initialize vector for normal
            
            // Multiply invStS by StI to solve for N
            for(int k = 0; k < 3; ++k){  // Iterate through each element of N
                for(int h = 0; h < 3; ++h){  // Perform matrix multiplication invStS * StI
                    N[k] += invStS[k][h] * I[h];
                }
            }
    
            // Compute albedo from the magnitude of N
            double alb = sqrt((N[0]*N[0]) + (N[1]*N[1]) + (N[2]*N[2]));  // Calculate magnitude of N
            if(alb > max_alb) max_alb = alb;  // Update max albedo if current is greater

            alb_values.push_back(alb); // Store the albedo in the vector for normalization

            //cout << "alb " << alb << endl;

            for(int k = 0; k < 3; ++k){  // Normalize N vector by dividing by albedo
                N[k] = N[k] / alb;
            } 
     
            // Draw a line to represent the normal if pixel is at a step interval
            if (i % step_sz == 0 && j % step_sz == 0) {  // Check if pixel position meets step interval
                int ctr_x = i;  // Set x-coordinate for normal line starting point
                int ctr_y = j;  // Set y-coordinate for normal line starting point
                int len = 10;  // Define length of the line representing the normal

                DrawLine(ctr_x, ctr_y, ctr_x + static_cast<int>(len * N[1]),
                        ctr_y + static_cast<int>(len * N[0]), 255, &norm_img);  // Draw line based on normal direction
                
                // Draw a small dot at the start of the normal line to improve visibility
                for (int dx = -1; dx <= 1; ++dx) {  // Iterate over a small neighborhood around the center
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (ctr_x + dx >= 0 && ctr_y + dy >= 0)  // Ensure coordinates are within bounds
                            norm_img.SetPixel(ctr_x + dx, ctr_y + dy, 255);  // Set pixel to white for visibility
                    }
                }
            }

            alb_img.SetPixel(i, j, alb);  // Set albedo image pixel with computed albedo value
        }
    }
    
    // Normalize and scale albedo values to [0, 255]
    for (size_t i = 0; i < alb_values.size(); ++i) {
        alb_values[i] = (alb_values[i] / max_alb) * 255;  // Normalize by max_alb and scale by 255
    }

   // Update alb_img with normalized and scaled albedo values
    int index = 0;
    for (int i = 0; i < obj_img[0].num_rows(); ++i) {
        for (int j = 0; j < obj_img[0].num_columns(); ++j) {
            alb_img.SetPixel(i, j, static_cast<int>(alb_values[index]));  // Set normalized albedo value
            ++index;
        }
    }


    // Save generated images to output files
    WriteImage(out_norm_img, norm_img);     
    WriteImage(out_alb_img, alb_img);      

    cout << "Maximum albedo value: " << max_alb << endl; 
    return 0;                              
}
