#include <windows.h>
#include <mmsystem.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

bool windowOpen = false;
float windowOpenAmount = 0.0f;
bool doorOpen = false;
float doorOpenAmount = 0.0f;

GLuint floorTexture = 0;
GLuint frontWallTexture = 0;
GLuint wallTexture = 0;
GLuint rightWallTexture = 0;

float fanAngle = 0.0f;
float fanSpeed = 8.0f;
bool fanOn = true;

float treadmillAngle = 0.0f;
bool treadmillOn = false;
float dumbbellY[6] = {0, 0, 0, 0, 0, 0};
float barbellY = 0.0f;
float pullUpY = 0.0f;
const float BARBELL_BASE_Y = 1.70f;
const float BARBELL_LIFT_RANGE = 0.35f;
const float PULL_UP_RANGE = 0.52f;

bool dumbbellUp[6] = {false, false, false, false, false, false};
bool barbellUp = false;
bool pullUpRaised = false;
bool lightWhite = true;
bool lightGreen = true;

float camX = 0.0f;
float camY = 3.2f;
float camZ = 12.0f;

float lookX = 0.0f;
float lookY = 2.0f;
float lookZ = 0.0f;

void playBenchPressSound() {
    // Play the weight plate sound when the bench press animation starts.
    mciSendStringA("stop plates_sound", NULL, 0, NULL);
    mciSendStringA("close plates_sound", NULL, 0, NULL);
    mciSendStringA("open \"plates.mp3\" type mpegvideo alias plates_sound", NULL, 0, NULL);
    mciSendStringA("play plates_sound from 0", NULL, 0, NULL);
}

void setNormalView() {
    // Set the camera to the default full-room view.
    camX = 0.0f;
    camY = 3.2f;
    camZ = 12.0f;

    lookX = 0.0f;
    lookY = 2.0f;
    lookZ = 0.0f;
}

void setDumbbellView() {
    // Move the camera near the dumbbell rack.
    camX = -3.8f;
    camY = 2.4f;
    camZ = 6.2f;

    lookX = -4.0f;
    lookY = 1.25f;
    lookZ = 1.5f;
}

void setBenchPressView() {
    // Move the camera near the bench press area.
    camX = 0.0f;
    camY = 2.7f;
    camZ = 5.2f;

    lookX = 0.0f;
    lookY = 1.45f;
    lookZ = -1.1f;
}

void setTreadmillView() {
    // Move the camera to the main treadmill view.
    camX = 3.6f;
    camY = 2.45f;
    camZ = -5.0f;

    lookX = 3.6f;
    lookY = 1.45f;
    lookZ = -1.5f;
}

void setTreadmillOppositeView() {
    // Move the camera to the opposite side of the treadmill.
    camX = 3.6f;
    camY = 2.45f;
    camZ = 2.0f;

    lookX = 3.6f;
    lookY = 1.45f;
    lookZ = -1.5f;
}

void drawCube(float x, float y, float z) {
    // Draw a cube using the given width, height, and depth.
    glPushMatrix();
    glScalef(x, y, z);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawCylinder(float radius, float height) {
    // Draw a vertical cylinder with the given radius and height.
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, 30, 30);
    gluDeleteQuadric(quad);
}

void setMaterialShininess(float value) {
    // Control how strongly the current material reflects light.
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, value);
}

void drawWallText(float x, float y, float z, const char* text, float scale) {
    // Draw scaled text at a fixed position on the wall.
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    for (const char* c = text; *c != '\0'; c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }

    glPopMatrix();
}

void drawGroundShadow(float x, float z, float width, float depth, float alpha) {
    // Draw a soft flat shadow below a gym object.
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, alpha);

    glBegin(GL_QUADS);
        glVertex3f(x - width / 2.0f, 0.025f, z - depth / 2.0f);
        glVertex3f(x + width / 2.0f, 0.025f, z - depth / 2.0f);
        glVertex3f(x + width / 2.0f, 0.025f, z + depth / 2.0f);
        glVertex3f(x - width / 2.0f, 0.025f, z + depth / 2.0f);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawSceneShadows() {
    // Draw the main equipment shadows on the room floor.
    drawGroundShadow(3.6f, -1.5f, 3.6f, 1.7f, 0.22f);
    drawGroundShadow(0.0f, -0.75f, 4.7f, 4.9f, 0.18f);
    drawGroundShadow(-3.5f, 1.5f, 4.9f, 1.2f, 0.18f);
    drawGroundShadow(0.0f, 4.0f, 5.0f, 1.0f, 0.10f);
}

GLuint loadTexture(const char* filename) {
    // Load an image file and return its OpenGL texture ID.
    int width, height, channels;

    // Flip the image so its orientation matches OpenGL coordinates.
    stbi_set_flip_vertically_on_load(true);

    // Read the image as RGB pixel data.
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);

    // Stop and return an invalid texture ID if loading fails.
    if (!data) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint textureID;
    // Create and activate a new OpenGL texture object.
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Set texture filtering and edge wrapping behavior.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Upload the pixels and generate smaller mipmap versions.
    gluBuild2DMipmaps(
        GL_TEXTURE_2D,
        GL_RGB,
        width,
        height,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );

    // Release image data after it has been copied to OpenGL.
    stbi_image_free(data);

    printf("Texture loaded successfully: %s\n", filename);

    return textureID;
}

void setupLights() {
    // Configure the room's white and green OpenGL lights.
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Set a low ambient light so the room is not completely dark.
    GLfloat globalAmbient[] = {0.10f, 0.10f, 0.10f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Main warm-white ceiling light controlled by key 7.
    if (lightWhite) {
        glEnable(GL_LIGHT0);

        GLfloat pos0[] = {0.0f, 5.75f, 1.2f, 1.0f};
        GLfloat white[] = {0.82f, 0.80f, 0.68f, 1.0f};
        GLfloat ambient0[] = {0.12f, 0.12f, 0.10f, 1.0f};
        GLfloat spec0[] = {0.55f, 0.55f, 0.48f, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, pos0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
    } else {
        glDisable(GL_LIGHT0);
    }

    // Green accent light controlled by key 8.
    if (lightGreen) {
        glEnable(GL_LIGHT1);

        GLfloat pos1[] = {-4.2f, 5.75f, -2.3f, 1.0f};
        GLfloat green[] = {0.0f, 0.48f, 0.12f, 1.0f};
        GLfloat ambient1[] = {0.0f, 0.07f, 0.03f, 1.0f};
        GLfloat spec1[] = {0.05f, 0.28f, 0.08f, 1.0f};

        glLightfv(GL_LIGHT1, GL_POSITION, pos1);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, green);
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
        glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
    } else {
        glDisable(GL_LIGHT1);
    }

    // Extra white light placed near the front-wall tube light.
    if (lightWhite) {
        glEnable(GL_LIGHT2);

        GLfloat pos2[] = {4.5f, 4.55f, -5.35f, 1.0f};
        GLfloat tubeWhite[] = {0.95f, 0.92f, 0.82f, 1.0f};
        GLfloat tubeAmbient[] = {0.08f, 0.08f, 0.07f, 1.0f};
        GLfloat tubeSpec[] = {0.75f, 0.72f, 0.65f, 1.0f};

        glLightfv(GL_LIGHT2, GL_POSITION, pos2);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, tubeWhite);
        glLightfv(GL_LIGHT2, GL_AMBIENT, tubeAmbient);
        glLightfv(GL_LIGHT2, GL_SPECULAR, tubeSpec);
    } else {
        glDisable(GL_LIGHT2);
    }
}

