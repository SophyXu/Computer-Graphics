#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\glut.h>
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define C_PI 3.14159265358979
#define BITMAP_ID 0x4D42
static float year = 0, month = 0, day = 0, angle = 30;

//�������ݽṹ
typedef struct{
	int width;				//������
	int height;				//����߶�
	unsigned int texID;		//������� ID
	unsigned char * data;	//ʵ����������
}texture;

static bool first = false;

texture * sun, *earth, *moon;			//����ָ��

GLdouble distanceSunToEarth = 16;//�յؾ���  
GLdouble distanceMoonToEarth = 4;//�µؾ���  

GLdouble gzAngleEarth = 0;//������̫���Ĺ�ת�Ƕ�  
GLdouble gzAngleMoon = 0;//�����Ƶ���Ĺ�ת�Ƕ� 

GLsizei zzAngleSun = 0;//������ת�Ƕ�   
GLsizei zzAngleEarth = 0;//������ת�Ƕ�  
GLsizei zzAngleMoon = 0;//������ת�Ƕ�  

double earthX = distanceSunToEarth;
double earthY = 0;
double earthZ = 0;

double flag = 1;

double moonX = distanceSunToEarth + distanceMoonToEarth;
double moonY = -3;
double moonZ = 0;

//����λͼ��Ϊ��������
unsigned char * LoadBmpFile(char * filename, BITMAPINFOHEADER * bmpInfoHeader)
{
	FILE * file;
	BITMAPFILEHEADER bmpFileHeader;
	unsigned char * image;
	unsigned int imageIdx = 0;
	unsigned char tempRGB;

	file = fopen(filename, "rb");
	if (file == NULL)
		return 0;
	fread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);			// ��ȡ BMP �ļ�ͷ
	if (bmpFileHeader.bfType != BITMAP_ID)							// ��֤�Ƿ���һ�� BMP �ļ�
	{
		fclose(file);
		return 0;
	}
	fread(bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);			// ��λͼ��Ϣͷ
	fseek(file, bmpFileHeader.bfOffBits, SEEK_SET);					// ���ļ�ָ���Ƶ�λͼ���ݵĿ�ʼ��
	image = (unsigned char *)malloc(bmpInfoHeader->biSizeImage);	// �����ڴ��λͼ����
	if (!image)
	{
		free(image);
		fclose(file);
		return 0;
	}
	fread(image, 1, bmpInfoHeader->biSizeImage, file);					// ��ȡλͼ����
	if (image == NULL)
	{
		fclose(file);
		return 0;
	}
	// ��ת R �� B ֵ�Եõ� RGB����Ϊλͼ��ɫ��ʽ�� BGR
	for (imageIdx = 0; imageIdx < bmpInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = image[imageIdx];
		image[imageIdx] = image[imageIdx + 2];
		image[imageIdx + 2] = tempRGB;
	}
	fclose(file);
	return image;
}

//���BMP�ļ���Ϣ
texture * LoadTexFile(char * filename)
{
	BITMAPINFOHEADER texInfo;//BitMapInfoHeader����BMPͷ�ļ�,����ά�ȡ���ɫ����Ϣ
	texture * thisTexture;

	thisTexture = (texture *)malloc(sizeof(texture));
	if (thisTexture == NULL)
		return 0;

	thisTexture->data = LoadBmpFile(filename, &texInfo);	// �����������ݲ������Ч��
	if (thisTexture->data == NULL)
	{
		free(thisTexture);
		return 0;
	}

	thisTexture->width = texInfo.biWidth;				// ��������Ŀ�͸�
	thisTexture->height = texInfo.biHeight;

	glGenTextures(1, &thisTexture->texID);				// �������������

	return thisTexture;
}

BOOL LoadAllTextures()
{
	sun = LoadTexFile("sun.bmp");
	if (sun == NULL)
		return FALSE;

	glBindTexture(GL_TEXTURE_2D, sun->texID);//����󶨣�����2D����
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//������ˣ���������ӳ������أ����Բ�ֵ
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, sun->width, sun->height, GL_RGB, GL_UNSIGNED_BYTE, sun->data);
	//���������ڴ�

	earth = LoadTexFile("earth.bmp");
	if (earth == NULL)
		return FALSE;

	glBindTexture(GL_TEXTURE_2D, earth->texID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, earth->width, earth->height, GL_RGB, GL_UNSIGNED_BYTE, earth->data);

	moon = LoadTexFile("moon.bmp");
	if (moon == NULL)
		return FALSE;

	glBindTexture(GL_TEXTURE_2D, moon->texID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, moon->width, moon->height, GL_RGB, GL_UNSIGNED_BYTE, moon->data);

	return TRUE;
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_LIGHTING);//��������
	glEnable(GL_LIGHT0);//ʹ��0�Ź���
	glEnable(GL_DEPTH_TEST); //������Ȳ��ԣ����������Զ���Զ����ر���ס��ͼ��
	glEnable(GL_TEXTURE_2D);	//���ö�ά����

	//�����Դ����
	GLfloat light0_ambient[] = { 1, 1, 1, 1 };	//������
	GLfloat light0_diffuse[] = { 1, 1, 1, 1 };	//ɢ���
	GLfloat light0_position[] = { 0, 0, 0, 1 };	//��Դλ��

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);//���ó���������
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);//���ò��Ϸ���ָ��
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);//ָ��0�Ź�Դλ��

	LoadAllTextures();			//��������
}

