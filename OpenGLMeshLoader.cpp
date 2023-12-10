#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <CMATH>
#include <math.h>
#include <iostream>
#include <vector>
#include <irrKlang.h>

using namespace irrklang;

using namespace std;

float DEG2RAD(float x) {
	return x * 3.14159265 / 180.0;
}

void handleMovement();

bool isPlayerFalling = false;
float fallingAnimSpeed = 0.1f;
float playerFallingCoord = 0.0f;
bool firstLevel = false;
bool moveLeft, moveRight, moveForward, moveBackward;
float resetJumpDelay = 40;
float jumpDelay = resetJumpDelay;

ISoundEngine* engine;

float cameraPosX = 0, cameraPosY = 0;

int WIDTH = 1920;
int HEIGHT = 1080;

int cntStatue;
float statueAngle[50];
float statueAngleSpeed[50];
float statuePos[50][3];
float statueFallingPos[50];
int statueFallDir[50];
int cntRock;
float rockPos[50][4];

float cameraSpeedX = 0.1f, cameraSpeedY = 0.001f;
float playerX = 50;
float playerY = 0.0;
float playerAngle = 180.0f; // Initial angle
float rotatePlayerKeyboard = 0;
float keyPos = 1, keyAdd = 0.01, keyRotation = 0;
float coinPos = 0 , coinAdd = 0.001, coinRotation = 0;
float curRock = 0;
bool keyTaken = false , keyLoaded = false;
bool keyLoaded2 = false;
float acceleration = 0;

// Define the position of the single statue
float statueX = 18.0; // Update with the actual x position of the statue
float statueZ = 0.0; // Update with the actual z position of the statue

// Define fixed positions for palm trees (adjust as desired)
float gemPositions[50][3];

vector<bool> gemExists (50 , true); // Array to track gem existence

// Array to hold tree positions
const int numTrees = 100;
float treePositions[numTrees][2];


// Define the number of trees and the grid parameters
int cnt = 0;


float playerBoundingRadius = 0.5f; 
float objectBoundingRadius = 0.5f;

float yLook = 1.5f;

int score[2];

int mouseX = WIDTH/2;
int mouseY = HEIGHT/2;
int keyID = -1;

bool isFP = true;
bool firstTime = true;
float coinPositions[50][3];
vector<bool> coinExists(50 , true);
float crystalPositions[3][2];
int cntCoins = 0;
bool playerIsFalling = true;


