#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <CMATH>
#include <math.h>
#include <iostream>

using namespace std;

#define DEG2RAD(a) (a * 0.0174532925)

int WIDTH = 1280;
int HEIGHT = 720;

float playerX = 5.0f;
float playerY = 1.5f;
float playerAngle = 0.0f; // Initial angle

int mouseX;
int mouseY;

bool isFP = true;

GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector3f
{
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(const Vector3f& v)
	{
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(const Vector3f& v)
	{
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n)
	{
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n)
	{
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit()
	{
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v)
	{
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};


class Camera
{
public:
	Vector3f eye, center, up;

	Camera(float eyeX, float eyeY , float eyeZ , float centerX , float centerY , float centerZ , float upX , float upY , float upZ )
	{
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d)
	{
		Vector3f right = Vector3f(1,0,0);
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d)
	{
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d)
	{
		Vector3f view = Vector3f(0,0,1);
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a)
	{
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}


	void rotateY(float a)
	{
		// Calculate the distance from the eye to the center
		Vector3f distance = center - eye;

		// Rotate the distance vector around the global Y-axis by angle 'a'
		float newX = cos(DEG2RAD(a)) * distance.x - sin(DEG2RAD(a)) * distance.z;
		float newZ = sin(DEG2RAD(a)) * distance.x + cos(DEG2RAD(a)) * distance.z;

		// Update the center position using the rotated distance vector
		center.x = eye.x + newX;
		center.z = eye.z + newZ;
	}



	void rotateZ(float a)
	{
		Vector3f right = up.cross(center - eye).unit();
		up = up * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = up.cross(center - eye);
		center = eye + right;
	}

	void look()
	{
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z);
	}

	void topView()
	{
		eye = Vector3f(0.0385149, 1.5629, 0.0452763);
		center = Vector3f(0.0372463, 0.563339, 0.0156568);
		up = Vector3f(0.999985, -0.00142483, 0.0052516);
	}

	void sideView()
	{
		eye = Vector3f(-0.0723678, 1.13253, 1.53654);
		center = Vector3f(-0.0330424, 0.584856, 0.700776);
		up = Vector3f(0.0257412, 0.836688, -0.547069);
	}

	void frontView()
	{
		eye = Vector3f(-1.19593, 1.22582, 0.0287551);
		center = Vector3f(-0.430711, 0.582224, 0.0137389);
		up = Vector3f(0.643747, 0.764794, 0.0260657);
	}

	void sideView2()
	{
		eye = Vector3f(0.0642788, 1.20623, -1.83474);
		center = Vector3f(0.0254846, 0.747269, -0.947131);
		up = Vector3f(0.0217775, 0.887678, 0.459949);
	}

	void backView()
	{
		eye = Vector3f(1.7142, 1.17294, 0.0201332);
		center = Vector3f(0.784413, 0.805003, 0.0305215);
		up = Vector3f(-0.368009, 0.928631, -0.047049);
	}
};

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_explorer;

Camera explorerCameraFP = Camera(5.00365, 1.80362, 1.65072,
	4.95831, 1.80926, 2.64968,
	-0.0360427, 0.999324, -0.00727399);
Camera explorerCameraTP = Camera(5.02084, 2.89375, -0.585861, 5.01413, 2.42989, 0.300026, 0.0146593, 0.885766, 0.4639);

// Textures
GLTexture tex_ground;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void setupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (isFP) {
		explorerCameraFP.look();
	}
	else {
		explorerCameraTP.look();
	}
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	RenderGround();


	// Draw Tree Model
	glPushMatrix();
	glTranslatef(-10, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-30, 0, -10);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-45, 0, -15);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20, 0, -25);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(playerX, 1.5, playerY);
	glRotated(playerAngle, 0, 1,0);
	model_explorer.Draw();
	glPopMatrix();

	// Draw house Model
	glPushMatrix();
	glRotatef(90.f, 1, 0, 0);
	model_house.Draw();
	glPopMatrix();


	//sky box
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();


	glutSwapBuffers();

	cout << explorerCameraFP.eye.x << " " << explorerCameraFP.eye.y << " " << explorerCameraFP.eye.z << '\n';
	cout << explorerCameraFP.center.x << " " << explorerCameraFP.center.y << " " << explorerCameraFP.center.z << '\n';
	cout << explorerCameraFP.up.x << " " << explorerCameraFP.up.y << " " << explorerCameraFP.up.z << '\n';
	
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y) {
	float moveSpeed = 0.1f; // Adjust the speed as needed
	float rotationAngle = 5.0f; // Adjust the rotation angle as needed

	switch (button) {
	case 'w':
		playerY += moveSpeed;
		playerAngle = 0.0f; // Set angle for upward movement
		explorerCameraFP.moveZ(moveSpeed);
		explorerCameraTP.moveZ(moveSpeed);
		break;
	case 'd':
		playerX -= moveSpeed;
		playerAngle = -90.0f; // Set angle for left movement
		explorerCameraFP.moveX(-moveSpeed);
		explorerCameraTP.moveX(-moveSpeed);
		//explorerCameraFP.rotateY(90.0f);
		break;
	case 's':
		playerY -= moveSpeed;
		playerAngle = 180.0f; // Set angle for downward movement
		explorerCameraFP.moveZ(-moveSpeed);
		explorerCameraTP.moveZ(-moveSpeed);
		break;
	case 'a':
		playerX += moveSpeed;
		playerAngle = 90.0f; // Set angle for right movement
		explorerCameraFP.moveX(moveSpeed);
		explorerCameraTP.moveX(moveSpeed);
		break;
	case 'u':
		explorerCameraFP.moveZ(moveSpeed);
		break;
	case 'o':
		explorerCameraFP.moveZ(-moveSpeed);
		break;
	case '1':
		isFP = true;
		break;
	case '2':
		isFP = false;
		break;
	case 27: // Escape key
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void Special(int key, int x, int y)
{
	float a = 1.0;

	switch (key)
	{
	case GLUT_KEY_UP:
		explorerCameraFP.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		explorerCameraFP.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		explorerCameraFP.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		explorerCameraFP.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMotion(int x, int y) {
	// Sensitivity adjustment for smoother rotation
	float sensitivity = 0.1f;

	// Calculate change in mouse position
	int dx = x - mouseX;
	int dy = y - mouseY;

	// Calculate rotation angles based on mouse movement
	float angleX = dx * sensitivity;
	float angleY = dy * sensitivity;

	// Get the direction vector from player to camera
	Vector3f direction = explorerCameraTP.eye - Vector3f(playerX, 1.5, playerY);

	// Rotate the direction vector around the player
	Vector3f rotatedDir = direction;
	rotatedDir.x = direction.x * cos(DEG2RAD(angleX)) - direction.z * sin(DEG2RAD(angleX));
	rotatedDir.z = direction.x * sin(DEG2RAD(angleX)) + direction.z * cos(DEG2RAD(angleX));
	
	// Calculate the new camera position after rotation
	explorerCameraTP.eye = Vector3f(playerX, 1.5, playerY) + rotatedDir;

	// Update the initial mouse position
	mouseX = x;
	mouseY = y;

	glutPostRedisplay(); // Re-draw scene
}


void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Store the initial mouse position when left button is clicked
		mouseX = x;
		mouseY = y;
	}
}




//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/tree1.3DS");
	model_explorer.Load("Models/explorer/workerman3DS.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(Special);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	//glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}