#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <iostream>
#include<windows.h>
#include <MMSystem.h>
using namespace std;
int WIDTH = 1280;
int HEIGHT = 720;
bool lights = true;
bool won = false;
GLuint tex;
GLuint tex1;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 10000;


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Player {
public:
	Vector3f position;
	int lives;
	int score;
	int rotationangle;
	

	Player() : position(0, 0, 20), lives(5), score(0) , rotationangle(0){}

	void loseLife() {
		if (lives > 0) {
			lives--;
		}
	}

	bool isAlive() {
		return lives > 0;
	}


};

Player player;  // Create a global player object
class Camera {
public:
	Vector3f eye, center, up;
	bool isThirdPersonView = true;
	Vector3f forward = Vector3f(0, 0, 0);
	

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);

	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}


	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void updateView() {
		 // Assuming the player's forward direction is along the negative z-axis
		
		if (isThirdPersonView) {
			// Set the camera for the third person view behind and above the player
			float distanceBehind = 7.0f; // The distance behind the player
			float heightAbove = 7.0f;    // The height above the player

			// Offset the camera position to be behind the player
			eye.x = player.position.x - (forward.x * distanceBehind);
			eye.y = player.position.y + heightAbove;
			eye.z = player.position.z - (forward.z * distanceBehind);

			// The center point the camera is looking at
			// This should be a point at the player's head height or slightly lower to look straight forward
			center.x = player.position.x + (forward.x * 10.0f); // This value ensures the camera is looking forward into the scene
			center.y = player.position.y + (heightAbove / 2.0f); // Adjust this to match where the player's "eyes" would be
			center.z = player.position.z + (forward.z * 10.0f);
		}
		else {
			// Set the camera for first person view at the player's eye level
			float eyeLevel = 5.0f; // The height of the eyes in your player model

			// Position the camera at the player's eye level
			eye.x = player.position.x;
			eye.y = player.position.y + eyeLevel;
			eye.z = player.position.z;

			// The center point the camera is looking at
			// This should be a point directly in front of the player
			center.x = player.position.x + forward.x;
			center.y = player.position.y + eyeLevel;
			center.z = player.position.z + forward.z;
		}

		// Update the camera to look from eye point to center point
		gluLookAt(eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z);
	}



	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	
	}




};

Camera camera;

double intR = 0.3;
double intG = 0.3;
double intB = 0.3;

double diffR = 1;
double diffG = 1;
double diffB = 1;

void setupLights() {
	if (lights == true) {
		//cout << "true";
		GLfloat l0Diffuse[] = { 1, 1, 1, 1.0f };
		GLfloat l0Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat l0Ambient[] = { intR, intG, intB, 1.0f };
		Vector3f forward = camera.forward.unit();
		// Set the light's position to be in front of the player
		GLfloat l0Position[] = { player.position.x + forward.x * 2.0f, player.position.y + 3.0f, player.position.z + forward.z * 2.0f, 1.0f };

		glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
		glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
		glLightfv(GL_LIGHT0, GL_SPECULAR, l0Spec);

		// You can adjust the spot direction to be more suitable for your game
		GLfloat l0Direction[] = { forward.x, forward.y, forward.z };
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	} 
	else if (lights == false) {
		//cout << "false";
		GLfloat l0Diffuse[] = { diffR, diffG, diffB, 1.0f };
		GLfloat l0Spec[] = { 1.0f, 1.0f, 0.0f, 1.0f };
		GLfloat l0Ambient[] = { intR, intG, intB, 1.0f };
		GLfloat l0Position[] = { 0, 20, 5, 1 };
		GLfloat l0Direction[] = { 0, -1, 0.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
		glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
	
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);

		
	}
}






void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glFlush();
}

void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	
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


Vector playerpos(0, 0, 20);



class Coin {
public:
	Vector3f position;
	bool collected;

	Coin(float x, float y, float z) : position(x, y, z), collected(false) {}
};