GLuint tex, tex_cave;
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

	void resetFP()
	{
		eye = Vector3f(playerX, 2.3, playerY);
		center = Vector3f(playerX + sin(DEG2RAD(cameraPosX)) * 3, 2.3, playerY - cos(DEG2RAD(cameraPosY)) * 3);
		up = Vector3f(0, 1, 0);
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
Model_3DS model_gem[2];
Model_3DS model_statue;
Model_3DS model_gate;
Model_3DS model_pond;
Model_3DS model_key, model_key_taken, model_key_loaded;
Model_3DS model_key2, model_key_taken2, model_key_loaded2;
Model_3DS model_rock[10];
Model_3DS model_coin[4];
Model_3DS model_crystal;


Camera explorerCameraFP = Camera(playerX, 2.3, playerY,
	playerX + sin(DEG2RAD(cameraPosX)) * 3, 2.3, playerY - cos(DEG2RAD(cameraPosY)) * 3,
	0, 1, 0);
Camera explorerCameraTP = Camera(playerX - sin(DEG2RAD(cameraPosX)) * 3, 2, playerY + cos(DEG2RAD(cameraPosY)) * 3,
	playerX, yLook, playerY, 0, 1, 0);

int dir[] = { 0, 90, 180, 270 };

int currentDir = 0;

// Textures
GLTexture tex_ground, tex_vortex, tex_sea, tex_cave_ground;

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
	
	if (coinPos >= 0.3) {
		coinAdd = -0.001;
	}
	if (coinPos <= -0.3) {
		coinAdd = 0.001;
	}
	coinPos += coinAdd;
	coinRotation += 1;
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

void RenderSea()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_sea.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-1000, -1, -1000);
	glTexCoord2f(5, 0);
	glVertex3f(1000, -1, -1000);
	glTexCoord2f(5, 5);
	glVertex3f(1000, -1, 1000);
	glTexCoord2f(0, 5);
	glVertex3f(-1000, -1, 1000);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderVortex()
{
	glPushMatrix();

	glTranslatef(45, 1.5, 0);
	glRotated(90 , 0, 1, 0);
	/*ang = fmod(ang + PORTAL_SPEED, 360);
	glTranslatef(0, PORTAL_SIDE / 2, 0);
	glRotatef(ang, 0.0f, 0.0f, 1.0f);*/

	float PORTAL_SIDE = 3.0;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, tex_vortex.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(PORTAL_SIDE / 2, -PORTAL_SIDE / 2, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-PORTAL_SIDE / 2, -PORTAL_SIDE / 2, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-PORTAL_SIDE / 2, PORTAL_SIDE / 2, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(PORTAL_SIDE / 2, PORTAL_SIDE / 2, 0);
	glEnd();
	/*glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);*/
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void setupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (WIDTH+0.0) / HEIGHT, 0.001, 1000);

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

void drawGem(float x, float z, int index , int type) {
	if (gemExists[index]) {
		// Draw 5 gems at fixed but scattered locations on the ground
		glPushMatrix();
		glTranslated(0, -0.7, 0);
		glTranslatef(x, keyPos, z);
		glRotated(keyRotation, 0, 1, 0);
		glScalef(0.02, 0.035, 0.02);
		model_gem[type].Draw();
		glPopMatrix();
	}
}


//=======================================================================
// Display Function
//=======================================================================

void drawGems() {
	int counter2 = 0;
	for (int i = 0, x = -33; i < 6; ++i, x += 3) {
		float y = sqrt(72 - (x + 25) * (x + 25)) - 25;
		drawGem(x, y, counter2, 0);
		gemPositions[counter2][0] = x;
		gemPositions[counter2++][1] = y;
		y = -sqrt(72 - (x + 25) * (x + 25)) - 25;
		drawGem(x, y, counter2, 0);
		gemPositions[counter2][0] = x;
		gemPositions[counter2++][1] = y;
	}

	for (int i = 0, x = -33; i < 6; ++i, x += 3) {
		float y = sqrt(72 - (x + 25) * (x + 25)) + 25;
		drawGem(x, y, counter2, 1);
		gemPositions[counter2][0] = x;
		gemPositions[counter2][2] = 1;
		gemPositions[counter2++][1] = y;
		y = -sqrt(72 - (x + 25) * (x + 25)) + 25;
		drawGem(x, y, counter2, 1);
		gemPositions[counter2][0] = x;
		gemPositions[counter2][2] = 1;
		gemPositions[counter2++][1] = y;
	}
}
void drawTrees() {
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
}
void drawPonds() {
	glPushMatrix();
	glTranslatef(10, 0, 35);
	model_pond.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10, 0, -35);
	model_pond.Draw();
	glPopMatrix();
}
void drawPlayer() {
	glPushMatrix();
	glTranslatef(playerX, playerFallingCoord, playerY);
	glRotated(playerAngle + rotatePlayerKeyboard, 0, 1, 0);
	glScaled(1.5, 1, 1.5);


	for (int i = 0; i < 21; i++) {
		if (i == cnt/4) {
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
		if (keyID == 0)
			model_key_taken.Draw();
		else
			model_key_taken2.Draw();
		glPopMatrix();
	}


	glPopMatrix();
}
void drawStatues() {
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
		model_key_loaded2.Draw();
		glPopMatrix();
	}
	glPopMatrix();


	glPushMatrix();
	glTranslatef(30, 0, -7);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(0.5, 1, 0.5);
	model_statue.Draw();
	glPopMatrix();

	if (keyLoaded2) {
		glPushMatrix();
		glTranslatef(-1.165, 1.3, 0);
		glRotatef(180, 1, 0, 0);
		glRotatef(-50, 0, 0, 1);
		model_key_loaded.Draw();
		glPopMatrix();
	}
	glPopMatrix();
}
void drawGate() {
	glPushMatrix();
	glTranslatef(45, 0, 0);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	model_gate.Draw();
	glPopMatrix();

	glPopMatrix();

}