void drawFrontWallPanel(float x1, float x2, float y1, float y2) {
    // Draw one rectangular section of the textured front wall.
    if (frontWallTexture != 0) {
        // Convert wall positions into texture coordinates.
        float u1 = (6.0f - x1) / 12.0f;
        float u2 = (6.0f - x2) / 12.0f;
        float v1 = y1 / 6.0f;
        float v2 = y2 / 6.0f;

        // Attach the correct texture point to each panel corner.
        glTexCoord2f(u1, v1); glVertex3f(x1, y1, -5.89f);
        glTexCoord2f(u2, v1); glVertex3f(x2, y1, -5.89f);
        glTexCoord2f(u2, v2); glVertex3f(x2, y2, -5.89f);
        glTexCoord2f(u1, v2); glVertex3f(x1, y2, -5.89f);
    } else {
        // Draw the same panel without texture as a fallback.
        glVertex3f(x1, y1, -5.89f);
        glVertex3f(x2, y1, -5.89f);
        glVertex3f(x2, y2, -5.89f);
        glVertex3f(x1, y2, -5.89f);
    }
}

void drawRoom() {
    // Draw the floor, walls, doorway, and ceiling of the gym room.
    glDisable(GL_LIGHTING);

    // Floor texture
    if (floorTexture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex3f(-6.0f, 0.01f, -6.0f);
            glTexCoord2f(1.0f, 0.0f);   glVertex3f( 6.0f, 0.01f, -6.0f);
            glTexCoord2f(1.0f, 1.0f);   glVertex3f( 6.0f, 0.01f,  6.0f);
            glTexCoord2f(0.0f, 1.0f);   glVertex3f(-6.0f, 0.01f,  6.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.45f, 0.85f, 0.50f);
        glPushMatrix();
        glTranslatef(0, -0.05f, 0);
        drawCube(12, 0.1f, 12);
        glPopMatrix();
    }

    // Front wall with doorway
    const float doorLeft = -5.25f;
    const float doorRight = -3.55f;
    const float doorTop = 3.25f;

    if (frontWallTexture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, frontWallTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            drawFrontWallPanel( 6.0f, doorRight, 0.0f, 6.0f);
            drawFrontWallPanel(doorLeft, -6.0f, 0.0f, 6.0f);
            drawFrontWallPanel(doorRight, doorLeft, doorTop, 6.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.45f, 0.68f, 0.95f);
        glBegin(GL_QUADS);
            drawFrontWallPanel( 6.0f, doorRight, 0.0f, 6.0f);
            drawFrontWallPanel(doorLeft, -6.0f, 0.0f, 6.0f);
            drawFrontWallPanel(doorRight, doorLeft, doorTop, 6.0f);
        glEnd();
    }

    // Left wall
    if (wallTexture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex3f(-5.89f, 0.0f,  6.0f);
            glTexCoord2f(1.0f, 0.0f);   glVertex3f(-5.89f, 0.0f, -6.0f);
            glTexCoord2f(1.0f, 1.0f);   glVertex3f(-5.89f, 6.0f, -6.0f);
            glTexCoord2f(0.0f, 1.0f);   glVertex3f(-5.89f, 6.0f,  6.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.50f, 0.75f, 1.0f);
        glPushMatrix();
        glTranslatef(-6, 3, 0);
        drawCube(0.2f, 6, 12);
        glPopMatrix();
    }

    // Right wall
    if (rightWallTexture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, rightWallTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex3f(5.89f, 0.0f, -6.0f);
            glTexCoord2f(1.0f, 0.0f);   glVertex3f(5.89f, 0.0f,  6.0f);
            glTexCoord2f(1.0f, 1.0f);   glVertex3f(5.89f, 6.0f,  6.0f);
            glTexCoord2f(0.0f, 1.0f);   glVertex3f(5.89f, 6.0f, -6.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.50f, 0.75f, 1.0f);
        glPushMatrix();
        glTranslatef(6, 3, 0);
        drawCube(0.2f, 6, 12);
        glPopMatrix();
    }

    // Ceiling
    if (lightWhite && lightGreen)
        glColor3f(0.22f, 0.38f, 0.30f);
    else if (lightWhite)
        glColor3f(0.28f, 0.30f, 0.27f);
    else if (lightGreen)
        glColor3f(0.05f, 0.24f, 0.10f);
    else
        glColor3f(0.04f, 0.05f, 0.06f);

    glPushMatrix();
    glTranslatef(0, 6, 0);
    drawCube(12, 0.1f, 12);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void drawLightIndicators() {
    // Show visual indicators for the current light states.
    glDisable(GL_LIGHTING);

    // White indicator becomes bright when the white light is on.
    glPushMatrix();
    glTranslatef(0.0f, 5.85f, 2.0f);

    if (lightWhite)
        glColor3f(1.0f, 1.0f, 0.75f);
    else
        glColor3f(0.15f, 0.15f, 0.15f);

    glutSolidSphere(0.18f, 20, 20);
    glPopMatrix();

    // Green indicator becomes bright when the green light is on.
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

void drawDoor() {
    // Draw the door frame, door panel, handle, and opening rotation.
    // Define the door size, hinge position, and animation angle.
    const float doorLeft = -5.25f;
    const float doorRight = -3.55f;
    const float doorWidth = doorRight - doorLeft;
    const float doorHeight = 3.25f;
    const float hingeX = doorLeft;
    const float doorZ = -5.78f;
    float openAngle = -105.0f * doorOpenAmount;

    glDisable(GL_TEXTURE_2D);
    setMaterialShininess(22.0f);

    // Draw the top, left, and right parts of the fixed door frame.
    glColor3f(0.12f, 0.075f, 0.035f);
    // Move to the hinge and rotate the complete door panel.
    glPushMatrix();
    glTranslatef((doorLeft + doorRight) / 2.0f, doorHeight + 0.05f, doorZ);
    drawCube(doorWidth + 0.22f, 0.16f, 0.16f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(doorLeft - 0.08f, doorHeight / 2.0f, doorZ);
    drawCube(0.16f, doorHeight + 0.14f, 0.16f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(doorRight + 0.08f, doorHeight / 2.0f, doorZ);
    drawCube(0.16f, doorHeight + 0.14f, 0.16f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(hingeX, doorHeight / 2.0f, doorZ + 0.02f);
    glRotatef(openAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(doorWidth / 2.0f, 0.0f, 0.0f);

    // Draw the main wooden door panel.
    glColor3f(0.38f, 0.19f, 0.07f);
    drawCube(doorWidth, doorHeight, 0.12f);

    // Draw raised horizontal and vertical decoration strips.
    glColor3f(0.52f, 0.28f, 0.11f);
    glPushMatrix();
    glTranslatef(0.0f, 0.62f, -0.065f);
    drawCube(doorWidth - 0.35f, 0.06f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.55f, -0.065f);
    drawCube(doorWidth - 0.35f, 0.06f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.55f, 0.0f, -0.065f);
    drawCube(0.06f, doorHeight - 0.55f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.55f, 0.0f, -0.065f);
    drawCube(0.06f, doorHeight - 0.55f, 0.025f);
    glPopMatrix();

    // Draw the golden spherical door handle.
    glColor3f(0.93f, 0.70f, 0.20f);
    glPushMatrix();
    glTranslatef(0.58f, 0.0f, -0.12f);
    glutSolidSphere(0.075f, 16, 16);
    glPopMatrix();

    glPopMatrix();

    setMaterialShininess(24.0f);
}

void drawWallDecorations() {
    // Draw the poster, room title, and wall clock.
    glDisable(GL_LIGHTING);

    // Motivational poster on the front-left wall
    // Draw the dark poster base and red inner panel.
    glColor3f(0.04f, 0.04f, 0.05f);
    glPushMatrix();
    glTranslatef(-3.85f, 3.90f, -5.82f);
    drawCube(1.95f, 1.05f, 0.030f);
    glPopMatrix();

    glColor3f(0.95f, 0.18f, 0.06f);
    glPushMatrix();
    glTranslatef(-3.85f, 3.90f, -5.79f);
    drawCube(1.70f, 0.78f, 0.025f);
    glPopMatrix();

    // Draw the four black sides of the poster frame.
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-3.85f, 4.41f, -5.75f);
    drawCube(1.95f, 0.08f, 0.040f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3.85f, 3.39f, -5.75f);
    drawCube(1.95f, 0.08f, 0.040f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.825f, 3.90f, -5.75f);
    drawCube(0.08f, 1.02f, 0.040f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.875f, 3.90f, -5.75f);
    drawCube(0.08f, 1.02f, 0.040f);
    glPopMatrix();

    // Write the motivational message over the poster.
    glColor3f(1.0f, 1.0f, 1.0f);
    drawWallText(-4.42f, 4.06f, -5.74f, "NO PAIN", 0.0018f);
    drawWallText(-4.38f, 3.78f, -5.74f, "NO GAIN", 0.0018f);

    // Gym room title above the front window
    glColor3f(0.95f, 0.18f, 0.06f);
    drawWallText(-0.98f, 5.21f, -5.74f, "GYM ROOM", 0.0030f);

    // Running analog clock on the upper-right front wall
    glPushMatrix();
    glTranslatef(4.75f, 5.35f, -5.74f);

    glDisable(GL_LIGHTING);

    // Draw the black outer circle of the clock.
    glColor3f(0.02f, 0.02f, 0.025f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.010f);
        for (int i = 0; i <= 48; i++) {
            float angle = i * 2.0f * 3.1416f / 48.0f;
            glVertex3f(cos(angle) * 0.39f, sin(angle) * 0.39f, 0.010f);
        }
    glEnd();

    // Draw the light-colored clock face.
    glColor3f(0.94f, 0.94f, 0.90f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.020f);
        for (int i = 0; i <= 48; i++) {
            float angle = i * 2.0f * 3.1416f / 48.0f;
            glVertex3f(cos(angle) * 0.32f, sin(angle) * 0.32f, 0.020f);
        }
    glEnd();

    // Draw twelve marks around the clock face.
    glColor3f(0.03f, 0.03f, 0.035f);
    for (int i = 0; i < 12; i++) {
        glPushMatrix();
        glRotatef(i * 30.0f, 0, 0, 1);
        glTranslatef(0.0f, 0.27f, 0.040f);
        drawCube(0.020f, 0.050f, 0.012f);
        glPopMatrix();
    }

    // Rotate the hour, minute, and second hands using animation time.
    glPushMatrix();
    glRotatef(-fanAngle * 0.015f, 0, 0, 1);
    glTranslatef(0.0f, 0.08f, 0.055f);
    glColor3f(1.0f, 0.25f, 0.72f);
    drawCube(0.026f, 0.18f, 0.012f);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-fanAngle * 0.08f, 0, 0, 1);
    glTranslatef(0.0f, 0.12f, 0.065f);
    glColor3f(1.0f, 0.25f, 0.72f);
    drawCube(0.018f, 0.26f, 0.012f);
    glPopMatrix();

    glColor3f(1.0f, 0.25f, 0.72f);
    glPushMatrix();
    glRotatef(-fanAngle * 0.30f, 0, 0, 1);
    glTranslatef(0.0f, 0.15f, 0.075f);
    drawCube(0.010f, 0.31f, 0.010f);
    glPopMatrix();

    glColor3f(0.02f, 0.02f, 0.025f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.085f);
    drawCube(0.070f, 0.070f, 0.012f);
    glPopMatrix();

    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void drawPullUpBar() {
    // Draw the pull-+up bar and its supporting frame.
    // Draw two wall-mounted support blocks.
    glPushMatrix();

    setMaterialShininess(70.0f);
    glColor3f(0.08f, 0.08f, 0.09f);

    glPushMatrix();
    glTranslatef(5.78f, 4.45f, -0.95f);
    drawCube(0.07f, 0.62f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.78f, 4.45f, 0.95f);
    drawCube(0.07f, 0.62f, 0.18f);
    glPopMatrix();

    // Draw the two bars extending outward from the wall.
    glColor3f(0.72f, 0.74f, 0.76f);
    // Draw the long horizontal bar held by the person.
    glPushMatrix();
    glTranslatef(5.70f, 4.65f, -0.95f);
    glRotatef(-90, 0, 1, 0);
    drawCylinder(0.045f, 0.85f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.70f, 4.65f, 0.95f);
    glRotatef(-90, 0, 1, 0);
    drawCylinder(0.045f, 0.85f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.88f, 4.65f, -1.18f);
    drawCylinder(0.050f, 2.36f);
    glPopMatrix();

    // Add red grips at both ends of the horizontal bar.
    glColor3f(0.95f, 0.18f, 0.06f);
    glPushMatrix();
    glTranslatef(4.88f, 4.65f, -1.18f);
    drawCylinder(0.070f, 0.16f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.88f, 4.65f, 1.02f);
    drawCylinder(0.070f, 0.16f);
    glPopMatrix();

    setMaterialShininess(24.0f);
    glPopMatrix();
}

void drawDumbbell(float x, float y, float z, float size) {
    // Draw one dumbbell at the given position and size.
    glPushMatrix();

    glTranslatef(x, y, z);
    glRotatef(90, 0, 1, 0);

    // Calculate all dumbbell parts from the supplied size.
    float handleLength = 0.55f * size;
    float plateSmall   = 0.15f * size;
    float plateBig     = 0.19f * size;
    float plateWidth   = 0.08f * size;

    // Draw the shiny metal center handle.
    setMaterialShininess(82.0f);
    glColor3f(0.88f, 0.90f, 0.92f);
    glPushMatrix();
    glTranslatef(0, 0, -handleLength / 2.0f);
    drawCylinder(0.04f * size, handleLength);
    glPopMatrix();

    // Draw the dark grip around the center of the handle.
    setMaterialShininess(12.0f);
    glColor3f(0.02f, 0.02f, 0.02f);
    glPushMatrix();
    glTranslatef(0, 0, -0.12f * size);
    drawCylinder(0.055f * size, 0.24f * size);
    glPopMatrix();

    // Draw large and small weight plates on both sides.
    glColor3f(0.05f, 0.05f, 0.05f);

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

    // Draw red end caps and metal locking collars.
    setMaterialShininess(48.0f);
    glColor3f(0.95f, 0.18f, 0.06f);

    glPushMatrix();
    glTranslatef(0, 0, -0.57f * size);
    drawCylinder(0.055f * size, 0.04f * size);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.50f * size);
    drawCylinder(0.055f * size, 0.04f * size);
    glPopMatrix();

    glColor3f(0.80f, 0.82f, 0.84f);
    glPushMatrix();
    glTranslatef(0, 0, -0.32f * size);
    drawCylinder(0.050f * size, 0.035f * size);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.27f * size);
    drawCylinder(0.050f * size, 0.035f * size);
    glPopMatrix();

    setMaterialShininess(24.0f);
    glPopMatrix();
}

void drawDumbbellRack() {
    // Draw the rack and all animated dumbbells placed on it.
    // Move the complete rack to the left side of the room.
    glPushMatrix();
    glTranslatef(-3.5f, 0, 1.5f);

    glColor3f(0.25f, 0.25f, 0.25f);

    // Draw two horizontal shelves.
    glPushMatrix();
    glTranslatef(0, 0.75f, 0);
    drawCube(4.4f, 0.12f, 0.35f);
    glPopMatrix();

    // Draw left, middle, and right vertical supports.
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

    // Store the three horizontal positions used on both shelves.
    float rackX[3] = {-1.45f, 0.0f, 1.45f};

    // Draw three smaller animated dumbbells on the lower shelf.
    for (int i = 0; i < 3; i++) {
        drawDumbbell(rackX[i], 0.98f + dumbbellY[i], 0.0f, 0.75f + i * 0.08f);
    }

    // Draw three larger animated dumbbells on the upper shelf.
    for (int i = 0; i < 3; i++) {
        drawDumbbell(rackX[i], 1.68f + dumbbellY[i + 3], 0.0f, 0.90f + i * 0.08f);
    }

    glPopMatrix();
}

void drawBodyCylinder(float radius, float height) {
    // Draw a cylinder used as part of a human body model.
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, 25, 25);
    gluDeleteQuadric(quad);
}

void drawLimbBetween(float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     float radius) {
    // Draw an arm or leg between two points in 3D space.
    // Find the direction and length between the two joint positions.
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    float length = sqrt(dx * dx + dy * dy + dz * dz);
    if (length <= 0.001f) return;

    glPushMatrix();
    glTranslatef(x1, y1, z1);

    // Rotate a cylinder so it points from the first joint to the second.
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
    // Draw and animate the person using the bench press.
    // Draw the blue torso lying on the bench.
    glColor3f(0.0f, 0.15f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.82f, 0.75f);
    glScalef(0.70f, 0.22f, 1.35f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw the head near the top of the bench.
    glColor3f(0.92f, 0.70f, 0.52f);
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, -0.05f);
    glutSolidSphere(0.23f, 25, 25);
    glPopMatrix();

    // Draw the two legs in dark clothing.
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

    // Draw blue shoes at the end of both legs.
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

    // Calculate arm joints from the animated barbell height.
    float barY = BARBELL_BASE_Y + barbellY;
    float barZ = -0.15f;

    float shoulderY = 0.88f;
    float shoulderZ = 0.35f;

    float leftShoulderX = -0.34f;
    float rightShoulderX = 0.34f;

    float leftHandX = -0.75f;
    float rightHandX = 0.75f;

    float leftElbowX = -0.55f;
    float rightElbowX = 0.55f;

    float elbowY = (shoulderY + barY) / 2.0f - 0.15f;
    float elbowZ = 0.00f;

    // Draw shoulders, upper arms, forearms, and hands gripping the bar.
    glColor3f(0.92f, 0.70f, 0.52f);

    glPushMatrix();
    glTranslatef(leftShoulderX, shoulderY, shoulderZ);
    glutSolidSphere(0.075f, 15, 15);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rightShoulderX, shoulderY, shoulderZ);
    glutSolidSphere(0.075f, 15, 15);
    glPopMatrix();

    drawLimbBetween(leftShoulderX, shoulderY, shoulderZ,
                    leftElbowX, elbowY, elbowZ,
                    0.065f);

    drawLimbBetween(leftElbowX, elbowY, elbowZ,
                    leftHandX, barY, barZ,
                    0.050f);

    drawLimbBetween(rightShoulderX, shoulderY, shoulderZ,
                    rightElbowX, elbowY, elbowZ,
                    0.065f);

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

void drawPullUpMan() {
    // Draw and animate the person performing pull-ups.
    // Move all body positions upward using the pull-up animation value.
    float lift = pullUpY;
    float handY = 4.65f;
    float shoulderY = 3.92f + lift;
    float torsoY = 3.36f + lift;
    float hipY = 2.78f + lift;

    float centerX = 4.78f;
    float handX = 4.88f;

    glPushMatrix();

    // Draw both raised arms and hands attached to the bar.
    glColor3f(0.92f, 0.70f, 0.52f);
    drawLimbBetween(centerX, shoulderY, -0.20f, handX, handY, -0.48f, 0.055f);
    drawLimbBetween(centerX, shoulderY, 0.20f, handX, handY, 0.48f, 0.055f);

    glPushMatrix();
    glTranslatef(handX, handY, -0.48f);
    glutSolidSphere(0.075f, 14, 14);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(handX, handY, 0.48f);
    glutSolidSphere(0.075f, 14, 14);
    glPopMatrix();

    // Draw the torso, neck, and head.
    glColor3f(0.03f, 0.18f, 0.65f);
    glPushMatrix();
    glTranslatef(centerX, torsoY, 0.0f);
    drawCube(0.34f, 1.12f, 0.60f);
    glPopMatrix();

    glColor3f(0.92f, 0.70f, 0.52f);
    glPushMatrix();
    glTranslatef(centerX, shoulderY + 0.12f, 0.0f);
    drawCube(0.11f, 0.24f, 0.13f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(centerX, shoulderY + 0.34f, 0.0f);
    glutSolidSphere(0.20f, 22, 22);
    glPopMatrix();

    // Draw both hanging legs.
    glColor3f(0.04f, 0.04f, 0.04f);
    drawLimbBetween(centerX, hipY + 0.06f, -0.17f, centerX, hipY - 1.22f, -0.24f, 0.065f);
    drawLimbBetween(centerX, hipY + 0.06f, 0.17f, centerX, hipY - 1.22f, 0.24f, 0.065f);

    // Draw red shoes below the legs.
    glColor3f(0.95f, 0.18f, 0.06f);
    glPushMatrix();
    glTranslatef(centerX, hipY - 1.32f, -0.26f);
    drawCube(0.18f, 0.09f, 0.28f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(centerX, hipY - 1.32f, 0.26f);
    drawCube(0.18f, 0.09f, 0.28f);
    glPopMatrix();

    glPopMatrix();
}

void drawTreadmillRunner() {
    // Draw and animate the person running on the treadmill.
    // Use a sine wave to create a repeating running motion.
    float phase = treadmillAngle * 3.1416f / 45.0f;
    float swing = sin(phase);
    float lift = fabs(swing);

    float centerX = -0.28f;
    float footY = 0.43f;
    float hipY = 1.26f;
    float shoulderY = 2.02f;

    glPushMatrix();

    // Draw the runner's torso, neck, head, and hair.
    setMaterialShininess(18.0f);

    glColor3f(0.08f, 0.24f, 0.78f);
    glPushMatrix();
    glTranslatef(centerX, 1.68f, 0.0f);
    drawCube(0.34f, 0.86f, 0.48f);
    glPopMatrix();

    glColor3f(0.92f, 0.70f, 0.52f);
    glPushMatrix();
    glTranslatef(centerX, 2.20f, 0.0f);
    drawCube(0.11f, 0.20f, 0.13f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(centerX + 0.04f, 2.42f, 0.0f);
    glutSolidSphere(0.19f, 22, 22);
    glPopMatrix();

    glColor3f(0.04f, 0.04f, 0.04f);
    glPushMatrix();
    glTranslatef(centerX - 0.03f, 2.56f, 0.0f);
    glScalef(0.34f, 0.13f, 0.34f);
    glutSolidSphere(1.0f, 18, 18);
    glPopMatrix();

    // Calculate opposite leg movement for a natural running cycle.
    float leftHipZ = -0.13f;
    float rightHipZ = 0.13f;
    float leftFootX = centerX + swing * 0.42f;
    float rightFootX = centerX - swing * 0.42f;
    float leftKneeX = centerX + swing * 0.22f;
    float rightKneeX = centerX - swing * 0.22f;
    float leftKneeY = 0.82f + (swing > 0.0f ? lift * 0.18f : 0.0f);
    float rightKneeY = 0.82f + (swing < 0.0f ? lift * 0.18f : 0.0f);
    float leftFootY = footY + (swing > 0.0f ? lift * 0.08f : 0.0f);
    float rightFootY = footY + (swing < 0.0f ? lift * 0.08f : 0.0f);

    // Draw both moving legs from hip to knee to foot.
    glColor3f(0.05f, 0.05f, 0.055f);
    drawLimbBetween(centerX, hipY, leftHipZ, leftKneeX, leftKneeY, leftHipZ, 0.060f);
    drawLimbBetween(leftKneeX, leftKneeY, leftHipZ, leftFootX, leftFootY, leftHipZ, 0.052f);
    drawLimbBetween(centerX, hipY, rightHipZ, rightKneeX, rightKneeY, rightHipZ, 0.060f);
    drawLimbBetween(rightKneeX, rightKneeY, rightHipZ, rightFootX, rightFootY, rightHipZ, 0.052f);

    // Draw shoes at the animated foot positions.
    glColor3f(0.95f, 0.18f, 0.06f);
    glPushMatrix();
    glTranslatef(leftFootX + 0.05f, leftFootY - 0.02f, leftHipZ);
    drawCube(0.28f, 0.08f, 0.16f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rightFootX + 0.05f, rightFootY - 0.02f, rightHipZ);
    drawCube(0.28f, 0.08f, 0.16f);
    glPopMatrix();

    // Swing the arms opposite to the legs.
    float leftShoulderZ = -0.24f;
    float rightShoulderZ = 0.24f;
    float leftHandX = centerX - swing * 0.34f;
    float rightHandX = centerX + swing * 0.34f;
    float leftElbowX = centerX - swing * 0.20f;
    float rightElbowX = centerX + swing * 0.20f;

    glColor3f(0.92f, 0.70f, 0.52f);
    drawLimbBetween(centerX, shoulderY, leftShoulderZ, leftElbowX, 1.64f, leftShoulderZ, 0.046f);
    drawLimbBetween(leftElbowX, 1.64f, leftShoulderZ, leftHandX, 1.26f, leftShoulderZ, 0.040f);
    drawLimbBetween(centerX, shoulderY, rightShoulderZ, rightElbowX, 1.64f, rightShoulderZ, 0.046f);
    drawLimbBetween(rightElbowX, 1.64f, rightShoulderZ, rightHandX, 1.26f, rightShoulderZ, 0.040f);

    glPushMatrix();
    glTranslatef(leftHandX, 1.26f, leftShoulderZ);
    glutSolidSphere(0.055f, 12, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rightHandX, 1.26f, rightShoulderZ);
    glutSolidSphere(0.055f, 12, 12);
    glPopMatrix();

    glPopMatrix();
}

void drawBenchPress() {
    // Draw the bench, barbell, plates, and bench press setup.
    // Move the complete bench press setup toward the back of the room.
    glPushMatrix();
    glTranslatef(0, 0, -1.5f);

    // Draw the long padded bench.
    setMaterialShininess(16.0f);
    glColor3f(0.03f, 0.03f, 0.035f);
    glPushMatrix();
    glTranslatef(0, 0.55f, 0.75f);
    drawCube(1.15f, 0.18f, 4.6f);
    glPopMatrix();

    // Place the animated person on top of the bench.
    drawManOnBench();

    // Draw three metal legs below the bench.
    setMaterialShininess(72.0f);
    glColor3f(0.62f, 0.64f, 0.66f);

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

    // Draw the two vertical barbell rack posts and their feet.
    glColor3f(0.70f, 0.72f, 0.74f);

    glPushMatrix();
    glTranslatef(-1.65f, 0.92f, -0.15f);
    drawCube(0.18f, 1.35f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65f, 0.92f, -0.15f);
    drawCube(0.18f, 1.35f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.65f, 0.08f, -0.15f);
    drawCube(0.65f, 0.12f, 0.35f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65f, 0.08f, -0.15f);
    drawCube(0.65f, 0.12f, 0.35f);
    glPopMatrix();

    // Draw dark supports that hold the barbell.
    setMaterialShininess(16.0f);
    glColor3f(0.10f, 0.10f, 0.11f);

    glPushMatrix();
    glTranslatef(-1.65f, 1.58f, -0.15f);
    drawCube(0.45f, 0.08f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65f, 1.58f, -0.15f);
    drawCube(0.45f, 0.08f, 0.18f);
    glPopMatrix();

    // Draw the metal bar at its animated height.
    setMaterialShininess(90.0f);
    glColor3f(0.86f, 0.88f, 0.90f);
    glPushMatrix();
    glTranslatef(-1.9f, BARBELL_BASE_Y + barbellY, -0.15f);
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.05f, 3.8f);
    glPopMatrix();

    // Draw black weight plates on both ends of the bar.
    setMaterialShininess(10.0f);
    glColor3f(0.03f, 0.03f, 0.035f);
    glPushMatrix();
    glTranslatef(-2.18f, BARBELL_BASE_Y + barbellY, -0.15f);
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.32f, 0.26f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.92f, BARBELL_BASE_Y + barbellY, -0.15f);
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.32f, 0.26f);
    glPopMatrix();

    // Draw red collars that lock the plates in place.
    setMaterialShininess(48.0f);
    glColor3f(0.95f, 0.18f, 0.06f);
    glPushMatrix();
    glTranslatef(-1.88f, BARBELL_BASE_Y + barbellY, -0.15f);
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.11f, 0.06f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.82f, BARBELL_BASE_Y + barbellY, -0.15f);
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.11f, 0.06f);
    glPopMatrix();

    setMaterialShininess(24.0f);
    glPopMatrix();
}

