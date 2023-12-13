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

#define EPSILON 0.00000001

void handleMovement();

bool win = false;
bool startMenu = true;
bool isPlayerFalling = false;
float fallingAnimSpeed = 0.03f;
float playerFallingCoord = 0.0f;
bool firstLevel = true;
bool moveLeft, moveRight, moveForward, moveBackward;
int constant = 4;
float resetJumpDelay = 40;
float jumpDelay = resetJumpDelay;
float teleportPosX = -125.233, teleportPosY = 48.8299;
float teleport2PosX = -162.88, teleport2PosY = 99.7334;
float teleportWidth = 2, teleportHeight = 2;
bool sailingRock = false;
float sailingRockX = 78.5, sailingRockY = 108.1;
float lavaYCoord = -2, lavaAnimSpeed = 0.02, lavaAnimSign = 1;

ISoundEngine* engine;

float cameraPosX = 0, cameraPosY = 0;

int WIDTH = 1920;
int HEIGHT = 1080;

int cntStatue;
float cntDragon = 0;
float statueAngle[50];
float statueAngleSpeed[50];
float statuePos[50][3];
float statueFallingPos[50];
int statueFallDir[50];
float statueEndPoint[50][3];
int cntRock;
float rockPos[100][4];
bool isDead = false;
float followDragonX = 0, followDragonY = 0;
float followDragonAngle = 180;
float rotateFollowDragonKeyboard = 0;
int cntTrees = 0;
int cntGems = 0;


struct Point {
	float x, y;
	Point(float _x = 0, float _y = 0) {
		x = _x;
		y = _y;
	}
};

bool isVertexInsidePolygon(const Point& vertex, const std::vector<Point>& polygon) {
	// Check if a vertex is inside a polygon using the point-in-polygon algorithm.
	// This is the same algorithm we discussed earlier for point-in-polygon testing.

	int n = polygon.size();
	double x = vertex.x;
	double y = vertex.y;
	bool inside = false;

	for (int i = 0; i < n; i++) {
		double x1 = polygon[i].x;
		double y1 = polygon[i].y;
		double x2 = polygon[(i + 1) % n].x;
		double y2 = polygon[(i + 1) % n].y;

		if ((y1 > y) != (y2 > y) && x < (x2 - x1) * (y - y1) / (y2 - y1) + x1) {
			inside = !inside;
		}
	}

	return inside;
}

bool doPolygonsIntersect(const std::vector<Point>& polygonA, const std::vector<Point>& polygonB) {
	for (int i = 0; i < polygonA.size(); i++) {
		if (isVertexInsidePolygon(polygonA[i], polygonB)) {
			return true;
		}
	}

	return false;
}

vector<Point> polygon1 = { Point(80, 97), Point(80, 118), Point(-50, 116), Point(-50, 79) };
vector<Point> polygon2 = { Point(-50, 82), Point(-50, 116), Point(-126, 114), Point(-126, 92) };

float cameraSpeedX = 0.1f, cameraSpeedY = 0.001f;
float playerX = 50;
float playerY = 0;
float playerAngle = 180.0f; // Initial angle
float rotatePlayerKeyboard = 0;
float keyPos = 1, keyAdd = 0.005, keyRotation = 0;
float coinPos = 0, coinAdd = 0.001, coinRotation = 0;
float curRock = 0;
bool keyTaken = false, allKeysLoaded = false;
bool keyLoaded2 = false;
bool key_taken[4] = { false, false, false, false };
bool key_loaded[4] = { false, false, false, false };
float acceleration = 0;
bool fallingSoundOn = false;


// Define the position of the single statue
float statueX = 18.0; // Update with the actual x position of the statue
float statueZ = 0.0; // Update with the actual z position of the statue

// Define fixed positions for palm trees (adjust as desired)
float gemPositions[100][3];

vector<bool> gemExists(100, true); // Array to track gem existence

// Array to hold tree positions
const int numTrees = 100;
float treePositions[numTrees][3];


// Define the number of trees and the grid parameters
int cnt = 0;


float playerBoundingRadius = 0.5f;
float objectBoundingRadius = 0.5f;

float yLook = 1.5f, yLookFP = 0;

int score[4];

int mouseX = WIDTH / 2;
int mouseY = HEIGHT / 2;
int keyID = -1;

int timer = 0;

bool isFP = true;
bool firstTime = true;
float coinPositions[50][3];
vector<bool> coinExists(50, true);
float crystalPositions[3][2];
vector<bool> crystalExists(3, true);
int cntCoins = 0;
bool playerIsFalling = true;
bool enableFalling = true;

float dragonAngle = 0;

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

	double length()
	{
		return sqrt(x * x + y * y + z * z);
	}
};


class Camera
{
public:
	Vector3f eye, center, up;

	Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
	{
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d)
	{
		Vector3f right = Vector3f(1, 0, 0);
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
		Vector3f view = Vector3f(0, 0, 1);
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
		followDragonAngle += -a;
		refresh();
	}

	void updateYCenterTP(float a)
	{
		yLook += a;
		refresh();
	}

	void updateYCenterFP(float a)
	{
		yLookFP += a;
	}

	void reset() {
		eye = Vector3f(20,5 , 20);
		center = Vector3f(0, 0, 0);
		up = Vector3f(0, 1, 0);
	}

	void refresh()
	{
		eye = Vector3f(playerX - sin(DEG2RAD(cameraPosX)) * 3, 2, playerY + cos(DEG2RAD(cameraPosY)) * 3);
		center = Vector3f(playerX, yLook, playerY);
	}

