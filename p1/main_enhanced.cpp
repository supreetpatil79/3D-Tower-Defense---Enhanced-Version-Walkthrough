#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#define M_PI 3.14159265358979323846
using namespace std;

// Constants
const int GRID_ROWS = 5;
const int GRID_COLS = 10;
const float CELL_SIZE = 2.0f;
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int ENEMY_SPAWN_INTERVAL = 200;
const int MAX_BASE_HEALTH = 10;
const int MAX_PARTICLES = 2000;

// Global Variables
int selectedRow = 0;
int selectedCol = 0;
int resources = 50;
int frameCount = 0;
int baseHealth = MAX_BASE_HEALTH;
int wave = 1;
bool gameOver = false;
bool gridOccupied[GRID_ROWS][GRID_COLS] = {false};
bool transitionRendered = false;
int level = 1;
int score = 0;
int maxScore = 0;
bool levelTransition = false;
int instructionPage = 1;
int comboCounter = 0;
float comboTimer = 0.0f;

// Camera and Interaction Variables
float cameraRotationX = 30.0f, targetCameraRotationX = 30.0f;
float cameraRotationY = 45.0f, targetCameraRotationY = 45.0f;
float cameraZoom = 35.0f, targetCameraZoom = 35.0f;
float animationFrames = 0.0f;
bool wireframeMode = false;
float screenShake = 0.0f;
float screenShakeDecay = 0.95f;

// Enhanced Lighting
GLfloat light_ambient[] = {0.3f, 0.3f, 0.4f, 1.0f};
GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light_position[] = {20.0f, 50.0f, 20.0f, 1.0f};

GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat mat_shininess[] = {100.0f};

// Enums
enum EnemyType { NORMAL, FAST, TANK };
enum TowerType { BASIC, SLOW, AREA };
enum GameState { WELCOME, INSTRUCTIONS, PLAYING, GAME_OVER };
enum ParticleType { EXPLOSION, MUZZLE_FLASH, TRAIL, SPARK, AMBIENT };

GameState gameState = WELCOME;

// Particle System
struct Particle {
  float x, y, z;
  float vx, vy, vz;
  float r, g, b, a;
  float size;
  float lifetime;
  float maxLifetime;
  bool active;
  ParticleType type;
};

struct FloatingText {
  std::string text;
  float x, y, z;
  float lifetime;
  float r, g, b;
  float vy;
  bool active;
};

struct Enemy {
  float x, z;
  float speed;
  int health;
  int maxHealth;
  bool alive;
  EnemyType type;
  float flashTimer;
  float deathTimer;
};

struct Tower {
  int row, col;
  int cooldown;
  int level;
  TowerType type;
  int health;
  float rotation;
  float targetRotation;
  float chargeTimer;
  float shootFlash;
};

struct Bullet {
  float x, z;
  float speed;
  int damage;
  bool active;
  float r, g, b;
  int trailCounter;
};

// Vectors
std::vector<Enemy> enemies;
std::vector<Tower> towers;
std::vector<Bullet> bullets;
std::vector<Particle> particles;
std::vector<FloatingText> floatingTexts;

// Function Prototypes
void spawnEnemy();
void createParticleExplosion(float x, float y, float z, float r, float g,
                             float b, int count);
void createMuzzleFlash(float x, float y, float z);
void createTrailParticle(float x, float y, float z, float r, float g, float b);
void updateParticles();
void drawParticles();
void updateFloatingText();
void drawFloatingText();
void addFloatingText(const std::string &text, float x, float y, float z,
                     float r, float g, float b);
void drawCube(float x, float y, float z, float size);
void drawText(const std::string &text, int x, int y);
void drawTextLarge(const std::string &text, int x, int y);
void drawGrid();
void drawTerrain();
void drawHealthBar(float x, float z, int health, int maxHealth);
void updateAndDrawEnemies();
void updateAndDrawBullets();
void towersShoot();
void display();
void timer(int = 0);
void keyboard(int key, int x, int y);
void normalKey(unsigned char key, int x, int y);
void reshape(int w, int h);
void init();
void drawGlowingSphere(float x, float y, float z, float radius, float r,
                       float g, float b);
void drawEnergyRing(float radius, float thickness, float r, float g, float b);

// Initialize OpenGL
void init() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_FOG);
  GLfloat fogColor[] = {0.02f, 0.02f, 0.1f, 1.0f};
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, 0.012f);
  glHint(GL_FOG_HINT, GL_NICEST);

  glClearColor(0.02f, 0.02f, 0.1f, 1.0f);
  srand(static_cast<unsigned>(time(0)));

  particles.reserve(MAX_PARTICLES);
  floatingTexts.reserve(100);
}

// Particle Functions
void createParticleExplosion(float x, float y, float z, float r, float g,
                             float b, int count) {
  for (int i = 0; i < count && particles.size() < MAX_PARTICLES; i++) {
    Particle p;
    p.x = x;
    p.y = y;
    p.z = z;

    float angle = (rand() % 360) * M_PI / 180.0f;
    float elevation = ((rand() % 60) - 30) * M_PI / 180.0f;
    float speed = 0.05f + (rand() % 100) / 500.0f;

    p.vx = cos(angle) * cos(elevation) * speed;
    p.vy = sin(elevation) * speed + 0.02f;
    p.vz = sin(angle) * cos(elevation) * speed;

    p.r = r + (rand() % 20 - 10) / 100.0f;
    p.g = g + (rand() % 20 - 10) / 100.0f;
    p.b = b + (rand() % 20 - 10) / 100.0f;
    p.a = 1.0f;

    p.size = 0.1f + (rand() % 30) / 100.0f;
    p.lifetime = 0.5f + (rand() % 100) / 100.0f;
    p.maxLifetime = p.lifetime;
    p.active = true;
    p.type = EXPLOSION;

    particles.push_back(p);
  }
  screenShake += 0.3f;
}

