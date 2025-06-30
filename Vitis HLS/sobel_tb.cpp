#include "sobel.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

// Function to read an image from a file
bool read_image_from_file(const char* filename, pixel_data input_image) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    for (int i = 0; i < IMAGE_HEIGHT * IMAGE_WIDTH; i++) {
        float pixel_value;
        if (!(file >> pixel_value)) {
            std::cerr << "Error reading pixel at index " << i << std::endl;
            file.close();
            return false;
        }
        input_image[i] = pixel_value;
    }

    file.close();
    std::cout << "Successfully loaded image from " << filename << std::endl;
    return true;
}

void write_image_to_file(const char* filename, result_data output_image) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
        return;
    }

    // Write image data with fixed precision
    file << std::fixed << std::setprecision(6);
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            int index = i * IMAGE_WIDTH + j;
            if (j > 0) file << " ";
            file << float(output_image[index]);
        }
        file << std::endl;
    }

    file.close();
    std::cout << "Output written to " << filename << std::endl;
}

void generate_test_pattern(pixel_data input_image) {
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            int index = i * IMAGE_WIDTH + j;

            // Simple test pattern with clear edges
            if (j < IMAGE_WIDTH / 3) {
                input_image[index] = pixel_t(0.0);  // Black region
            } else if (j < 2 * IMAGE_WIDTH / 3) {
                input_image[index] = pixel_t(0.5);  // Gray region
            } else {
                input_image[index] = pixel_t(1.0);  // White region
            }
        }
    }
    std::cout << "Generated test pattern with vertical edges" << std::endl;
}

// Function to display image statistics
void display_image_stats(const char* name, result_data output_image) {
    float min_val = 1000.0f, max_val = -1000.0f, sum = 0.0f;
    int non_zero_count = 0;

    for (int i = 0; i < IMAGE_HEIGHT * IMAGE_WIDTH; i++) {
        float val = float(output_image[i]);
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
        sum += val;
        if (val > 0.001f) non_zero_count++;
    }

    float avg = sum / (IMAGE_HEIGHT * IMAGE_WIDTH);

    std::cout << "\n" << name << " Statistics:" << std::endl;
    std::cout << "  Min value: " << std::fixed << std::setprecision(6) << min_val << std::endl;
    std::cout << "  Max value: " << std::fixed << std::setprecision(6) << max_val << std::endl;
    std::cout << "  Average:   " << std::fixed << std::setprecision(6) << avg << std::endl;
    std::cout << "  Non-zero pixels: " << non_zero_count << " out of " << (IMAGE_HEIGHT * IMAGE_WIDTH) << std::endl;
}

int main() {
    std::cout << "=== Starting Sobel Filter Test Bench ===" << std::endl;
    std::cout << "Image dimensions: " << IMAGE_HEIGHT << "x" << IMAGE_WIDTH << std::endl;
    std::cout << "Target device: ZedBoard XC7Z020" << std::endl;

    // Allocate memory for input and output arrays
    static pixel_data input_image;
    static result_data output_image;

    // Initialize arrays to zero
    std::memset(input_image, 0, sizeof(pixel_data));
    std::memset(output_image, 0, sizeof(result_data));

    // Try to read input image from file
    bool use_test_pattern = false;
    if (!read_image_from_file("C:/FPGA_DATA/fpga_input_image.txt", input_image)) { //path for our test data for the test bench
        std::cout << "File not found. Generating test pattern instead..." << std::endl;
        generate_test_pattern(input_image);
        use_test_pattern = true;
    }

    // Test with fewer threshold values to reduce simulation time
    ap_fixed<8, 4> thresholds[] = {0.0, 0.2, 0.5};
    int num_thresholds = sizeof(thresholds) / sizeof(thresholds[0]);

    std::cout << "\nTesting with " << num_thresholds << " different thresholds..." << std::endl;

    for (int t = 0; t < num_thresholds; t++) {
        ap_fixed<8, 4> threshold = thresholds[t];
        std::cout << "\n--- Processing with threshold: " << float(threshold) << " ---" << std::endl;

        // Clear output array
        std::memset(output_image, 0, sizeof(result_data));

        // Call the Sobel filter function
        std::cout << "Calling sobel_filter function..." << std::endl;
        sobel_filter(input_image, output_image, threshold);
        std::cout << "sobel_filter function completed" << std::endl;

        // Generate output filenames
        std::string threshold_str = std::to_string(static_cast<int>(float(threshold) * 100));
        std::string base_name = use_test_pattern ? "test_pattern_" : "image_";
        std::string output_filename = base_name + "output_th" + threshold_str + ".txt";

        // Save results
        write_image_to_file(output_filename.c_str(), output_image);

        // Display statistics
        std::string stats_name = "Threshold " + std::to_string(float(threshold));
        display_image_stats(stats_name.c_str(), output_image);
    }

    std::cout << "\n=== Test Completed Successfully ===" << std::endl;

    return 0;
}
