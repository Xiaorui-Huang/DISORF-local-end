#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<thread> // For std::this_thread::sleep_for

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp> // Include for VideoCapture
                                      //
#include "System.h"
#include "Packet.h"

using namespace std;
using namespace cv;

// Function to initialize and capture an image from the USB camera
bool InitializeAndCapture(VideoCapture& cap, cv::Mat& imRGB, double& tframe) {
    // Capture an image
    if (cap.isOpened() && cap.read(imRGB)) {
        // Use std::chrono to generate a timestamp for the frame
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
        tframe = value.count();
        return true;
    } else {
        cerr << "Failed to capture image with USB camera" << endl;
        return false;
    }
}

int main(int argc, char **argv)
{
    char address[254] = "127.0.0.1"; // max domain length 253 char
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


    cout << endl << "Starting ORB-SLAM2 with USB camera" << endl;

    // Initialize USB camera
    VideoCapture cap(0); // 0 is the ID for the default USB camera
    if (!cap.isOpened()) {
        cerr << "Error opening USB camera" << endl;
        return 1;
    }

    // Set camera properties
    cap.set(CAP_PROP_FRAME_WIDTH, 640); // Or any other resolution
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(CAP_PROP_FPS, 30); // Set FPS to 30 (or whatever your application requires)
    // Initialize ORB-SLAM2 system
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR,address,port,true);

    cv::Mat imRGB;
    double tframe = 0.0;

    int num_frame = 1000;
    for (int ni = 0; ni < num_frame; ni++) {
        // Capture an image from the USB camera and initialize the timestamp
        if (!InitializeAndCapture(cap, imRGB, tframe)) {
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
