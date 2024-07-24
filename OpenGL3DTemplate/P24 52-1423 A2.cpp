#include <math.h>
#include <cmath>
#include <string>
#include <iostream>
#include < windows.h >
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
float statueRotationAngle = 0.0f;
float playerX = 0.0f;
float playerZ = -2.3f;
float playerRotation = 0.0f;
int GameTime = 120;
float wall1 =0.5f;
float wall2 = 0.5f;
float wall3 = 0.5f;
bool wallscolor = true;
bool targetcollected = false;
bool doanimation = false;
float rotationAngle = 0.0f;
int windowWidth = 800;
int windowHeight = 600;
bool isBackgroundMusicStarted = false;
bool isGameStarted = false;
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

class Camera {
public:
    Vector3f eye, center, up;

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

    void look() {
        gluLookAt(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            up.x, up.y, up.z
        );
    }
};

Camera camera;
void Counter(int value) {
    if (GameTime != 0) {
        GameTime -= 1;
        glutTimerFunc(1000, Counter, 0); // Set up the timer for the next second
    }
    glutPostRedisplay();
}
void drawCounter() {
    glColor3f(0.0f, 0.0f, 0.0f); // Set text color to black

    // Convert the integer GameTime to a string
    std::string timeString = std::to_string(GameTime);

    // Draw the counter at the specified position on the screen
    glRasterPos3f(0.0f, 3.0f, 0.0f); // Set the position to (0, 5, 0)
    for (char& character : timeString) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, character); // Use a suitable font
    }
}


void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 640 / 480, 0.001, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look();
}

// 1 primitive
void drawGround() {
    glColor3f(0.7f, 0.4f, 0.2f);
    // Draw the ground as a smaller rectangle
    glBegin(GL_QUADS);
    glVertex3f(-3.0f, 0.0f, -3.0f);
    glVertex3f(-3.0f, 0.0f, 3.0f);
    glVertex3f(3.0f, 0.0f, 3.0f);
    glVertex3f(3.0f, 0.0f, -3.0f);
    glEnd();

}

