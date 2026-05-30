#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

bool windowOpen = false;
float windowOpenAmount = 0.0f;

GLuint floorTexture = 0;

float fanAngle = 0.0f;
float fanSpeed = 8.0f;
bool fanOn = true;

float treadmillAngle = 0.0f;
float dumbbellY[6] = {0, 0, 0, 0, 0, 0};
float barbellY = 0.0f;

bool dumbbellUp[6] = {false, false, false, false, false, false};
bool barbellUp = false;
bool lightWhite = true;
bool lightGreen = true;

float camX = 0.0f;
float camY = 3.2f;
float camZ = 12.0f;

float lookX = 0.0f;
float lookY = 2.0f;
float lookZ = 0.0f;

void setNormalView() {
    camX = 0.0f;
    camY = 3.2f;
    camZ = 12.0f;

    lookX = 0.0f;
    lookY = 2.0f;
    lookZ = 0.0f;
}

void setDumbbellView() {
    camX = -3.8f;
    camY = 2.4f;
    camZ = 6.2f;

    lookX = -4.0f;
    lookY = 1.25f;
    lookZ = 1.5f;
}

void setBenchPressView() {
    camX = 0.0f;
    camY = 2.7f;
    camZ = 5.2f;

    lookX = 0.0f;
    lookY = 1.45f;
    lookZ = -1.1f;
}

void drawCube(float x, float y, float z) {
    glPushMatrix();
    glScalef(x, y, z);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawCylinder(float radius, float height) {
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, 30, 30);
    gluDeleteQuadric(quad);
}

GLuint loadTexture(const char* filename) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);

    if (!data) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );

    stbi_image_free(data);

    printf("Texture loaded successfully: %s\n", filename);

    return textureID;
}

void setupLights() {
    glEnable(GL_LIGHTING);

    if (lightWhite) {
        glEnable(GL_LIGHT0);

        GLfloat pos0[] = {0, 5.8f, 2, 1};
        GLfloat white[] = {0.90f, 0.90f, 0.75f, 1.0f};
        GLfloat ambient0[] = {0.20f, 0.20f, 0.15f, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, pos0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    } else {
        glDisable(GL_LIGHT0);
    }

    if (lightGreen) {
        glEnable(GL_LIGHT1);

        GLfloat pos1[] = {-4, 5.8f, -2, 1};
        GLfloat green[] = {0.0f, 0.85f, 0.15f, 1.0f};
        GLfloat ambient1[] = {0.0f, 0.15f, 0.05f, 1.0f};

        glLightfv(GL_LIGHT1, GL_POSITION, pos1);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, green);
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    } else {
        glDisable(GL_LIGHT1);
    }
}

