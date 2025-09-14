![](https://github.com/SleepyToast11/Nifti_viewer/blob/master/readme_annex/4D%20scroll%20example.gif)

# NIfTI Viewer

A lightweight viewer for NIfTI files with support for interactive scrolling across dimensions.

---

## Usage

1. Launch the program — a shell interface will appear.  
2. Navigate to the desired file location.  
3. Open the file with the command:  
open <filename>

vbnet
Copy code
4. Resize the windows as needed.  
5. Scroll through dimensions by clicking on the red bar in each view.  
6. Close all 4 views.  
7. Repeat the process or exit with:  
exit

yaml
Copy code

---

## Dependencies

The program requires the following libraries:  
- [SFML](https://www.sfml-dev.org/)  
- [nifti_clib](https://nifti.nimh.nih.gov/nifti-1)  

> **Note:**  
> - Designed for Linux environments (uses `pthread` and `X11`).  
> - Tested on **Ubuntu 22.04 LTS**.

---

## How It Works

- The shell spawns a **controller** when opening a file.  
- On success, the controller launches **3 MRI windows** and **1 time window**, each in its own thread.  
- Scrollbar inputs send update messages to the controller, which broadcasts them to all windows (including the sender).  
- Each window prepares the next image in a background thread, then swaps it into view once ready.  
- This process repeats until all windows are closed.

---