//////////////////////////////////////////////////////////////////////////////////////////
void gltDrawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks)
{
	GLfloat drho = (GLfloat)(3.141592653589) / (GLfloat)iStacks;
	GLfloat dtheta = 2.0f * (GLfloat)(3.141592653589) / (GLfloat)iSlices;
	GLfloat ds = 1.0f / (GLfloat)iSlices;
	GLfloat dt = 1.0f / (GLfloat)iStacks;
	GLfloat t = 1.0f;
	GLfloat s = 0.0f;
	GLint i, j;

	for (i = 0; i < iStacks; i++)
	{
		GLfloat rho = (GLfloat)i * drho;
		GLfloat srho = (GLfloat)(sin(rho));
		GLfloat crho = (GLfloat)(cos(rho));
		GLfloat srhodrho = (GLfloat)(sin(rho + drho));
		GLfloat crhodrho = (GLfloat)(cos(rho + drho));

		glBegin(GL_TRIANGLE_STRIP);
		s = 0.0f;
		for (j = 0; j <= iSlices; j++)
		{
			GLfloat theta = (j == iSlices) ? 0.0f : j * dtheta;
			GLfloat stheta = (GLfloat)(-sin(theta));
			GLfloat ctheta = (GLfloat)(cos(theta));

			GLfloat x = stheta * srho;
			GLfloat y = ctheta * srho;
			GLfloat z = crho;

			glTexCoord2f(s, t);
			glNormal3f(x, y, z);
			glVertex3f(x * fRadius, y * fRadius, z * fRadius);

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;
			glTexCoord2f(s, t - dt);
			s += ds;
			glNormal3f(x, y, z);
			glVertex3f(x * fRadius, y * fRadius, z * fRadius);
		}
		glEnd();

		t -= dt;
	}
}

void drawCircle(GLfloat r)
{
	int j = 0;
	glBegin(GL_POINTS);
	for (j = 0; j<100; j++)
	{
		GLfloat theta = j * 2 * C_PI / 100;
		glVertex3f(r*cos(theta), 0.0f, r*sin(theta));
	}
	glEnd();
}

////////////////////////////////////////////////////////////////////////////////////
GLfloat mat_ambient1[] = { 1, 0, 0, 1 };
GLfloat mat_emission[] = { 1, 1, 1, 0 };
GLfloat mat_ambient2[] = { 0.4, 0.4, 0.8, 1 };
GLfloat no_emission[] = { 0, 0, 0, 1 };

void drawSun(GLdouble x0, GLdouble y0, GLdouble z0)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, sun->texID);
	glTranslatef(0, 0, -40);//������ƽ�Ƶ�ָ��λ��
	glRotatef(90, 1.0, 0.0, 0.0);//��������ת
	glRotatef(zzAngleSun, 0, 0, 1);//�Դ�
	//ָ�����ڹ��ռ���ĵ�ǰ�������ԣ���������ǰ��
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);//���ʵĻ�����ɫ
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);//���ʵ�ɢ����ɫ
	gltDrawSphere(5, 30, 40);//��������
	glPopMatrix();
}

void drawEarth(GLdouble x0, GLdouble y0, GLdouble z0)
{
//	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, earth->texID);
	glTranslatef(x0, y0, z0 - 40);//������ƽ�Ƶ�ָ��λ��
	glRotatef(90, 1.0, 0.0, 0.0);//��������ת
	glRotatef(180-20, 0, 1.0, 0.0);

	glRotatef(zzAngleEarth, 0.0, 0.0, 1.0);//��ת��б�Ƕ�
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
	gltDrawSphere(2, 30, 40);

	glPopMatrix();
}

void drawMoon(GLdouble x0, GLdouble y0, GLdouble z0)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, moon->texID);
	glTranslatef(x0, y0, z0 - 40);//������ƽ�Ƶ�ָ��λ��
	glRotatef(90.0, 1.0, 0.0, 0.0);//��������ת
	glRotatef(zzAngleMoon, 0.0, 0.0, 1.0);//�Դ�
	gltDrawSphere(0.5, 30, 10);
	glPopMatrix();
}

void reshape(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, width, height); // Reset The Current Viewport
	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard(unsigned char key, int x, int y)
{
	gzAngleEarth += C_PI / 180;	//������̫����ת1��
	earthX = (GLdouble)distanceSunToEarth * cos(gzAngleEarth);
	//�������x������
	earthZ = -1 * distanceSunToEarth * sin(gzAngleEarth);
	//�������z������,��ʱ����ת
	zzAngleSun = (zzAngleMoon + 5) % 360;//̫����ת�Ƕ�
	zzAngleEarth = (zzAngleEarth + 20) % 360;//������ת�Ƕ�
	zzAngleMoon = (zzAngleMoon + 10) % 360;//������ת�Ƕ�

	gzAngleMoon += C_PI / 180 * 13.52;
	//���������Ƶ���Ĺ�ת�Ƕ�
	moonX = earthX + distanceMoonToEarth * cos(gzAngleMoon);
	//��������x������
	moonZ = earthZ - distanceMoonToEarth * sin(gzAngleMoon);
	//��������z������
	moonY = cos(gzAngleMoon) * -3;
	day += gzAngleEarth;
	glutPostRedisplay(); //���»���ͼ��

}

void display()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//��̫�� 
	drawSun(0, 0, 0);
	//������  
	drawEarth(earthX, earthY, earthZ);
	//������  
	drawMoon(moonX, moonY, moonZ);

	glFlush();
	glutSwapBuffers();

}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("̫��ϵ");

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