void drawRoom() {
    glDisable(GL_LIGHTING);

    // Floor texture
    if (floorTexture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex3f(-6.0f, 0.01f, -6.0f);
            glTexCoord2f(8.0f, 0.0f);   glVertex3f( 6.0f, 0.01f, -6.0f);
            glTexCoord2f(8.0f, 8.0f);   glVertex3f( 6.0f, 0.01f,  6.0f);
            glTexCoord2f(0.0f, 8.0f);   glVertex3f(-6.0f, 0.01f,  6.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.45f, 0.85f, 0.50f);
        glPushMatrix();
        glTranslatef(0, -0.05f, 0);
        drawCube(12, 0.1f, 12);
        glPopMatrix();
    }

    // Back wall
    glColor3f(0.45f, 0.68f, 0.95f);
    glPushMatrix();
    glTranslatef(0, 3, -6);
    drawCube(12, 6, 0.2f);
    glPopMatrix();

    // Left wall
    glColor3f(0.50f, 0.75f, 1.0f);
    glPushMatrix();
    glTranslatef(-6, 3, 0);
    drawCube(0.2f, 6, 12);
    glPopMatrix();

    // Right wall
    glColor3f(0.50f, 0.75f, 1.0f);
    glPushMatrix();
    glTranslatef(6, 3, 0);
    drawCube(0.2f, 6, 12);
    glPopMatrix();

    // Ceiling
    glColor3f(0.12f, 0.22f, 0.32f);
    glPushMatrix();
    glTranslatef(0, 6, 0);
    drawCube(12, 0.1f, 12);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void drawLightIndicators() {
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(0.0f, 5.85f, 2.0f);

    if (lightWhite)
        glColor3f(1.0f, 1.0f, 0.75f);
    else
        glColor3f(0.15f, 0.15f, 0.15f);

    glutSolidSphere(0.18f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.0f, 5.85f, -2.0f);

    if (lightGreen)
        glColor3f(0.0f, 1.0f, 0.15f);
    else
        glColor3f(0.05f, 0.12f, 0.05f);

    glutSolidSphere(0.18f, 20, 20);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void drawDumbbell(float x, float y, float z, float size) {
    glPushMatrix();

    glTranslatef(x, y, z);
    glRotatef(90, 0, 1, 0);

    float handleLength = 0.55f * size;
    float plateSmall   = 0.15f * size;
    float plateBig     = 0.19f * size;
    float plateWidth   = 0.08f * size;

    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(0, 0, -handleLength / 2.0f);
    drawCylinder(0.04f * size, handleLength);
    glPopMatrix();

    glColor3f(0.20f, 0.20f, 0.20f);
    glPushMatrix();
    glTranslatef(0, 0, -0.12f * size);
    drawCylinder(0.055f * size, 0.24f * size);
    glPopMatrix();

    glColor3f(0.75f, 0.75f, 0.75f);

    glPushMatrix();
    glTranslatef(0, 0, -0.38f * size);
    drawCylinder(plateBig, plateWidth);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -0.48f * size);
    drawCylinder(plateSmall, plateWidth);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.30f * size);
    drawCylinder(plateBig, plateWidth);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.40f * size);
    drawCylinder(plateSmall, plateWidth);
    glPopMatrix();

    glColor3f(1.0f, 0.75f, 0.0f);

    glPushMatrix();
    glTranslatef(0, 0, -0.57f * size);
    drawCylinder(0.055f * size, 0.04f * size);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.50f * size);
    drawCylinder(0.055f * size, 0.04f * size);
    glPopMatrix();

    glPopMatrix();
}

void drawDumbbellRack() {
    glPushMatrix();
    glTranslatef(-3.5f, 0, 1.5f);

    glColor3f(0.25f, 0.25f, 0.25f);

    glPushMatrix();
    glTranslatef(0, 0.75f, 0);
    drawCube(4.4f, 0.12f, 0.35f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 1.45f, 0);
    drawCube(4.4f, 0.12f, 0.35f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.1f, 0.65f, 0);
    drawCube(0.12f, 1.35f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.65f, 0);
    drawCube(0.10f, 1.35f, 0.16f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.1f, 0.65f, 0);
    drawCube(0.12f, 1.35f, 0.18f);
    glPopMatrix();

    float lowerX[3] = {-1.45f, 0.0f, 1.45f};

    for (int i = 0; i < 3; i++) {
        drawDumbbell(lowerX[i], 0.98f + dumbbellY[i], 0.0f, 0.75f + i * 0.08f);
    }

    float upperX[3] = {-1.45f, 0.0f, 1.45f};

    for (int i = 0; i < 3; i++) {
        drawDumbbell(upperX[i], 1.68f + dumbbellY[i + 3], 0.0f, 0.90f + i * 0.08f);
    }

    glPopMatrix();
}

void drawBodyCylinder(float radius, float height) {
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, 25, 25);
    gluDeleteQuadric(quad);
}

void drawLimbBetween(float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     float radius) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    float length = sqrt(dx * dx + dy * dy + dz * dz);
    if (length <= 0.001f) return;

    glPushMatrix();
    glTranslatef(x1, y1, z1);

    float angle = acos(dz / length) * 180.0f / 3.1416f;
    float ax = -dy;
    float ay = dx;

    if (fabs(ax) > 0.001f || fabs(ay) > 0.001f) {
        glRotatef(angle, ax, ay, 0.0f);
    }

    drawBodyCylinder(radius, length);

    glPopMatrix();
}

