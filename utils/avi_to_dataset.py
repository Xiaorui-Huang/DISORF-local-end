import os
import cv2
import time

def convert_avi_to_tum_format(video_path, output_dir):
    # Create the output directory if it doesn't exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Create the rgb directory inside the output directory
    rgb_dir = os.path.join(output_dir, "rgb")
    if not os.path.exists(rgb_dir):
        os.makedirs(rgb_dir)

    # Open the AVI video file
    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        print("Error opening video file.")
        return

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
            ret, frame = cap.read()
            if not ret:
                break

            # Get the timestamp (you can also use the frame index as a timestamp if needed)
            timestamp = time.time()

            # Get the left half of the frame (assuming side-by-side stereo video)
            height, width, _ = frame.shape
            left_half = frame[:, :width // 2]

            # Save the left half as a PNG file
            image_filename = f"{timestamp:.6f}.png"
            image_path = os.path.join(rgb_dir, image_filename)
            cv2.imwrite(image_path, left_half)

            # Write the timestamp and image path to rgb.txt
            rgb_file.write(f"{timestamp:.6f} rgb/{image_filename}\n")
            
            frame_idx += 1

    # When everything is done, release the video capture object
    cap.release()

# Example usage
video_path = "/home/disorf/Desktop/sign.avi"
output_dir = "/home/disorf/Desktop/uwm_dataset_sign"
convert_avi_to_tum_format(video_path, output_dir)