void drawTreadmill() {
    // Draw the treadmill and animate its moving belt.
    // Position and rotate the complete treadmill in the room.
    glPushMatrix();
    glTranslatef(3.6f, 0.25f, -1.5f);
    glRotatef(90, 0, 1, 0);

    // Draw the main treadmill base.
    setMaterialShininess(64.0f);
    glColor3f(0.58f, 0.62f, 0.64f);
    drawCube(3.0f, 0.25f, 1.3f);

    // Draw small rear support feet.
    setMaterialShininess(12.0f);
    glColor3f(0.03f, 0.03f, 0.035f);
    glPushMatrix();
    glTranslatef(-1.22f, 0.18f, -0.55f);
    drawCube(0.14f, 0.12f, 0.10f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.22f, 0.18f, 0.55f);
    drawCube(0.14f, 0.12f, 0.10f);
    glPopMatrix();

    // Draw the dark running belt.
    glColor3f(0.01f, 0.01f, 0.012f);
    glPushMatrix();
    glTranslatef(0, 0.28f, 0);
    drawCube(2.4f, 0.06f, 0.9f);
    glPopMatrix();

    // Draw shiny side rails around the belt.
    setMaterialShininess(88.0f);
    glColor3f(0.78f, 0.80f, 0.82f);
    glPushMatrix();
    glTranslatef(0, 0.35f, -0.52f);
    drawCube(2.6f, 0.05f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.35f, 0.52f);
    drawCube(2.6f, 0.05f, 0.08f);
    glPopMatrix();

    // Move a red stripe to show that the belt is running.
    glColor3f(0.95f, 0.18f, 0.06f);
    glPushMatrix();
    float stripeMove = sin(treadmillAngle * 0.05f) * 0.7f;
    glTranslatef(stripeMove, 0.34f, 0);
    drawCube(0.20f, 0.02f, 0.85f);
    glPopMatrix();

    // Place the animated runner above the belt.
    drawTreadmillRunner();

    // Draw the two upright handle supports and crossbar.
    setMaterialShininess(76.0f);
    glColor3f(0.70f, 0.72f, 0.74f);

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

    // Draw the hand grips at both sides.
    setMaterialShininess(10.0f);
    glColor3f(0.03f, 0.03f, 0.035f);
    glPushMatrix();
    glTranslatef(1.25f, 1.55f, -0.52f);
    drawCube(0.18f, 0.16f, 0.20f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.25f, 1.55f, 0.52f);
    drawCube(0.18f, 0.16f, 0.20f);
    glPopMatrix();

    // Draw the treadmill control console.
    glColor3f(0.08f, 0.08f, 0.09f);
    glPushMatrix();
    glTranslatef(1.45f, 1.78f, 0);
    drawCube(0.20f, 0.55f, 1.0f);
    glPopMatrix();

    // Disable lighting so the display looks self-illuminated.
    glDisable(GL_LIGHTING);
    glColor3f(0.08f, 0.55f, 0.42f);

    glPushMatrix();
    glTranslatef(1.45f, 1.78f, 0);
    drawCube(0.24f, 0.35f, 0.75f);
    glPopMatrix();

    glColor3f(0.01f, 0.03f, 0.04f);
    glPushMatrix();
    glTranslatef(1.30f, 1.84f, 0.0f);
    drawCube(0.05f, 0.24f, 0.62f);
    glPopMatrix();

    // Draw display lines and three red control buttons.
    glColor3f(0.00f, 0.95f, 0.62f);
    glPushMatrix();
    glTranslatef(1.265f, 1.89f, -0.12f);
    drawCube(0.025f, 0.035f, 0.22f);
    glPopMatrix();

    glColor3f(0.15f, 0.78f, 1.0f);
    glPushMatrix();
    glTranslatef(1.265f, 1.81f, 0.12f);
    drawCube(0.025f, 0.035f, 0.24f);
    glPopMatrix();

    glColor3f(0.95f, 0.18f, 0.06f);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(1.26f, 1.68f, -0.20f + i * 0.20f);
        glutSolidSphere(0.045f, 12, 12);
        glPopMatrix();
    }

    // Show green when running and red when stopped.
    if (treadmillOn)
        glColor3f(0.0f, 0.95f, 0.25f);
    else
        glColor3f(0.9f, 0.05f, 0.05f);

    glPushMatrix();
    glTranslatef(1.22f, 1.96f, 0.0f);
    glutSolidSphere(0.055f, 12, 12);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    setMaterialShininess(24.0f);
    glPopMatrix();
}

