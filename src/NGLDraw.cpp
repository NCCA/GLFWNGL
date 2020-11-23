#include "NGLDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Transformation.h>
const static float INCREMENT=0.01f;
const static float ZOOM=0.05f;
NGLDraw::NGLDraw()
{
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // we are creating a shader called PBR to save typos
  // in the code create some constexpr
  constexpr auto shaderProgram  = "PBR";
  constexpr auto vertexShader  = "PBRVertex";
  constexpr auto fragShader    = "PBRFragment";
  // create the shader program
  ngl::ShaderLib::createShaderProgram( shaderProgram );
  // now we are going to create empty shaders for Frag and Vert
  ngl::ShaderLib::attachShader( vertexShader, ngl::ShaderType::VERTEX );
  ngl::ShaderLib::attachShader( fragShader, ngl::ShaderType::FRAGMENT );
  // attach the source
  ngl::ShaderLib::loadShaderSource( vertexShader, "shaders/PBRVertex.glsl" );
  ngl::ShaderLib::loadShaderSource( fragShader, "shaders/PBRFragment.glsl" );
  // compile the shaders
  ngl::ShaderLib::compileShader( vertexShader );
  ngl::ShaderLib::compileShader( fragShader );
  // add them to the program
  ngl::ShaderLib::attachShaderToProgram( shaderProgram, vertexShader );
  ngl::ShaderLib::attachShaderToProgram( shaderProgram, fragShader );
  // now we have associated that data we can link the shader
  ngl::ShaderLib::linkProgramObject( shaderProgram );
  // and make it active ready to load values
  ngl::ShaderLib::use(shaderProgram);
 // We now create our view matrix for a static camera
  ngl::Vec3 from( 0.0f, 2.0f, 2.0f );
  ngl::Vec3 to( 0.0f, 0.0f, 0.0f );
  ngl::Vec3 up( 0.0f, 1.0f, 0.0f );
  // now load to our new camera
  m_view=ngl::lookAt(from,to,up);
  ngl::ShaderLib::setUniform( "camPos", from );
  // setup the default shader material and light porerties
  // these are "uniform" so will retain their values
  ngl::ShaderLib::setUniform("lightPosition",0.0f,2.0f,0.0f);
  ngl::ShaderLib::setUniform("lightColor",400.0f,400.0f,400.0f);
  ngl::ShaderLib::setUniform("exposure",2.2f);
  ngl::ShaderLib::setUniform("albedo",0.950f, 0.71f, 0.29f);

  ngl::ShaderLib::setUniform("metallic",1.02f);
  ngl::ShaderLib::setUniform("roughness",0.38f);
  ngl::ShaderLib::setUniform("ao",0.2f);
  ngl::VAOPrimitives::createTrianglePlane("floor",20,20,1,1,ngl::Vec3::up());

  ngl::ShaderLib::use(ngl::nglCheckerShader);
  ngl::ShaderLib::setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("checkOn",true);
  ngl::ShaderLib::setUniform("lightPos",0.0f,2.0f,0.0f);
  ngl::ShaderLib::setUniform("colour1",0.9f,0.9f,0.9f,1.0f);
  ngl::ShaderLib::setUniform("colour2",0.6f,0.6f,0.6f,1.0f);
  ngl::ShaderLib::setUniform("checkSize",60.0f);

}

NGLDraw::~NGLDraw()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLDraw::resize(int _w, int _h)
{
  glViewport(0,0,_w,_h);
  // now set the camera size values as the screen size has changed
  m_project=ngl::perspective(45.0f,float(_w)/_h,0.05f,350.0f);
}

void NGLDraw::draw()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // Rotation based on the mouse position for our global transform
  ngl::Transformation trans;
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(static_cast<float>(m_spinXFace));
  rotY.rotateY(static_cast<float>(m_spinYFace));
  // multiply the rotations
  m_mouseGlobalTX=rotX*rotY;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  // draw
  loadMatricesToShader();
  ngl::VAOPrimitives::draw("teapot");
  ngl::ShaderLib::use(ngl::nglCheckerShader);
  ngl::Mat4 tx;
  tx.translate(0.0f,-0.45f,0.0f);
  ngl::Mat4 MVP=m_project*m_view*m_mouseGlobalTX*tx;
  ngl::Mat3 normalMatrix=m_view*m_mouseGlobalTX;
  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP",MVP);
  ngl::ShaderLib::setUniform("normalMatrix",normalMatrix);
  ngl::VAOPrimitives::draw("floor");

}

 
void NGLDraw::loadMatricesToShader()
{
  ngl::ShaderLib::use("PBR");
  struct transform
  {
    ngl::Mat4 MVP;
    ngl::Mat4 normalMatrix;
    ngl::Mat4 M;
  };

   transform t;
   t.M=m_view*m_mouseGlobalTX;

   t.MVP=m_project*t.M;
   t.normalMatrix=t.M;
   t.normalMatrix.inverse().transpose();
   ngl::ShaderLib::setUniformBuffer("TransformUBO",sizeof(transform),&t.MVP.m_00);


}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mouseMoveEvent (int _button, float _x, float _y)
{
  if(m_rotate && _button == GLFW_MOUSE_BUTTON_LEFT)
  {
    int diffx=_x-m_origX;
    int diffy=_y-m_origY;
    m_spinXFace += static_cast<int>(0.5f * diffy);
    m_spinYFace += static_cast<int>(0.5f * diffx);
    m_origX = _x;
    m_origY = _y;
    draw();

  }
  // right mouse translate code
  else if(m_translate && _button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    int diffX = static_cast<int>(_x - m_origXPos);
    int diffY = static_cast<int>(_y - m_origYPos);
    m_origXPos=_x;
    m_origYPos=_y;
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    draw();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mousePressEvent (int _button, float _x, float _y)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_button == GLFW_MOUSE_BUTTON_LEFT)
  {
    m_origX = _x;
    m_origY = _y;
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_button == GLFW_MOUSE_BUTTON_RIGHT )
  {
    m_origXPos = _x;
    m_origYPos = _y;
    m_translate=true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mouseReleaseEvent ( int _button)
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_button == GLFW_MOUSE_BUTTON_LEFT)
  {
    m_rotate=false;
  }
  // right mouse translate mode
  if (_button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::wheelEvent(float _x, float _y)
{

  // check the diff of the wheel position (0 means no change)
  if(_y > 0)
  {
    m_modelPos.m_z+=ZOOM;
    this->draw();
  }
  else if(_y <0 )
  {
    m_modelPos.m_z-=ZOOM;
    this->draw();
  }

  // check the diff of the wheel position (0 means no change)
  if(_x > 0)
  {
    m_modelPos.m_x-=ZOOM;
    this->draw();
  }
  else if(_x <0 )
  {
    m_modelPos.m_x+=ZOOM;
    this->draw();
  }
}
//----------------------------------------------------------------------------------------------------------------------
