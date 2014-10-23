#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\glut.h>
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define C_PI 3.14159265358979
#define BITMAP_ID 0x4D42
static float year = 0, month = 0, day = 0, angle = 30;

//纹理数据结构
typedef struct{
	int width;				//纹理宽度
	int height;				//纹理高度
	unsigned int texID;		//纹理对象 ID
	unsigned char * data;	//实际纹理数据
}texture;

static bool first = false;

texture * sun, *earth, *moon;			//纹理指针

GLdouble distanceSunToEarth = 16;//日地距离  
GLdouble distanceMoonToEarth = 4;//月地距离  

GLdouble gzAngleEarth = 0;//地球绕太阳的公转角度  
GLdouble gzAngleMoon = 0;//月球绕地球的公转角度 

GLsizei zzAngleSun = 0;//地球自转角度   
GLsizei zzAngleEarth = 0;//地球自转角度  
GLsizei zzAngleMoon = 0;//地球自转角度  

double earthX = distanceSunToEarth;
double earthY = 0;
double earthZ = 0;

double flag = 1;

double moonX = distanceSunToEarth + distanceMoonToEarth;
double moonY = -3;
double moonZ = 0;

//调入位图作为纹理数据
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
	fread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);			// 读取 BMP 文件头
	if (bmpFileHeader.bfType != BITMAP_ID)							// 验证是否是一个 BMP 文件
	{
		fclose(file);
		return 0;
	}
	fread(bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);			// 读位图信息头
	fseek(file, bmpFileHeader.bfOffBits, SEEK_SET);					// 将文件指针移到位图数据的开始处
	image = (unsigned char *)malloc(bmpInfoHeader->biSizeImage);	// 分配内存给位图数据
	if (!image)
	{
		free(image);
		fclose(file);
		return 0;
	}
	fread(image, 1, bmpInfoHeader->biSizeImage, file);					// 读取位图数据
	if (image == NULL)
	{
		fclose(file);
		return 0;
	}
	// 反转 R 和 B 值以得到 RGB，因为位图颜色格式是 BGR
	for (imageIdx = 0; imageIdx < bmpInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = image[imageIdx];
		image[imageIdx] = image[imageIdx + 2];
		image[imageIdx + 2] = tempRGB;
	}
	fclose(file);
	return image;
}

//获得BMP文件信息
texture * LoadTexFile(char * filename)
{
	BITMAPINFOHEADER texInfo;//BitMapInfoHeader——BMP头文件,包含维度、颜色等信息
	texture * thisTexture;

	thisTexture = (texture *)malloc(sizeof(texture));
	if (thisTexture == NULL)
		return 0;

	thisTexture->data = LoadBmpFile(filename, &texInfo);	// 调入纹理数据并检查有效性
	if (thisTexture->data == NULL)
	{
		free(thisTexture);
		return 0;
	}

	thisTexture->width = texInfo.biWidth;				// 设置纹理的宽和高
	thisTexture->height = texInfo.biHeight;

	glGenTextures(1, &thisTexture->texID);				// 生成纹理对象名

	return thisTexture;
}

BOOL LoadAllTextures()
{
	sun = LoadTexFile("sun.bmp");
	if (sun == NULL)
		return FALSE;

	glBindTexture(GL_TEXTURE_2D, sun->texID);//纹理绑定，操作2D纹理
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理过滤，纹理象素映射成像素，线性插值
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, sun->width, sun->height, GL_RGB, GL_UNSIGNED_BYTE, sun->data);
	//生成纹理到内存

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

	glEnable(GL_LIGHTING);//启动光照
	glEnable(GL_LIGHT0);//使用0号光照
	glEnable(GL_DEPTH_TEST); //启用深度测试，根据坐标的远近自动隐藏被遮住的图形
	glEnable(GL_TEXTURE_2D);	//启用二维纹理

	//定义光源性质
	GLfloat light0_ambient[] = { 1, 1, 1, 1 };	//环境光
	GLfloat light0_diffuse[] = { 1, 1, 1, 1 };	//散射光
	GLfloat light0_position[] = { 0, 0, 0, 1 };	//光源位置

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);//设置场景环境光
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);//设置材料反射指数
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);//指定0号光源位置

	LoadAllTextures();			//调入纹理
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
	glTranslatef(0, 0, -40);//将星球平移到指定位置
	glRotatef(90, 1.0, 0.0, 0.0);//将球体旋转
	glRotatef(zzAngleSun, 0, 0, 1);//自传
	//指定用于光照计算的当前材质属性，用于物体前面
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);//材质的环境颜色
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);//材质的散射颜色
	gltDrawSphere(5, 30, 40);//画出球体
	glPopMatrix();
}

void drawEarth(GLdouble x0, GLdouble y0, GLdouble z0)
{
//	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, earth->texID);
	glTranslatef(x0, y0, z0 - 40);//将坐标平移到指定位置
	glRotatef(90, 1.0, 0.0, 0.0);//将球体旋转
	glRotatef(180-20, 0, 1.0, 0.0);

	glRotatef(zzAngleEarth, 0.0, 0.0, 1.0);//自转倾斜角度
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
	glTranslatef(x0, y0, z0 - 40);//将星球平移到指定位置
	glRotatef(90.0, 1.0, 0.0, 0.0);//将球体旋转
	glRotatef(zzAngleMoon, 0.0, 0.0, 1.0);//自传
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
	gzAngleEarth += C_PI / 180;	//地球绕太阳公转1度
	earthX = (GLdouble)distanceSunToEarth * cos(gzAngleEarth);
	//计算地球x轴坐标
	earthZ = -1 * distanceSunToEarth * sin(gzAngleEarth);
	//计算地球z轴坐标,逆时针旋转
	zzAngleSun = (zzAngleMoon + 5) % 360;//太阳自转角度
	zzAngleEarth = (zzAngleEarth + 20) % 360;//地球自转角度
	zzAngleMoon = (zzAngleMoon + 10) % 360;//月亮自转角度

	gzAngleMoon += C_PI / 180 * 13.52;
	//计算月球绕地球的公转角度
	moonX = earthX + distanceMoonToEarth * cos(gzAngleMoon);
	//计算月球x轴坐标
	moonZ = earthZ - distanceMoonToEarth * sin(gzAngleMoon);
	//计算月球z轴坐标
	moonY = cos(gzAngleMoon) * -3;
	day += gzAngleEarth;
	glutPostRedisplay(); //重新绘制图像

}

void display()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//画太阳 
	drawSun(0, 0, 0);
	//画地球  
	drawEarth(earthX, earthY, earthZ);
	//画月亮  
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
	glutCreateWindow("太阳系");

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
