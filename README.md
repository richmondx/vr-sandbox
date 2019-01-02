# VR Sandbox
A VR playground created in Unreal Engine. 
Features include: 
 - Supports non-VR mode
 - Drawing on surfaces
 - Painting in 3D space
 - Importing of custom models & materials
 
The project was created with Unreal Studio.

Additional 3rd party plugins include:
 - Substance
 - [Assimp](https://github.com/assimp/assimp) 
 - [Runtime Mesh Component](https://github.com/Koderz/RuntimeMeshComponent)

## Installation
1. Copy the precompiled Assimp binaries into the `Binaries` folder. You can use the `copy-libs.bat` script inside the project to automatically copy the required 64bit files from `Plugins/Assimp`.  The 32bit windows binaries are also provided (not tested)
2. Right click on `.uproject` and generate Visual Studio project files
3. Open the VS project and rebuild the project

## Editor usage
The built project will open the Main menu by default where you can choose to boot either in VR or non-VR mode. In the Editor you can just use either FirstPersonCharacter for non-vr mode or MotionControllerPawn for VR mode
