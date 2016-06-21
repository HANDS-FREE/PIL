#The Pilot Intelligent Library
------------------------------------------------------------------------------

##1. Introduction
------------------------------------------------------------------------------
The [PIL](http://zhaoyong.adv-ci.com/pil/) library includes some pretty useful tools for C++ programing espesially in the areas of moblile robotics and computer vision. 

### 1.1. Website: 
- http://adv-ci.com/pil/
- http://zhaoyong.adv-ci.com/pil/
         
### 1.2. Folder structure
* PIL --- top dir
 - src       --- source code of the library
 - -  **base**     ---  base things ([std=c++11])
 - - - *ClassLoader*--- used for plugin warp and loading
 - - - *Debug*      --- debug utils, exceptions, Logger
 - - - *IO*         --- some basic io devices support like UART 
 - - - *Path*       --- a general path parser
 - - - *Platform*   --- basic platform and enviroment macros
 - - - *Svar*       --- the svar script language for configuration, value passing and command invoking
 - - - *Thread*     --- thread implementation with mutex, semaphore
 - - - *Time*       --- time utils including Timer, DataTime
 - - - *Types*      --- basic types like Int, Point, SPtr, SO3, SE3, VecParament, Random .etc
 - - - *Utils*      --- some other utils for string and so on
 - -  **network**  --- network transmission framework for IP/TCP, UDP (pi_base [system] [poco])
 - - - *Utils*      --- some other utils for string and so on
 - -  **hardware** --- inplementation of UART, GPS, IMU and joystick, etc. (pi_base)
 - - - *UART*       --- UART implementation
 - - - *GPS*        --- GPS implementation
 - - - *IMU*        --- IMU implementation
 - - - *Joystick*   --- Joystick implementation
 - -  **cv**	   --- some computer vision utils based on OpenCV (pi_base opencv [cvd] [ffmpeg])
 - - - *Camera*     --- Camera and Undistorter
 - -  **gui**      --- display things (Qt QGLviewer)
 - - - *gl*         --- opengl display framework based on Qt and OpenGL (QGLViewer), including some basic objects
 - - - *widgets*    --- some costum widgets for Qt
 - -  **lua**	   --- the lua warper of some APIs (lua)
- apps      --- source code of examples
- scripts   --- compile scripts
- Thirdparty--- some other librarys PIL depends on

##2. Resources and Compilation on Linux
------------------------------------------------------------------------------
###2.1. Resources
  * Download the latest code with: 
    * Git: 
    
            git clone https://github.com/zdzhaoyong/PIL

###2.2. Dependency
**OpenCV** : sudo apt-get install libopencv-dev 

**Qt** : sudo apt-get install build-essential g++ libqt4-core libqt4-dev libqt4-gui qt4-doc qt4-designer 

**QGLViewer** : sudo apt-get install libqglviewer-dev libqglviewer2 

**Boost** : sudo apt-get install libboost1.48-all-dev

###2.2. Compilation
As we develop the whole project on Linux, only compilation *Makefile* for linux is provided. If you are using linux systems, it can be compiled with one command:

    cd PIL;make

The compilation should be ok in most circumstances and once not, you may need to modified the file *scripts/LIBS_CONFIG.linux* and config the library dependences manually.
##3. Usage and examples
Lots of examples are provided in folder apps, cd to the app folder you wanna and input *make run* to excute the application.

##4. Contact
------------------------------------------------------------------------------
If you have any problem, or you have some suggestions for this code, please contact Yong Zhao by zd5945@126.com, thank you very much!

