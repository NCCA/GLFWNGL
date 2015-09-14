#include <cstdlib>
#include <iostream>
#include "NGLDraw.h"
#include <ngl/NGLInit.h>
#include <GLFW/glfw3.h>
#include <boost/scoped_ptr.hpp>


// now we create an instance of our ngl class, this will init NGL and setup basic
// opengl stuff ext. When this falls out of scope the dtor will be called and cleanup
// our gl stuff
// Alas this has to be GLOBAL (Yuk) due to the use of callback in GLFW and we need to access it
// it has to be a pointer as we need to ensure we have a context before we init the class (or re-engineer)
// to have a initGL functions.
// best to make this a scoped pointer for safety,
boost::scoped_ptr<NGLDraw> scene;
// used to store the current active button for the mouse callbacks
static int s_activeButton;
// key callback
void keyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
// mouse button press callback
void mouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods);
// mouse move callback
void cursorPosCallback(GLFWwindow* _window, double _xpos, double _ypos);
// mouse wheel callback
void scrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);


int main()
{
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
  {
      return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(1024, 720, "GLFW and NGL", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  // set the key callback
  glfwSetKeyCallback(window, keyCallback);
  // set mouse callback
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  // mouse cursor move callback
  glfwSetCursorPosCallback(window, cursorPosCallback);
  // scroll wheel callback
  glfwSetScrollCallback(window, scrollCallback);
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  scene.reset(new NGLDraw);
  // now clear the screen and swap whilst NGL inits (which may take time)
  glClear(GL_COLOR_BUFFER_BIT);
  // resize the ngl to set the screen size and camera stuff
  scene->resize(1024,720);
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {

    /* Render here */
    scene->draw();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();

  }
  glfwTerminate();


}

void keyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
    if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS)
    {
      exit(EXIT_SUCCESS);
    }
    if (_key == GLFW_KEY_W && _action == GLFW_PRESS)
    {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    if (_key == GLFW_KEY_S && _action == GLFW_PRESS)
    {
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }

}

void mouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
  double x,y;
  glfwGetCursorPos	(_window,&x,&y);
  if (_action == GLFW_PRESS)
  {
    s_activeButton=_button;
    scene->mousePressEvent(_button,x,y);
  }

  else if (_action == GLFW_RELEASE)
  {
    s_activeButton=_button;
    scene->mouseReleaseEvent(_button);
  }

}

void cursorPosCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
  double x,y;
  glfwGetCursorPos(_window,&x,&y);

  scene->mouseMoveEvent(s_activeButton,_xpos,_ypos);
}

void scrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
{
  scene->wheelEvent(_xoffset,_yoffset);
}