// 2 primitives
void drawBoundaryWalls() { 
    // Set the color for the walls (e.g., gray)
    glColor3f(wall1,wall2,wall3);
    // Draw the three walls
    // Draw the three walls as elongated cuboids
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 3.0f);
    glScalef(6.0f, 3.0f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.0f, 1.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(6.0f, 3.0f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3.0f, 1.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(6.0f, 3.0f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

       // Draw cones on top of each wall
    glColor3f(0, 0.1, 0.4);
    for (float i = -2.9f; i < 3.0f; i += 0.2f) {
        // Left side cones
        glPushMatrix();
        glTranslatef(-3.0f, 2.6f, i + 0.1f);
        glutSolidCone(0.15f, 0.3f, 10, 10);
        glPopMatrix();

        // Right side cones
        glPushMatrix();
        glTranslatef(3.0f, 2.6f, i + 0.1f);
        glutSolidCone(0.15f, 0.3f, 10, 10);
        glPopMatrix();

        // Back side cones
        glPushMatrix();
        glTranslatef(i + 0.1f, 2.6f, 3.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glutSolidCone(0.15f, 0.3f, 10, 10);
        glPopMatrix();
    }
}

//10 primitives
void drawPlayer() {

    glPushMatrix();
    glTranslatef(playerX, 0.75f, playerZ);
    glRotatef(playerRotation, 0.0f, 1.0f, 0.0f);
    glTranslatef(-playerX, -0.75f, -playerZ);
    // Draw the body
    glPushMatrix();
    glColor3f(0.0f, 0.1f, 0.4f);
    glTranslatef(playerX, 0.2f, playerZ);
    glScalef(0.3f, 0.5f, 0.2f);
    glutSolidCube(0.5f);
    glPopMatrix();
    // Draw the head
    glPushMatrix();
    glColor3f(0.8f, 0.6f, 0.4f);
    glTranslatef(playerX, 0.5f, playerZ);
    glutSolidSphere(0.15f, 10, 10);
    // Draw the nose
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.2f);
    glutSolidCone(0.04f, 0.1f, 7, 7);
    glPopMatrix();

    // Draw the ears
    glPushMatrix();
    glTranslatef(0.15f, 0.0f, 0.0f);
    glutSolidSphere(0.05f, 10, 10);
    glTranslatef(-0.3f, 0.0f, 0.0f);
    glutSolidSphere(0.05f, 10, 10);
    glPopMatrix();

    // Draw the hair
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.25f, 0.0f);
    glutSolidSphere(0.25f, 15, 15);
    glPopMatrix();

    
    GLUquadric* quadric = gluNewQuadric();
    glColor3f(0.8f, 0.6f, 0.4f);
    // Left arm
    glPushMatrix();
    glTranslatef(playerX - 0.2, 0.25f, playerZ);
    glScalef(0.5f, 0.5f, 0.5f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, 0.05f, 0.05f, 0.3f, 12, 1);
    glPopMatrix();
    // Right arm
    glPushMatrix();
    glTranslatef(playerX + 0.2f, 0.25f, playerZ);
    glScalef(0.5f, 0.5f, 0.5f);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, 0.05f, 0.05f, 0.3f, 12, 1);
    glPopMatrix();

    // Legs
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color for the legs

    float legStartY = 0.5f - (0.5f * 0.5f); // The bottom of the body cube
    float legLength = 0.4f; // The length of the legs

    // Left leg
    glPushMatrix();
    glTranslatef(playerX - 0.1f, legStartY - (legLength / 2), playerZ); // Move to the bottom of the torso
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate to point the legs downwards
    gluCylinder(quadric, 0.05f, 0.05f, legLength, 12, 1);
    glPopMatrix();

    // Right leg
    glPushMatrix();
    glTranslatef(playerX + 0.1f, legStartY - (legLength / 2), playerZ); // Move to the bottom of the torso
    glRotatef(90.0f, 3.0f, 0.0f, 0.0f); // Rotate to point the legs downwards
    gluCylinder(quadric, 0.05f, 0.05f, legLength, 12, 1);
    glPopMatrix();
    //glTranslatef(playerX, 0.75f, playerZ);
    gluDeleteQuadric(quadric); // Clean up the quadric object
    glPopMatrix();

}


// 3 primitives
void drawTree(float x, float z) {
    // Set the trunk color (e.g., brown)
    glColor3f(0.4f, 0.2f, 0.0f);

    // Draw the trunk using a cuboid
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(0.2f, 1.0f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Set the leaves color (e.g., green)
    glColor3f(0.0f, 0.8f, 0.0f);

    // Draw the leaves using a sphere
    glPushMatrix();
    glTranslatef(x, 1.5f, z);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    // Draw additional branches using cones
    glColor3f(0.4f, 0.2f, 0.0f);  // Use trunk color for branches

    // First branch
    glPushMatrix();
    glTranslatef(x, 1.2f, z);
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.3, 1.0, 20, 20);
    glPopMatrix();

    // Second branch
    glPushMatrix();
    glTranslatef(x, 1.2f, z);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.3, 1.0, 20, 20);
    glPopMatrix();

    // Third branch
    glPushMatrix();
    glTranslatef(x, 1.2f, z);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCone(0.3, 1.0, 20, 20);
    glPopMatrix();

    // Fourth branch
    glPushMatrix();
    glTranslatef(x, 1.2f, z);
    glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCone(0.3, 1.0, 20, 20);
    glPopMatrix();
}