void drawSpecialRock(float angle, float radius) {
	angle += 180;
	glPushMatrix();
	glTranslated(2 - sin(DEG2RAD(angle)) * radius, -3.2, 50 + cos(DEG2RAD(angle)) * radius);
	glScaled(0.2, 0.2, 0.2);
	model_rock[2].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 2 - 2 + 2 - sin(DEG2RAD(angle)) * radius;
	rockPos[cntRock][1] = 29.4 - 30 + 50 + cos(DEG2RAD(angle)) * radius;
	rockPos[cntRock][2] = 3;
	rockPos[cntRock++][3] = 2.2;

	rockPos[cntRock][0] = -3 - 2 + 2 - sin(DEG2RAD(angle)) * radius;
	rockPos[cntRock][1] = 30 - 30 + 50 + cos(DEG2RAD(angle)) * radius;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.5;

	rockPos[cntRock][0] = -7 - 2 + 2 - sin(DEG2RAD(angle)) * radius;
	rockPos[cntRock][1] = 30.6 - 30 + 50 + cos(DEG2RAD(angle)) * radius;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.7;

	rockPos[cntRock][0] = -10 - 2 + 2 - sin(DEG2RAD(angle)) * radius;
	rockPos[cntRock][1] = 31.5 - 30 + 50 + cos(DEG2RAD(angle)) * radius;
	rockPos[cntRock][2] = 1;
	rockPos[cntRock++][3] = 1;

	rockPos[cntRock][0] = 6.5 - 2 + 2 - sin(DEG2RAD(angle)) * radius;
	rockPos[cntRock][1] = 30 - 30 + 50 + cos(DEG2RAD(angle)) * radius;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.5;

	rockPos[cntRock][0] = 10.5 - 2 + 2 - sin(DEG2RAD(angle)) * radius;
	rockPos[cntRock][1] = 30.6 - 30 + 50 + cos(DEG2RAD(angle)) * radius;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.5;

	rockPos[cntRock][0] = 13.5 - 2 + 2 - sin(DEG2RAD(angle)) * radius;
	rockPos[cntRock][1] = 31.5 - 30 + 50 + cos(DEG2RAD(angle)) * radius;
	rockPos[cntRock][2] = 1;
	rockPos[cntRock++][3] = 1;
}