void drawManOnBench() {
    glColor3f(0.0f, 0.15f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.82f, 0.75f);
    glScalef(0.70f, 0.22f, 1.35f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.92f, 0.70f, 0.52f);
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, -0.05f);
    glutSolidSphere(0.23f, 25, 25);
    glPopMatrix();

    glColor3f(0.04f, 0.04f, 0.04f);

    glPushMatrix();
    glTranslatef(-0.22f, 0.72f, 1.65f);
    glScalef(0.20f, 0.18f, 1.90f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.22f, 0.72f, 1.65f);
    glScalef(0.20f, 0.18f, 1.90f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.0f, 0.1f, 0.8f);

    glPushMatrix();
    glTranslatef(-0.22f, 0.68f, 2.70f);
    glScalef(0.22f, 0.12f, 0.30f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.22f, 0.68f, 2.70f);
    glScalef(0.22f, 0.12f, 0.30f);
    glutSolidCube(1.0);
    glPopMatrix();

    float barY = 2.15f + barbellY;
    float barZ = -0.15f;

    float shoulderY = 1.00f;
    float shoulderZ = 0.15f;

    float leftShoulderX = -0.32f;
    float rightShoulderX = 0.32f;

    float leftHandX = -0.75f;
    float rightHandX = 0.75f;

    float leftElbowX = -0.55f;
    float rightElbowX = 0.55f;

    float elbowY = (shoulderY + barY) / 2.0f - 0.15f;
    float elbowZ = 0.00f;

    glColor3f(0.92f, 0.70f, 0.52f);

    drawLimbBetween(leftShoulderX, shoulderY, shoulderZ,
                    leftElbowX, elbowY, elbowZ,
                    0.055f);

    drawLimbBetween(leftElbowX, elbowY, elbowZ,
                    leftHandX, barY, barZ,
                    0.050f);

    drawLimbBetween(rightShoulderX, shoulderY, shoulderZ,
                    rightElbowX, elbowY, elbowZ,
                    0.055f);

    drawLimbBetween(rightElbowX, elbowY, elbowZ,
                    rightHandX, barY, barZ,
                    0.050f);

    glPushMatrix();
    glTranslatef(leftHandX, barY, barZ);
    glutSolidSphere(0.08f, 15, 15);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rightHandX, barY, barZ);
    glutSolidSphere(0.08f, 15, 15);
    glPopMatrix();
}

void drawBenchPress() {
    glPushMatrix();
    glTranslatef(0, 0, -1.5f);

    glColor3f(0.02f, 0.02f, 0.02f);
    glPushMatrix();
    glTranslatef(0, 0.55f, 0.75f);
    drawCube(1.15f, 0.18f, 4.6f);
    glPopMatrix();

    drawManOnBench();

    glColor3f(0.35f, 0.35f, 0.35f);

    glPushMatrix();
    glTranslatef(0, 0.25f, -1.1f);
    drawCube(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.25f, 0.65f);
    drawCube(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.25f, 2.4f);
    drawCube(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    glColor3f(0.45f, 0.45f, 0.45f);

    glPushMatrix();
    glTranslatef(-1.65f, 1.25f, -0.15f);
    drawCube(0.18f, 2.0f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65f, 1.25f, -0.15f);
    drawCube(0.18f, 2.0f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.65f, 0.08f, -0.15f);
    drawCube(0.65f, 0.12f, 0.35f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65f, 0.08f, -0.15f);
    drawCube(0.65f, 0.12f, 0.35f);
    glPopMatrix();

    glColor3f(0.25f, 0.25f, 0.25f);

    glPushMatrix();
    glTranslatef(-1.65f, 2.0f, -0.15f);
    drawCube(0.45f, 0.08f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65f, 2.0f, -0.15f);
    drawCube(0.45f, 0.08f, 0.18f);
    glPopMatrix();

    glColor3f(0.02f, 0.02f, 0.02f);
    glPushMatrix();
    glTranslatef(-1.9f, 2.15f + barbellY, -0.15f);
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.05f, 3.8f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.05f, 2.15f + barbellY, -0.15f);
    glutSolidSphere(0.32f, 25, 25);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.05f, 2.15f + barbellY, -0.15f);
    glutSolidSphere(0.32f, 25, 25);
    glPopMatrix();

    glPopMatrix();
}