// 3 primitives
void drawWaterFountain(float x, float y, float z) {
    // Set the base color
    glColor3f(0.0f, 0.1f, 0.4f);

    // Draw the base cylinder
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);  // Rotate around Y-axis
    glScalef(1.5f, 0.1f, 1.5f); // Scale to create a flat base
    glutSolidCube(1.0);
    glPopMatrix();

    // Set the water reservoir color
    glColor3f(0.0f, 0.0f, 0.5f);

    // Draw the water reservoir sphere
    glPushMatrix();
    glTranslatef(x, y + 0.1f, z);
    glutSolidSphere(0.3, 30, 30);
    glPopMatrix();

    // Set the water spout color
    glColor3f(0.0f, 0.0f, 1.0f);

    // Draw the water spout cone pointing upwards
    glPushMatrix();
    glTranslatef(x, y + 0.2f, z);
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);  // Rotate around Y-axis
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate the cone to point upwards
    glutSolidCone(0.30, 0.5, 30, 30);
    glPopMatrix();
}

void updateRotation() {
    if (doanimation) {
        // Adjust the rotation speed as needed
        rotationAngle += 0.1f;
    }
}

// 4 primitives
void drawSeesaw(float x, float z, float scale) {
    // Rotate the seesaw by 90 degrees around the y-axis
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);  // Rotate around Y-axis

    // Triangle as the pivot
    glColor3f(0.5f, 1.0f, 0.6f);  // Brown color for the pivot
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.01f * scale, 0.0f);  // Top
    glVertex3f(-0.1f * scale, 0.05f * scale, 0.0f);  // Bottom left
    glVertex3f(0.1f * scale, 0.05f * scale, 0.0f);  // Bottom right
    glEnd();

    // Rectangle as the bar
    glColor3f(0.7f, 0.7f, 0.7f);  // Light gray color for the bar
    glBegin(GL_QUADS);
    glVertex3f(-0.1f * scale, 0.05f * scale, 0.02f * scale);  // Bottom left
    glVertex3f(-0.1f * scale, 0.05f * scale, -0.02f * scale);  // Top left
    glVertex3f(0.1f * scale, 0.05f * scale, -0.02f * scale);  // Top right
    glVertex3f(0.1f * scale, 0.05f * scale, 0.02f * scale);  // Bottom right
    glEnd();

    // Circles as places to sit
    glColor3f(0.0f, 0.1f, 0.4f);
    glPushMatrix();
    glTranslatef(-0.1f * scale, 0.05f * scale, 0.0f);  // Adjust the height if needed
    glutSolidSphere(0.03 * scale, 20, 20);  // Left seat
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.1f * scale, 0.05f * scale, 0.0f);  // Adjust the height if needed
    glutSolidSphere(0.03 * scale, 20, 20);  // Right seat
    glPopMatrix();

    // Restore the original transformation matrix
    glPopMatrix();
}


// 6 primitives
void drawCar(float x, float z) {
    // Body of the car
    glColor3f(1.0f, 0.0f, 0.0f);  // Red color
    glPushMatrix();
    glTranslatef(x, 0.1f, z);
    glScalef(1.2f, 1.2f, 1.2f);  // Scale the car to 75% of its original size
    glutSolidCube(0.2);
    glPopMatrix();

    // Roof of the car
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue color
    glPushMatrix();
    glTranslatef(x, 0.2f, z);
    glScalef(0.9f, 0.6f, 0.6f);  // Scale the roof separately
    glutSolidCube(0.2);
    glPopMatrix();

    // Wheels of the car
    glColor3f(0.0f, 0.0f, 0.0f);  // Black color
    glPushMatrix();
    glTranslatef(x - 0.1f, 0.05f, z + 0.1f);
    glutSolidSphere(0.09, 20, 20);  // Front left wheel
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x + 0.1f, 0.05f, z + 0.1f);
    glutSolidSphere(0.09, 20, 20);  // Front right wheel
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x - 0.1f, 0.05f, z - 0.1f);
    glutSolidSphere(0.09, 20, 20);  // Rear left wheel
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x + 0.1f, 0.05f, z - 0.1f);
    glutSolidSphere(0.09, 20, 20);  // Rear right wheel
    glPopMatrix();
}
void drawRoad() {
    // Draw the road surface (black color)
    glColor3f(0.2f, 0.1f, 0.4f);
    glPushMatrix();
    glTranslatef(2.5f, 0.0f, 2.5f);
    glScalef(1.0f, 0.01f, 3.5f);  // Adjust the road size as needed
    glutSolidCube(1.0);
    glPopMatrix();


}