void drawSky(GLuint tex , int radius) {
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, radius, radius, radius);
	gluDeleteQuadric(qobj);


	glPopMatrix();
}
void RenderCaveGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_cave_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-300, -2, -300);
	glTexCoord2f(5, 0);
	glVertex3f(300, -2, -300);
	glTexCoord2f(5, 5);
	glVertex3f(300, -2, 300);
	glTexCoord2f(0, 5);
	glVertex3f(-300, -2, 300);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void drawRock(){

	cntRock = 0;
	glPushMatrix();
	glTranslated(50, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 50;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(40, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 40;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(23, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 23;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(12, -0.25, 0);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 12;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(2, -0.25, 7);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 2;
	rockPos[cntRock][1] = 7;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(2, -0.25, 20);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 2;
	rockPos[cntRock][1] = 20;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(-7, -0.25, 0);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -7;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(-22, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -22;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-32, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -32;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-42, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -42;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-52, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -52;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-62, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -62;
	rockPos[cntRock][1] = 0;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	drawSpecialRock(0, 20);

	glPushMatrix();
	glTranslated(2, -3.2, 70);
	glRotated(180, 0, 1, 0);
	glScaled(0.2, 0.2, 0.2);
	model_rock[2].Draw();
	glPopMatrix();

	rockPos[cntRock][0] = 2;
	rockPos[cntRock][1] = 29.4 + 41;
	rockPos[cntRock][2] = 3;
	rockPos[cntRock++][3] = 2.2;

	rockPos[cntRock][0] = -3;
	rockPos[cntRock][1] = 30 + 40;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.5;

	rockPos[cntRock][0] = -7;
	rockPos[cntRock][1] = 30.6 + 38.5;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.7;

	rockPos[cntRock][0] = -10;
	rockPos[cntRock][1] = 31.5 + 36.7;
	rockPos[cntRock][2] = 1;
	rockPos[cntRock++][3] = 1;

	rockPos[cntRock][0] = 6.5;
	rockPos[cntRock][1] = 30 + 40;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.5;

	rockPos[cntRock][0] = 10.5;
	rockPos[cntRock][1] = 30.6 + 39;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 1.5;

	rockPos[cntRock][0] = 13.5;
	rockPos[cntRock][1] = 31.5 + 37;
	rockPos[cntRock][2] = 1;
	rockPos[cntRock++][3] = 1;

	glPushMatrix();
	glTranslated(-20, -3.2, 50);
	glRotated(90, 0, 1, 0);
	glScaled(0.2, 0.2, 0.2);
	model_rock[2].Draw();
	glPopMatrix();

	rockPos[cntRock][0] = -20.6;
	rockPos[cntRock][1] = 49.4;
	rockPos[cntRock][2] = 2.2;
	rockPos[cntRock++][3] = 3;

	rockPos[cntRock][0] = -19.8;
	rockPos[cntRock][1] = 44.4;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -19;
	rockPos[cntRock][1] = 41.4;
	rockPos[cntRock][2] = 1.7;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -18.2;
	rockPos[cntRock][1] = 38.5;
	rockPos[cntRock][2] = 1;
	rockPos[cntRock++][3] = 1;

	rockPos[cntRock][0] = -20;
	rockPos[cntRock][1] = 54;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -19.5;
	rockPos[cntRock][1] = 58;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -18.5;
	rockPos[cntRock][1] = 61.5;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 1.5;

	glPushMatrix();
	glTranslated(20, -3.2, 50);
	glRotated(-90, 0, 1, 0);
	glScaled(0.2, 0.2, 0.2);
	model_rock[2].Draw();
	glPopMatrix();

	rockPos[cntRock][0] = -20.6 + 41.5;
	rockPos[cntRock][1] = 49.4;
	rockPos[cntRock][2] = 2.2;
	rockPos[cntRock++][3] = 3;

	rockPos[cntRock][0] = -19.8 + 40;
	rockPos[cntRock][1] = 44.4;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -19 + 38.3;
	rockPos[cntRock][1] = 41.4;
	rockPos[cntRock][2] = 1.7;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -18.2 + 36.7;
	rockPos[cntRock][1] = 38.5;
	rockPos[cntRock][2] = 1;
	rockPos[cntRock++][3] = 1;

	rockPos[cntRock][0] = -20 + 40;
	rockPos[cntRock][1] = 54;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -19.5 + 38.7;
	rockPos[cntRock][1] = 58;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 2;

	rockPos[cntRock][0] = -18.5 + 37.5;
	rockPos[cntRock][1] = 61.5;
	rockPos[cntRock][2] = 1.5;
	rockPos[cntRock++][3] = 1.5;

	glPushMatrix();
	glTranslated(2, -0.25, -7);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 2;
	rockPos[cntRock][1] = -7;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(2, -0.25, -20);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 2;
	rockPos[cntRock][1] = -20;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(2, -3.2, 50);
	glScaled(0.2, 0.2, 0.2);
	model_rock[3].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(2, -3.2, 45);
	//glScaled(0.2, 0.2, 0.2);
	model_rock[4].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(7, -3.2, 50);
	glRotated(-90, 0, 1, 0);
	//glScaled(0.2, 0.2, 0.2);
	model_rock[4].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-4, -3.2, 50);
	glRotated(90, 0, 1, 0);
	//glScaled(0.2, 0.2, 0.2);
	model_rock[4].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(2, -3.2, 55);
	glRotated(180, 0, 1, 0);
	//glScaled(0.2, 0.2, 0.2);
	model_rock[4].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-37, -0.25, 50);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -37;
	rockPos[cntRock][1] = 50;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-47, -0.25, 50);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -47;
	rockPos[cntRock][1] = 50;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-62, -0.8, 50);
	glScaled(0.1, 0.1, 0.1);
	model_rock[6].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-74, -0.8, 50);
	glScaled(0.1, 0.1, 0.1);
	model_rock[5].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-86, -0.8, 50);
	glScaled(0.1, 0.1, 0.1);
	model_rock[7].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-105, -0.25, 50);
	glScaled(1, 0.2, 1);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -105;
	rockPos[cntRock][1] = 50;
	rockPos[cntRock][2] = 2.4 * 5;
	rockPos[cntRock++][3] = 2.4 * 5;

	glPushMatrix();
	glTranslated(-50, 0, 100);
	glScaled(2.1, 2.1, 2.1);
	model_rock[2].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(75, -0.8, 120);
	glScaled(0.1, 0.1, 0.1);
	model_rock[5].Draw();
	glPopMatrix();

	cntStatue = 0;
	statuePos[cntStatue][0] = 45;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 120;
	statueFallDir[cntStatue] = -1;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3, 3, 3);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	statuePos[cntStatue][0] = 15;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 90;
	statueFallDir[cntStatue] = 1;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3, 3, 3);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	statuePos[cntStatue][0] = -20;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 105;
	statueFallDir[cntStatue] = -1;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3, 3, 3);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();
	
	statuePos[cntStatue][0] = -55;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 77;
	statueFallDir[cntStatue] = 1;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3, 3, 3);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	statuePos[cntStatue][0] = -100;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 110;
	statueFallDir[cntStatue] = -1;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3, 3, 3);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-142, -0.25, 105);
	glScaled(1, 0.2, 1);
	glRotated(10, 0, 1, 0);
	model_rock[1].Draw();
	glPopMatrix();

}
void drawCoin(float x, float z , Model_3DS coin) {
	if (coinExists[cntCoins]) {
		// Draw 5 gems at fixed but scattered locations on the ground
		glPushMatrix();
		glTranslated(x, 1.5 + coinPos, z);
		glRotated(coinRotation, 0, 1, 0);
		glScalef(0.5, 0.5, 0.5);
		coin.Draw();
		glPopMatrix();
		coinPositions[cntCoins][0] = x;
		coinPositions[cntCoins++][1] = z;
	}
	else {
		cntCoins++;
	}
}
void drawCoins() {
	cntCoins = 0;
	for (int i = 1; i < 3; i++) {
		drawCoin(50 - i * 5, 0, model_coin[3]);
	}

	for (int i = 0; i < 2; i++) {
		drawCoin(25 - i * 5, 0, model_coin[3]);
	}

	drawCoin(12, 0, model_coin[3]);
	drawCoin(2, 7, model_coin[3]);
	drawCoin(2, 20, model_coin[3]);
	drawCoin(2, 71, model_coin[3]);
	drawCoin(-7, 69, model_coin[3]);
	drawCoin(11, 69.5, model_coin[3]);
	drawCoin(-20.5, 50, model_coin[3]);
	drawCoin(-19, 41, model_coin[3]);
	drawCoin(-19.5, 59, model_coin[3]);
	drawCoin(20.5, 50, model_coin[3]);
	drawCoin(19, 41, model_coin[3]);
	drawCoin(19.5, 59, model_coin[3]);
	for (int i = 0; i < 3; i++) {
		drawCoin(-37 - i * 5, 50, model_coin[3]);
	}
	drawCoin(-62, 50, model_coin[3]);
	drawCoin(-74, 50, model_coin[3]);
	drawCoin(-86, 50, model_coin[3]);
}
void drawCrystals() {
	glPushMatrix();
	glTranslated(-105, 0.3, 45);
	glScaled(0.07, 0.07, 0.07);
	model_crystal.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-142, 0.3, 98);
	glScaled(0.07, 0.07, 0.07);
	model_crystal.Draw();
	glPopMatrix();
}