void createMuzzleFlash(float x, float y, float z) {
  for (int i = 0; i < 15 && particles.size() < MAX_PARTICLES; i++) {
    Particle p;
    p.x = x + (rand() % 20 - 10) / 50.0f;
    p.y = y + (rand() % 20 - 10) / 50.0f;
    p.z = z + (rand() % 20 - 10) / 50.0f;
    p.vx = (rand() % 20 - 10) / 200.0f;
    p.vy = (rand() % 20 - 10) / 200.0f;
    p.vz = (rand() % 20 - 10) / 200.0f;
    p.r = 1.0f;
    p.g = 0.7f + (rand() % 30) / 100.0f;
    p.b = 0.2f;
    p.a = 1.0f;
    p.size = 0.15f + (rand() % 20) / 100.0f;
    p.lifetime = 0.2f;
    p.maxLifetime = 0.2f;
    p.active = true;
    p.type = MUZZLE_FLASH;
    particles.push_back(p);
  }
}

void createTrailParticle(float x, float y, float z, float r, float g, float b) {
  if (particles.size() >= MAX_PARTICLES)
    return;
  Particle p;
  p.x = x + (rand() % 10 - 5) / 100.0f;
  p.y = y + (rand() % 10 - 5) / 100.0f;
  p.z = z + (rand() % 10 - 5) / 100.0f;
  p.vx = (rand() % 10 - 5) / 500.0f;
  p.vy = (rand() % 10 - 5) / 500.0f;
  p.vz = (rand() % 10 - 5) / 500.0f;
  p.r = r;
  p.g = g;
  p.b = b;
  p.a = 0.8f;
  p.size = 0.08f;
  p.lifetime = 0.3f;
  p.maxLifetime = 0.3f;
  p.active = true;
  p.type = TRAIL;
  particles.push_back(p);
}

void updateParticles() {
  for (auto &p : particles) {
    if (!p.active)
      continue;

    p.x += p.vx;
    p.y += p.vy;
    p.z += p.vz;

    p.vy -= 0.002f; // Gravity
    p.lifetime -= 0.016f;

    float lifeRatio = p.lifetime / p.maxLifetime;
    p.a = lifeRatio;

    if (p.type == EXPLOSION) {
      p.vx *= 0.98f;
      p.vy *= 0.98f;
      p.vz *= 0.98f;
    }

    if (p.lifetime <= 0 || p.y < 0) {
      p.active = false;
    }
  }

  particles.erase(std::remove_if(particles.begin(), particles.end(),
                                 [](const Particle &p) { return !p.active; }),
                  particles.end());
}

void drawParticles() {
  glDisable(GL_LIGHTING);
  glDepthMask(GL_FALSE);

  for (const auto &p : particles) {
    if (!p.active)
      continue;

    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    glColor4f(p.r, p.g, p.b, p.a);

    if (p.type == MUZZLE_FLASH) {
      glutSolidSphere(p.size, 8, 8);
    } else {
      glPointSize(p.size * 10.0f);
      glBegin(GL_POINTS);
      glVertex3f(0, 0, 0);
      glEnd();
    }

    glPopMatrix();
  }

  glDepthMask(GL_TRUE);
  glEnable(GL_LIGHTING);
}

void addFloatingText(const std::string &text, float x, float y, float z,
                     float r, float g, float b) {
  FloatingText ft;
  ft.text = text;
  ft.x = x;
  ft.y = y;
  ft.z = z;
  ft.r = r;
  ft.g = g;
  ft.b = b;
  ft.lifetime = 1.5f;
  ft.vy = 0.02f;
  ft.active = true;
  floatingTexts.push_back(ft);
}

void updateFloatingText() {
  for (auto &ft : floatingTexts) {
    if (!ft.active)
      continue;
    ft.y += ft.vy;
    ft.lifetime -= 0.016f;
    if (ft.lifetime <= 0)
      ft.active = false;
  }

  floatingTexts.erase(
      std::remove_if(floatingTexts.begin(), floatingTexts.end(),
                     [](const FloatingText &ft) { return !ft.active; }),
      floatingTexts.end());
}