//6 primitives
void drawKiosk(float x, float y, float z) {
    // Set the kiosk color 0.4/0.4/0.5  
    glColor3f(0.4f, 0.4f, 0.5f);

    // Draw the main structure (two cubes)
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.5f, 1.0f, 0.5f); // Adjust the size as needed
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y + 0.5f, z);
    glScalef(0.4f, 0.5f, 0.4f); // Adjust the size as needed
    glutSolidCube(1.0);
    glPopMatrix();

    // Set the roof color 0.1/0/0.5 
    glColor3f(0.1f, 0.0f, 0.5f);

    // Draw the roof like a prism
    glPushMatrix();
    glTranslatef(x, y + 0.7f, z);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate the cone to align with the kiosk
    glutSolidCone(0.5, 0.5, 20, 20); // Adjust the size as needed
    glPopMatrix();

    //2nd shape 
    
    // Set the kiosk color 0.4/0.4/0.5  
    glColor3f(0.4f, 0.4f, 0.5f);

    // Draw the main structure (two cubes)
    glPushMatrix();
    glTranslatef(x, y, z-0.5);
    glScalef(0.5f, 1.0f, 0.5f); // Adjust the size as needed
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y + 0.5f, z - 0.5);
    glScalef(0.4f, 0.5f, 0.4f); // Adjust the size as needed
    glutSolidCube(1.0);
    glPopMatrix();

    // Set the roof color 0.1/0/0.5 
    glColor3f(0.1f, 0.0f, 0.5f);

    // Draw the roof like a prism
    glPushMatrix();
    glTranslatef(x, y + 0.7f, z - 0.5);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate the cone to align with the kiosk
    glutSolidCone(0.5, 0.5, 20, 20); // Adjust the size as needed
    glPopMatrix();
}

// 3 primitives
void drawStatue(float x, float y, float z, float size) {
    // Update the animation angle (adjust speed as needed)
    statueRotationAngle += 1.0f;

    // Draw the main structure (cube)
    glColor3f(0.8f, 0.6f, 0.4f);  // Brown color
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(statueRotationAngle, 0.0f, 1.0f, 0.0f);  // Rotate around the y-axis
    glScalef(size, size, size);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw the triangular roof
    glColor3f(0.5f, 0.2f, 0.0f);  // Reddish color
    glPushMatrix();
    glTranslatef(x, y + size, z);
    glRotatef(statueRotationAngle, 0.0f, 1.0f, 0.0f);  // In sync with the cube rotation
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);  // Rotate 45 degrees around the y-axis
    glutSolidCone(size * 0.7, size, 4, 2);  // Adjust the size as needed
    glPopMatrix();

    // Draw the rectangular door
    glColor3f(0.2f, 0.4f, 0.6f);  // Blue color
    glPushMatrix();
    glTranslatef(x, y - size * 0.5, z + size * 0.5);
    glRotatef(statueRotationAngle, 0.0f, 1.0f, 0.0f);  // In sync with the cube rotation
    glScalef(size * 0.3, size * 0.5, 0.05);  // Adjust the size as needed
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawText(float x, float y, const std::string& text) {
    glColor3f(1.0f, 1.0f, 1.0f); // Set text color (white)
    glRasterPos2f(x, y);

    // Loop through each character in the string and render it
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}
// 7 primitives
void drawFerrisWheel(float x, float y, float z, float scale) {
    // Support Rectangles
    glColor3f(1.0, 1.0, 1.0); // White color
    glPushMatrix();
    glTranslatef(x, y - 2.0 * scale, z);
    glScalef(scale * 2.0, scale / 5.0, scale);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y - 2.0 * scale, z);
    glRotatef(90.0, 0, 0, 1);
    glScalef(scale * 2.0, scale / 5.0, scale);
    glutSolidCube(1.0);
    glPopMatrix();

    // Main structure
    glColor3f(1.0, 1.0, 1.0); // White color
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale); // Apply scaling
    glutSolidTorus(0.1, 1.0, 30, 30);
    glutSolidTorus(0.2, 0.1, 30, 30);
    glPopMatrix();

    // Circular frame
    glColor3f(1.0, 1.0, 1.0); // White color
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale); // Apply scaling
    glutSolidTorus(0.1, 2.0, 30, 30);
    glPopMatrix();

    // Capsules
    glColor3f(0.1f, 0.0f, 0.5f); // Purple color
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(scale, scale, scale); // Apply scaling
        glRotatef(i * 45.0, 0, 0, 1);
        glTranslatef(1.8, 0, 0);
        glutSolidSphere(0.3, 30, 30);
        glPopMatrix();
    }

    // Central hub
    glColor3f(1.0, 1.0, 1.0); // White color
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale); // Apply scaling
    glutSolidCone(0.2, 1.0, 30, 30);
    glPopMatrix();
}

