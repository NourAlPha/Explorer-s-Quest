#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <CMATH>
#include <math.h>
#include <iostream>

using namespace std;

float DEG2RAD(float x) {
	return x * 3.14159265 / 180.0;
}

float cameraPosX = 0, cameraPosY = 0;

int WIDTH = 1280;
int HEIGHT = 720;

float cameraSpeedX = 0.1f, cameraSpeedY = 0.001f;
float playerX = 5.0f;
float playerY = 1.5f;
float playerAngle = 180.0f; // Initial angle
float keyPos = 1, keyAdd = 0.01, keyRotation = 0;
bool keyTaken = false , keyLoaded = false;

// Define the position of the single statue
float statueX = 18.0; // Update with the actual x position of the statue
float statueZ = 0.0; // Update with the actual z position of the statue

// Define fixed positions for palm trees (adjust as desired)
float gemPositions[5][2] = {
	{-10.0, -10.0},
	{8.0, -12.0},
	{-5.0, 6.0},
	{12.0, 8.0},
	{-7.0, 15.0}
};

bool gemExists[5] = { true, true, true, true, true }; // Array to track gem existence

// Array to hold tree positions
const int numTrees = 100;
float treePositions[numTrees][2];


// Define the number of trees and the grid parameters
int cnt = 0;


float playerBoundingRadius = 0.5f; 
float objectBoundingRadius = 0.5f;

float yLook = 1.0f;

int score = 0;

int mouseX = WIDTH/2;
int mouseY = HEIGHT/2;

bool isFP = true;
bool firstTime = true;

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
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) - right * sin(DEG2RAD(a));
		right = up.cross(view);
		center = eye + view;
	}

	void rotateYTP(float a)
	{
		cameraPosX += a;
		cameraPosY += a;
		playerAngle += -a;
		refresh();
	}

	void updateYCenterTP(float a)
	{
		yLook += a;
		refresh();
	}

	void updateYCenterFP(float a)
	{
		center.y += a;
	}

	void refresh()
	{
		eye = Vector3f(playerX - sin(DEG2RAD(cameraPosX)) * 3, 2, playerY + cos(DEG2RAD(cameraPosY)) * 3);
		center = Vector3f(playerX, yLook, playerY);
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
Model_3DS model_tree[3];
Model_3DS model_explorer[21];
Model_3DS model_gem;
Model_3DS model_statue;
Model_3DS model_gate;
Model_3DS model_pond;
Model_3DS model_key, model_key_taken, model_key_loaded;


Camera explorerCameraFP = Camera(5.00352, 2.09995, 1.55395,
	5.04473, 2.11483, 0.554911,
	0, 1, 0);
Camera explorerCameraTP = Camera(playerX - sin(DEG2RAD(cameraPosX)) * 3, 2, playerY + cos(DEG2RAD(cameraPosY)) * 3,
	playerX, yLook, playerY, 0, 1, 0);

int dir[] = { 0, 90, 180, 270 };

int currentDir = 0;

// Textures
GLTexture tex_ground;

//=======================================================================
// Animation Function
//=======================================================================

void Anim()
{
	if (keyPos >= 1.3) {
		keyAdd = -0.01;
	}
	if (keyPos <= 0.7) {
		keyAdd = 0.01;
	}
	keyPos += keyAdd;
	keyRotation += 5;
	glutPostRedisplay();
}


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
	glVertex3f(-50, 0, -50);
	glTexCoord2f(5, 0);
	glVertex3f(50, 0, -50);
	glTexCoord2f(5, 5);
	glVertex3f(50, 0, 50);
	glTexCoord2f(0, 5);
	glVertex3f(-50, 0, 50);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void setupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, WIDTH / HEIGHT, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (isFP) {
		explorerCameraFP.look();
	}
	else {
		explorerCameraTP.look();
	}
}

// Function to generate a random float between min and max
float randomFloat(float min, float max) {
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}