class Snow {
public:
	Vector3f position;
	bool collected;
	Snow(float x, float y, float z) : position(x, y, z), collected(false) {}
};

class Gingerman {
public:
	Vector3f position;
	bool collected;

	Gingerman(float x, float y, float z) : position(x, y, z), collected(false) {}

};


int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_coin;
Model_3DS model_santa;
Model_3DS railway;
Model_3DS coin;
Model_3DS Star;
Model_3DS Giftbox;
Model_3DS snowman;
Model_3DS Ginger;
Model_3DS Snowflake;
// Textures
GLTexture tex_ground;
GLTexture tex_ground2;

GLTexture tex_sky;


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

void renderBitmapString(float x, float y, void* font, const char* string) {
	const char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
void renderBitmapString2(float x, float y, void* font, const char* string) {
	glRasterPos2f(x, y);
	for (int i = 0; string[i] != '\0'; i++) {
		glutBitmapCharacter(font, string[i]);
	}
}

void drawHealthBar(int lives, int maxLives, float x, float y, float width, float height) {
	float totalGap = width * 0.2; // Total gap space, 20% of the total width
	float usableWidth = width - totalGap; // Width that can be used for dashes
	float dashWidth = usableWidth / maxLives; // Width of each dash
	float gap = totalGap / maxLives; // Gap between each dash

	// Draw each dash
	for (int i = 0; i < lives; ++i) {
		glColor3f(0.0, 1.0, 0.0); // Green color for health
		glBegin(GL_QUADS);
		glVertex2f(x + i * (dashWidth + gap), y);
		glVertex2f(x + i * (dashWidth + gap), y + height);
		glVertex2f(x + i * (dashWidth + gap) + dashWidth, y + height);
		glVertex2f(x + i * (dashWidth + gap) + dashWidth, y);
		glEnd();
	}
}
const int NUM_COINS = 3; // Adjust as needed
Coin coins[NUM_COINS] = {
	Coin(1, 0, 7),
	Coin(-14, 0, 6),
	Coin(15, 0, -3)
};
Vector3f starPosition = Vector3f(0, 3, -21);
Vector3f Olafposition = Vector3f(0, 2, -19);

Snow snow={ Snow(0, 4, 4)};

Gingerman ginger[2] = {
	Gingerman(-14, 0, -5),
	Gingerman(14, 0, -12)
};

bool level1 = true;
bool level2 = true;

void resetLightIntensity(int value) {
	diffR = 1.0;
	diffG = 1.0;
	diffB = 1.0;
	lights = true;
	glutPostRedisplay(); // Redraw the scene with the updated light color
};



void checkCollisionsAndUpdate2() {
	Vector3f GiftPosition[3] = {
		Vector3f(1,0,-4),
		Vector3f(16,0,5),
		Vector3f(-13,0,10)

	};
	float dx = player.position.x - Olafposition.x;
	float dy = player.position.y - Olafposition.y;
	float dz = player.position.z - Olafposition.z;
	float olaf = sqrt(dx * dx + dy * dy + dz * dz);

	float Olafdistance = 3.5f;

	if (olaf < Olafdistance) {
		PlaySound(TEXT("winwin.wav"), NULL, SND_ASYNC | SND_FILENAME);
		level2 = false;

	
	}

	float collisionDistance = 2.0f;
	float snowcollisionDistance = 5.0f;
	float gingercollision = 5.0f;
	for (int i = 0; i < 3; ++i) {
		float dx = player.position.x - GiftPosition[i].x;
		float dy = player.position.y - GiftPosition[i].y;
		float dz = player.position.z - GiftPosition[i].z;
		float distance = sqrt(dx * dx + dy * dy + dz * dz);

		if (distance < collisionDistance) {
			PlaySound(TEXT("gift.wav"), NULL, SND_ASYNC | SND_FILENAME);
			player.position = Vector3f(0, 0, 20);
			player.loseLife();
			lights = false;
			diffR = 1.0;
			diffG = 0.0;
			diffB = 0.0;
			
			glutTimerFunc(2000, resetLightIntensity, 0);
			break;
		}
	}
	for (int i = 0; i <= 1; ++i) {
		if (!ginger[i].collected) {
			float dx = player.position.x - ginger[i].position.x;
			float dy = player.position.y - ginger[i].position.y;
			float dz = player.position.z - ginger[i].position.z;
			float distance = sqrt(dx * dx + dy * dy + dz * dz);

			if (distance < gingercollision) {
				PlaySound(TEXT("ginger.wav"), NULL, SND_ASYNC | SND_FILENAME);
				ginger[i].collected = true;
				lights = false;
				player.score += 10;
				diffR = 0.0;
				diffG = 1.0;
				diffB = 0.0;
				
				
				glutTimerFunc(2000, resetLightIntensity, 0);


			}
		}
	}

	for (int i = 0; i < 1; ++i) {
		if (!snow.collected) {
			
			float dx = player.position.x - snow.position.x;
			float dy = player.position.y - snow.position.y;
			float dz = player.position.z - snow.position.z;
			float distance = sqrt(dx * dx + dy * dy + dz * dz);

			if (distance < snowcollisionDistance) {
				PlaySound(TEXT("snow.wav"), NULL, SND_ASYNC | SND_FILENAME);
				snow.collected = true;
				lights = false;
				player.score += 10;
				diffR = 0.0;
				diffG = 1.0;
				diffB = 0.0;


				glutTimerFunc(2000, resetLightIntensity, 0);


			}
		}
	}
	
}



void checkCollisionsAndUpdate() {
	// Tree positions
	Vector3f treePositions[3] = {
		Vector3f(0, 0, 0),
		Vector3f(-15, 0, -5),
		Vector3f(15, 0, 5)
	};

	float dx = player.position.x - starPosition.x;
	float dy = player.position.y - starPosition.y;
	float dz = player.position.z - starPosition.z;
	float stardistance = sqrt(dx * dx + dy * dy + dz * dz);

	float starCollisionDistance = 3.5f;
	
	// Collision threshold (distance)
	float collisionDistance = 2.0f; // Adjust this value as needed for your scene scale
	float coinCollectionDistance = 2.0f;
	if (stardistance < starCollisionDistance) {
		PlaySound(TEXT("level1.wav"), NULL, SND_ASYNC | SND_FILENAME);
		level1 = false;
		lights = false;
		diffR = 0.0;
		diffG = 1.0;
		diffB = 0.0;
		glutTimerFunc(2000, resetLightIntensity, 0);
		player.position = Vector3f(0, 0, 20);
	}
	// Check each tree position
	for (int i = 0; i < 3; ++i) {
		float dx = player.position.x - treePositions[i].x;
		float dy = player.position.y - treePositions[i].y;
		float dz = player.position.z - treePositions[i].z;
		float distance = sqrt(dx * dx + dy * dy + dz * dz);
		

		if (distance < collisionDistance) {
			PlaySound(TEXT("treehit.wav"), NULL, SND_ASYNC | SND_FILENAME);
			player.position = Vector3f(0, 0, 20);
			player.loseLife();
			lights = false;
			diffR = 1.0;
			diffG = 0.0;
			diffB = 0.0;

			glutTimerFunc(1000, resetLightIntensity, 0); // Reset after 1 second

			break;
		}
	}
	for (int i = 0; i < NUM_COINS; ++i) {
		if (!coins[i].collected) {
			float dx = player.position.x - coins[i].position.x;
			float dy = player.position.y - coins[i].position.y;
			float dz = player.position.z - coins[i].position.z;
			float distance = sqrt(dx * dx + dy * dy + dz * dz);

			if (distance < coinCollectionDistance) {
				coins[i].collected = true;
				PlaySound(TEXT("coinhit.wav"), NULL, SND_ASYNC | SND_FILENAME);
				player.score += 10;
				lights = false;
				diffR = 0.0;
				diffG = 1.0;
				diffB = 0.0;
				glutTimerFunc(2000, resetLightIntensity, 0);


			}
		}
	}


}




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

	Eye = Vector(0, 10, 30);
	At = Vector(0, 0, 0);
	Up = Vector(0, 1, 0);

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
void RenderGround2()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);	// Bind the ground texture

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

