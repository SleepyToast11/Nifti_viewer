![](https://github.com/SleepyToast11/Nifti_viewer/blob/master/readme_annex/4D%20scroll%20example.gif)

Here is a simple a simple nifti viewer.
#Steps to use
-When launched, a shell will appear where one can navigate to the wanted file
-Open the file using the command "open" and followed
-Resize windows to liking
-Scroll through the dimension by clicking on the red bar of each views
-Close all 4 views
-repeate from start or exit using exit

#Dependancies
Program requires the use of and installation of SFML and nifti_clib libraries. Also code is meant to be used in a linux environment (pthread) and X11 as tested on Ubuntu 22.04LTS.

#Quick code overview
A shell will create a controller when a file is required to open. Upon success the controller will spawn 3 MRIWindows and 1 TimeWindow. All three run in seperate threads. When an input from the scrollbar is received, an update message is sent to the controller which sends it to all the windows including the sender. The windows have a thread receive and prepare the next image to be shown. Wwhen ready the image is swapped in between frames. This continues until all windows are closed.

![alt text](https://github.com/SleepyToast11/Nifti_viewer/blob/master/readme_annex/Code%20layout.png "Code layout")
