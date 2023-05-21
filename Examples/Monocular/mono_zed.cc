#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<thread> // For std::this_thread::sleep_for

#include<opencv2/core/core.hpp>

// Include the ZED SDK
#include <sl/Camera.hpp>

#include "System.h"
#include "Packet.h"
using namespace std;
using namespace sl;

// Function to initialize and capture an image from the ZED camera
bool InitializeAndCapture(Camera& zed, cv::Mat& imRGB, double& tframe) {
    // // Set configuration parameters for the ZED camera
    // InitParameters init_parameters;
    // init_parameters.camera_resolution = RESOLUTION::VGA; // Or any other resolution
    // init_parameters.camera_fps = 30; // Set FPS to 30 (or whatever your application requires)
    // init_parameters.depth_mode = DEPTH_MODE::NONE; // Depth not required for this example

    // // Open the camera
    // ERROR_CODE returned_state = zed.open(init_parameters);
    // if (returned_state != ERROR_CODE::SUCCESS) {
    //     cerr << "Error opening ZED camera: " << returned_state << endl;
    //     return false;
    // }

    // Capture an image
    if (zed.grab() == ERROR_CODE::SUCCESS) {
        sl::Mat image_zed;
        zed.retrieveImage(image_zed, VIEW::LEFT); // Capture the left image
        
        // Convert sl::Mat to cv::Mat
        cv::Mat image_ocv = cv::Mat(image_zed.getHeight(), image_zed.getWidth(), CV_8UC4, image_zed.getPtr<sl::uchar1>(sl::MEM::CPU));
        cv::cvtColor(image_ocv, imRGB, cv::COLOR_BGRA2BGR);

        // Use std::chrono to generate a timestamp for the frame
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
        tframe = value.count();

        // Close the camera
        // zed.close();

        return true;
    } else {
        cerr << "Failed to capture image with ZED camera" << endl;
        // zed.close();
        return false;
    }
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        cerr << endl << "Usage: ./zed_orbslam path_to_vocabulary path_to_settings" << endl;
        return 1;
    }
    const char* address = "127.0.0.1";
    unsigned short port = 5001;

    // Updated usage check
    if (argc < 3 || argc > 4) {
        cerr << endl 
             << "Usage: "<< argv[0] << " path_to_vocabulary path_to_settings [address:port]" << endl
             << "       [address:port] - Default 127.0.0.1:5001" << endl;
        return 1;
    }

    // Parse the address and port if provided
    if (argc == 4) {
        string addressPort = argv[3];
        parseAddressPort(addressPort, address, port);
    }

    cout << endl << "Starting ORB-SLAM2 with ZED camera" << endl;

    // Initialize ZED camera
    Camera zed;

    // Set configuration parameters for the ZED camera
    InitParameters init_parameters;
    init_parameters.camera_resolution = RESOLUTION::VGA; // Or any other resolution
    init_parameters.camera_fps = 30; // Set FPS to 30 (or whatever your application requires)
    init_parameters.depth_mode = DEPTH_MODE::NONE; // Depth not required for this example

    // Open the camera
    ERROR_CODE returned_state = zed.open(init_parameters);
    if (returned_state != ERROR_CODE::SUCCESS) {
        cerr << "Error opening ZED camera: " << returned_state << endl;
        return false;
    }

    // Initialize ORB-SLAM2 system
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR, address, port,true);

    cv::Mat imRGB;
    double tframe = 0.0;

    int num_frame = 1000;
    for(int ni=0; ni<num_frame; ni++){
        // Capture an image from the ZED camera and initialize the timestamp
        if (!InitializeAndCapture(zed, imRGB, tframe)) {
            return 1; // Error handling if image capture failed
        }

        // Pass the image to the SLAM system
        SLAM.TrackMonocular(imRGB, tframe); // Note: Depth map is passed as empty since it's not used in this example
    }
    
    
    // Wait a bit before shutting down to observe any processing (optional)
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveTrajectoryTUM("CameraTrajectory.txt");
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    return 0;
}