void fallStatue()
{
	for (int i = 0; i < cntStatue; i++) {
		if (abs(statueAngle[i]) >= 90) {
			statueFallingPos[i] += -0.03;
			continue;
		}
		statueAngle[i] += statueAngleSpeed[i];
	}
}

void handleFallingStatues()
{
	for (int i = 0; i < cntStatue; i++) {
		if ((playerX - statuePos[i][0]) * (playerX - statuePos[i][0]) + 
			(playerY - statuePos[i][2]) * (playerY - statuePos[i][2]) <= 400) {
			statueAngleSpeed[i] = statueFallDir[i] * 0.5;
		}
	}
	fallStatue();
}

void fallPlayer() {
	if (!playerIsFalling)return;

	playerFallingCoord -= 0.02;

	if (playerFallingCoord <= -5) {
		playerFallingCoord = 0;
		playerX = 50;
		playerY = 0;
		explorerCameraTP.refresh();
		explorerCameraFP.resetFP();
	}
}

void handleFallPlayer()
{
	for (int i = 0; i < cntRock; i++) {
		if (rockPos[i][0] - rockPos[i][2] <= playerX && playerX <= rockPos[i][0] + rockPos[i][2]
			&& rockPos[i][1] - rockPos[i][3] <= playerY && playerY <= rockPos[i][1] + rockPos[i][3]) {
			playerIsFalling = false;
		}
	}
	fallPlayer();
}

