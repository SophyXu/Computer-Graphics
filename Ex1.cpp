#include <gl\glut.h>

void redraw()
{//��ɫ100����ɫ110����ɫ001,��ɫ010����ɫ101
  glClear(GL_COLOR_BUFFER_BIT);  
  glBegin(GL_TRIANGLE_FAN);
  glColor3f(1, 1, 1);   //���Ķ�����Ϊ��ɫ
  glVertex3f(0, 0, 0.5);
	
  glColor3f(1, 0, 0);
  glVertex3f(0, 0.5, 0.5);	
	
  glColor3f(1, 0, 0);
  glVertex3f(0.1, 0.3, 0.5);

  glColor3f(1, 0, 1);
  glVertex3f(0.4, 0.3, 0.5);

  glColor3f(1, 0, 1);
  glVertex3f(0.25, 0, 0.5);

  glColor3f(1, 0, 1);
  glVertex3f(0.4, -0.3, 0.5);

  glColor3f(0, 0, 1);
  glVertex3f(0.1, -0.3, 0.5);

  glColor3f(0, 1, 1);
  glVertex3f(0, -0.5, 0.5);

  glColor3f(0, 1, 1);
  glVertex3f(-0.1, -0.3, 0.5);

  glColor3f(0, 1, 0);
  glVertex3f(-0.4, -0.3, 0.5);

  glColor3f(0, 1, 0);
  glVertex3f(-0.25, 0, 0.5);

  glColor3f(1, 1, 0);
  glVertex3f(-0.4, 0.3, 0.5);

  glColor3f(1, 1, 0);
  glVertex3f(-0.1, 0.3, 0.5);

  glColor3f(1, 0, 0);
  glVertex3f(0, 0.5, 0.5);

  glEnd();
  glutSwapBuffers();
}

int main (int argc,  char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  int windowHandle
        = glutCreateWindow("Simple GLUT App");

  glutDisplayFunc(redraw);

  glutMainLoop();

  return 0;
}
