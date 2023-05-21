# Local End of DISORF
DISORF aims for training NeRF for 3d reconstruction in real-time. It uses distributed archeture so that the whole NeRF-SLAM system is divided into the SLAM part for collecting posed images and the NeRF training and visualization part. In this way, By designing specialized local-end for embedded devices like Jetson, we allow resource-constrained devices to leverage power of NeRF. This repository contains local-end for devices like Ubuntu PC and Jetson board. It is developed upon ORB-SLAM 2 (https://github.com/raulmur/ORB_SLAM2).


## License
ORB-SLAM2 is released under a [GPLv3 license](https://github.com/raulmur/ORB_SLAM2/blob/master/License-gpl.txt). For a list of all code/library dependencies (and associated licenses), please see [Dependencies.md](https://github.com/raulmur/ORB_SLAM2/blob/master/Dependencies.md).

## System requirement
We have tested the code in **Ubuntu 20.04**. A powerful CPU is necessary for real-time performance.

## Building local-end
Clone the repository:
```
git clone https://github.com/Xiaorui-Huang/DISORF-local-end.git
```

<!-- Install ZED camera from [Z](https://www.stereolabs.com/developers/release) -->

We provide a script `build.sh` to download and build all dependencies. Note that some of the dependencies of ORB-SLAM2 are obsolete. So we update code for packages like Pangolin.
```
sh build.sh
```

This will create **libORB_SLAM2.so**  at *lib* folder and the executables **mono_tum**, **mono_kitti**, **rgbd_tum**, **stereo_kitti**, **mono_euroc** and **stereo_euroc** in *Examples* folder.

## Quick Start
Download Sample dataset [here](https://drive.google.com/file/d/1b-nJHb8XWxKbaJAq4rk8M2Up9hYSd4uU/view?usp=drive_link), cd to the repo and place the file there. 
```bash
tar -xzvf Replica_sample_dataset.tar.gz -C ./data
./Examples/Monocular/mono_replica Vocabulary/ORBvoc.txt Examples/Monocular/REPLICAoffice0.yaml data/Replica/office0 true 127.0.0.1:5001
```

**Note**: Ensure that remote server is listening on the specified port. Depending on your setup, you may need to set up ssh tunneling to the remote server or port forwarding on your router. An easy way to test the connection is to use a computer on the same local network as the remote server.

## Preparation 

It's crucial to retrieve the intrinsics of the camera (including distortion coefficients if applicable through camera calibration) that is used to capture the dataset. The instrinsics are stored in a yaml file. See [`REPLICAoffice0.yaml`](Examples/Monocular/REPLICAoffice0.yaml) for an example. 

## Streaming 
- Using Camera
    <!-- - Camera Intrinsics -->
    - ZED branch\
    If you have a ZED stereo camera, you can transmit images in real-time with our **[disorf rover branch](https://github.com/Xiaorui-Huang/DISORF-local-end/tree/disorf_rover)**.
    
    - Install ZED SDK from https://www.stereolabs.com/developers/release.
    - Retrieve the intrinsics for your zed camera with the SDK and update the `zed.yaml` in `local_end/Examples/Monocular`. Run the following command to start the local end:
        ```
        ./Examples/Monocular/mono_zed Vocabulary/ORBvoc.txt Examples/Monocular/zed.yaml
        ```
    
- Using Video file
    - We prepared scripts in `utils` to convert AVI or SVO into an images dataset in Replica format. Use it to convert your video and use `mono_replica` to run the local end with your dataset as an argument.



## Monocular Examples
### TUM Dataset

1. Download a sequence from http://vision.in.tum.de/data/datasets/rgbd-dataset/download and uncompress it.

2. Execute the following command. Change `TUMX.yaml` to TUM1.yaml,TUM2.yaml or TUM3.yaml for freiburg1, freiburg2 and freiburg3 sequences respectively. Change `PATH_TO_SEQUENCE_FOLDER`to the uncompressed sequence folder.
```
./Examples/Monocular/mono_tum Vocabulary/ORBvoc.txt Examples/Monocular/TUMX.yaml PATH_TO_SEQUENCE_FOLDER
```

### KITTI Dataset  

1. Download the dataset (grayscale images) from http://www.cvlibs.net/datasets/kitti/eval_odometry.php 

2. Execute the following command. Change `KITTIX.yaml`by KITTI00-02.yaml, KITTI03.yaml or KITTI04-12.yaml for sequence 0 to 2, 3, and 4 to 12 respectively. Change `PATH_TO_DATASET_FOLDER` to the uncompressed dataset folder. Change `SEQUENCE_NUMBER` to 00, 01, 02,.., 11. 
```
./Examples/Monocular/mono_kitti Vocabulary/ORBvoc.txt Examples/Monocular/KITTIX.yaml PATH_TO_DATASET_FOLDER/dataset/sequences/SEQUENCE_NUMBER
```