//=======================================================================
// Function to check collision between the player and gems
//=======================================================================

bool checkCollisionCoins(float playerX, float playerY) {
	if (firstLevel)return false;
	for (int i = 0; i < cntCoins; ++i) {
		if (coinExists[i]) {
			float coinX = coinPositions[i][0];
			float coinZ = coinPositions[i][1];
			float distance = sqrt((playerX - coinX) * (playerX - coinX) + (playerY - coinZ) * (playerY - coinZ));


			if (distance < playerBoundingRadius + objectBoundingRadius) {
				// Collision detected, remove the gem
				coinExists[i] = false;
				//score[(int)gemPositions[i][2]]++;
				engine->play2D("Sounds/pickup.wav", false);
				return true;
			}
		}
	}
	return false; // No collision detected
}

void myDisplay1()
{

	setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	RenderGround();
	if(keyLoaded && keyLoaded2)
		RenderVortex();
	
	handleMovement();

	if (playerX >= 43.5 && playerX <= 46.5 && playerY >= -1 && playerY <= 1 && keyLoaded && keyLoaded2)
	{
		firstLevel = false;
		playerX = 5.0f;
		playerY = 1.5f;
		playerFallingCoord = 0.0f;
		explorerCameraTP.refresh();
		explorerCameraFP.resetFP();
		return;
	}


	if (!isPlayerFalling && (playerX > 50.5 || playerX < -50.5 || playerY > 50.5 || playerY < -50.5)) {
		isPlayerFalling = true;
		engine->play2D("Sounds/fall.wav");
	}

	if (isPlayerFalling) {
		playerFallingCoord -= fallingAnimSpeed;
		if (playerFallingCoord < -5) {
			isPlayerFalling = false;
			playerX = 5.0f;
			playerY = 1.5f;
			playerFallingCoord = 0.0f;
			explorerCameraTP.refresh();
			explorerCameraFP.resetFP();
		}
	}

	if (score[0] == 12 && !keyTaken && !keyLoaded2) {
		glPushMatrix();
		glTranslatef(-25, keyPos, -25);
		glRotatef(keyRotation, 0, 1, 0);
		//glScalef(0.3, 0.3, 0.3);
		model_key.Draw();
		glPopMatrix();
	}

	if (score[1] == 12 && !keyTaken && !keyLoaded) {
		glPushMatrix();
		glTranslatef(-25, keyPos, 25);
		glRotatef(keyRotation, 0, 1, 0);
		//glScalef(0.3, 0.3, 0.3);
		model_key2.Draw();
		glPopMatrix();
	}

	float spacing = 8.0; // Adjust the spacing between trees
	drawGems();
	drawTrees();
	drawPonds();
	drawPlayer();
	drawStatues();
	drawGate();
	RenderSea();
	drawSky(tex , 150);

	glutSwapBuffers();
	
}



void myDisplay2()
{

	cout << playerX << " " << playerY << '\n';

	setupCamera();
	curRock += 0.03;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	RenderCaveGround();
	drawSky(tex_cave, 300);
	drawPlayer();
	drawRock();
	drawCoins();
	checkCollisionCoins(playerX , playerY);
	drawCrystals();

	handleMovement();
	if (playerFallingCoord <= 0 && acceleration <= 0)
		acceleration = 0;
	else
		acceleration -= 0.004;
	
	playerFallingCoord += acceleration;

	playerIsFalling = false;
	handleFallPlayer();

	jumpDelay -= 0.5;
	if (jumpDelay < 0)jumpDelay = 0;
	handleFallingStatues();



	glutSwapBuffers();

}

void myDisplay(void)
{
	myDisplay2();
}

//=======================================================================
// Function to check collision between the player and trees
//=======================================================================

