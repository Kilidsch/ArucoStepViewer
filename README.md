# Aruco Test Viewer

This application visualizes some intermediate steps of the aruco marker detection in OpenCV.
This is supposed to help with parameter selection. Instead of fiddling with parameters until
the overall results are ideal, one is able to fine-tune the parameters independently.

This application essentially copies the private implementation of OpenCV and adds
some calls to push intermediary images to the GUI. The OpenCV version currently
recreated is 4.5.5 (note that the aruco detection was reworked since then)

> [!NOTE]  
> Does (currently) not work with AprilTag

> [!NOTE]
> When showing a video, changing the threshold level is more responsive using the keyboard

This code is licensed under the Apache 2.0 license.
