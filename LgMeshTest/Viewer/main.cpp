#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Window.h"

#include <iostream>
#include <vector>

#include "core/PolygonMesh.h"

void initGL( void );

int main( int argc, char** argv )
{
  if ( argc < 2 ) {
    std::cerr<<"Usage : "<<argv[0]<<" input.obj"<<std::endl;
    return -1;
  }
  std::cerr<<argv[1]<<std::endl;

  LgMesh::PolygonMesh mesh;
  mesh.read(argv[1]);

  //// instantiate 4 vertex handles
  //LgMesh::PolygonMesh::Vertex v0,v1,v2,v3;

  //// add 4 vertices
  //v0 = mesh.add_vertex(LgMesh::Vec3(0,0,0));
  //v1 = mesh.add_vertex(LgMesh::Vec3(1,0,0));
  //v2 = mesh.add_vertex(LgMesh::Vec3(0,1,0));
  //v3 = mesh.add_vertex(LgMesh::Vec3(0,0,1));

  //// add 4 triangular faces
  //mesh.add_triangle(v0,v1,v3);
  //mesh.add_triangle(v1,v2,v3);
  //mesh.add_triangle(v2,v0,v3);
  //mesh.add_triangle(v0,v2,v1);

  mesh.update_face_normals();
  mesh.update_vertex_normals();

  std::cout << "vertices: " << mesh.n_vertices() << std::endl;
  std::cout << "edges: "    << mesh.n_edges()    << std::endl;
  std::cout << "faces: "    << mesh.n_faces()    << std::endl;

  //if ( !mesh.read(argv[1]) ) return -2;
  Eigen::Vector3d bmin(-1, -1, -1);
  Eigen::Vector3d bmax(1, 1, 1);
  //mesh.getBoundingBox(bmin, bmax);
  Camera camera;
  camera.init( bmin, bmax );


  if ( glfwInit() == GL_FALSE ) return -1;
  Window win( 640, 480, "hello world" );
  if ( !win ) return -1;
  GLFWwindow* window = win.getWindow();

  initGL();
  while ( !glfwWindowShouldClose( window ) ) {

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    double fov, zNear, zFar;
    camera.getPerspective( fov, zNear, zFar );
    gluPerspective( fov, win.getAspectRatio(),  zNear, zFar );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    Eigen::Vector3d eye, center, up;
    camera.getLookAt( eye, center, up );
    gluLookAt( eye.x(), eye.y(), eye.z(), center.x(), center.y(), center.z(), up.x(), up.y(), up.z() );

    //オブジェクトの宙鮫
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    LgMesh::PolygonMesh::Face_iterator   fit;
    LgMesh::PolygonMesh::Vertex_around_face_circulator vfc, vfc_end;
    LgMesh::PolygonMesh::Face_attribute<LgMesh::Vec3> f_normals = mesh.get_face_attribute<LgMesh::Vec3>("f:normal");

    for (fit = mesh.faces_begin(); fit != mesh.faces_end(); ++fit)
    {
      glBegin(GL_TRIANGLES);
      LgMesh::Vec3 n = f_normals[*fit];
      glNormal3d(n.x(), n.y(), n.z());

      vfc = mesh.vertices(*fit);
      vfc_end = vfc;

      do 
      {
        LgMesh::Vec3 vp = mesh.position((*vfc));
        glVertex3d ( vp.x(), vp.y(), vp.z());
      } while (++vfc != vfc_end);

      glEnd();

    }
    glfwSwapBuffers( window );


    //マウスイベントの函誼
    glfwWaitEvents();
    double oldx, oldy, newx, newy;
    win.getMousePosition( oldx, oldy, newx, newy );
    if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_1 ) != GLFW_RELEASE ) {
      camera.rotate( oldx, oldy, newx, newy );
    }
  }
  return 0;
}

void initGL( void )
{
  glEnable( GL_DEPTH_TEST );
  glClearColor( 0,0,1,1 );
  glPointSize( 2.0 );

  static float light0_ambient[] =  {0.1f, 0.1f, 0.1f, 1.0f};
  static float light0_diffuse[] =  {1.0f, 1.0f, 1.0f, 0.0f};
  static float light0_position[] = {0.0f, 0.0f,1000.0f, 0.0f};
  static float light0_specular[] = {0.4f, 0.4f, 0.4f, 1.0f};

  glLightfv( GL_LIGHT0, GL_AMBIENT, light0_ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, light0_specular );
  glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
  glLightModelf( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

  glEnable( GL_LIGHT0 );
  glEnable( GL_LIGHTING );

}