void drawTreadmill() {
    glPushMatrix();
    glTranslatef(3.6f, 0.25f, -1.5f);
    glRotatef(90, 0, 1, 0);

    glColor3f(0.35f, 0.75f, 0.75f);
    drawCube(3.0f, 0.25f, 1.3f);

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0, 0.28f, 0);
    drawCube(2.4f, 0.06f, 0.9f);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    float stripeMove = sin(treadmillAngle * 0.05f) * 0.7f;
    glTranslatef(stripeMove, 0.34f, 0);
    drawCube(0.20f, 0.02f, 0.85f);
    glPopMatrix();

    glColor3f(0.35f, 0.35f, 0.35f);

    glPushMatrix();
    glTranslatef(1.25f, 0.9f, -0.45f);
    glRotatef(-10, 0, 0, 1);
    drawCube(0.12f, 1.4f, 0.12f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.25f, 0.9f, 0.45f);
    glRotatef(-10, 0, 0, 1);
    drawCube(0.12f, 1.4f, 0.12f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.25f, 1.55f, 0);
    drawCube(0.12f, 0.12f, 1.2f);
    glPopMatrix();

    glColor3f(0.15f, 0.15f, 0.15f);
    glPushMatrix();
    glTranslatef(1.45f, 1.78f, 0);
    drawCube(0.20f, 0.55f, 1.0f);
    glPopMatrix();

    float r = (sin(treadmillAngle * 0.08f) + 1.0f) / 2.0f;
    float g = (sin(treadmillAngle * 0.10f + 2.0f) + 1.0f) / 2.0f;
    float b = (sin(treadmillAngle * 0.12f + 4.0f) + 1.0f) / 2.0f;

    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);

    glPushMatrix();
    glTranslatef(1.45f, 1.78f, 0);
    drawCube(0.24f, 0.35f, 0.75f);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawFan(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 5.7f, z);

    glColor3f(0.2f, 0.2f, 0.2f);
    glutSolidSphere(0.18f, 20, 20);

    glRotatef(fanAngle, 0, 1, 0);

    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(i * 90, 0, 1, 0);
        glTranslatef(0.7f, 0, 0);

        glColor3f(0.1f, 0.1f, 0.1f);
        drawCube(1.2f, 0.05f, 0.25f);

        glPopMatrix();
    }

    glPopMatrix();
}

