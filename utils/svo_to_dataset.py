import os
import time
import cv2
import pyzed.sl as sl

def convert_svo_to_tum_format(video_path, output_dir):
    # Create the output directory if it doesn't exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Create the rgb directory inside the output directory
    rgb_dir = os.path.join(output_dir, "rgb")
    if not os.path.exists(rgb_dir):
        os.makedirs(rgb_dir)

    # Open the SVO video file using ZED SDK
    zed = sl.Camera()
    init_params = sl.InitParameters()
    init_params.set_from_svo_file(video_path)
    init_params.svo_real_time_mode = False  # Disable real-time mode

    if zed.open(init_params) != sl.ERROR_CODE.SUCCESS:
        print("Error opening video file.")
        return

    # Prepare runtime parameters
    runtime_parameters = sl.RuntimeParameters()

    # Prepare image containers
    image_left = sl.Mat()

    # Open the rgb.txt file for writing
    rgb_txt_path = os.path.join(output_dir, "rgb.txt")
    with open(rgb_txt_path, "w") as rgb_file:
        # Write the header lines
        rgb_file.write("# color images\n")
        rgb_file.write(f"# file: '{video_path}'\n")
        rgb_file.write("# timestamp filename\n")
        
        frame_idx = 0
        while True:
            # Capture frame-by-frame
            if zed.grab(runtime_parameters) == sl.ERROR_CODE.END_OF_SVOFILE_REACHED:
                break

            zed.retrieve_image(image_left, sl.VIEW.LEFT)
            timestamp = zed.get_timestamp(sl.TIME_REFERENCE.IMAGE).get_milliseconds() / 1000.0

            # Convert sl.Mat to numpy array
            left_image = image_left.get_data()

            # Save the left image as a PNG file
            image_filename = f"{timestamp:.6f}.png"
            image_path = os.path.join(rgb_dir, image_filename)
            cv2.imwrite(image_path, left_image)

            # Write the timestamp and image path to rgb.txt
            rgb_file.write(f"{timestamp:.6f} rgb/{image_filename}\n")
            
            frame_idx += 1

    # When everything is done, release the video capture object
    zed.close()

# Example usage
video_path = "/home/disorf/Desktop/VGA_SN35582216_18-06-39.svo"
output_dir = "/home/disorf/Desktop/uwm_dataset_sign"
convert_svo_to_tum_format(video_path, output_dir)