void drawFan(float x, float z) {
    // Draw the ceiling fan and rotate its blades when enabled.
    // Move the fan to its requested ceiling position.
    glPushMatrix();
    glTranslatef(x, 5.35f, z);

    // Draw the ceiling mount and vertical rod.
    glColor3f(0.18f, 0.18f, 0.18f);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCylinder(0.05f, 0.10f);
    glPopMatrix();

    glColor3f(0.22f, 0.22f, 0.22f);
    glPushMatrix();
    glTranslatef(0.0f, 0.14f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCylinder(0.035f, 0.41f);
    glPopMatrix();

    // Draw the center motor housing.
    glColor3f(0.2f, 0.2f, 0.2f);
    glutSolidSphere(0.18f, 20, 20);

    // Rotate all four blades using the current fan angle.
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

void drawWindow() {
    // Draw the outdoor view, glass panels, frame, and window animation.
    glPushMatrix();
    glTranslatef(0, 3.2f, -5.88f);

    glDisable(GL_LIGHTING);

    // Outside sky background
    glColor3f(0.55f, 0.85f, 1.0f);
    glPushMatrix();
    glTranslatef(0, 0.0f, 0.01f);
    drawCube(2.8f, 1.8f, 0.04f);
    glPopMatrix();
    // Outside garden ground
    glColor3f(0.15f, 0.60f, 0.18f);
    glPushMatrix();
    glTranslatef(0, -0.55f, 0.05f);
    drawCube(2.8f, 0.65f, 0.05f);
    glPopMatrix();
    // Outside flowers
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
    // 4 window glass panels fade on/off

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
    // Window frame

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
    // Window state indicator
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
    // Draw the wall mirror with its frame and glass shine.
    glPushMatrix();
    glTranslatef(4.5f, 3.0f, -5.85f);

    // Draw the four metallic sides of the mirror frame.
    setMaterialShininess(92.0f);
    glColor3f(0.72f, 0.74f, 0.76f);

    glPushMatrix();
    glTranslatef(0.0f, 0.96f, 0.04f);
    drawCube(1.50f, 0.08f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.96f, 0.04f);
    drawCube(1.50f, 0.08f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.71f, 0.0f, 0.04f);
    drawCube(0.08f, 2.00f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.71f, 0.0f, 0.04f);
    drawCube(0.08f, 2.00f, 0.08f);
    glPopMatrix();

    // Draw the blue-gray mirror surface inside the frame.
    setMaterialShininess(36.0f);
    glColor3f(0.47f, 0.61f, 0.70f);
    drawCube(1.5f, 2.0f, 0.05f);

    glColor3f(0.36f, 0.47f, 0.55f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.035f);
    drawCube(1.35f, 1.82f, 0.025f);
    glPopMatrix();

    // Soft glass shine. A clean mirror looks better than a fake flat reflection.
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(-0.25f, 0.35f, 0.07f);
    glRotatef(-18, 0, 0, 1);
    glColor4f(0.92f, 0.97f, 1.0f, lightWhite ? 0.32f : 0.08f);
    glBegin(GL_QUADS);
        glVertex3f(-0.06f, -0.72f, 0.0f);
        glVertex3f( 0.08f, -0.72f, 0.0f);
        glVertex3f( 0.08f,  0.72f, 0.0f);
        glVertex3f(-0.06f,  0.72f, 0.0f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.68f, 0.075f);
    glColor4f(0.78f, 0.90f, 1.0f, lightWhite ? 0.26f : 0.04f);
    glBegin(GL_QUADS);
        glVertex3f(-0.62f, -0.05f, 0.0f);
        glVertex3f( 0.62f, -0.05f, 0.0f);
        glVertex3f( 0.62f,  0.05f, 0.0f);
        glVertex3f(-0.62f,  0.05f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    setMaterialShininess(24.0f);
    glPopMatrix();
}

void drawTubeLight() {
    // Draw the tube light fixture and its glowing surface.
    // Draw a transparent glow around the tube when it is on.
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (lightWhite) {
        glColor4f(1.0f, 0.95f, 0.72f, 0.28f);
        glBegin(GL_QUADS);
            glVertex3f(3.25f, 4.08f, -5.86f);
            glVertex3f(5.75f, 4.08f, -5.86f);
            glVertex3f(5.75f, 4.78f, -5.86f);
            glVertex3f(3.25f, 4.78f, -5.86f);
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    // Draw the tube itself using bright or dim color by light state.
    glPushMatrix();
    glTranslatef(3.60f, 4.52f, -5.70f);
    glRotatef(90, 0, 1, 0);

    if (lightWhite)
        glColor3f(1.0f, 0.96f, 0.82f);
    else
        glColor3f(0.28f, 0.28f, 0.25f);

    drawCylinder(0.045f, 1.80f);
    glPopMatrix();

    // Draw dark holders at both ends of the tube.
    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(3.55f, 4.52f, -5.70f);
    drawCube(0.10f, 0.12f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.45f, 4.52f, -5.70f);
    drawCube(0.10f, 0.12f, 0.08f);
    glPopMatrix();
}

void reshape(int width, int height) {
    // Update the viewport and perspective after window resizing.
    // Prevent division by zero when the window height becomes zero.
    if (height == 0) height = 1;

    float aspect = (float)width / (float)height;

    // Match the OpenGL viewport to the new window dimensions.
    glViewport(0, 0, width, height);

    // Create a 55-degree perspective projection.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(55.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void display() {
    // Clear the screen and render the complete 3D gym scene.
    // Clear the previous color and depth information.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Position the camera and define where it is looking.
    gluLookAt(
        camX, camY, camZ,
        lookX, lookY, lookZ,
        0.0f, 1.0f, 0.0f
    );

    // Update lights before drawing objects for the current frame.
    setupLights();

    // Draw every room and gym object in display order.
    drawRoom();
    drawDoor();
    drawWallDecorations();
    drawSceneShadows();
    drawLightIndicators();
    drawWindow();
    drawMirror();
    drawTubeLight();
    drawPullUpBar();
    drawPullUpMan();
    drawDumbbellRack();
    drawBenchPress();
    drawTreadmill();
    drawFan(-2.5f, 1.5f);
    drawFan(2.5f, 1.5f);

    // Show the completed frame using double buffering.
    glutSwapBuffers();
}

void update(int value) {
    // Update all active animations and request the next frame.
    // Smoothly accelerate or slow down the ceiling fans.
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

    // Rotate the treadmill animation while it is switched on.
    if (treadmillOn) {
        treadmillAngle += 3.0f;
        if (treadmillAngle > 360.0f)
            treadmillAngle -= 360.0f;
    }

    // Move each dumbbell toward its selected raised or resting height.
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

    // Move the barbell smoothly up or down.
    float barTarget = barbellUp ? BARBELL_LIFT_RANGE : 0.0f;

    if (barbellY < barTarget)
        barbellY += 0.035f;

    if (barbellY > barTarget)
        barbellY -= 0.035f;

    // Move the pull-up person toward the raised or lowered position.
    float pullUpTarget = pullUpRaised ? PULL_UP_RANGE : 0.0f;

    if (pullUpY < pullUpTarget)
        pullUpY += 0.025f;

    if (pullUpY > pullUpTarget)
        pullUpY -= 0.025f;

    if (fabs(pullUpY - pullUpTarget) < 0.026f)
        pullUpY = pullUpTarget;

    // Fade the window glass between fully closed and fully open.
    float windowTarget = windowOpen ? 1.0f : 0.0f;

    if (windowOpenAmount < windowTarget)
        windowOpenAmount += 0.03f;

    if (windowOpenAmount > windowTarget)
        windowOpenAmount -= 0.03f;

    if (windowOpenAmount < 0.0f)
        windowOpenAmount = 0.0f;

    if (windowOpenAmount > 1.0f)
        windowOpenAmount = 1.0f;

    // Rotate the door smoothly between closed and open states.
    float doorTarget = doorOpen ? 1.0f : 0.0f;

    if (doorOpenAmount < doorTarget)
        doorOpenAmount += 0.035f;

    if (doorOpenAmount > doorTarget)
        doorOpenAmount -= 0.035f;

    if (doorOpenAmount < 0.0f)
        doorOpenAmount = 0.0f;

    if (doorOpenAmount > 1.0f)
        doorOpenAmount = 1.0f;

    // Redraw and run this update again after about 16 milliseconds.
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    // Handle regular keys for movement, views, and animations.
    switch (key) {
    // Keys 1-6 toggle the six dumbbells individually.
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

    // B lifts the barbell and plays the weight plate sound.
    case 'b':
    case 'B':
        barbellUp = !barbellUp;
        playBenchPressSound();
        break;

    // U raises or lowers the pull-up person.
    case 'u':
    case 'U':
        pullUpRaised = !pullUpRaised;
        break;

    // Keys 7 and 8 toggle the white and green lights.
    case '7':
        lightWhite = !lightWhite;
        break;

    case '8':
        lightGreen = !lightGreen;
        break;

    // P starts or stops both ceiling fans.
    case 'p':
    case 'P':
        fanOn = !fanOn;
        break;

    // T starts or stops the treadmill and runner.
    case 't':
    case 'T':
        treadmillOn = !treadmillOn;
        break;

    // D opens or closes the front door.
    case 'd':
    case 'D':
        doorOpen = !doorOpen;
        break;

    // O opens or closes the front window glass.
    case 'o':
    case 'O':
        windowOpen = !windowOpen;
        break;

    // W and S move the camera forward and backward.
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

    // A and F move the camera left and right.
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

    // Q and E move the camera up and down.
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

    // Number and letter keys select preset camera views.
    case '9':
        setDumbbellView();
        break;

    case '0':
        setBenchPressView();
        break;

    case 'v':
    case 'V':
        setTreadmillView();
        break;

    case 'n':
    case 'N':
        setTreadmillOppositeView();
        break;

    case 'r':
    case 'R':
        setNormalView();
        break;

    // Escape closes the program.
    case 27:
        exit(0);
    }

    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
    // Handle arrow keys for changing the camera direction.
    // Arrow keys move the camera's look target.
    switch (key) {
    case GLUT_KEY_UP:
        lookY += 0.25f;
        break;

    case GLUT_KEY_DOWN:
        lookY -= 0.25f;
        break;

    case GLUT_KEY_LEFT:
        lookX -= 0.25f;
        break;

    case GLUT_KEY_RIGHT:
        lookX += 0.25f;
        break;
    }

    // Keep vertical camera aiming inside a useful range.
    if (lookY < 0.2f)
        lookY = 0.2f;

    if (lookY > 5.5f)
        lookY = 5.5f;

    glutPostRedisplay();
}

void init() {
    // Initialize OpenGL settings, textures, lighting, and camera state.
    // Enable depth, material color, smooth shading, and normal correction.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    // Set the default material reflection properties.
    GLfloat specular[] = {0.28f, 0.28f, 0.28f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);

    // Set the color shown behind the 3D scene.
    glClearColor(0.14f, 0.20f, 0.32f, 1.0f);

    // Load floor and wall images into OpenGL textures.
    floorTexture = loadTexture("green.jpg");
    frontWallTexture = loadTexture("front.png");
    wallTexture = loadTexture("wall.png");
    rightWallTexture = loadTexture("right_wall.png");

    // Print a fallback message for any texture that could not load.
    if (floorTexture == 0) {
        printf("green.jpg load hoy nai. Floor fallback green hobe.\n");
    }

    if (frontWallTexture == 0) {
        printf("front.png load hoy nai. Front wall fallback blue hobe.\n");
    }

    if (wallTexture == 0) {
        printf("wall.png load hoy nai. Left wall fallback blue hobe.\n");
    }

    if (rightWallTexture == 0) {
        printf("right_wall.png load hoy nai. Right wall fallback blue hobe.\n");
    }

    // Return to the model-view matrix for normal scene drawing.
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    // Create the GLUT window, register callbacks, and start the program.
    // Initialize GLUT and request color, depth, and double buffering.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Set the initial window size and title.
    glutInitWindowSize(1000, 700);
    glutCreateWindow("3D Gym Room - OpenGL Project");

    // Prepare all OpenGL state and project resources.
    init();

    // Connect GLUT events to this program's callback functions.
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);

    // Enter the continuous GLUT event and rendering loop.
    glutMainLoop();

    return 0;
}