void Display() {
    setupCamera();
    setupLights();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!isGameStarted){  // Set the side view from the open wall
        camera.eye = Vector3f(0.0f, 1.0f, -4.0f);  // Adjust the position based on your scene
        camera.center = Vector3f(0.0f, 0.0f, 0.0f);
        camera.up = Vector3f(0.0f, 1.0f, 0.0f);
        camera.rotateX(20);
        camera.moveZ(-2);
        camera.moveY(0.55);
        isGameStarted = true;
    }
    if (GameTime!=0) {

        if (!isBackgroundMusicStarted) {
            // Start background music
            PlaySound(TEXT("back.wav"), NULL, SND_ASYNC | SND_FILENAME);
            isBackgroundMusicStarted = true;
        }

        // Draw the ground
        drawGround();
        // Draw the walls
        if (GameTime % 10 == 0) {
            wallscolor = !wallscolor;
            if (wallscolor) {
                wall1 = 0.5f;
                wall2 = 0.5f;
                wall3 = 0.5f;
            }
            else {
                wall1 = 0.3f;
                wall2 = 0.9f;
                wall3 = 1.0f;
            }
        }
        drawBoundaryWalls();
        // Draw the player
        drawPlayer();
        // Draw a tree
        for (float i = -2.0f; i <= 2.0f; i += 2.0f)
            drawTree(-2.0f, i);

        // Draw the seesaw
        for (float i = -2.0f; i < 3.0f; i += 1.0f)
            drawSeesaw(-1.0f, i, 3.5f);

        // Draw a 4 cars
        drawCar(2.5f, 2.5f);
        drawCar(2.5f, 2.1f);
        drawCar(2.5f, 1.7f);
        drawCar(2.5f, 1.3f);
        drawRoad();
        // Draw the counter
        drawCounter();
        // Draw the kiosk
        for (float i = 0.0f; i < 3.0f; i += 1.0f)
            drawKiosk(2.5f, 0.1f, -i);
        // Draw the water fountain
        drawWaterFountain(0.0f, 0.0f, 0.0f);
        // Draw the ferris wheel
        drawFerrisWheel(0.0f,1.0f, 2.6f ,0.5);
        if (!targetcollected) {
            // Draw the statue
            drawStatue(0.0f, 0.0f, 1.5f, 0.5f);
        }
        updateRotation();
        glFlush();
    }
    //end screen
   else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Set the side view from the open wall
        camera.eye = Vector3f(0.0f, 1.5f, -4.0f);  // Adjust the position based on your scene
        camera.center = Vector3f(0.0f, 0.0f, 0.0f);
        camera.up = Vector3f(0.0f, 1.0f, 0.0f);
        if (targetcollected) {
            drawText(-0.5f, 0.0f, "Congratulations, you won!");
            PlaySound(TEXT("win.WAV"), NULL, SND_ASYNC | SND_FILENAME);
        } else {
            drawText(-0.5f, 0.0f, "Bad luck, you lost!");
            PlaySound(TEXT("lose.WAV"), NULL, SND_ASYNC | SND_FILENAME);
        }

        glFlush();
    }
}
bool canMove(float newX, float newZ) {
    // Check if the new position is within the bounding walls
    if (newX < -2.7f || newX > 2.7f || newZ < -2.7f || newZ > 2.7f) {
        PlaySound(TEXT("khabat.WAV"), NULL, SND_ASYNC | SND_FILENAME);
        return false;
    }

    // Check if the new position collides with the trees
    for (float i = -2.0f; i <= 2.0f; i += 2.0f) {
        float treeX = -2.0f;
        float treeZ = i;
        float treeRadius = 1.5f;

        float distanceToTree = sqrt((newX - treeX) * (newX - treeX) + (newZ - treeZ) * (newZ - treeZ));

        if (distanceToTree < treeRadius) {
            PlaySound(TEXT("khabat.WAV"), NULL, SND_ASYNC | SND_FILENAME);
            return false; // Collision with a tree
        }
    }

    // Check if the new position collides with the seesaws
    for (float i = -2.0f; i < 3.0f; i += 1.0f) {
        float seesawX = -1.0f;
        float seesawZ = i;
        float seesawScale = 3.5f;
        float seesawCollisionRadius = 0.1f * seesawScale;  // Adjust the collision radius if needed

        float distanceToSeesaw = sqrt((newX - seesawX) * (newX - seesawX) + (newZ - seesawZ) * (newZ - seesawZ));

        if (distanceToSeesaw < seesawCollisionRadius) {
            PlaySound(TEXT("khabat.WAV"), NULL, SND_ASYNC | SND_FILENAME);
            return false; // Collision with a seesaw
        }
    }

    // Check if the new position collides with the cars
    for (float carZ = 2.5f; carZ >= 1.3f; carZ -= 0.4f) {
        float carX = 2.5f;
        float carScale = 1.0f;  // Adjust the car scale if needed
        float carCollisionRadius = 0.1f * carScale;  // Adjust the collision radius if needed

        float distanceToCar = sqrt((newX - carX) * (newX - carX) + (newZ - carZ) * (newZ - carZ));

        if (distanceToCar < carCollisionRadius) {
            PlaySound(TEXT("khabat.WAV"), NULL, SND_ASYNC | SND_FILENAME);
            return false; // Collision with a car
        }
    }

    // Check if the new position collides with the kiosks
    for (float i = 0.0f; i < 3.0f; i += 0.5f) {
        float kioskX = 2.5f;
        float kioskZ = -i;
        float kioskCollisionRadius = 0.7f; // Adjust the collision radius if needed

        float distanceToKiosk = sqrt((newX - kioskX) * (newX - kioskX) + (newZ - kioskZ) * (newZ - kioskZ));

        if (distanceToKiosk < kioskCollisionRadius) {
            PlaySound(TEXT("khabat.WAV"), NULL, SND_ASYNC | SND_FILENAME);
            return false; // Collision with a kiosk
        }
    }


    float fountainBaseX = 0.0f;
    float fountainBaseZ = 0.0f;
    float fountainBaseScale = 1.5f;
    float fountainBaseCollisionRadius = 0.7f * fountainBaseScale;  // Adjust the collision radius if needed

    float distanceToFountainBase = sqrt((newX - fountainBaseX) * (newX - fountainBaseX) + (newZ - fountainBaseZ) * (newZ - fountainBaseZ));

    if (distanceToFountainBase < fountainBaseCollisionRadius) {
        PlaySound(TEXT("khabat.WAV"), NULL, SND_ASYNC | SND_FILENAME);
        return false; // Collision with the fountain base
    }

    // Check if the new position collides with the statue
    float statueX = 0.0f;
    float statueZ = 1.5f;
    float statueSize = 0.5f;
    float statueCollisionRadius = statueSize;

    float distanceToStatue = sqrt((newX - statueX) * (newX - statueX) + (newZ - statueZ) * (newZ - statueZ));

    if ((distanceToStatue < statueCollisionRadius) && !targetcollected) {
        // Collision with the statue
        PlaySound(TEXT("target.WAV"), NULL, SND_ASYNC | SND_FILENAME);
        targetcollected = true;
    }
    // Check if the new position collides with the Ferris Wheel central hub
    float ferrisWheelHubX = 0.0f;  // Ferris Wheel central hub X position
    float ferrisWheelHubZ = 2.6f;  // Ferris Wheel central hub Z position
    float ferrisWheelHubRadius = 0.7f;  // Ferris Wheel central hub collision radius

    float distanceToFerrisWheelHub = sqrt((newX - ferrisWheelHubX) * (newX - ferrisWheelHubX) +
        (newZ - ferrisWheelHubZ) * (newZ - ferrisWheelHubZ));

    if (distanceToFerrisWheelHub < ferrisWheelHubRadius) {
        PlaySound(TEXT("khabat.WAV"), NULL, SND_ASYNC | SND_FILENAME);
        return false; // Collision with the Ferris Wheel central hub
    }

    return true; // Move is allowed
}

