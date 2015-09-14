#SDLNGL
![alt tag](http://nccastaff.bournemouth.ac.uk/jmacey/GraphicsLib/Demos/SDLNGL.png)

This is the simplest SDL / NGL demo using a basic SDL2.0 window and core profile OpenGL context

Note that this .pro file contains the calls to add the output of sdl2-config to the build using
```C
`QMAKE_CXXFLAGS+=$$system(sdl2-config  --cflags)`

`message(output from sdl2-config --cflags added to CXXFLAGS= $$QMAKE_CXXFLAGS)`

`LIBS+=$$system(sdl2-config  --libs)`

`message(output from sdl2-config --libs added to LIB=$$LIBS)`
```
This is different from the Qt versions of the .pro file

This demo has been modified to work with glsl 330 and OpenGL 3.1 as default. If you need a higher OpenGL version modify main.cpp and change the following lines
```C++
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

```
changing the major and minor version numbers.