## **Description**
This project implements a photometric stereo system to compute the surface normals and albedo of objects using images captured under different lighting conditions. It is a computational vision task designed for image analysis and reconstruction.

The project consists of three key programs:
1. **Sphere Localization**: Identifies the sphere’s position and radius in an image.
2. **Light Source Analysis**: Calculates light source directions and intensities.
3. **Object Surface Recovery**: Computes surface normals and albedo of an object.

---

## **Files**
- **Source Code**:
  - `s1.cc`: Locates the sphere’s center and radius.
  - `s2.cc`: Computes light source directions and intensities.
  - `s3.cc`: Recovers object normals and albedo.
  - `image.h`, `image.cc`: Image processing utilities.

- **Supporting Files**:
  - `Makefile`: Automates the compilation of all executables (`s1`, `s2`, `s3`).
  - `README.md`: This file.
  - `thresholds.txt`: Stores the threshold value for the third program.

---

## **Compilation and Execution**

### **Compilation**
Run the following command to compile all programs:
```bash
make all
```

### **Execution**
#### **Program 1: Sphere Localization**
```bash
./s1 {input_gray_level_image} {threshold_value} {output_parameters_file}
```
- **Input**: Grayscale image of a sphere, threshold value.
- **Output**: Text file containing the sphere’s center coordinates and radius.

#### **Program 2: Light Source Analysis**
```bash
./s2 {input_parameters_file} {sphere_image1} {sphere_image2} {sphere_image3} {output_directions_file}
```
- **Input**: Parameters from Program 1, three sphere images.
- **Output**: Text file with light source directions and intensities.

#### **Program 3: Object Surface Recovery**
```bash
./s3 {input_directions_file} {object_image1} {object_image2} {object_image3} {step} {threshold} {output_normals_image} {output_albedo_image}
```
- **Input**: Directions file from Program 2, object images, step size, and threshold.
- **Output**: Needle map (normals image) and albedo image.

---

## **Assumptions and Details**
1. The object surface follows Lambertian reflectance.
2. Images are captured using orthographic projection.
3. Light sources are non-singular and have varying intensities.
4. Input/output images are in the PGM format and can be visualized using tools like [GIMP](https://www.gimp.org).

---

## **How It Works**
1. **Thresholding and Circle Detection**:
   - Locates the sphere’s center and radius by thresholding the image to create a binary mask.
   - Computes centroid and radius using geometric calculations.

2. **Light Source Direction and Intensity**:
   - Calculates surface normals of the brightest sphere points.
   - Estimates light source direction using the brightest pixels and scales their magnitude by intensity.

3. **Surface Normals and Albedo**:
   - Computes surface normals using photometric stereo formulas.
   - Generates a needle map and albedo image scaled for visualization.

---

## **Usage Notes**
- Ensure the programs run correctly on the specified environment.
- Follow the naming conventions for all input/output files to avoid compilation and runtime errors.
- Use the provided `Makefile` for consistency.

---