float jumpForwardDistance = 10.0f; // Total distance to move forward in the Z-axis
float jumpHeight = 3.0f; // Maximum height to reach in the Y-axis
bool isJumping = false;
float jumpProgress = 0.0f;


void startJump() {
	if (!isJumping) {
		isJumping = true;
		jumpProgress = 0.0f;
		
	}
}

void updateJump() {
	if (isJumping) {
		// Calculate the fraction of the jump completed
		float fraction = jumpProgress / jumpHeight;

		// Vertical movement
		if (jumpProgress < jumpHeight / 2) {
			// Ascending
			player.position.y = 2 * jumpProgress;
		}
		else {
			// Descending
			player.position.y = 2 * (jumpHeight - jumpProgress);
		}
		if (player.position.z >= -21) {
			if (player.rotationangle == 180) {
				player.position.z -= jumpForwardDistance / (jumpHeight * 2);
			}
		}
		if (player.position.z <= 20) {
			if (player.rotationangle == 0) {
				player.position.z += jumpForwardDistance / (jumpHeight * 2);
			}
		}
		// Update jump progress
		jumpProgress += 0.5; // Adjust this value based on your game's speed and desired jump duration

		// Check if jump is completed
		if (jumpProgress >= jumpHeight) {
			isJumping = false;
			player.position.y = 0; // Reset Y position after landing
			// Optionally, reset Z position if needed
		}
	}
}