	void resetFP()
	{
		eye = Vector3f(playerX, playerFallingCoord + 2.3, playerY);
		center = Vector3f(playerX + sin(DEG2RAD(cameraPosX)) * 3, yLookFP + playerFallingCoord + 2.3, playerY - cos(DEG2RAD(cameraPosY)) * 3);
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

float dotProduct(const Vector3f& v1, const Vector3f& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float distancePointToLine(Vector3f& point, Vector3f& linePoint1, Vector3f& linePoint2) {
	// Calculate the direction vector of the line
	Vector3f lineDirection = linePoint2 - linePoint1;

	// Calculate the vector from one line point to the given point
	Vector3f fromLinePoint = point - linePoint1;

	// Calculate the projection of the vector onto the line direction
	float projection = dotProduct(fromLinePoint, lineDirection) / dotProduct(lineDirection, lineDirection);

	// Check if the projection is outside the bounds of the line segment
	if (projection < 0.0f) {
		// The projection is before the start of the line segment
		return fromLinePoint.length(); // Return the distance to the start point
	}
	else if (projection > 1.0f) {
		// The projection is after the end of the line segment
		Vector3f fromLinePoint2 = point - linePoint2;
		return fromLinePoint2.length(); // Return the distance to the end point
	}
	else {
		// The projection is within the bounds of the line segment
		Vector3f projectedPoint = linePoint1 + lineDirection * projection;
		return (point - projectedPoint).length(); // Return the distance to the projected point
	}
}

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

float lightFlicker = 0.0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree[3];
Model_3DS model_explorer[21];
Model_3DS model_gem[4];
Model_3DS model_statue;
Model_3DS model_gate_level1;
Model_3DS model_gate_level2;
Model_3DS model_pond;
Model_3DS model_key1, model_key_taken1, model_key_loaded1;
Model_3DS model_key2, model_key_taken2, model_key_loaded2;
Model_3DS model_key[4];
Model_3DS model_rock[10];
Model_3DS model_coin[4];
Model_3DS model_crystal;
Model_3DS model_portal[4];
Model_3DS model_dragon[31];
Model_3DS model_dragon2[31];
Model_3DS model_dragon3[34];
Model_3DS model_forest;


Camera explorerCameraFP = Camera(playerX, playerFallingCoord + 2.3, playerY,
	playerX + sin(DEG2RAD(cameraPosX)) * 3, yLookFP + playerFallingCoord + 2.3, playerY - cos(DEG2RAD(cameraPosY)) * 3,
	0, 1, 0);
Camera explorerCameraTP = Camera(playerX - sin(DEG2RAD(cameraPosX)) * 3, 2, playerY + cos(DEG2RAD(cameraPosY)) * 3,
	playerX, yLook, playerY, 0, 1, 0);

int dir[] = { 0, 90, 180, 270 };

int currentDir = 0;

// Textures
GLTexture tex_ground, tex_vortex, tex_sea, tex_cave_ground, tex_wallpaper_exit, tex_wallpaper_level2, tex_wallpaper_start, tex_wallpaper_win, tex_wallpaper_lose, tex_score;
int wallpaper = 0;
bool playSound = false, soundPlayed = false;

GLfloat lightPosX = 50.0, lightPosY = -50.0, lightAddX = -0.1, lightAddY = 0.1;

//=======================================================================
// Animation Function
//=======================================================================

void Anim(){
	if (lightPosX > 50) {
		lightAddX = -0.1;
	}
	if (lightPosX < -50) {
		lightAddX = 0.1;
	}
	lightPosX += lightAddX;

	if (lightPosY > 50) {
		lightAddY = -0.1;
	}
	if (lightPosY < -50) {
		lightAddY = 0.1;
	}
	lightPosY += lightAddY;

	cntDragon += 0.3;
	dragonAngle += 0.3;
	if (cntDragon > 30) cntDragon = 0;
	if (keyPos >= 1.4) {
		keyAdd = -0.005;
	}
	if (keyPos <= 0.6) {
		keyAdd = 0.005;
	}
	keyPos += keyAdd;
	keyRotation += 1;

	if (coinPos >= 0.3) {
		coinAdd = -0.001;
	}
	if (coinPos <= -0.3) {
		coinAdd = 0.001;
	}
	coinPos += coinAdd;
	coinRotation += 1;
	
	/*lightFlicker += 0.1;
	if (lightFlicker >= 1) {
		lightIntensity[0] = 0.7;
		lightIntensity[1] = 0.7;
		lightIntensity[2] = 0.7;
	}*/

	glutPostRedisplay();
}

void Timer(int value) {

	timer++;
	
	if (timer % 35 == 0 && !firstLevel) {
		engine->play2D("Sounds/dragon.mp3");
	}

	glutPostRedisplay();
	glutTimerFunc(1000, Timer, 0);
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
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void setupLights() {


	GLfloat lmodel_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat l0Diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l0Spec[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat l0Ambient[] = { 0.6f, 0.6f, 0.6f,	1.0f };
	GLfloat l0Position[] = {lightPosX, 5.0f, lightPosY, true };
	GLfloat l0Direction[] = { 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
	//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0);
	//glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);
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

	//InitLightSource();

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
	glVertex3f(-100, 0, -100);
	glTexCoord2f(5, 0);
	glVertex3f(100, 0, -100);
	glTexCoord2f(5, 5);
	glVertex3f(100, 0, 100);
	glTexCoord2f(0, 5);
	glVertex3f(-100, 0, 100);
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

	glTranslatef(0, 1.5, 0);
	glRotated(90, 0, 1, 0);
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
	gluPerspective(60, (WIDTH + 0.0) / HEIGHT, 0.001, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (isFP) {
		explorerCameraFP.resetFP();
		explorerCameraFP.look();
	}
	else {
		explorerCameraTP.refresh();
		explorerCameraTP.look();
	}
}

void setupCamera2()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (WIDTH + 0.0) / HEIGHT, 0.001, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	explorerCameraTP.reset();
	explorerCameraTP.look();
}

// Function to generate a random float between min and max
float randomFloat(float min, float max) {
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}

void drawGem(float x, float z, int index, int type) {
	if (gemExists[index]) {
		// Draw 5 gems at fixed but scattered locations on the ground
		glPushMatrix();
		glTranslated(0, -0.2, 0);
		glTranslatef(x, keyPos, z);
		glRotated(keyRotation, 0, 1, 0);
		glScalef(0.03, 0.03, 0.03);
		model_gem[type].Draw();
		glPopMatrix();
	}
}


//=======================================================================
// Display Function
//=======================================================================

void drawGems(float centerX, float centerY, float radius , float type) {
	for (int i = 0, x = -radius + centerX; i < 11; ++i, x += 3) {
		float y = sqrt(radius*radius - (x - centerX) * (x - centerX)) + centerY;
		drawGem(x, y, cntGems, type);
		gemPositions[cntGems][0] = x;
		gemPositions[cntGems][1] = y;
		gemPositions[cntGems++][2] = type;
		y = -sqrt(radius*radius - (x - centerX) * (x - centerX)) + centerY;
		drawGem(x, y, cntGems, type);
		gemPositions[cntGems][0] = x;
		gemPositions[cntGems][1] = y;
		gemPositions[cntGems++][2] = type;
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
		if (i == cnt / constant) {
			glPushMatrix();
			glScaled(0.15, 0.23, 0.15);
			model_explorer[i].Draw();
			glPopMatrix();
		}
	}
	if (keyTaken) {
		glPushMatrix();
		glTranslatef(-0.55, 0.4, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 1, 0);
		if (key_taken[0])
			model_key[0].Draw();
		if (key_taken[1])
			model_key[1].Draw();
		if (key_taken[2])
			model_key[2].Draw();
		if (key_taken[3])
			model_key[3].Draw();

		glPopMatrix();
	}


	glPopMatrix();
}
void drawStatues() {

	// green statue
	glPushMatrix();
	glTranslatef(-15, 0, 15);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(0.5, 1, 0.5);
	model_statue.Draw();
	glPopMatrix();

	if (key_loaded[0]) {
		glPushMatrix();
		glTranslatef(-1.165, 1.3, 0);
		glRotatef(180, 1, 0, 0);
		glRotatef(-50, 0, 0, 1);
		model_key[0].Draw();
		glPopMatrix();
	}
	glPopMatrix();


	// redstatue
	glPushMatrix();
	glTranslatef(-15, 0, -15);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(0.5, 1, 0.5);
	model_statue.Draw();
	glPopMatrix();

	if (key_loaded[1]) {
		glPushMatrix();
		glTranslatef(-1.165, 1.3, 0);
		glRotatef(180, 1, 0, 0);
		glRotatef(-50, 0, 0, 1);
		model_key[1].Draw();
		glPopMatrix();
	}
	glPopMatrix();


	//blue statue
	glPushMatrix();
	glTranslatef(-30, 0, 5);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(0.5, 1, 0.5);
	model_statue.Draw();
	glPopMatrix();

	if (key_loaded[2]) {
	glPushMatrix();
	glTranslatef(-1.165, 1.3, 0);
	glRotatef(180, 1, 0, 0);
	glRotatef(-50, 0, 0, 1);
	model_key[2].Draw();
	glPopMatrix();
	}
	glPopMatrix();


	// yellow statue
	glPushMatrix();
	glTranslatef(-30, 0, -5);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(0.5, 1, 0.5);
	model_statue.Draw();
	glPopMatrix();

	if (key_loaded[3]) {
	glPushMatrix();
	glTranslatef(-1.165, 1.3, 0);
	glRotatef(180, 1, 0, 0);
	glRotatef(-50, 0, 0, 1);
	model_key[3].Draw();
	glPopMatrix();
	}
	glPopMatrix();
}
void drawGate() {
	glPushMatrix();
	glTranslatef(0, 0, 0);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	model_gate_level1.Draw();
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

void drawSky(GLuint tex, int radius) {
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
	glVertex3f(-300, lavaYCoord, -300);
	glTexCoord2f(5, 0);
	glVertex3f(300, lavaYCoord, -300);
	glTexCoord2f(5, 5);
	glVertex3f(300, lavaYCoord, 300);
	glTexCoord2f(0, 5);
	glVertex3f(-300, lavaYCoord, 300);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void drawRock() {

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
	rockPos[cntRock][0] = -62;
	rockPos[cntRock][1] = 50;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 2;

	glPushMatrix();
	glTranslated(-74, -0.8, 50);
	glScaled(0.1, 0.1, 0.1);
	model_rock[5].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -74;
	rockPos[cntRock][1] = 50;
	rockPos[cntRock][2] = 1.85;
	rockPos[cntRock++][3] = 2.25;

	glPushMatrix();
	glTranslated(-86, -0.8, 50);
	glScaled(0.1, 0.1, 0.1);
	model_rock[7].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -86;
	rockPos[cntRock][1] = 50;
	rockPos[cntRock][2] = 2;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(-105, -0.25, 50);
	glScaled(1, 0.2, 1);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -105;
	rockPos[cntRock][1] = 50;
	rockPos[cntRock][2] = 2.4 * 4.4;
	rockPos[cntRock++][3] = 2.4 * 4.4;

	glPushMatrix();
	glTranslated(-50, 0, 100);
	glScaled(2.1, 2.1, 2.1);
	model_rock[8].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(sailingRockX, -0.8, sailingRockY);
	glScaled(0.1, 0.1, 0.1);
	model_rock[5].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = sailingRockX;
	rockPos[cntRock][1] = sailingRockY;
	rockPos[cntRock][2] = 1.85;
	rockPos[cntRock++][3] = 2.25;

	cntStatue = 0;
	statuePos[cntStatue][0] = 45;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 114;
	statueFallDir[cntStatue] = -1;
	statueEndPoint[cntStatue][0] = 45;
	statueEndPoint[cntStatue][1] = statueFallingPos[cntStatue] - 1.5 - sin(DEG2RAD(statueAngle[cntStatue] - 90)) * 21.5;
	statueEndPoint[cntStatue][2] = 114 + cos(DEG2RAD(statueAngle[cntStatue] - 90)) * 21.5;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3, 3, 3);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	statuePos[cntStatue][0] = 15;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 91.3;
	statueFallDir[cntStatue] = 1;
	statueEndPoint[cntStatue][0] = 15;
	statueEndPoint[cntStatue][1] = statueFallingPos[cntStatue] - 1.5 - sin(DEG2RAD(statueAngle[cntStatue] - 90)) * 30;
	statueEndPoint[cntStatue][2] = 91.3 + cos(DEG2RAD(statueAngle[cntStatue] - 90)) * 30;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3.7, 3.7, 3.7);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	statuePos[cntStatue][0] = -20;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 112;
	statueFallDir[cntStatue] = -1;
	statueEndPoint[cntStatue][0] = -20;
	statueEndPoint[cntStatue][1] = statueFallingPos[cntStatue] - 1.5 - sin(DEG2RAD(statueAngle[cntStatue] - 90)) * 33;
	statueEndPoint[cntStatue][2] = 112 + cos(DEG2RAD(statueAngle[cntStatue] - 90)) * 33;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(4, 4, 4);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	statuePos[cntStatue][0] = -55;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 87.8;
	statueFallDir[cntStatue] = 1;
	statueEndPoint[cntStatue][0] = -55;
	statueEndPoint[cntStatue][1] = statueFallingPos[cntStatue] - 1.5 - sin(DEG2RAD(statueAngle[cntStatue] - 90)) * 33;
	statueEndPoint[cntStatue][2] = 87.8 + cos(DEG2RAD(statueAngle[cntStatue] - 90)) * 33;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(4, 4, 4);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	statuePos[cntStatue][0] = -100;
	statuePos[cntStatue][1] = -1.5 + statueFallingPos[cntStatue];
	statuePos[cntStatue][2] = 110;
	statueFallDir[cntStatue] = -1;
	statueEndPoint[cntStatue][0] = -100;
	statueEndPoint[cntStatue][1] = statueFallingPos[cntStatue] - 1.5 - sin(DEG2RAD(statueAngle[cntStatue] - 90)) * 21.5;
	statueEndPoint[cntStatue][2] = 110 + cos(DEG2RAD(statueAngle[cntStatue] - 90)) * 21.5;
	glPushMatrix();
	glTranslated(statuePos[cntStatue][0], statuePos[cntStatue][1], statuePos[cntStatue][2]);
	glScaled(3, 3, 3);
	glRotated(90, 0, 1, 0);
	glRotated(statueAngle[cntStatue++], 0, 0, 1);
	model_statue.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-142, -0.25, 100);
	glScaled(1, 0.2, 1);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -142;
	rockPos[cntRock][1] = 100;
	rockPos[cntRock][2] = 2.4 * 4.4;
	rockPos[cntRock++][3] = 2.4 * 4.4;


	/////////////////////////////////////////////////////////
	/////// right side
	/////////////////////////////////////////////////////////
	glPushMatrix();
	glTranslated(-5, -0.25, -32);

	glPushMatrix();
	glTranslated(-22, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -27;
	rockPos[cntRock][1] = -32;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-32, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -37;
	rockPos[cntRock][1] = -32;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-42, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -47;
	rockPos[cntRock][1] = -32;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-52, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -57;
	rockPos[cntRock][1] = -32;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPushMatrix();
	glTranslated(-62, -0.25, 0);
	glScaled(0.1, 0.1, 0.1);
	model_rock[0].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -67;
	rockPos[cntRock][1] = -32;
	rockPos[cntRock][2] = 6;
	rockPos[cntRock++][3] = 4.2;

	glPopMatrix();

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
	glTranslated(2, -0.25, -19);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 2;
	rockPos[cntRock][1] = -19;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(2, -0.25, -32);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = 2;
	rockPos[cntRock][1] = -32;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;

	glPushMatrix();
	glTranslated(-10, -0.25, -32);
	glScaled(0.2, 0.2, 0.2);
	model_rock[1].Draw();
	glPopMatrix();
	rockPos[cntRock][0] = -10;
	rockPos[cntRock][1] = -32;
	rockPos[cntRock][2] = 2.4;
	rockPos[cntRock++][3] = 2.4;



}
void drawCoin(float x, float z, Model_3DS coin) {
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
	if (crystalExists[0]) {
		glPushMatrix();
		glTranslated(-105, 0.3, 45);
		glScaled(0.07, 0.07, 0.07);
		model_crystal.Draw();
		glPopMatrix();
		crystalPositions[0][0] = -105;
		crystalPositions[0][1] = 45;
	}
	if (crystalExists[1]) {
		glPushMatrix();
		glTranslated(-142, 0.3, 98);
		glScaled(0.07, 0.07, 0.07);
		model_crystal.Draw();
		glPopMatrix();
		crystalPositions[1][0] = -142;
		crystalPositions[1][1] = 98;
	}
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
			if (statueAngleSpeed[i] == 0)
				engine->play2D("Sounds/statue.mp3");
			statueAngleSpeed[i] = statueFallDir[i] * 0.5;
		}
	}
	fallStatue();
}

void fallPlayer() {
	if (!playerIsFalling)return;

	if (!fallingSoundOn) {
		fallingSoundOn = true;
		engine->play2D("Sounds/fall.wav");
	}
	playerFallingCoord -= 0.03;

	if (playerFallingCoord <= -3) {
		fallingSoundOn = false;
		playerFallingCoord = 0;
		playerX = 50;
		playerY = 0;
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
// Function to check collision between the player and coins
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

//=======================================================================
// Function to check collision between the player and crystals
//=======================================================================
bool checkCollisionCrystals(float playerX, float playerY) {
	if (firstLevel)return false;
	for (int i = 0; i < 2; ++i) {
		if (crystalExists[i]) {
			float crystalX = crystalPositions[i][0];
			float crystalZ = crystalPositions[i][1];
			float distance = sqrt((playerX - crystalX) * (playerX - crystalX) + (playerY - crystalZ) * (playerY - crystalZ));
			if (distance < playerBoundingRadius + objectBoundingRadius) {
				crystalExists[i] = false;
				engine->play2D("Sounds/item-pick-up.mp3", false);
				engine->play2D("Sounds/energyflow.wav", false);
				return true;
			}
		}
	}
	return false; // No collision detected
}
void drawSquare(float x, float y, float width, float height)
{
	glPushMatrix();
	glTranslatef(x, 1, y);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(width, 0, 0);
	glVertex3f(width, 0, height);
	glVertex3f(0, 0, height);
	glEnd();
	glPopMatrix();
}
void drawSquare() {

	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex3f(80, 0, 97);
	glVertex3f(80, 0, 118);
	glVertex3f(-50, 0, 116);
	glVertex3f(-50, 0, 79);
	glEnd();
	glPopMatrix();
}
void drawSquare2()
{
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex3f(-50, 0, 82);
	glVertex3f(-50, 0, 116);
	glVertex3f(-126, 0, 114);
	glVertex3f(-126, 0, 92);
	glEnd();
	glPopMatrix();
}
void checkCollisionFallingStatue()
{
	if (!sailingRock)return;
	for (int i = 0; i < cntStatue; i++) {
		Vector3f player = Vector3f(playerX, 2.2, playerY);
		Vector3f startPos = Vector3f(statuePos[i][0], statuePos[i][1], statuePos[i][2]);
		Vector3f endPos = Vector3f(statueEndPoint[i][0], statueEndPoint[i][1], statueEndPoint[i][2]);
		if (distancePointToLine(player, startPos, endPos) <= 5) {
			isDead = true;
			engine->play2D("Sounds/hit.wav");	
		}
	}
}
void drawPortal() {
	if (!crystalExists[0]) {
		glPushMatrix();
		glTranslated(-125, -1, 50);
		glRotated(-90, 0, 1, 0);
		model_portal[0].Draw();
		glPopMatrix();
	}
	if (!crystalExists[1]) {
		glPushMatrix();
		glTranslated(-163, -1, 100);
		glRotated(-90, 0, 1, 0);
		model_portal[0].Draw();
		glPopMatrix();
	}
}
void drawGateLv2() {
	if (!crystalExists[0] && !crystalExists[1]) {
		glPushMatrix();
		glTranslated(-70, 1, -32);
		glRotated(90, 0, 1, 0);
		glScaled(0.05, 0.05, 0.05);
		model_gate_level2.Draw();
		glPopMatrix();
	}
}

//=======================================================================
// Function to check collision between the player and gems
//=======================================================================
bool checkCollisionGem() {
	if (!firstLevel)return false;
	for (int i = 0; i < cntGems; ++i) {
		if (gemExists[i]) {
			float gemX = gemPositions[i][0];
			float gemZ = gemPositions[i][1];
			float distance = sqrt((playerX - gemX) * (playerX - gemX) + (playerY - gemZ) * (playerY - gemZ));

			if (distance < playerBoundingRadius + objectBoundingRadius) {
				gemExists[i] = false;
				score[(int) gemPositions[i][2]]++;
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
	if (!firstLevel || keyTaken)return false;
	float distance = sqrt((playerX - 50) * (playerX - 50) + (playerY - 50) * (playerY - 50));
	if (distance < 2) {
		if (score[0] == 22) {
			engine->play2D("Sounds/item-pick-up.mp3", false);
			key_taken[0] = true;
			keyTaken = true;
			return true;
		}
		return false;
	}
	distance = sqrt((playerX + 50) * (playerX + 50) + (playerY - 50) * (playerY - 50));
	if (distance < 2) {
		if (score[2] == 22) {
			engine->play2D("Sounds/item-pick-up.mp3", false);
			key_taken[2] = true;
			keyTaken = true;
			return true;
		}
		return false;
	}
	distance = sqrt((playerX - 50) * (playerX - 50) + (playerY + 50) * (playerY + 50));
	if (distance < 2) {
		if (score[1] == 22) {
			engine->play2D("Sounds/item-pick-up.mp3", false);
			key_taken[1] = true;
			keyTaken = true;
			return true;
		}
		return false;
	}
	distance = sqrt((playerX + 50) * (playerX + 50) + (playerY + 50) * (playerY + 50));
	if (distance < 2) {
		if (score[3] == 22) {
			engine->play2D("Sounds/item-pick-up.mp3", false);
			key_taken[3] = true;
			keyTaken = true;
			return true;
		}
		return false;
	}
	return false; // No collision detected
}
void drawDragon() {
	
	glPushMatrix();
	glTranslated(-50, 15, -70);
	glRotated(dragonAngle, 0, 1, 0);
	glTranslated(-100, 0, 0);
	glScaled(10, 10, 10);
	model_dragon[(int)cntDragon].Draw();
	glPopMatrix();


	/*glPushMatrix();
	glTranslated(-50, 5, -70);
	glRotated(dragonAngle, 0, 1, 0);
	glTranslated(-100, 0, 0);
	glScaled(10, 10, 10);
	model_dragon2[(int)cntDragon].Draw();
	glPopMatrix();*/

	glPushMatrix();
	glTranslated(0, 40, -70);
	glRotated(dragonAngle, 0, 1, 0);
	glTranslated(-100, 0, 0);
	glScaled(1000, 1000, 1000);
	model_dragon3[(int)cntDragon].Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(followDragonX, 2, followDragonY);
	glRotated(followDragonAngle + rotateFollowDragonKeyboard, 0, 1, 0);
	glScaled(0.4, 0.4, 0.4);
	model_dragon[(int)cntDragon].Draw();
	glPopMatrix();
}
void drawForest(float posX, float posY, float scale) {
	glPushMatrix();
	glTranslated(posX, -3, posY);
	glScaled(scale, scale, scale);
	model_forest.Draw();
	glPopMatrix();

	treePositions[cntTrees][0] = -24.1 + posX;
	treePositions[cntTrees][1] = -14.2 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = -29 + posX;
	treePositions[cntTrees][1] = 8 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = -28.1 + posX;
	treePositions[cntTrees][1] = -35.1 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = -45.6 + posX;
	treePositions[cntTrees][1] = -13.3 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = -6.2 + posX;
	treePositions[cntTrees][1] = -27.6 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = -45 + posX;
	treePositions[cntTrees][1] = 18.9 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = 24 + posX;
	treePositions[cntTrees][1] = -10.6 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = 16.5 + posX;
	treePositions[cntTrees][1] = -31.8 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = -29.5 + posX;
	treePositions[cntTrees][1] = 31.2 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = -6.78 + posX;
	treePositions[cntTrees][1] = 26.25 + posY;
	treePositions[cntTrees++][2] = scale / 3;

	treePositions[cntTrees][0] = 1.19 + posX;
	treePositions[cntTrees][1] = 47.08 + posY;
	treePositions[cntTrees++][2] = scale / 3;
	
}
void drawKeys() {

	//green key   && !keyTaken && !keyLoaded2
	if (score[0] == 22 && !key_taken[0] && !key_loaded[0]) {
	glPushMatrix();
	glTranslatef(50, keyPos, 50);
	glRotatef(keyRotation, 0, 1, 0);
	glScalef(1.5, 1.5, 1.5);
	model_key[0].Draw();
	glPopMatrix();
	}
	
	//red key  && !keyTaken && !keyLoaded
	if (score[1] == 22 && !key_taken[1] && !key_loaded[1]) {
	glPushMatrix();
	glTranslatef(50, keyPos, -50);
	glRotatef(keyRotation, 0, 1, 0);
	glScalef(1.5, 1.5, 1.5);
	model_key[1].Draw();
	glPopMatrix();
	}

	//blue key  && !keyTaken && !keyLoaded2
	if (score[2] == 22 && !key_taken[2] && !key_loaded[2]) {
	glPushMatrix();
	glTranslatef(-50, keyPos, 50);
	glRotatef(keyRotation, 0, 1, 0);
	glScalef(1.5, 1.5, 1.5);
	model_key[2].Draw();
	glPopMatrix();
	}

	//yellow key  && !keyTaken && !keyLoaded
	if (score[3] == 22 && !key_taken[3] && !key_loaded[3]) {
	glPushMatrix();
	glTranslatef(-50, keyPos, -50);
	glRotatef(keyRotation, 0, 1, 0);
	glScalef(1.5, 1.5, 1.5);
	model_key[3].Draw();
	glPopMatrix();
	}
}

//=======================================================================
// Function to check collision between the player and statue2
//=======================================================================

bool checkCollisionStatue2(float playerX, float playerY) {
	if (!firstLevel)return false;
	float distance = sqrt((playerX + 15) * (playerX + 15) + (playerY - 15) * (playerY - 15));
	if (distance  < 3) {
		if (key_taken[0]) {
			engine->play2D("Sounds/unlock.wav", false);
			keyTaken = false;
			key_taken[0] = false;
			key_loaded[0] = true;
			return true;
		}
		return false;
	}
	distance = sqrt((playerX + 15) * (playerX + 15) + (playerY + 15) * (playerY + 15));
	if (distance < 3) {
		if (key_taken[1]) {
			engine->play2D("Sounds/unlock.wav", false);
			keyTaken = false;
			key_taken[1] = false;
			key_loaded[1] = true;
			return true;
		}
		return false;
	}
	distance = sqrt((playerX + 30) * (playerX + 30) + (playerY - 5) * (playerY - 5));
	if (distance < 3) {
		if (key_taken[2]) {
			engine->play2D("Sounds/unlock.wav", false);
			keyTaken = false;
			key_taken[2] = false;
			key_loaded[2] = true;
			return true;
		}
		return false;
	}
	distance = sqrt((playerX + 30) * (playerX + 30) + (playerY + 5) * (playerY + 5));
	if (distance < 3) {
		if (key_taken[3]) {
			engine->play2D("Sounds/unlock.wav", false);
			keyTaken = false;
			key_taken[3] = false;
			key_loaded[3] = true;
			return true;
		}
		return false;
	}
	if (key_loaded[0] && key_loaded[1] && key_loaded[2] && key_loaded[3] && !allKeysLoaded) {
		allKeysLoaded = true;
		engine->play2D("Sounds/energyflow.wav", false);
	}
	return false; 
}
void drawWallpaper() {
	glPushMatrix();

	glTranslatef(0, 0, 0);
	glRotated(45 ,0, 1, 0);
	glRotated(-10, 1, 0, 0);
	glScaled(2.65, 2.65, 2.65);


	float PORTAL_WIDTH = 16.0 , PORTAL_HEIGHT = 9.0;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	if (wallpaper == 0) {
		glBindTexture(GL_TEXTURE_2D, tex_wallpaper_start.texture[0]);
	}
	else if (wallpaper == 1) {
		glBindTexture(GL_TEXTURE_2D, tex_wallpaper_level2.texture[0]);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, tex_wallpaper_exit.texture[0]);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-PORTAL_WIDTH / 2, -PORTAL_HEIGHT / 2,0 );
	glTexCoord2f(1.0, 0.0); glVertex3f(PORTAL_WIDTH / 2, -PORTAL_HEIGHT / 2, 0);
	glTexCoord2f(1.0, 1.0); glVertex3f(PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2, 0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2, 0);
	glEnd();
	/*glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);*/
	glEnable(GL_LIGHTING);

	glPopMatrix();
}
void drawWallpaperWin() {
	glPushMatrix();

	glTranslatef(0, 0, 0);
	glRotated(45 ,0, 1, 0);
	glRotated(-10, 1, 0, 0);
	glScaled(3.7, 3.7, 3.7);


	float PORTAL_WIDTH = 16.0 , PORTAL_HEIGHT = 9.0;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, tex_wallpaper_win.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-PORTAL_WIDTH / 2, -PORTAL_HEIGHT / 2,0 );
	glTexCoord2f(1.0, 0.0); glVertex3f(PORTAL_WIDTH / 2, -PORTAL_HEIGHT / 2, 0);
	glTexCoord2f(1.0, 1.0); glVertex3f(PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2, 0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2, 0);
	glEnd();
	/*glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);*/
	glEnable(GL_LIGHTING);

	glPopMatrix();
}
void drawScore() {
	glPushMatrix();
	glTranslated(playerX - sin(DEG2RAD(cameraPosX + 180)) * 3, 2, playerY + cos(DEG2RAD(cameraPosY + 180)) * 3);
	
	// playerX - sin(DEG2RAD(cameraPosX)) * 3, 2, playerY + cos(DEG2RAD(cameraPosY)) * 3
	float PORTAL_WIDTH = 1.743, PORTAL_HEIGHT = 1;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, tex_score.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-PORTAL_WIDTH / 2, -PORTAL_HEIGHT / 2, 0);
	glTexCoord2f(1.0, 0.0); glVertex3f(PORTAL_WIDTH / 2, -PORTAL_HEIGHT / 2, 0);
	glTexCoord2f(1.0, 1.0); glVertex3f(PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2, 0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2, 0);
	glEnd();
	/*glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);*/
	glEnable(GL_LIGHTING);

	glPopMatrix();
}



void myDisplay_startMenu() {

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);*/

	drawWallpaper();

	glutSwapBuffers();
}
void myDisplay_win() {

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };

	setupCamera2();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	drawWallpaperWin();

	glutSwapBuffers();
}


void myDisplay1()
{
	setupLights();
	cout << playerX << " " << playerY << '\n';
	GLfloat lightIntensity[] = { 0.5,0.5, 0.5, 1.0f };
	GLfloat lightPosition[] = { lightPosX, 0, 0.0f, 0.0f };
	setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	cntTrees = 0;
	cntGems = 0;

	if (firstTime) {
		//engine->play2D("Sounds/story1.mp3", false);
		firstTime = false;
		playerX = -42.7548 ;
		playerY = -1.73641;
	}

	// Draw Ground
	RenderGround();
	if (allKeysLoaded)
		RenderVortex();
	float spacing = 8.0; // Adjust the spacing between trees
	// green side
	drawGems(50, 50, 15, 0);
	// red side
	drawGems(50, -50, 15, 1);
	// blue side
	drawGems(-50, 50, 15, 2);
	// yellow side
	drawGems(-50, -50, 15, 3);
	drawForest(-50, -50, 5);
	drawForest(-50, 50, 5);
	drawForest(50, -50, 5);
	drawForest(50, 50, 5);
	drawPonds();
	drawPlayer();
	drawStatues();
	drawGate();
	RenderSea();
	drawSky(tex, 300);
	drawKeys();

	handleMovement();
	checkCollisionGem();
	checkCollisionKey(playerX, playerY);
	checkCollisionStatue2(playerX, playerY);
	

	if (playerX >= -0.5 && playerX <= 0.5 && playerY >= -1.5 && playerY <= 1.5 && allKeysLoaded)
	{
		firstLevel = false;
		firstTime = true;
		playerX = 5.0f;
		playerY = 1.5f;
		playerFallingCoord = 0.0f;
		return;
	}

	if (!isPlayerFalling && (playerX > 100.5 || playerX < -100.5 || playerY > 100.5 || playerY < -100.5)) {
		isPlayerFalling = true;
		engine->play2D("Sounds/fall.wav");
	}

	if (isPlayerFalling) {
		playerFallingCoord -= fallingAnimSpeed;
		if (playerFallingCoord < -5) {
			isPlayerFalling = false;
			playerX = -42.7548;
			playerY = -1.73641;
			playerFallingCoord = 0.0f;
		}
	}

	if (playerFallingCoord <= 0 && acceleration <= 0)
		acceleration = 0;
	else
		acceleration -= 0.004;
	playerFallingCoord += acceleration;

	jumpDelay -= 0.5;
	if (jumpDelay < 0)jumpDelay = 0;

	glutSwapBuffers();

}
void myDisplay2()
{
	setupCamera();
	curRock += 0.03;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	if (firstTime) {
		//engine->play2D("Sounds/story2.mp3", false);
		constant = 4;
		firstTime = false;
		playerX = 50;
		playerY = 0;
	}

	followDragonX = playerX - sin(DEG2RAD(cameraPosX - 90));
	followDragonY = playerY + cos(DEG2RAD(cameraPosY - 90));

	RenderCaveGround();
	drawSky(tex_cave, 300);
	drawPlayer();
	drawRock();
	drawCoins();
	checkCollisionCoins(playerX, playerY);
	drawCrystals();
	checkCollisionCrystals(playerX, playerY);
	drawPortal();
	drawGateLv2();
	drawDragon();

	handleMovement();
	if (playerFallingCoord <= 0 && acceleration <= 0)
		acceleration = 0;
	else
		acceleration -= 0.004;

	if (!crystalExists[0] && teleportPosX - teleportWidth <= playerX && playerX <= teleportPosX + teleportWidth
		&& teleportPosY - teleportHeight <= playerY && playerY <= teleportPosY + teleportHeight) {
		engine->play2D("Sounds/teleport.mp3", false);
		sailingRock = true;
		playerX = sailingRockX;
		playerY = sailingRockY;
	}

	if (!crystalExists[1] && teleport2PosX - teleportWidth <= playerX && playerX <= teleport2PosX + teleportWidth
		&& teleport2PosY - teleportHeight <= playerY && playerY <= teleport2PosY + teleportHeight) {
		engine->play2D("Sounds/teleport.mp3", false);
		playerX = -110.698;
		playerY = 51.3945;
	}

	if (!crystalExists[0] && !crystalExists[1] && playerX >= -70.5 && playerX <= -69 && playerY >= -34.5 && playerY <= -29)
	{
		win = true;
	}

	if (sailingRock) {
		sailingRockX = playerX;
		sailingRockY = playerY;
	}

	playerFallingCoord += acceleration;

	if (enableFalling) {
		playerIsFalling = playerFallingCoord <= 0;
		handleFallPlayer();
	}
	else {
		playerIsFalling = false;
	}

	checkCollisionFallingStatue();
	if (isDead) {
		playerX = 50;
		playerY = 0;
		playerFallingCoord = 0.0f;
		isDead = false;
		sailingRock = false;
	}

	jumpDelay -= 0.5;
	if (jumpDelay < 0)jumpDelay = 0;
	handleFallingStatues();

	glutSwapBuffers();

}

void myDisplay(void)
{
	if (startMenu) {
		myDisplay_startMenu();
	}
	else if (win) {
		myDisplay_win();
	}
	else {
		if (firstLevel)
			myDisplay1();
		else
			myDisplay2();
	}

	if (!soundPlayed && playSound) {
		engine->play2D("Sounds/ambient.mp3", true);
		soundPlayed = true;
	}
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
		if (distance <= treePositions[i][2]) {
			// Collision detected, prevent player from moving
			return true;
		}
	}
	return false; // No collision detected
}

//=======================================================================
// Function to check collision between the player and statue
//=======================================================================

bool checkOkSailing(float x, float y)
{
	if (firstLevel)return true;
	if (!sailingRock)return true;
	vector<Point> player = { Point(x, y) };
	if (doPolygonsIntersect(player, polygon1) || doPolygonsIntersect(player, polygon2))
		return true;
	return false;

}

//=======================================================================
// Keyboard Function
//=======================================================================

void handleMovement()
{
	if (win) return;
	float moveSpeed = 0.05; // Adjust the speed as needed
	float rotationAngle = 5.0f; // Adjust the rotation angle as needed
	Vector3f view = explorerCameraTP.center - explorerCameraTP.eye;

	if (moveForward || moveBackward || moveRight || moveLeft) {
		cnt++;

		if (cnt > 20 * constant) cnt = 0;
	}

	if (moveForward && moveLeft) {
		rotateFollowDragonKeyboard = 45;
		rotatePlayerKeyboard = 45;
	}
	else if (moveForward && moveRight) {
		rotateFollowDragonKeyboard = -45;
		rotatePlayerKeyboard = -45;
	}
	else if (moveForward) {
		rotateFollowDragonKeyboard = 0;
		rotatePlayerKeyboard = 0;
	}
	else if (moveBackward && moveLeft) {
		rotateFollowDragonKeyboard = 135;
		rotatePlayerKeyboard = 135;
	}
	else if (moveBackward && moveRight) {
		rotateFollowDragonKeyboard = -135;
		rotatePlayerKeyboard = -135;
	}
	else if (moveBackward) {
		rotateFollowDragonKeyboard = 180;
		rotatePlayerKeyboard = 180;
	}
	else if (moveLeft) {
		rotateFollowDragonKeyboard = 90;
		rotatePlayerKeyboard = 90;
	}
	else if (moveRight) {
		rotateFollowDragonKeyboard = -90;
		rotatePlayerKeyboard = -90;
	}
	else {
		rotateFollowDragonKeyboard = 0;
		rotatePlayerKeyboard = 0;
	}

	if (moveForward) {
		if (!checkCollisionTree(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z) &&
			checkOkSailing(playerX + moveSpeed * view.x, playerY + moveSpeed * view.z) &&
			 (firstLevel || !firstLevel && !playerIsFalling) &&
			!isPlayerFalling) {
			playerY += moveSpeed * view.z;
			playerX += moveSpeed * view.x;
			explorerCameraFP.moveZ(moveSpeed * view.z);
			explorerCameraFP.moveX(moveSpeed * view.x);
		}
	}
	else if (moveBackward) {
		if (!checkCollisionTree(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z) &&
			checkOkSailing(playerX - moveSpeed * view.x, playerY - moveSpeed * view.z) &&
			 (firstLevel || !firstLevel && !playerIsFalling) &&
			!isPlayerFalling) {
			playerY -= moveSpeed * view.z;
			playerX -= moveSpeed * view.x;
			explorerCameraFP.moveZ(-moveSpeed * view.z);
			explorerCameraFP.moveX(-moveSpeed * view.x);
		}
	}
	if (moveRight) {
		if (!checkCollisionTree(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x) &&
			checkOkSailing(playerX - moveSpeed * view.z, playerY + moveSpeed * view.x) &&
			 (firstLevel || !firstLevel && !playerIsFalling) &&
			!isPlayerFalling) {
			playerY += moveSpeed * view.x;
			playerX -= moveSpeed * view.z;
			explorerCameraFP.moveZ(moveSpeed * view.x);
			explorerCameraFP.moveX(-moveSpeed * view.z);
		}
	}
	else if (moveLeft) {
		if (!checkCollisionTree(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x) &&
			checkOkSailing(playerX + moveSpeed * view.z, playerY - moveSpeed * view.x) &&
			(firstLevel || !firstLevel && !playerIsFalling) &&
			!isPlayerFalling) {
			playerY -= moveSpeed * view.x;
			playerX += moveSpeed * view.z;
			explorerCameraFP.moveZ(-moveSpeed * view.x);
			explorerCameraFP.moveX(moveSpeed * view.z);
		}
	}
}
void handleEnter() {
	startMenu = false;
	if (wallpaper <= 1) {
		playSound = true;
	}
	if (wallpaper == 1) {
		firstLevel = false;
	}
	else if(wallpaper == 2){
		exit(0);
	}
}

void myKeyboard(unsigned char button, int x, int y) {
	if (startMenu) {
		switch (button) {
		case 27:
			exit(0);
			break;
		case 13:
			handleEnter();
			break;
		}
	}
	else {
		float moveSpeed = 0.1f; // Adjust the speed as needed
		if (win) {
			if (button == 27) {
				exit(0);
			}
			return;
		}

		switch (button) {
		case ' ':
			if (acceleration == 0 && jumpDelay <= 0 && (firstLevel && !isPlayerFalling || !firstLevel && !playerIsFalling)) {
				engine->play2D("Sounds/jumppp22.ogg");
				acceleration = 0.13;
				jumpDelay = resetJumpDelay;
				sailingRock = false;
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
		case 'x':
			enableFalling = !enableFalling;
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
	}
	glutPostRedisplay();
}

void myKeyboardUp(unsigned char button, int x, int y) {
	if (win) return;
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
	if (startMenu) {
		engine->play2D("Sounds/menu.mp3", false);
		switch (key)
		{
		case GLUT_KEY_DOWN: wallpaper++; break;
		case GLUT_KEY_UP: wallpaper--; break;
		}
		if (wallpaper < 0) wallpaper = 0;
		if (wallpaper > 2) wallpaper = 2;
	}
	else {
		if (win) return;
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
	}
	glutPostRedisplay();
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMotion(int x, int y)
{
	if (win) return;
	int dx = x - mouseX;

	if (dx) {
		explorerCameraTP.rotateYTP(dx * cameraSpeedX);
		explorerCameraFP.rotateY(dx * cameraSpeedX);
	}

	glutWarpPointer(WIDTH / 2, HEIGHT / 2);
	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	if (win) return;
	y = HEIGHT - y;
	x = WIDTH - x;

	mouseX = x;
	mouseY = y;
}


void pressMotion(int x, int y)
{
	if (win) return;
	int dy = y - mouseY;

	if (dy)
	{
		if (isFP)
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
	if (win) return;
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
	model_gem[0].Load("Models/gems/green.3DS");
	model_gem[1].Load("Models/gems/red.3DS");
	model_gem[2].Load("Models/gems/blue.3DS");
	model_gem[3].Load("Models/gems/yellow.3DS");
	model_gate_level1.Load("Models/gate/portal.3DS");
	model_key[0].Load("Models/key/greenKey.3DS");
	model_key[1].Load("Models/key/redKey.3DS");
	model_key[2].Load("Models/key/blueKey.3DS");
	model_key[3].Load("Models/key/yellowKey.3DS");
	model_key1.Load("Models/key/redKey.3DS");
	model_key_loaded1.Load("Models/key/redKey.3DS");
	model_key_taken1.Load("Models/key/redKey.3DS");
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
	model_rock[8].Load("Models/wall/wall1.3DS");
	model_crystal.Load("Models/house/diamond.3DS");
	model_portal[0].Load("models/portal/portal1.3DS");
	model_portal[1].Load("models/portal/portal2.3DS");
	model_portal[2].Load("models/portal/portal3.3DS");
	model_portal[3].Load("models/portal/portal4.3DS");
	model_gate_level2.Load("models/gate/gate2.3DS");
	model_dragon[0].Load("models/dragon/dragon1.3DS");
	model_dragon[1].Load("models/dragon/dragon2.3DS");
	model_dragon[2].Load("models/dragon/dragon3.3DS");
	model_dragon[3].Load("models/dragon/dragon4.3DS");
	model_dragon[4].Load("models/dragon/dragon5.3DS");
	model_dragon[5].Load("models/dragon/dragon6.3DS");
	model_dragon[6].Load("models/dragon/dragon7.3DS");
	model_dragon[7].Load("models/dragon/dragon8.3DS");
	model_dragon[8].Load("models/dragon/dragon9.3DS");
	model_dragon[9].Load("models/dragon/dragon10.3DS");
	model_dragon[10].Load("models/dragon/dragon11.3DS");
	model_dragon[11].Load("models/dragon/dragon12.3DS");
	model_dragon[12].Load("models/dragon/dragon13.3DS");
	model_dragon[13].Load("models/dragon/dragon14.3DS");
	model_dragon[14].Load("models/dragon/dragon15.3DS");
	model_dragon[15].Load("models/dragon/dragon16.3DS");
	model_dragon[16].Load("models/dragon/dragon17.3DS");
	model_dragon[17].Load("models/dragon/dragon18.3DS");
	model_dragon[18].Load("models/dragon/dragon19.3DS");
	model_dragon[19].Load("models/dragon/dragon20.3DS");
	model_dragon[20].Load("models/dragon/dragon21.3DS");
	model_dragon[21].Load("models/dragon/dragon22.3DS");
	model_dragon[22].Load("models/dragon/dragon23.3DS");
	model_dragon[23].Load("models/dragon/dragon24.3DS");
	model_dragon[24].Load("models/dragon/dragon25.3DS");
	model_dragon[25].Load("models/dragon/dragon26.3DS");
	model_dragon[26].Load("models/dragon/dragon27.3DS");
	model_dragon[27].Load("models/dragon/dragon28.3DS");
	model_dragon[28].Load("models/dragon/dragon29.3DS");
	model_dragon[29].Load("models/dragon/dragon30.3DS");
	model_dragon[30].Load("models/dragon/dragon31.3DS");
	model_dragon2[0].Load("models/dragon2/dragon1.3DS");
	model_dragon2[1].Load("models/dragon2/dragon2.3DS");
	model_dragon2[2].Load("models/dragon2/dragon3.3DS");
	model_dragon2[3].Load("models/dragon2/dragon4.3DS");
	model_dragon2[4].Load("models/dragon2/dragon5.3DS");
	model_dragon2[5].Load("models/dragon2/dragon6.3DS");
	model_dragon2[6].Load("models/dragon2/dragon7.3DS");
	model_dragon2[7].Load("models/dragon2/dragon8.3DS");
	model_dragon2[8].Load("models/dragon2/dragon9.3DS");
	model_dragon2[9].Load("models/dragon2/dragon10.3DS");
	model_dragon2[10].Load("models/dragon2/dragon11.3DS");
	model_dragon2[11].Load("models/dragon2/dragon12.3DS");
	model_dragon2[12].Load("models/dragon2/dragon13.3DS");
	model_dragon2[13].Load("models/dragon2/dragon14.3DS");
	model_dragon2[14].Load("models/dragon2/dragon15.3DS");
	model_dragon2[15].Load("models/dragon2/dragon16.3DS");
	model_dragon2[16].Load("models/dragon2/dragon17.3DS");
	model_dragon2[17].Load("models/dragon2/dragon18.3DS");
	model_dragon2[18].Load("models/dragon2/dragon19.3DS");
	model_dragon2[19].Load("models/dragon2/dragon20.3DS");
	model_dragon2[20].Load("models/dragon2/dragon21.3DS");
	model_dragon2[21].Load("models/dragon2/dragon22.3DS");
	model_dragon2[22].Load("models/dragon2/dragon23.3DS");
	model_dragon2[23].Load("models/dragon2/dragon24.3DS");
	model_dragon2[24].Load("models/dragon2/dragon25.3DS");
	model_dragon2[25].Load("models/dragon2/dragon26.3DS");
	model_dragon2[26].Load("models/dragon2/dragon27.3DS");
	model_dragon2[27].Load("models/dragon2/dragon28.3DS");
	model_dragon2[28].Load("models/dragon2/dragon29.3DS");
	model_dragon2[29].Load("models/dragon2/dragon30.3DS");
	model_dragon2[30].Load("models/dragon2/dragon31.3DS");
	model_dragon3[0].Load("models/dragon3/dragon1.3DS");
	model_dragon3[1].Load("models/dragon3/dragon2.3DS");
	model_dragon3[2].Load("models/dragon3/dragon3.3DS");
	model_dragon3[3].Load("models/dragon3/dragon4.3DS");
	model_dragon3[4].Load("models/dragon3/dragon5.3DS");
	model_dragon3[5].Load("models/dragon3/dragon6.3DS");
	model_dragon3[6].Load("models/dragon3/dragon7.3DS");
	model_dragon3[7].Load("models/dragon3/dragon8.3DS");
	model_dragon3[8].Load("models/dragon3/dragon9.3DS");
	model_dragon3[9].Load("models/dragon3/dragon10.3DS");
	model_dragon3[10].Load("models/dragon3/dragon11.3DS");
	model_dragon3[11].Load("models/dragon3/dragon12.3DS");
	model_dragon3[12].Load("models/dragon3/dragon13.3DS");
	model_dragon3[13].Load("models/dragon3/dragon14.3DS");
	model_dragon3[14].Load("models/dragon3/dragon15.3DS");
	model_dragon3[15].Load("models/dragon3/dragon16.3DS");
	model_dragon3[16].Load("models/dragon3/dragon17.3DS");
	model_dragon3[17].Load("models/dragon3/dragon18.3DS");
	model_dragon3[18].Load("models/dragon3/dragon19.3DS");
	model_dragon3[19].Load("models/dragon3/dragon20.3DS");
	model_dragon3[20].Load("models/dragon3/dragon21.3DS");
	model_dragon3[21].Load("models/dragon3/dragon22.3DS");
	model_dragon3[22].Load("models/dragon3/dragon23.3DS");
	model_dragon3[23].Load("models/dragon3/dragon24.3DS");
	model_dragon3[24].Load("models/dragon3/dragon25.3DS");
	model_dragon3[25].Load("models/dragon3/dragon26.3DS");
	model_dragon3[26].Load("models/dragon3/dragon27.3DS");
	model_dragon3[27].Load("models/dragon3/dragon28.3DS");
	model_dragon3[28].Load("models/dragon3/dragon29.3DS");
	model_dragon3[29].Load("models/dragon3/dragon30.3DS");
	model_dragon3[30].Load("models/dragon3/dragon31.3DS");
	model_dragon3[31].Load("models/dragon3/dragon32.3DS");
	model_dragon3[32].Load("models/dragon3/dragon33.3DS");
	model_dragon3[33].Load("models/dragon3/dragon34.3DS");
	model_forest.Load("models/forest/forest3.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_sea.Load("Textures/sea.bmp");
	tex_vortex.Load("Textures/vortex1.bmp");
	tex_cave_ground.Load("Textures/lava1.bmp");
	tex_wallpaper_start.Load("Textures/wallpaper_start.bmp");
	tex_wallpaper_level2.Load("Textures/wallpaper_level2.bmp");
	tex_wallpaper_exit.Load("Textures/wallpaper_exit.bmp");
	tex_wallpaper_win.Load("Textures/wallpaper_win.bmp");
	tex_wallpaper_lose.Load("Textures/wallpaper_lose.bmp");
	tex_score.Load("Textures/score.bmp");
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
	glutTimerFunc(0, Timer, 0);

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