void Keyboard(unsigned char key, int x, int y) {
    if (GameTime != 0) {
        float d = 0.01;
        switch (key) {
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
            // Camera view keys
        case 'i':
            // Set the top view
            camera.eye = Vector3f(0.0f, 3.0f, 0.0f);
            camera.center = Vector3f(0.0f, 0.0f, 0.0f);
            camera.up = Vector3f(0.0f, 0.0f, 1.0f);
            camera.moveZ(-3);
            camera.rotateX(4);
            break;
        case 'o':
            // Set the side view from the open wall
            camera.eye = Vector3f(0.0f, 1.0f, -4.0f);  // Adjust the position based on your scene
            camera.center = Vector3f(0.0f, 0.0f, 0.0f);
            camera.up = Vector3f(0.0f, 1.0f, 0.0f);
            camera.rotateX(20);
            camera.moveZ(-2);
            camera.moveY(0.55);
            break;
        case 'p':
            // Set the side view from the left side wall with a slight upward angle
            camera.eye = Vector3f(4.0f, 4.5f, 0.0f);  // Adjust the position based on your scene
            camera.center = Vector3f(-2.0f, 0.0f, 0.0f);
            camera.up = Vector3f(0.0f, 0.1f, 0.0f);
            camera.moveZ(1.1);
            camera.moveY(0.25);
            camera.moveY(-0.25);
            break;

            // Player movement keys
        case 'y':
            // Move forward and face forward
            if (canMove(playerX, playerZ + d)) {
                playerZ += d;
                playerRotation = 0.0;
            }
            break;

        case 'h':
            // Move backward and face us
            if (canMove(playerX, playerZ - d)) {  // Check if within the bounding walls
                playerZ -= d;
                playerRotation = 180.0;
            }
            break;

        case 'j':
            // Move left and face left
            if (canMove(playerX - d, playerZ)) {  // Check if within the bounding walls
                playerX -= d;
                playerRotation = -90.0;
            }
            break;

        case 'g':
            // Move right and face right
            if (canMove(playerX + d, playerZ)) {  // Check if within the bounding walls
                playerX += d;
                playerRotation = 90.0;
            }
            break;
        case 'n':
            doanimation = true;
            PlaySound(TEXT("animation.WAV"), NULL, SND_ASYNC | SND_FILENAME);
            break;
        case 'm':
            doanimation = false;
            PlaySound(TEXT("back.WAV"), NULL, SND_ASYNC | SND_FILENAME);
            break;

        }


        glutPostRedisplay();
    }
}

void Special(int key, int x, int y) {
    if (GameTime != 0) {
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
}
void main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(640, 480);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("3D Game");
    glutDisplayFunc(Display);
    glutTimerFunc(0, Counter, 0);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Special);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glutMainLoop();
}
