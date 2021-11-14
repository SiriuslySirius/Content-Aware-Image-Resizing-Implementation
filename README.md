# Content-Aware-Image-Resizing-Implementation
**Based on this algorithm:** https://en.wikipedia.org/wiki/Seam_carving
</br>
**Main Contributors:** Abelson Abueg and Brandon Miranda
</br>
**Credit to David Xiao** (https://github.com/davidshower/seam-carving) for the general algorithmic functions needed for Seam Carving.
</br>

## Video Preview:
https://user-images.githubusercontent.com/38868532/141663918-ddeedbaf-c054-4337-a639-814cfbc10206.mp4

## Our Contributions:
* Code made compatible with OpenCV version 4.5.4
* 2-Directional (Vertical and Horizontal) Seam Carving Implementation vs David Xiao's 1-Directional Seam Carving Implementation
* Step-by-Step Frame Preview of Seam Carving Process through a Slider. 

## OpenCV Instructions for Windows with Microsoft Visual Studio 2019
We used Microsoft Visual Studio 2019 for working with Windows. To compile our code, you will need to open 'CAIRI.sln' and follow these instruction on how to get OpenCV working on Visual Studio on your Windows machine before building the solution:
* __Install and Configure OpenCV on Windows 10 and Microsoft Visual 2019:__ https://towardsdatascience.com/install-and-configure-opencv-4-2-0-in-windows-10-vc-d132c52063a1
  * Installing OpenCV
  * Setting your environment variable for OpenCV
  * Setting up OpenCV in Microsoft Visual Studio 2019
  * __Important:__  Make sure when your doing this, set the build to x64 _Release_. It is important to set it to _Release_ because the OpenCV windows will not work in _Debug_ mode.

After you have configured OpenCV in your machine and in Microsoft Visual Studio, you'll then need to build the solution. Again, make sure you are building in x64 Release.

## Instructions for Running the Program on Windows
* After configuring and setting up OpenCV, build the solution, and you will find executable (.exe) file in “*\CAIRI\x64\release”. 
* To run the program, open Powershell and navigate to the path: “*\CAIRI\x64\release”.
* Below are the basic arguments you can use for the program:
  * Help
    * .\Content-Aware-Image-Resizing-Implementation.exe -h
  * Seam Carving (Vertical and Horizontal Seam Carving; make sure you are resizing it smaller than the original image.)
    * .\Content-Aware-Image-Resizing-Implementation.exe <INT: Desired Width> <INT: Desired Height> <STRING: Image Filepath>
    * EXAMPLE: .\Content-Aware-Image-Resizing-Implementation.exe 750 468 'C:\Users\User\Downloads\kinkakuji.jpg

## Future Features:
* Seam Adding
* Object Removal
* Software GUI
* Save Images