void drawGem(float x, float z, int index) {
	if (gemExists[index]) {
		// Draw 5 gems at fixed but scattered locations on the ground
		glPushMatrix();
		glTranslatef(x, 0, z);
		glScalef(0.05, 0.05, 0.05);
		model_gem.Draw();
		glPopMatrix();
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
	/*glPushMatrix();
	glTranslatef(0, 0, 12);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();*/

	if (score == 5 && !keyTaken && !keyLoaded) {
		glPushMatrix();
		glTranslatef(0, keyPos, 0);
		glRotatef(keyRotation, 0, 1, 0);
		//glScalef(0.3, 0.3, 0.3);
		model_key.Draw();
		glPopMatrix();
	}
	float spacing = 8.0; // Adjust the spacing between trees

	// Loop to draw additional trees
	int counter = 0;
	for (int i = 0, x = -40; i < 6; ++i, x += 6) {
		float y = sqrt(225 - (x + 25) * (x + 25)) - 25;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(0.7, 0.7, 0.7);
		model_tree[0].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
		y = -sqrt(225 - (x + 25) * (x + 25)) - 25;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(0.7, 0.7, 0.7);
		model_tree[0].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
	}

	for (int i = 0, x = -40; i < 6; ++i, x += 6) {
		float y = sqrt(225 - (x + 25) * (x + 25)) + 25;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(0.7, 0.7, 0.7);
		model_tree[1].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
		y = -sqrt(225 - (x + 25) * (x + 25)) + 25;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(0.7, 0.7, 0.7);
		model_tree[1].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
	}

	for (int i = 0, x = 20; i < 6; i++, x += 5)
	{
		float y = sqrt(900 - (x - 50) * (x - 50));
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
		y = -sqrt(900 - (x - 50) * (x - 50));
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
	}

	for (int i = 0, x = 0; i < 6; i++, x += 4)
	{
		float y = sqrt(100 - (x - 10) * (x - 10)) + 35;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
		y = -sqrt(100 - (x - 10) * (x - 10)) + 35;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
	}

	glPushMatrix();
	//glRotatef(-90, 0, 1, 0);
	//glScalef(0.5, 1, 0.5);
	glTranslatef(10, 0, 35);
	model_pond.Draw();
	glPopMatrix();

	for (int i = 0, x = 0; i < 6; i++, x += 4)
	{
		float y = sqrt(100 - (x - 10) * (x - 10)) - 35;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
		y = -sqrt(100 - (x - 10) * (x - 10)) - 35;
		glPushMatrix();
		glTranslatef(x, 0, y);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = y;
	}

	glPushMatrix();
	//glRotatef(-90, 0, 1, 0);
	//glScalef(0.5, 1, 0.5);
	glTranslatef(10, 0, -35);
	model_pond.Draw();
	glPopMatrix();

	for (int i = 0, x = -45; i < 3; i++, x += 20) {
		glPushMatrix();
		glTranslatef(x, 0, 5);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = 5;
		glPushMatrix();
		glTranslatef(x, 0, -5);
		glScalef(2, 3, 2);
		model_tree[2].Draw();
		glPopMatrix();
		treePositions[counter][0] = x;
		treePositions[counter++][1] = -5;
	}

	// draw player
	glPushMatrix();
	glTranslatef(playerX, 0, playerY);
	glRotated(playerAngle, 0, 1, 0);

	for (int i = 0; i < 21; i++) {
		if (i == cnt) {
			glPushMatrix();
			glScaled(0.15, 0.23, 0.15);
			model_explorer[i].Draw();
			glPopMatrix();
		}
	}
	if (keyTaken) {
		glPushMatrix();
		glTranslatef(-0.45, 0.25, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 1, 0);
		model_key_taken.Draw();
		glPopMatrix();
	}


	glPopMatrix();


	// draw statue
	glPushMatrix();
	glTranslatef(30, 0, 7);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(0.5, 1, 0.5);
	model_statue.Draw();
	glPopMatrix();

	if (keyLoaded) {
		glPushMatrix();
		glTranslatef(-1.165, 1.3, 0);
		glRotatef(180, 1, 0, 0);
		glRotatef(-50, 0, 0, 1);
		model_key_loaded.Draw();
		glPopMatrix();
	}
	glPopMatrix();

	// draw statue
	glPushMatrix();
	glTranslatef(30, 0, -7);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(0.5, 1, 0.5);
	model_statue.Draw();
	glPopMatrix();

	if (keyLoaded) {
		glPushMatrix();
		glTranslatef(-1.165, 1.3, 0);
		glRotatef(180, 1, 0, 0);
		glRotatef(-50, 0, 0, 1);
		model_key_loaded.Draw();
		glPopMatrix();
	}
	glPopMatrix();


	// draw gate
	glPushMatrix();
	glTranslatef(45, 0, 0);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	//glScalef(0.5, 1, 0.5);
	model_gate.Draw();
	glPopMatrix();

	glPopMatrix();

	// draw pond
	glPushMatrix();
	glTranslatef(40, 0, 7);

	glPopMatrix();


	// Assuming a 30x30 ground area
	float groundWidth = 30.0;
	float groundLength = 30.0;


	for (int i = 0; i < 5; ++i) {
		drawGem(gemPositions[i][0], gemPositions[i][1], i);
	}

	////sky box
	//glPushMatrix();

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

	/*cout << explorerCameraFP.eye.x << " " << explorerCameraFP.eye.y << " " << explorerCameraFP.eye.z << '\n';
	cout << explorerCameraFP.center.x << " " << explorerCameraFP.center.y << " " << explorerCameraFP.center.z << '\n';
	cout << explorerCameraFP.up.x << " " << explorerCameraFP.up.y << " " << explorerCameraFP.up.z << '\n';
	cout << score;*/
}
//=======================================================================
// Function to check collision between the player and trees
//=======================================================================

	bool checkCollisionTree(float playerX, float playerY) {
	for (int i = 0; i < numTrees; ++i) {
		float treeX = treePositions[i][0];
		float treeZ = treePositions[i][1];
		float distance = sqrt((playerX - treeX) * (playerX - treeX) + (playerY - treeZ) * (playerY - treeZ));

		// Check if the distance between player and tree is less than the sum of their radii
		if (distance + 0.7 < playerBoundingRadius + objectBoundingRadius) {
			// Collision detected, prevent player from moving
			return true;
		}
	}
	return false; // No collision detected
}
//=======================================================================
// Function to check collision between the player and gems
//=======================================================================

	bool checkCollisionGem(float playerX, float playerY) {
		for (int i = 0; i < 5; ++i) {
			if (gemExists[i]) {
				float gemX = gemPositions[i][0];
				float gemZ = gemPositions[i][1];
				float distance = sqrt((playerX - gemX) * (playerX - gemX) + (playerY - gemZ) * (playerY - gemZ));

				if (distance < playerBoundingRadius + objectBoundingRadius) {
					// Collision detected, remove the gem
					gemExists[i] = false;
					score++;
					return true;
				}
			}
		}
		return false; // No collision detected
	}

//=======================================================================
// Function to check collision between the player and key
//=======================================================================

	bool checkCollisionKey(float playerX, float playerY) {
		float distance = sqrt((playerX ) * (playerX) + (playerY) * (playerY));
		if (distance < playerBoundingRadius + objectBoundingRadius) {
			return true;
		}
		return false; // No collision detected
	}

//=======================================================================
// Function to check collision between the player and statue
//=======================================================================

	bool checkCollisionStatue(float playerX, float playerY) {
		float distance = sqrt((playerX - 18) * (playerX - 18) + (playerY) * (playerY));
		if (distance - 0.1 < playerBoundingRadius + objectBoundingRadius) {
			return true;
		}
		return false; // No collision detected
	}

//=======================================================================
// Function to check collision between the player and statue2
//=======================================================================

	bool checkCollisionStatue2(float playerX, float playerY) {
		float distance = sqrt((playerX - 18) * (playerX - 18) + (playerY) * (playerY));
		if (distance - 0.4 < playerBoundingRadius + objectBoundingRadius) {
			return true;
		}
		return false; // No collision detected
	}

//=======================================================================
// Keyboard Function
//=======================================================================

	
/*void moveFunctionHelper(float moveSpeed)
{
	if (currentDir == 0) {
		playerAngle = 0;
		playerY += moveSpeed;
		explorerCameraFP.moveZ(moveSpeed);
		for(int i=0; i<4; i++)
			explorerCameraTP[i].moveZ(moveSpeed);
	}
	else if (currentDir == 1) {
		playerAngle = 270;
		playerX -= moveSpeed;
		explorerCameraFP.moveX(-moveSpeed);
		for (int i = 0; i < 4; i++)
			explorerCameraTP[i].moveX(-moveSpeed);
	}
	else if (currentDir == 2) {
		playerAngle = 180;
		playerY -= moveSpeed;
		explorerCameraFP.moveZ(-moveSpeed);
		for (int i = 0; i < 4; i++)
			explorerCameraTP[i].moveZ(-moveSpeed);
	}
	else {
		playerAngle = 90;
		playerX += moveSpeed;
		explorerCameraFP.moveX(moveSpeed);
		for (int i = 0; i < 4; i++)
			explorerCameraTP[i].moveX(moveSpeed);
	}
}*/

void myKeyboard(unsigned char button, int x, int y) {
	float moveSpeed = 0.1f; // Adjust the speed as needed
	float rotationAngle = 5.0f; // Adjust the rotation angle as needed
	
	Vector3f view = explorerCameraTP.center - explorerCameraTP.eye;

	switch (button) {
	case 'w':
		if (!checkCollisionTree(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z ) && 
			!checkCollisionStatue(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z) && 
			!checkCollisionGem(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z)) {
			playerY += moveSpeed * view.z;
			playerX += moveSpeed * view.x;
			explorerCameraFP.moveZ(moveSpeed * view.z);
			explorerCameraFP.moveX(moveSpeed * view.x);
			explorerCameraTP.refresh();
		}
		break;
	case 'd':
		if (!checkCollisionTree(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x) && 
			!checkCollisionStatue(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x) &&
			!checkCollisionGem(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x)) {
			playerY += moveSpeed * view.x;
			playerX -= moveSpeed * view.z;
			explorerCameraFP.moveZ(moveSpeed * view.x);
			explorerCameraFP.moveX(-moveSpeed * view.z);
			explorerCameraTP.refresh();
		}
		break;
	case 's':
		if (!checkCollisionTree(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z) && 
			!checkCollisionStatue(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z) &&
			!checkCollisionGem(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z)) {
			playerY -= moveSpeed * view.z;
			playerX -= moveSpeed * view.x;
			explorerCameraFP.moveZ(-moveSpeed * view.z);
			explorerCameraFP.moveX(-moveSpeed * view.x);
			explorerCameraTP.refresh();
		}
		break;
	case 'a':
		if (!checkCollisionTree(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x) && 
			!checkCollisionStatue(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x) &&
			!checkCollisionGem(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x)) {
			playerY -= moveSpeed * view.x;
			playerX += moveSpeed * view.z;
			explorerCameraFP.moveZ(-moveSpeed * view.x);
			explorerCameraFP.moveX(moveSpeed * view.z);
			explorerCameraTP.refresh();
		}
		break;
	case '1':
		isFP = true;
		break;
	case '2':
		isFP = false;
		break;
	case 'i':
		explorerCameraFP.moveY(moveSpeed);
		break;
	case 'k':
		explorerCameraFP.moveY(-moveSpeed);
		break;
	case 'j':
		explorerCameraFP.moveX(moveSpeed);
		break;
	case 'l':
		explorerCameraFP.moveX(-moveSpeed);
		break;
	case 'u':
		explorerCameraFP.moveZ(moveSpeed);
		break;
	case 'o':
		explorerCameraFP.moveZ(-moveSpeed);
		break;
	case 27: // Escape key
		exit(0);
		break;
	default:
		break;
	}
	if (checkCollisionKey(playerX, playerY) && score == 5 && !keyLoaded) {
		keyTaken = true;
	}
	if (checkCollisionStatue2(playerX, playerY) && keyTaken) {
		keyTaken = false;
		keyLoaded = true;
	}
	cnt++;
	if (cnt > 20) cnt = 0;
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
void myMotion(int x, int y)
{

	int dx = x - mouseX;

	if (dx) {
		explorerCameraTP.rotateYTP(dx * cameraSpeedX);
		explorerCameraFP.rotateY(dx * cameraSpeedX);
	}

	glutWarpPointer(WIDTH/2, HEIGHT/2);
	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;
	x = WIDTH - x;

	mouseX = x;
	mouseY = y;
}


void pressMotion(int x, int y)
{
	int dy = y - mouseY;

	if (dy)
	{
		explorerCameraTP.updateYCenterTP(-dy * cameraSpeedY);
		explorerCameraFP.updateYCenterFP(-dy * cameraSpeedY);
	}

	glutWarpPointer(WIDTH / 2, HEIGHT / 2);
	glutPostRedisplay();	//Re-draw scene
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
	model_tree[0].Load("Models/tree/tree1.3DS");
	model_tree[1].Load("Models/tree/tree/tree.3ds");
	model_tree[2].Load("Models/tree/tree/firtree1.3ds");
	model_explorer[0].Load("Models/explorer/charact.3DS");
	model_explorer[1].Load("Models/explorer/charact1.3DS");
	model_explorer[2].Load("Models/explorer/charact2.3DS");
	model_explorer[3].Load("Models/explorer/charact3.3DS");
	model_explorer[4].Load("Models/explorer/charact4.3DS");
	model_explorer[5].Load("Models/explorer/charact5.3DS");
	model_explorer[6].Load("Models/explorer/charact6.3DS");
	model_explorer[7].Load("Models/explorer/charact7.3DS");
	model_explorer[8].Load("Models/explorer/charact8.3DS");
	model_explorer[9].Load("Models/explorer/charact9.3DS");
	model_explorer[10].Load("Models/explorer/charact10.3DS");
	model_explorer[11].Load("Models/explorer/charact11.3DS");
	model_explorer[12].Load("Models/explorer/charact12.3DS");
	model_explorer[13].Load("Models/explorer/charact13.3DS");
	model_explorer[14].Load("Models/explorer/charact14.3DS");
	model_explorer[15].Load("Models/explorer/charact15.3DS");
	model_explorer[16].Load("Models/explorer/charact16.3DS");
	model_explorer[17].Load("Models/explorer/charact17.3DS");
	model_explorer[18].Load("Models/explorer/charact18.3DS");
	model_explorer[19].Load("Models/explorer/charact19.3DS");
	model_explorer[20].Load("Models/explorer/charact20.3DS");
	model_statue.Load("Models/house/column.3DS");
	model_pond.Load("Models/house/pond.3DS");
	model_gem.Load("Models/house/diamond.3DS");
	model_gate.Load("Models/gate/portal.3DS");
	model_key.Load("Models/key/key4.3DS");
	model_key_loaded.Load("Models/key/key4.3DS");
	model_key_taken.Load("Models/key/key4.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/sky.bmp", true);
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

	glutPassiveMotionFunc(myMotion);

	glutWarpPointer(WIDTH / 2, HEIGHT / 2);

	glutIdleFunc(Anim);

	glutMotionFunc(pressMotion);
	//glutMouseFunc(myMouse);

	glutFullScreen();

	glutSetCursor(GLUT_CURSOR_NONE);

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