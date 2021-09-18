# openglTutorial
Tracking changes in my Udemy course

## Setting up the environment
To avoid dependency on a concrete setup, this project works with environment variables to set the correct dependencies for GLEW and GLFW. 
First, [GLFW](https://www.glfw.org/download) and [GLEW](http://glew.sourceforge.net/) need to be installed on the local machine.
Also, [GLM](https://github.com/g-truc/glm) (the gl math library) needs to be installed. It is a header only library.

Second, the following environment varibles need to be set: 
```batch
SET GLEWINCLUDEDIR=\Your\Path\Here\glew-2.1.0\include
SET GLFWINCLUDEDIR=\Your\Path\Here\glfw-3.3.4.bin.WIN32\include
SET GLEWLIBRARYDIR32=\Your\Path\Here\glew-2.1.0\lib\Release\Win32
SET GLFWLIBRARYDIR32=\Your\Path\Here\glfw-3.3.4.bin.WIN32\lib-vc2019
SET GLEWLIBRARYDIR64=\Your\Path\Here\glew-2.1.0\lib\Release\x64
SET GLFWLIBRARYDIR64=\Your\Path\Here\glfw-3.3.4.bin.WIN64\lib-vc2019
SET GLMINCLUDEDIR=\Your\Path\Here\glm
```
I do this in a batch file that calls Visual Studio with the solution file subsequently. 
This should enable you to compile the program. 

To run the program, the **glew32.dll** needs to be found by the program. This can be achieved by copying it to the working directory of the program, which, when running the debugger of Visual Studio, is the project folder (**OpenGLCourseApp** in this case). Although this sounds confusing, it must be the 32bit version of glew32.dll for x86 compiled probrams and the 64bit version of glew32.dll for x64 compiled programs.