void displayWinMessage() {
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// Switch to 2D projection for drawing the HUD
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Set text color to green for "YOU WIN"
	glColor3f(0.0f, 1.0f, 0.0f);
	renderBitmapString(WIDTH / 2 - 50, HEIGHT / 2 + 10, GLUT_BITMAP_HELVETICA_18, "YOU WIN!");

	// Display the final score
	char scoreString[50];
	sprintf(scoreString, "Final Score: %d", player.score);
	renderBitmapString(WIDTH / 2 - 50, HEIGHT / 2 - 10, GLUT_BITMAP_HELVETICA_18, scoreString);

	// Restore the original projection and modelview matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Re-enable lighting and depth test
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}


void myDisplaylevel2(void){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (player.isAlive()&& level2==true) {
		
		camera.updateView();
		setupCamera();
		setupLights();
		RenderGround2();


		glPushMatrix();
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(0, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
		glPopMatrix();


		glPushMatrix();
		glTranslated(player.position.x, player.position.y, player.position.z);
		glRotatef(player.rotationangle, 0, 1, 0);
		glRotatef(90.f, -1, 0, 0);
		glScalef(0.03, 0.03, 0.03);
		model_santa.Draw();
		glPopMatrix();


		glPushMatrix();
		glScalef(0.1, 0.1, 0.1);
		railway.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(15,0,0);
		glScalef(0.1, 0.1, 0.1);
		railway.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslated(-15, 0, 0);
		glScalef(0.1, 0.1, 0.1);
		railway.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(2, 2, -4);
		glScalef(0.2, 0.2, 0.2);
		Giftbox.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(16, 2, 5);
		glScalef(0.2, 0.2, 0.2);
		Giftbox.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(-13, 2, 10);
		glScalef(0.2, 0.2, 0.2);
		Giftbox.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 2, -19);
		glRotatef(90.f, 1, 0, 0);
		glRotatef(90.f, 0, 0, -1);
		glScalef(2, 2, 2);
		snowman.Draw();
		glPopMatrix();

		if (!ginger[0].collected) {
			glPushMatrix();
			glTranslated(-14, 0, -5);
			glRotatef(90.f, 1, 0, 0);
			glScalef(7, 7, 7);
			Ginger.Draw();
			glPopMatrix();
		}
		if (!ginger[1].collected) {
			glPushMatrix();
			glTranslated(14, 0, -12);
			glRotatef(90.f, 1, 0, 0);
			glScalef(7, 7, 7);
			Ginger.Draw();
			glPopMatrix();
		}

		if (!snow.collected) {
			glPushMatrix();
			glTranslated(0, 4, 4);
			glRotatef(90.f, 1, 0, 0);
			glScalef(0.03, 0.03, 0.03);
			Snowflake.Draw();
			glPopMatrix();
		}


	

		checkCollisionsAndUpdate2();
		updateJump();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		// Switch to 2D projection for drawing HUD
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Draw the health bar and score
		glColor3f(1.0, 1.0, 1.0); // White color for text
		drawHealthBar(player.lives, 5, 50, HEIGHT - 50, 200, 20);
		char scoreString[50];
		sprintf(scoreString, "Score: %d", player.score);
		renderBitmapString(50, HEIGHT - 80, GLUT_BITMAP_HELVETICA_18, scoreString);

		// Restore 3D projection and modelview matrices
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		// Re-enable lighting and depth test
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);

		

	
	}
	else if(!player.isAlive() && level2 == true) {
		
		glDisable(GL_LIGHTING); 
		glDisable(GL_DEPTH_TEST); 

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT); 

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Set text color to red for "Game Over"
		glColor3f(1.0f, 0.0f, 0.0f);
		renderBitmapString(WIDTH / 2 - 50, HEIGHT / 2 + 10, GLUT_BITMAP_HELVETICA_18, "Game Over");

		// Display the final score
		char scoreString[50];
		sprintf(scoreString, "Final Score: %d", player.score);
		renderBitmapString(WIDTH / 2 - 50, HEIGHT / 2 - 10, GLUT_BITMAP_HELVETICA_18, scoreString);

		// Restore the original projection matrix
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_DEPTH_TEST); // Re-enable depth testing
		glEnable(GL_LIGHTING); // Re-enable lighting
	}else 	if (level2 == false && player.isAlive()) {
		displayWinMessage();


	}



	glutSwapBuffers();
}
void myDisplay(void)
{
	if (level1 == true) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (player.isAlive()) {

			camera.updateView();
			setupCamera();
			setupLights();
		
			// Draw Ground
			RenderGround();

			glPushMatrix();
			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(0, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex1);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 100, 100, 100);
			gluDeleteQuadric(qobj);
			glPopMatrix();

			glPushMatrix();
			glScalef(0.1, 0.1, 0.1);
			railway.Draw();
			glPopMatrix();

			glPushMatrix();
			glTranslated(15, 0, 0);
			glScalef(0.1, 0.1, 0.1);
			railway.Draw();
			glPopMatrix();


			glPushMatrix();
			glTranslated(-15, 0, 0);
			glScalef(0.1, 0.1, 0.1);
			railway.Draw();
			glPopMatrix();

			// Draw Tree Model
			glPushMatrix();
			glTranslatef(0, 0, 0);
			glScalef(0.03, 0.03, 0.03);
			model_tree.Draw();
			glPopMatrix();


			glPushMatrix();
			glTranslatef(-15, 0, -5);
			glScalef(0.03, 0.03, 0.03);
			model_tree.Draw();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(15, 0, 5);
			glScalef(0.03, 0.03, 0.03);
			model_tree.Draw();
			glPopMatrix();

			glPushMatrix();
			glTranslated(player.position.x, player.position.y, player.position.z);
			glRotatef(player.rotationangle, 0, 1, 0);
			glRotatef(90.f, -1, 0, 0);
			glScalef(0.03, 0.03, 0.03);
			model_santa.Draw();
			glPopMatrix();
		
			updateJump();
			checkCollisionsAndUpdate();


			glPushMatrix();
			glTranslatef(0, 3, -20);
			glRotatef(90.f, 0, 1, 0);
			glScalef(0.01, 0.01, 0.01);
			Star.Draw();
			glPopMatrix();

			for (int i = 0; i < NUM_COINS; ++i) {
				if (!coins[i].collected) {
					glPushMatrix();
					glTranslatef(coins[i].position.x, 2.5, coins[i].position.z);
					glRotatef(90.f, 0, 1, 0);
					glScalef(2, 2, 2); // Adjust the size as needed
				
					coin.Draw(); // Assuming coin.Draw() is how you render a coin
					glPopMatrix();
				}
			}


			// Switch to 2D projection
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0, WIDTH, 0, HEIGHT);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			// Disable lighting and depth test for 2D overlay
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);

			// Set a contrasting color for the text
			glColor3f(1.0, 1.0, 1.0); // White color

			drawHealthBar(player.lives, 5, 50, HEIGHT - 50, 200, 20);
			char scoreString[50];
			sprintf(scoreString, "Score: %d", player.score);
			renderBitmapString(50, HEIGHT - 80, GLUT_BITMAP_HELVETICA_18, scoreString);

			// Restore 3D projection and modelview matrices
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);

			// Re-enable lighting and depth test
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST);
		}
		else {
			
			glDisable(GL_LIGHTING); 
			glDisable(GL_DEPTH_TEST); 

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0, WIDTH, 0, HEIGHT); 

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glColor3f(1.0f, 0.0f, 0.0f);
			renderBitmapString(WIDTH / 2 - 50, HEIGHT / 2 + 10, GLUT_BITMAP_HELVETICA_18, "Game Over");

		
			char scoreString[50];
			sprintf(scoreString, "Final Score: %d", player.score);
			renderBitmapString(WIDTH / 2 - 50, HEIGHT / 2 - 10, GLUT_BITMAP_HELVETICA_18, scoreString);

		
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
		}


		glutSwapBuffers();
	}
	else if (level1==false){
		
		myDisplaylevel2();
	}
};