void drawFlowerPot(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Pot
    glColor3f(0.55f, 0.25f, 0.08f);
    glPushMatrix();
    glTranslatef(0.0f, -0.35f, 0.0f);
    glScalef(0.28f, 0.30f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Stem
    glColor3f(0.0f, 0.45f, 0.08f);
    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCylinder(0.025f, 0.45f);
    glPopMatrix();

    // Leaves
    glColor3f(0.0f, 0.55f, 0.12f);

    glPushMatrix();
    glTranslatef(-0.12f, 0.08f, 0.0f);
    glScalef(0.18f, 0.07f, 0.04f);
    glutSolidSphere(1.0, 15, 15);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.12f, 0.14f, 0.0f);
    glScalef(0.18f, 0.07f, 0.04f);
    glutSolidSphere(1.0, 15, 15);
    glPopMatrix();

    // Flower center
    glColor3f(1.0f, 0.85f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    glutSolidSphere(0.07f, 15, 15);
    glPopMatrix();

    // Flower petals
    glColor3f(1.0f, 0.15f, 0.35f);

    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glTranslatef(0.0f, 0.42f, 0.0f);
        glRotatef(i * 60.0f, 0, 0, 1);
        glTranslatef(0.12f, 0.0f, 0.0f);
        glScalef(0.09f, 0.045f, 0.035f);
        glutSolidSphere(1.0, 15, 15);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawWindow() {
    glPushMatrix();
    glTranslatef(0, 3.2f, -5.88f);

    glDisable(GL_LIGHTING);

    // =========================
    // Outside sky background
    // =========================
    glColor3f(0.55f, 0.85f, 1.0f);
    glPushMatrix();
    glTranslatef(0, 0.0f, 0.01f);
    drawCube(2.8f, 1.8f, 0.04f);
    glPopMatrix();

    // =========================
    // Outside garden ground
    // =========================
    glColor3f(0.15f, 0.60f, 0.18f);
    glPushMatrix();
    glTranslatef(0, -0.55f, 0.05f);
    drawCube(2.8f, 0.65f, 0.05f);
    glPopMatrix();

    // =========================
    // Outside flowers
    // =========================
    float flowerX[5] = {-1.05f, -0.55f, 0.0f, 0.55f, 1.05f};

    for (int i = 0; i < 5; i++) {
        float x = flowerX[i];

        // Stem
        glColor3f(0.0f, 0.35f, 0.05f);
        glPushMatrix();
        glTranslatef(x, -0.30f, 0.12f);
        drawCube(0.035f, 0.45f, 0.035f);
        glPopMatrix();

        // Leaves
        glColor3f(0.0f, 0.50f, 0.08f);

        glPushMatrix();
        glTranslatef(x - 0.08f, -0.18f, 0.13f);
        glScalef(0.12f, 0.05f, 0.03f);
        glutSolidSphere(1.0, 15, 15);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(x + 0.08f, -0.08f, 0.13f);
        glScalef(0.12f, 0.05f, 0.03f);
        glutSolidSphere(1.0, 15, 15);
        glPopMatrix();

        // Petals
        if (i % 2 == 0)
            glColor3f(1.0f, 0.15f, 0.35f);
        else
            glColor3f(1.0f, 0.75f, 0.10f);

        for (int j = 0; j < 6; j++) {
            glPushMatrix();
            glTranslatef(x, 0.05f, 0.16f);
            glRotatef(j * 60.0f, 0, 0, 1);
            glTranslatef(0.10f, 0.0f, 0.0f);
            glScalef(0.075f, 0.040f, 0.030f);
            glutSolidSphere(1.0, 15, 15);
            glPopMatrix();
        }

        // Flower center
        glColor3f(1.0f, 1.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, 0.05f, 0.18f);
        glutSolidSphere(0.045f, 15, 15);
        glPopMatrix();
    }

    // =========================
    // 4 window glass panels fade on/off
    // =========================
    // windowOpenAmount = 0 means closed / visible
    // windowOpenAmount = 1 means open / vanished

    float alpha = 1.0f - windowOpenAmount;

    if (alpha > 0.02f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Glass color with alpha
        glColor4f(0.72f, 0.90f, 1.0f, alpha);

        // Bigger panel size to remove empty gaps
        float panelW = 1.42f;
        float panelH = 0.88f;
        float panelZ = 0.18f;

        // Top-left glass
        glPushMatrix();
        glTranslatef(-0.70f, 0.44f, panelZ);
        drawCube(panelW, panelH, 0.035f);
        glPopMatrix();

        // Top-right glass
        glPushMatrix();
        glTranslatef(0.70f, 0.44f, panelZ);
        drawCube(panelW, panelH, 0.035f);
        glPopMatrix();

        // Bottom-left glass
        glPushMatrix();
        glTranslatef(-0.70f, -0.44f, panelZ);
        drawCube(panelW, panelH, 0.035f);
        glPopMatrix();

        // Bottom-right glass
        glPushMatrix();
        glTranslatef(0.70f, -0.44f, panelZ);
        drawCube(panelW, panelH, 0.035f);
        glPopMatrix();

        glDisable(GL_BLEND);
    }

    // =========================
    // Window frame
    // =========================
    glColor3f(0.02f, 0.02f, 0.02f);

    // Outer top frame
    glPushMatrix();
    glTranslatef(0, 0.92f, 0.25f);
    drawCube(3.0f, 0.10f, 0.10f);
    glPopMatrix();

    // Outer bottom frame
    glPushMatrix();
    glTranslatef(0, -0.92f, 0.25f);
    drawCube(3.0f, 0.10f, 0.10f);
    glPopMatrix();

    // Outer left frame
    glPushMatrix();
    glTranslatef(-1.45f, 0, 0.25f);
    drawCube(0.10f, 1.9f, 0.10f);
    glPopMatrix();

    // Outer right frame
    glPushMatrix();
    glTranslatef(1.45f, 0, 0.25f);
    drawCube(0.10f, 1.9f, 0.10f);
    glPopMatrix();

    // Middle vertical frame
    glPushMatrix();
    glTranslatef(0, 0, 0.26f);
    drawCube(0.08f, 1.8f, 0.10f);
    glPopMatrix();

    // Middle horizontal frame
    glPushMatrix();
    glTranslatef(0, 0, 0.26f);
    drawCube(2.8f, 0.08f, 0.10f);
    glPopMatrix();

    // =========================
    // Window state indicator
    // =========================
    if (windowOpen)
        glColor3f(0.0f, 0.9f, 0.1f);   // open indicator green
    else
        glColor3f(0.9f, 0.05f, 0.05f); // closed indicator red

    glPushMatrix();
    glTranslatef(1.65f, 0.95f, 0.30f);
    glutSolidSphere(0.08f, 15, 15);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    glPopMatrix();
}
void drawMirror() {
    glPushMatrix();
    glTranslatef(4.5f, 3.0f, -5.85f);

    glColor3f(0.8f, 0.9f, 1.0f);
    drawCube(1.5f, 2.0f, 0.05f);

    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glTranslatef(0, 0, 0.06f);
    drawCube(1.7f, 0.1f, 0.08f);
    glPopMatrix();

    glPopMatrix();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;

    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(55.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        camX, camY, camZ,
        lookX, lookY, lookZ,
        0.0f, 1.0f, 0.0f
    );

    setupLights();

    drawRoom();
    drawLightIndicators();
    drawWindow();
    drawMirror();
    drawDumbbellRack();
    drawBenchPress();
    drawTreadmill();
    drawFan(-2.5f, 1.5f);
    drawFan(2.5f, 1.5f);

    glutSwapBuffers();
}

void update(int value) {
    if (fanOn) {
        if (fanSpeed < 8.0f)
            fanSpeed += 0.15f;
    } else {
        if (fanSpeed > 0.0f)
            fanSpeed -= 0.15f;
    }

    if (fanSpeed < 0.0f)
        fanSpeed = 0.0f;

    fanAngle += fanSpeed;

    if (fanAngle > 360.0f)
        fanAngle -= 360.0f;

    treadmillAngle += 3.0f;
    if (treadmillAngle > 360.0f)
        treadmillAngle -= 360.0f;

    for (int i = 0; i < 6; i++) {
        float liftHeight;

        if (i < 3)
            liftHeight = 0.28f;
        else
            liftHeight = 0.55f;

        float target = dumbbellUp[i] ? liftHeight : 0.0f;

        if (dumbbellY[i] < target)
            dumbbellY[i] += 0.03f;

        if (dumbbellY[i] > target)
            dumbbellY[i] -= 0.03f;
    }

    float barTarget = barbellUp ? 0.55f : 0.0f;

    if (barbellY < barTarget)
        barbellY += 0.035f;

    if (barbellY > barTarget)
        barbellY -= 0.035f;

        // Smooth window open/close fade animation
float windowTarget = windowOpen ? 1.0f : 0.0f;

if (windowOpenAmount < windowTarget)
    windowOpenAmount += 0.03f;

if (windowOpenAmount > windowTarget)
    windowOpenAmount -= 0.03f;

if (windowOpenAmount < 0.0f)
    windowOpenAmount = 0.0f;

if (windowOpenAmount > 1.0f)
    windowOpenAmount = 1.0f;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        dumbbellUp[0] = !dumbbellUp[0];
        break;

    case '2':
        dumbbellUp[1] = !dumbbellUp[1];
        break;

    case '3':
        dumbbellUp[2] = !dumbbellUp[2];
        break;

    case '4':
        dumbbellUp[3] = !dumbbellUp[3];
        break;

    case '5':
        dumbbellUp[4] = !dumbbellUp[4];
        break;

    case '6':
        dumbbellUp[5] = !dumbbellUp[5];
        break;

    case 'b':
    case 'B':
        barbellUp = !barbellUp;
        break;

    case '7':
        lightWhite = !lightWhite;
        break;

    case '8':
        lightGreen = !lightGreen;
        break;

    case 'p':
    case 'P':
        fanOn = !fanOn;
        break;

    case 'o':
    case 'O':
    windowOpen = !windowOpen;
    break;

    case 'w':
    case 'W':
        camZ -= 0.4f;
        lookZ -= 0.4f;
        break;

    case 's':
    case 'S':
        camZ += 0.4f;
        lookZ += 0.4f;
        break;

    case 'a':
    case 'A':
        camX -= 0.4f;
        lookX -= 0.4f;
        break;

    case 'f':
    case 'F':
        camX += 0.4f;
        lookX += 0.4f;
        break;

    case 'q':
    case 'Q':
        camY += 0.3f;
        lookY += 0.3f;
        break;

    case 'e':
    case 'E':
        camY -= 0.3f;
        lookY -= 0.3f;
        break;

    case '9':
        setDumbbellView();
        break;

    case '0':
        setBenchPressView();
        break;

    case 'r':
    case 'R':
        setNormalView();
        break;

    case 27:
        exit(0);
    }

    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0.14f, 0.20f, 0.32f, 1.0f);

    floorTexture = loadTexture("green.jpg");

    if (floorTexture == 0) {
        printf("green.jpg load hoy nai. Floor fallback green hobe.\n");
    }

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("3D Gym Room - OpenGL Project");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);

    glutMainLoop();

    return 0;
}