void drawFloatingText() {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  for (const auto &ft : floatingTexts) {
    if (!ft.active)
      continue;

    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    GLint viewport[4];
    GLdouble winX, winY, winZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    gluProject(ft.x, ft.y, ft.z, modelMatrix, projMatrix, viewport, &winX,
               &winY, &winZ);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float alpha = ft.lifetime / 1.5f;
    glColor4f(ft.r, ft.g, ft.b, alpha);
    glRasterPos2i((int)winX, (int)winY);
    for (char c : ft.text) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

void spawnEnemy() {
  int row = rand() % GRID_ROWS;
  Enemy e;
  e.x = GRID_COLS * CELL_SIZE;
  e.z = row * CELL_SIZE;

  int type = rand() % 3;
  if (type == 0) {
    e.speed = 0.02f + 0.002f * level;
    e.health = 80 + 10 * level;
    e.type = NORMAL;
  } else if (type == 1) {
    e.speed = 0.05f + 0.003f * level;
    e.health = 50 + 8 * level;
    e.type = FAST;
  } else {
    e.speed = 0.01f + 0.001f * level;
    e.health = 150 + 20 * level;
    e.type = TANK;
  }

  e.maxHealth = e.health;
  e.alive = true;
  e.flashTimer = 0.0f;
  e.deathTimer = 0.0f;
  enemies.push_back(e);

  if (level == 2) {
    enemies.push_back(e);
  } else if (level == 3) {
    enemies.push_back(e);
    enemies.push_back(e);
  }
}

void drawCube(float x, float y, float z, float size) {
  glPushMatrix();
  glTranslatef(x, y, z);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glutSolidCube(size);
  glDisable(GL_LIGHTING);
  glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
  glutWireCube(size * 1.02f);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void drawGlowingSphere(float x, float y, float z, float radius, float r,
                       float g, float b) {
  glPushMatrix();
  glTranslatef(x, y, z);

  glDisable(GL_LIGHTING);
  glColor4f(r, g, b, 0.8f);
  glutSolidSphere(radius, 16, 16);

  glColor4f(r, g, b, 0.3f);
  glutSolidSphere(radius * 1.5f, 16, 16);

  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void drawEnergyRing(float radius, float thickness, float r, float g, float b) {
  glDisable(GL_LIGHTING);
  glColor4f(r, g, b, 0.6f);
  glutWireTorus(thickness, radius, 12, 24);
  glEnable(GL_LIGHTING);
}

void drawGlassPanel(int x, int y, int w, int h) {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glColor4f(0.05f, 0.05f, 0.15f, 0.85f);
  glBegin(GL_QUADS);
  glVertex2i(x, y);
  glVertex2i(x + w, y);
  glVertex2i(x + w, y + h);
  glVertex2i(x, y + h);
  glEnd();

  glLineWidth(2.0f);
  glColor4f(0.3f, 0.7f, 1.0f, 0.8f);
  glBegin(GL_LINE_LOOP);
  glVertex2i(x, y);
  glVertex2i(x + w, y);
  glVertex2i(x + w, y + h);
  glVertex2i(x, y + h);
  glEnd();

  glColor4f(0.5f, 0.9f, 1.0f, 0.4f);
  glBegin(GL_LINES);
  glVertex2i(x, y + h);
  glVertex2i(x + w, y + h);
  glEnd();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

void drawText(const std::string &text, int x, int y) {
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glRasterPos2i(x, y);
  for (char c : text) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_LIGHTING);
}

void drawTextLarge(const std::string &text, int x, int y) {
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glRasterPos2i(x, y);
  for (char c : text) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_LIGHTING);
}

void drawTerrain() {
  glDisable(GL_LIGHTING);
  glColor3f(0.01f, 0.03f, 0.08f);
  glBegin(GL_QUADS);
  glVertex3f(-100.0f, 0.0f, -100.0f);
  glVertex3f(-100.0f, 0.0f, 100.0f);
  glVertex3f(100.0f, 0.0f, 100.0f);
  glVertex3f(100.0f, 0.0f, -100.0f);
  glEnd();

  float pulse = (sin(animationFrames * 2.0f) + 1.0f) * 0.5f;
  glColor3f(0.1f + pulse * 0.1f, 0.2f + pulse * 0.1f, 0.4f + pulse * 0.1f);
  glBegin(GL_LINES);
  for (float i = -100; i <= 100; i += 5) {
    glVertex3f(i, 0.01f, -100.0f);
    glVertex3f(i, 0.01f, 100.0f);
    glVertex3f(-100.0f, 0.01f, i);
    glVertex3f(100.0f, 0.01f, i);
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

void drawHealthBar(float x, float z, int health, int maxHealth) {
  float w = 1.0f * health / maxHealth;
  glPushMatrix();
  glTranslatef(x, 1.8f, z);

  glDisable(GL_LIGHTING);
  glColor4f(0.1f, 0.1f, 0.1f, 0.7f);
  glBegin(GL_QUADS);
  glVertex3f(-0.5f, -0.08f, 0);
  glVertex3f(0.5f, -0.08f, 0);
  glVertex3f(0.5f, 0.08f, 0);
  glVertex3f(-0.5f, 0.08f, 0);
  glEnd();

  if (w > 0.6)
    glColor3f(0.0f, 1.0f, 0.5f);
  else if (w > 0.3)
    glColor3f(1.0f, 1.0f, 0.0f);
  else
    glColor3f(1.0f, 0.0f, 0.0f);

  glBegin(GL_QUADS);
  glVertex3f(-0.5f, -0.08f, 0.01f);
  glVertex3f(-0.5f + w, -0.08f, 0.01f);
  glVertex3f(-0.5f + w, 0.08f, 0.01f);
  glVertex3f(-0.5f, 0.08f, 0.01f);
  glEnd();
  glEnable(GL_LIGHTING);

  glPopMatrix();
}

void drawTowerHealthBar(float x, float z, int health, int maxHealth) {
  float w = 1.0f * health / maxHealth;
  glPushMatrix();
  glTranslatef(x, 1.5f, z);
  glScalef(w, 0.1f, 0.1f);
  glDisable(GL_LIGHTING);
  glColor3f(0.0f, 1.0f, 0.5f);
  glutSolidCube(1.0f);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void drawGrid() {
  for (int r = 0; r < GRID_ROWS; r++) {
    for (int c = 0; c < GRID_COLS; c++) {
      glPushMatrix();
      glTranslatef(c * CELL_SIZE, 0.05f, r * CELL_SIZE);

      if (r == selectedRow && c == selectedCol) {
        float pulse = (sin(animationFrames * 6.0f) + 1.0f) * 0.5f;
        glDisable(GL_LIGHTING);
        glColor4f(0.0f, 1.0f, 1.0f, 0.2f + pulse * 0.5f);
        glBegin(GL_QUADS);
        glVertex3f(-CELL_SIZE / 2, 0, -CELL_SIZE / 2);
        glVertex3f(CELL_SIZE / 2, 0, -CELL_SIZE / 2);
        glVertex3f(CELL_SIZE / 2, 0, CELL_SIZE / 2);
        glVertex3f(-CELL_SIZE / 2, 0, CELL_SIZE / 2);
        glEnd();

        glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(-CELL_SIZE / 2, 0.02f, -CELL_SIZE / 2);
        glVertex3f(CELL_SIZE / 2, 0.02f, -CELL_SIZE / 2);
        glVertex3f(CELL_SIZE / 2, 0.02f, CELL_SIZE / 2);
        glVertex3f(-CELL_SIZE / 2, 0.02f, CELL_SIZE / 2);
        glEnd();
        glLineWidth(1.0f);
        glEnable(GL_LIGHTING);
      } else {
        glDisable(GL_LIGHTING);
        glColor4f(0.15f, 0.25f, 0.35f, 0.3f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(-CELL_SIZE / 2 * 0.95f, 0, -CELL_SIZE / 2 * 0.95f);
        glVertex3f(CELL_SIZE / 2 * 0.95f, 0, -CELL_SIZE / 2 * 0.95f);
        glVertex3f(CELL_SIZE / 2 * 0.95f, 0, CELL_SIZE / 2 * 0.95f);
        glVertex3f(-CELL_SIZE / 2 * 0.95f, 0, CELL_SIZE / 2 * 0.95f);
        glEnd();
        glEnable(GL_LIGHTING);
      }
      glPopMatrix();
    }
  }

  for (auto &t : towers) {
    float x = t.col * CELL_SIZE;
    float z = t.row * CELL_SIZE;

    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(t.rotation, 0, 1, 0);

    switch (t.type) {
    case BASIC: {
      glColor3f(0.5f, 0.5f, 0.6f);
      drawCube(0, 0.4f, 0, 1.2f);
      glColor3f(0.9f, 0.3f, 0.3f);
      drawCube(0, 1.2f, 0, 0.9f);

      float glow = 0.5f + (sin(animationFrames * 5.0f) + 1.0f) * 0.25f;
      if (t.shootFlash > 0)
        glow = 1.0f;
      drawGlowingSphere(0, 1.2f, 0, 0.3f, 1.0f * glow, 0.3f * glow,
                        0.3f * glow);
      break;
    }
    case SLOW: {
      glColor3f(0.3f, 0.3f, 0.5f);
      drawCube(0, 0.3f, 0, 1.0f);
      glColor3f(0.2f, 0.7f, 1.0f);
      glPushMatrix();
      glTranslatef(0, 0.8f, 0);
      glRotatef(-90, 1, 0, 0);
      GLUquadric *quad = gluNewQuadric();
      gluCylinder(quad, 0.3f, 0.1f, 1.2f, 16, 16);
      gluDeleteQuadric(quad);
      glPopMatrix();

      for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(0, 1.0f + i * 0.4f, 0);
        glRotatef(animationFrames * 120.0f + i * 120, 0, 1, 0);
        drawEnergyRing(0.4f + i * 0.1f, 0.04f, 0.2f, 0.7f, 1.0f);
        glPopMatrix();
      }
      break;
    }
    case AREA: {
      glColor3f(0.6f, 0.6f, 0.7f);
      drawCube(0, 0.5f, 0, 1.5f);

      glPushMatrix();
      glTranslatef(0, 1.3f, 0);
      glRotatef(animationFrames * 60.0f, 0, 1, 0);
      float glow = 0.6f + (sin(animationFrames * 4.0f) + 1.0f) * 0.2f;
      drawGlowingSphere(0, 0, 0, 0.5f, 0.2f * glow, 1.0f * glow, 0.2f * glow);

      for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(i * 90 + animationFrames * 180.0f, 0, 1, 0);
        glTranslatef(0.9f, 0, 0);
        glColor3f(0.2f, 1.0f, 0.2f);
        glutSolidCube(0.25f);
        glPopMatrix();
      }
      glPopMatrix();
      break;
    }
    }
    glPopMatrix();
    drawTowerHealthBar(
        x, z, t.health,
        (t.type == BASIC ? 1000 : (t.type == SLOW ? 600 : 1500)));
  }
}

void updateAndDrawEnemies() {
  for (auto &e : enemies) {
    if (!e.alive)
      continue;

    e.x -= e.speed;
    if (e.flashTimer > 0)
      e.flashTimer -= 0.016f;

    if (e.x < 0) {
      e.alive = false;
      baseHealth--;
      createParticleExplosion(e.x, 0.6f, e.z, 1.0f, 0.2f, 0.2f, 30);
      if (baseHealth <= 0) {
        gameOver = true;
        if (score > maxScore)
          maxScore = score;
        return;
      }
    }

    glPushMatrix();
    glTranslatef(e.x, 0.6f, e.z);

    float bob = sin(animationFrames * 5.0f + e.x) * 0.12f;
    glTranslatef(0, bob, 0);

    if (e.flashTimer > 0) {
      glColor3f(1.0f, 0.3f, 0.3f);
    }

    switch (e.type) {
    case NORMAL: {
      glColor3f(1.0f, 1.0f, 0.4f);
      glRotatef(animationFrames * 120.0f, 0, 1, 0);
      glutSolidSphere(0.5f, 16, 16);

      glDisable(GL_LIGHTING);
      glColor4f(1.0f, 1.0f, 0.6f, 0.4f);
      glutWireTorus(0.03, 0.65, 8, 20);
      glEnable(GL_LIGHTING);
      break;
    }
    case FAST: {
      glColor3f(0.3f, 1.0f, 0.6f);
      glRotatef(-90, 0, 1, 0);
      glScalef(1.8f, 0.4f, 0.4f);
      glutSolidCone(0.5f, 1.0f, 16, 16);

      if (frameCount % 3 == 0) {
        createTrailParticle(e.x, 0.6f + bob, e.z, 0.3f, 1.0f, 0.6f);
      }
      break;
    }
    case TANK: {
      glColor3f(0.5f, 0.2f, 0.9f);
      drawCube(0, 0, 0, 1.3f);

      glDisable(GL_LIGHTING);
      float pulse = (sin(animationFrames * 3.0f) + 1.0f) * 0.5f;
      glColor4f(1.0f, 0.3f, 1.0f, 0.8f);
      drawCube(0.5f, 0, 0, 0.5f * pulse);
      glEnable(GL_LIGHTING);
      break;
    }
    }
    glPopMatrix();

    drawHealthBar(e.x, e.z, e.health, e.maxHealth);
  }
}

void updateAndDrawBullets() {
  for (size_t i = 0; i < bullets.size(); ++i) {
    Bullet &b = bullets[i];
    if (!b.active)
      continue;
    b.x += b.speed;

    if (b.trailCounter++ % 2 == 0) {
      createTrailParticle(b.x, 0.8f, b.z, b.r, b.g, b.b);
    }

    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(b.x, 0.8f, b.z);
    glColor4f(b.r, b.g, b.b, 1.0f);
    glutSolidSphere(0.2f, 12, 12);
    glColor4f(b.r, b.g, b.b, 0.5f);
    glutSolidSphere(0.35f, 12, 12);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    for (auto &e : enemies) {
      if (!e.alive)
        continue;
      if (fabs(e.x - b.x) < 0.5f && fabs(e.z - b.z) < 0.5f) {
        e.health -= b.damage;
        e.flashTimer = 0.1f;

        createParticleExplosion(b.x, 0.8f, b.z, b.r, b.g, b.b, 20);
        addFloatingText("-" + std::to_string(b.damage), b.x, 1.5f, b.z, 1.0f,
                        1.0f, 0.0f);

        if (e.health <= 0) {
          e.alive = false;
          resources += 10;
          score++;
          comboCounter++;
          comboTimer = 2.0f;

          float r = (e.type == NORMAL ? 1.0f : (e.type == FAST ? 0.3f : 0.5f));
          float g = (e.type == NORMAL ? 1.0f : (e.type == FAST ? 1.0f : 0.2f));
          float b = (e.type == NORMAL ? 0.4f : (e.type == FAST ? 0.6f : 0.9f));
          createParticleExplosion(e.x, 0.6f, e.z, r, g, b, 50);

          addFloatingText("+" + std::to_string(10 + comboCounter), e.x, 1.5f,
                          e.z, 0.0f, 1.0f, 0.5f);
        }
        b.active = false;
        break;
      }
    }

    if (b.x > GRID_COLS * CELL_SIZE)
      b.active = false;
  }
}

void towersShoot() {
  for (auto &t : towers) {
    if (t.shootFlash > 0)
      t.shootFlash -= 0.05f;

    if (t.cooldown > 0) {
      t.cooldown--;
      continue;
    }

    Bullet b;
    b.x = t.col * CELL_SIZE + 1.0f;
    b.z = t.row * CELL_SIZE;
    b.speed = 0.12f + 0.02f * t.level;
    b.damage = 15 * t.level;
    b.active = true;
    b.trailCounter = 0;

    switch (t.type) {
    case BASIC:
      b.r = 1.0f;
      b.g = 0.3f;
      b.b = 0.3f;
      break;
    case SLOW:
      b.r = 0.2f;
      b.g = 0.7f;
      b.b = 1.0f;
      break;
    case AREA:
      b.r = 0.2f;
      b.g = 1.0f;
      b.b = 0.2f;
      break;
    }

    bullets.push_back(b);
    t.cooldown = 50 - t.level * 8;
    t.shootFlash = 1.0f;

    createMuzzleFlash(b.x, 0.8f, b.z);
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  if (gameState == WELCOME) {
    glDisable(GL_DEPTH_TEST);

    float titlePulse = (sin(animationFrames * 3.0f) + 1.0f) * 0.5f;
    glColor4f(0.2f + titlePulse * 0.3f, 0.7f + titlePulse * 0.3f, 1.0f, 1.0f);
    drawTextLarge("3D TOWER DEFENSE", WINDOW_WIDTH / 2 - 180,
                  WINDOW_HEIGHT / 2 + 100);

    drawGlassPanel(WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT / 2 - 80, 500, 150);

    float buttonPulse = (sin(animationFrames * 4.0f) + 1.0f) * 0.5f;
    glColor4f(0.5f + buttonPulse * 0.5f, 0.9f, 1.0f, 1.0f);
    drawText("Press 'I' for Instructions", WINDOW_WIDTH / 2 - 120,
             WINDOW_HEIGHT / 2 + 10);
    drawText("Press 'S' to Start Game", WINDOW_WIDTH / 2 - 110,
             WINDOW_HEIGHT / 2 - 30);

    glEnable(GL_DEPTH_TEST);
  } else if (gameState == INSTRUCTIONS) {
    glDisable(GL_DEPTH_TEST);

    drawGlassPanel(WINDOW_WIDTH / 2 - 350, WINDOW_HEIGHT / 2 - 250, 700, 500);

    glColor3f(0.3f, 0.9f, 1.0f);
    drawTextLarge("INSTRUCTIONS", WINDOW_WIDTH / 2 - 100,
                  WINDOW_HEIGHT / 2 + 200);

    glColor3f(1.0f, 1.0f, 1.0f);
    if (instructionPage == 1) {
      drawText("LEVEL 1: EASY", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 140);
      drawText("- Arrow Keys: Move Selection", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 100);
      drawText("- P: Place Slow Tower (10 resources)", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 60);
      drawText("- WASD: Rotate Camera", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 20);
      drawText("- Q/E: Zoom In/Out", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 - 20);
      drawText("- Defend your base from enemies!", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 - 60);
    } else if (instructionPage == 2) {
      drawText("LEVEL 2: MEDIUM", WINDOW_WIDTH / 2 - 90,
               WINDOW_HEIGHT / 2 + 140);
      drawText("- P: Slow Tower (10 resources)", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 100);
      drawText("- S: Basic Tower (20 resources)", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 60);
      drawText("- More enemies spawn!", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 20);
      drawText("- Enemies are faster and stronger", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 - 20);
    } else if (instructionPage == 3) {
      drawText("LEVEL 3: HARD", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 140);
      drawText("- P: Slow Tower (10 resources)", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 100);
      drawText("- S: Basic Tower (20 resources)", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 60);
      drawText("- A: Area Tower (30 resources)", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 + 20);
      drawText("- Maximum difficulty!", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 - 20);
      drawText("- Survive the toughest enemies", WINDOW_WIDTH / 2 - 150,
               WINDOW_HEIGHT / 2 - 60);
    }

    glColor3f(0.7f, 0.7f, 0.7f);
    drawText("Press 'N' for Next | 'B' for Back | 'M' for Main Menu",
             WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT / 2 - 180);
    glEnable(GL_DEPTH_TEST);
  } else if (gameState == PLAYING) {
    animationFrames += 0.016f;

    if (comboTimer > 0) {
      comboTimer -= 0.016f;
      if (comboTimer <= 0)
        comboCounter = 0;
    }

    cameraRotationX += (targetCameraRotationX - cameraRotationX) * 0.1f;
    cameraRotationY += (targetCameraRotationY - cameraRotationY) * 0.1f;
    cameraZoom += (targetCameraZoom - cameraZoom) * 0.1f;

    float shakeX = (rand() % 100 - 50) / 100.0f * screenShake;
    float shakeY = (rand() % 100 - 50) / 100.0f * screenShake;
    screenShake *= screenShakeDecay;

    float cameraX = GRID_COLS * CELL_SIZE / 2 +
                    cameraZoom * cos(cameraRotationY * M_PI / 180.0f) *
                        cos(cameraRotationX * M_PI / 180.0f) +
                    shakeX;
    float cameraY =
        cameraZoom * sin(cameraRotationX * M_PI / 180.0f) + 10.0f + shakeY;
    float cameraZ = GRID_ROWS * CELL_SIZE / 2 +
                    cameraZoom * sin(cameraRotationY * M_PI / 180.0f) *
                        cos(cameraRotationX * M_PI / 180.0f);

    gluLookAt(cameraX, cameraY, cameraZ, GRID_COLS * CELL_SIZE / 2, 2.0f,
              GRID_ROWS * CELL_SIZE / 2, 0, 1, 0);

    drawTerrain();
    drawGrid();
    updateAndDrawEnemies();
    towersShoot();
    updateAndDrawBullets();
    updateParticles();
    drawParticles();
    updateFloatingText();
    drawFloatingText();

    if (gameOver) {
      drawGlassPanel(WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT / 2 - 120, 500, 240);
      glColor3f(1.0f, 0.3f, 0.3f);
      drawTextLarge("GAME OVER", WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 + 60);
      glColor3f(1.0f, 1.0f, 1.0f);
      drawText("Final Score: " + std::to_string(score), WINDOW_WIDTH / 2 - 80,
               WINDOW_HEIGHT / 2 + 10);
      drawText("Max Score: " + std::to_string(maxScore), WINDOW_WIDTH / 2 - 70,
               WINDOW_HEIGHT / 2 - 30);
      glColor3f(0.5f, 0.9f, 1.0f);
      drawText("Press 'R' to Restart", WINDOW_WIDTH / 2 - 80,
               WINDOW_HEIGHT / 2 - 80);
    } else if (levelTransition) {
      drawGlassPanel(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 80, 400, 160);
      glColor3f(0.2f, 1.0f, 0.5f);
      drawTextLarge("LEVEL " + std::to_string(level - 1) + " COMPLETE!",
                    WINDOW_WIDTH / 2 - 140, WINDOW_HEIGHT / 2 + 30);
      glColor3f(1.0f, 1.0f, 1.0f);
      drawText("Press 'N' to Continue", WINDOW_WIDTH / 2 - 90,
               WINDOW_HEIGHT / 2 - 20);
    } else {
      drawGlassPanel(10, WINDOW_HEIGHT - 160, 280, 150);
      glColor3f(0.3f, 0.9f, 1.0f);
      drawText("COMMAND CENTER", 20, WINDOW_HEIGHT - 30);
      glColor3f(1.0f, 1.0f, 1.0f);
      drawText("Base Health: " + std::to_string(baseHealth), 20,
               WINDOW_HEIGHT - 60);
      drawText("Resources: " + std::to_string(resources), 20,
               WINDOW_HEIGHT - 90);
      drawText("Wave: " + std::to_string(wave), 20, WINDOW_HEIGHT - 120);

      drawGlassPanel(WINDOW_WIDTH - 280, WINDOW_HEIGHT - 130, 270, 120);
      glColor3f(1.0f, 1.0f, 0.3f);
      drawText("SCORE: " + std::to_string(score), WINDOW_WIDTH - 260,
               WINDOW_HEIGHT - 40);
      glColor3f(0.5f, 1.0f, 0.5f);
      drawText("LEVEL: " + std::to_string(level), WINDOW_WIDTH - 260,
               WINDOW_HEIGHT - 70);
      if (comboCounter > 0) {
        glColor3f(1.0f, 0.5f, 1.0f);
        drawText("COMBO: x" + std::to_string(comboCounter), WINDOW_WIDTH - 260,
                 WINDOW_HEIGHT - 100);
      }

      drawGlassPanel(10, 10, 450, 50);
      glColor3f(0.7f, 0.7f, 0.7f);
      drawText("P(10) S(20) A(30) | Arrows: Move | WASD: Camera | Q/E: Zoom",
               20, 30);

      int particleCount = particles.size();
      drawGlassPanel(WINDOW_WIDTH - 200, 10, 190, 50);
      glColor3f(0.5f, 0.5f, 0.5f);
      drawText("Particles: " + std::to_string(particleCount),
               WINDOW_WIDTH - 180, 30);
    }
  }

  glutSwapBuffers();
}

void timer(int) {
  if (!gameOver && !levelTransition && gameState == PLAYING) {
    frameCount++;
    if (frameCount % ENEMY_SPAWN_INTERVAL == 0) {
      for (int i = 0; i < wave; i++) {
        spawnEnemy();
      }
    }

    if (frameCount % 1000 == 0) {
      wave++;
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                 [](const Enemy &e) { return !e.alive; }),
                  enemies.end());

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b) { return !b.active; }),
                  bullets.end());

    int maxWaves = (level == 1) ? 3 : (level == 2) ? 4 : 5;

    for (size_t i = 0; i < towers.size(); ++i) {
      towers[i].health--;
      if (towers[i].health <= 0) {
        gridOccupied[towers[i].row][towers[i].col] = false;
        createParticleExplosion(towers[i].col * CELL_SIZE, 1.0f,
                                towers[i].row * CELL_SIZE, 0.5f, 0.5f, 0.5f,
                                40);
        towers.erase(towers.begin() + i);
        --i;
      }
    }

    if (wave > maxWaves) {
      baseHealth += 5;
      enemies.clear();
      bullets.clear();
      towers.clear();
      resources = 50;
      std::fill(&gridOccupied[0][0], &gridOccupied[0][0] + sizeof(gridOccupied),
                false);
      if (baseHealth <= 0) {
        gameOver = true;
        if (score > maxScore) {
          maxScore = score;
        }
      } else {
        level++;
        score += 100;
        wave = 1;
        if (level > 3) {
          gameOver = true;
          if (score > maxScore) {
            maxScore = score;
          }
        } else {
          levelTransition = true;
        }
      }
    }
  }
  glutPostRedisplay();
  glutTimerFunc(16, timer, 0);
}

void keyboard(int key, int x, int y) {
  if (gameOver)
    return;

  switch (key) {
  case GLUT_KEY_UP:
    if (selectedRow > 0)
      selectedRow--;
    break;
  case GLUT_KEY_DOWN:
    if (selectedRow < GRID_ROWS - 1)
      selectedRow++;
    break;
  case GLUT_KEY_LEFT:
    if (selectedCol > 0)
      selectedCol--;
    break;
  case GLUT_KEY_RIGHT:
    if (selectedCol < GRID_COLS - 1)
      selectedCol++;
    break;
  }
  glutPostRedisplay();
}

void normalKey(unsigned char key, int x, int y) {
  if (gameState == WELCOME) {
    if (key == 'i' || key == 'I') {
      gameState = INSTRUCTIONS;
    } else if (key == 's' || key == 'S') {
      gameState = PLAYING;
    }
    return;
  }

  if (gameState == INSTRUCTIONS) {
    if (key == 'n' || key == 'N') {
      if (instructionPage < 3)
        instructionPage++;
    } else if (key == 'b' || key == 'B') {
      if (instructionPage > 1)
        instructionPage--;
    } else if (key == 'm' || key == 'M') {
      gameState = WELCOME;
    }
    return;
  }

  if (gameOver && (key == 'r' || key == 'R')) {
    gameOver = false;
    level = 1;
    wave = 1;
    score = 0;
    baseHealth = MAX_BASE_HEALTH;
    resources = 50;
    frameCount = 0;
    enemies.clear();
    bullets.clear();
    towers.clear();
    particles.clear();
    floatingTexts.clear();
    comboCounter = 0;
    comboTimer = 0.0f;
    std::fill(&gridOccupied[0][0], &gridOccupied[0][0] + sizeof(gridOccupied),
              false);
    gameState = WELCOME;
    return;
  }

  if (gameOver)
    return;

  if (levelTransition && (key == 'n' || key == 'N')) {
    levelTransition = false;
    transitionRendered = false;
    wave = 1;
    return;
  }

  if (gameState == PLAYING) {
    switch (key) {
    case 'w':
    case 'W':
      targetCameraRotationX = std::min(89.0f, targetCameraRotationX + 5.0f);
      break;
    case 's':
    case 'S':
      targetCameraRotationX = std::max(5.0f, targetCameraRotationX - 5.0f);
      break;
    case 'a':
    case 'A':
      targetCameraRotationY -= 10.0f;
      break;
    case 'd':
    case 'D':
      targetCameraRotationY += 10.0f;
      break;
    case 'q':
    case 'Q':
      targetCameraZoom = std::max(15.0f, targetCameraZoom - 3.0f);
      break;
    case 'e':
    case 'E':
      targetCameraZoom = std::min(60.0f, targetCameraZoom + 3.0f);
      break;
    }
  }

  if (level == 1) {
    if ((key == 'p' || key == 'P') && resources >= 10) {
      if (!gridOccupied[selectedRow][selectedCol]) {
        Tower t = {selectedRow, selectedCol, 0,    1,    SLOW,
                   600,         0.0f,        0.0f, 0.0f, 0.0f};
        towers.push_back(t);
        gridOccupied[selectedRow][selectedCol] = true;
        resources -= 10;
        createParticleExplosion(selectedCol * CELL_SIZE, 0.5f,
                                selectedRow * CELL_SIZE, 0.2f, 0.7f, 1.0f, 30);
      }
    }
  } else if (level == 2) {
    if ((key == 'p' || key == 'P') && resources >= 10) {
      if (!gridOccupied[selectedRow][selectedCol]) {
        Tower t = {selectedRow, selectedCol, 0,    1,    SLOW,
                   600,         0.0f,        0.0f, 0.0f, 0.0f};
        towers.push_back(t);
        gridOccupied[selectedRow][selectedCol] = true;
        resources -= 10;
        createParticleExplosion(selectedCol * CELL_SIZE, 0.5f,
                                selectedRow * CELL_SIZE, 0.2f, 0.7f, 1.0f, 30);
      }
    } else if ((key == 's' || key == 'S') && resources >= 20) {
      if (!gridOccupied[selectedRow][selectedCol]) {
        Tower t = {selectedRow, selectedCol, 0,    2,    BASIC,
                   1000,        0.0f,        0.0f, 0.0f, 0.0f};
        towers.push_back(t);
        gridOccupied[selectedRow][selectedCol] = true;
        resources -= 20;
        createParticleExplosion(selectedCol * CELL_SIZE, 0.5f,
                                selectedRow * CELL_SIZE, 0.9f, 0.3f, 0.3f, 30);
      }
    }
  } else if (level == 3) {
    if ((key == 'p' || key == 'P') && resources >= 10) {
      if (!gridOccupied[selectedRow][selectedCol]) {
        Tower t = {selectedRow, selectedCol, 0,    1,    SLOW,
                   600,         0.0f,        0.0f, 0.0f, 0.0f};
        towers.push_back(t);
        gridOccupied[selectedRow][selectedCol] = true;
        resources -= 10;
        createParticleExplosion(selectedCol * CELL_SIZE, 0.5f,
                                selectedRow * CELL_SIZE, 0.2f, 0.7f, 1.0f, 30);
      }
    } else if ((key == 's' || key == 'S') && resources >= 20) {
      if (!gridOccupied[selectedRow][selectedCol]) {
        Tower t = {selectedRow, selectedCol, 0,    2,    BASIC,
                   1000,        0.0f,        0.0f, 0.0f, 0.0f};
        towers.push_back(t);
        gridOccupied[selectedRow][selectedCol] = true;
        resources -= 20;
        createParticleExplosion(selectedCol * CELL_SIZE, 0.5f,
                                selectedRow * CELL_SIZE, 0.9f, 0.3f, 0.3f, 30);
      }
    } else if ((key == 'a' || key == 'A') && resources >= 30) {
      if (!gridOccupied[selectedRow][selectedCol]) {
        Tower t = {selectedRow, selectedCol, 0,    3,    AREA,
                   1500,        0.0f,        0.0f, 0.0f, 0.0f};
        towers.push_back(t);
        gridOccupied[selectedRow][selectedCol] = true;
        resources -= 30;
        createParticleExplosion(selectedCol * CELL_SIZE, 0.5f,
                                selectedRow * CELL_SIZE, 0.2f, 1.0f, 0.2f, 30);
      }
    }
  }

  glutPostRedisplay();
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float)w / h, 1, 100);
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("3D Tower Defense - ENHANCED");

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(keyboard);
  glutKeyboardFunc(normalKey);
  glutTimerFunc(0, timer, 0);

  glutMainLoop();
}