void myKeyboard(unsigned char button, int x, int y)
{
	float d = 0.01;
	float playerspeed = 0.3;
	float playerspeed2 = 14.5;

	switch (button) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 'i':
		if (player.position.z - playerspeed >= -21) {
			player.position.z -= playerspeed;
			player.rotationangle = 180;
			camera.forward.z = -1;
			camera.forward.x = 0;
		}
		break;

	case 'k':
		if (player.position.z + playerspeed <= 20) {
			player.position.z += playerspeed;
			player.rotationangle = 0;
			camera.forward.z = 1;
			camera.forward.x = 0;
		}
		break;
	case 'j':
		if(player.position.x- playerspeed>=-12)
		player.position.x -= playerspeed2;
		player.rotationangle = -90;
		camera.forward.x = -1;
		camera.forward.z = 0;
		break;
	case 'l':
		if (player.position.x + playerspeed <= 12)
		player.position.x += playerspeed2;
		player.rotationangle = 90;
		camera.forward.x = 1;
		camera.forward.z = 0;
		break;

	case 'v': 
		camera.isThirdPersonView = !camera.isThirdPersonView;
			break;

	case'z':
		startJump();
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}
	switch (button)
	{
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}


void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}



void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Start the jump if not already jumping
		startJump();
	}
	
	
}


void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}


void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/Christmas/FINAL.3ds");
	model_santa.Load("Models/santa/s.3ds");
	railway.Load("Models/railway/railway2.3ds");
	coin.Load("Models/Coin22/HANA.3ds");
	Star.Load("Models/Star/star big.3ds");
	Giftbox.Load("Models/Giftbox/Giftbox.3ds");
	snowman.Load("Models/Olaf/YARAB2.3ds");
	Ginger.Load("Models/Trial/ginger.3ds");
	Snowflake.Load("Models/SnowFlake/SNOWYARAB.3ds");

	// Loading texture files
	tex_ground.Load("Textures/snow.bmp");
	tex_ground2.Load("Textures/Grass.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	loadBMP(&tex1, "Textures/starry2.bmp", true);
}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);


	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	
	glShadeModel(GL_SMOOTH);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	
	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}