bool checkCollisionTree(float playerX, float playerY) {
	if (!firstLevel)return false;
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
		if (!firstLevel)return false;
		for (int i = 0; i < 24; ++i) {
			if (gemExists[i]) {
				float gemX = gemPositions[i][0];
				float gemZ = gemPositions[i][1];
				float distance = sqrt((playerX - gemX) * (playerX - gemX) + (playerY - gemZ) * (playerY - gemZ));

				if (distance < playerBoundingRadius + objectBoundingRadius) {
					// Collision detected, remove the gem
					gemExists[i] = false;
					score[(int)gemPositions[i][2]]++;
					engine->play2D("Sounds/pickup.wav", false);
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
		if (!firstLevel)return false;
		float distance = sqrt((playerX + 25) * (playerX + 25) + (playerY + 25) * (playerY + 25));
		if (distance < playerBoundingRadius + objectBoundingRadius) {
			if (score[0] == 12) {
				keyID = 0;
			}
			return true;
		}
		distance = sqrt((playerX + 25) * (playerX + 25) + (playerY - 25) * (playerY - 25));
		if (distance < playerBoundingRadius + objectBoundingRadius) {
			if (score[1] == 12) {
				keyID = 1;
			}
			return true;
		}
		return false; // No collision detected
	}

//=======================================================================
// Function to check collision between the player and statue
//=======================================================================

	bool checkCollisionStatue(float playerX, float playerY) {
		if(!firstLevel)return false;
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
		if (!firstLevel)return false;
		float distance = sqrt((playerX - 30) * (playerX - 30) + (playerY - 7) * (playerY - 7));
		if (distance - 0.4 < playerBoundingRadius + objectBoundingRadius) {
			if(keyID == 1)
			return true;
		}
		distance = sqrt((playerX - 30) * (playerX - 30) + (playerY + 7) * (playerY + 7));
		if (distance - 0.4 < playerBoundingRadius + objectBoundingRadius) {
			if (keyID == 0)
				return true;
		}
		return false; // No collision detected
	}



//=======================================================================
// Keyboard Function
//=======================================================================

void handleMovement()
	{
		float moveSpeed = 0.05f; // Adjust the speed as needed
		float rotationAngle = 5.0f; // Adjust the rotation angle as needed
		Vector3f view = explorerCameraTP.center - explorerCameraTP.eye;

		if (moveForward || moveBackward || moveRight || moveLeft) {
			cnt++;
			if (cnt > 80) cnt = 0;
		}

		if (moveForward && moveLeft) {
			rotatePlayerKeyboard = 45;
		}
		else if (moveForward && moveRight) {
			rotatePlayerKeyboard = -45;
		}
		else if (moveForward) {
			rotatePlayerKeyboard = 0;
		}
		else if (moveBackward && moveLeft) {
			rotatePlayerKeyboard = 135;
		}
		else if (moveBackward && moveRight) {
			rotatePlayerKeyboard = -135;
		}
		else if (moveBackward) {
			rotatePlayerKeyboard = 180;
		}
		else if (moveLeft) {
			rotatePlayerKeyboard = 90;
		}
		else if (moveRight) {
			rotatePlayerKeyboard = -90;
		}
		else {
			rotatePlayerKeyboard = 0;
		}

		if (moveForward) {
			if (!checkCollisionTree(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z) &&
				!checkCollisionStatue(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z) && !playerIsFalling &&
				!checkCollisionGem(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z) && !isPlayerFalling) {
				playerY += moveSpeed * view.z;
				playerX += moveSpeed * view.x;
				explorerCameraFP.moveZ(moveSpeed * view.z);
				explorerCameraFP.moveX(moveSpeed * view.x);
				explorerCameraTP.refresh();
			}
		}
		else if(moveBackward){
			if (!checkCollisionTree(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z) &&
				!checkCollisionStatue(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z) && !playerIsFalling &&
				!checkCollisionGem(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z) && !isPlayerFalling) {
				playerY -= moveSpeed * view.z;
				playerX -= moveSpeed * view.x;
				explorerCameraFP.moveZ(-moveSpeed * view.z);
				explorerCameraFP.moveX(-moveSpeed * view.x);
				explorerCameraTP.refresh();
			}
		}
		if (moveRight) {
			if (!checkCollisionTree(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x) &&
				!checkCollisionStatue(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x) && !playerIsFalling &&
				!checkCollisionGem(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x) && !isPlayerFalling) {
				playerY += moveSpeed * view.x;
				playerX -= moveSpeed * view.z;
				explorerCameraFP.moveZ(moveSpeed * view.x);
				explorerCameraFP.moveX(-moveSpeed * view.z);
				explorerCameraTP.refresh();
			}
		}
		else if(moveLeft){
			if (!checkCollisionTree(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x) &&
				!checkCollisionStatue(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x) && !playerIsFalling &&
				!checkCollisionGem(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x) && !isPlayerFalling) {
				playerY -= moveSpeed * view.x;
				playerX += moveSpeed * view.z;
				explorerCameraFP.moveZ(-moveSpeed * view.x);
				explorerCameraFP.moveX(moveSpeed * view.z);
				explorerCameraTP.refresh();
			}
		}
	}

void myKeyboard(unsigned char button, int x, int y) {
	float moveSpeed = 0.1f; // Adjust the speed as needed
	

	switch (button) {
	case ' ':
		if (acceleration == 0 && jumpDelay <= 0 && !playerIsFalling) {
			engine->play2D("Sounds/jumppp22.ogg");
			acceleration = 0.13;
			jumpDelay = resetJumpDelay;
		}
		break;
	case 'w':
		moveForward = true;
		break;
	case 'd':
		moveRight = true;
		break;
	case 's':
		moveBackward = true;
		break;
	case 'a':
		moveLeft = true;
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
	if (checkCollisionKey(playerX, playerY) && !keyTaken) {
		if (!keyLoaded2 && keyID == 0) {
			engine->play2D("Sounds/item-pick-up.mp3");
			keyTaken = true;
		}
		if (!keyLoaded && keyID == 1) {
			engine->play2D("Sounds/item-pick-up.mp3");
			keyTaken = true;
		}
	}
	if (checkCollisionStatue2(playerX, playerY) && keyTaken) {
		keyTaken = false;
		engine -> play2D("Sounds/unlock.wav", false);
		if(keyID == 0)
			keyLoaded2 = true;
		else 
			keyLoaded = true;
		if(keyLoaded && keyLoaded2)
			engine->play2D("Sounds/energyflow.wav", false);
		keyID = -1;
	}
	glutPostRedisplay();
}

void myKeyboardUp(unsigned char button, int x, int y) {
	switch (button) {
	case 'w':
		moveForward = false;
		break;
	case 'd':
		moveRight = false;
		break;
	case 's':
		moveBackward = false;
		break;
	case 'a':
		moveLeft = false;
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
		if(isFP)
			explorerCameraFP.updateYCenterFP(-dy * cameraSpeedY);
		else
			explorerCameraTP.updateYCenterTP(-dy * cameraSpeedY);
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
	model_gem[0].Load("Models/gems/gem3.3DS");
	model_gem[1].Load("Models/gems/gem4.3DS");
	model_gate.Load("Models/gate/portal.3DS");
	model_key.Load("Models/key/redKey.3DS");
	model_key_loaded.Load("Models/key/redKey.3DS");
	model_key_taken.Load("Models/key/redKey.3DS");
	model_key2.Load("Models/key/greenKey.3DS");
	model_key_loaded2.Load("Models/key/greenKey.3DS");
	model_key_taken2.Load("Models/key/greenKey.3DS");
	model_rock[0].Load("Models/rocks/rock3/rock1.3DS");
	model_rock[1].Load("Models/rocks/rock2/rock2.3DS");
	model_rock[2].Load("Models/rocks/rock5/rock1.3DS");
	model_rock[3].Load("Models/rocks/rock6/rock3.3DS");
	model_rock[4].Load("Models/rocks/rock4/rock2.3DS");
	model_coin[0].Load("Models/coins/coin_1.3DS");
	model_coin[1].Load("Models/coins/coin_2.3DS");
	model_coin[2].Load("Models/coins/coin_3.3DS");
	model_coin[3].Load("Models/coins/New Folder (2)/coin01.3ds");
	model_rock[5].Load("Models/rocks/rock7/rock1.3DS");
	model_rock[6].Load("Models/rocks/rock7/rock2.3DS");
	model_rock[7].Load("Models/rocks/rock7/rock3.3DS");
	model_crystal.Load("Models/house/diamond.3DS");


	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_sea.Load("Textures/sea.bmp");
	tex_vortex.Load("Textures/vortex1.bmp");
	tex_cave_ground.Load("Textures/lava1.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	loadBMP(&tex_cave, "Textures/hell-sky.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	engine = createIrrKlangDevice();
	if (!engine)
		return;
	engine->play2D("Sounds/ambient.mp3", true);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);
	glutKeyboardUpFunc(myKeyboardUp);

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
	engine->drop();

}