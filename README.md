# Aruco Test Viewer

This application visualizes some intermediate steps of the aruco marker detection in OpenCV.
This is supposed to help with parameter selection. Instead of fiddling with parameters until
the overall results are ideal, one is able to fine-tune the parameters independently.

This application essentially copies the private implementation of OpenCV and adds
some calls to push intermediary images to the GUI. The OpenCV version currently
recreated is 4.5.5 (note that the aruco detection was reworked since then).

> [!NOTE]  
> Does (currently) not work with AprilTag

[Showcase of GUI, going through available tabs showing the visualization of the steps of the algorithm](https://github.com/user-attachments/assets/27830c85-ce9a-4e86-a7a4-8c1cb8f21822)

Currently, the application needs to be started from the commandline. Use the `--help` option to find out how exactly to call it.
For building the application, [vcpkg](https://github.com/microsoft/vcpkg) can be used to install the dependencies from the `vcpkg.json` manifest.

This code is licensed under the Apache 2.0 license.  
For the GUI, we use [FLTK](https://github.com/fltk/fltk).  
For CLI parsing [argparse](https://github.com/p-ranav/argparse).  
And of course, for the aruco detection, [OpenCV](https://github.com/opencv/opencv).
