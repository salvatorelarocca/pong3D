// vedi .vscode/tasks.json

#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

#define KEY_ESC 27
#define BACKSPACE 8
#define X_POS 50
#define Y_POS 50

/* Definizione di colori per proprieta' dei materiali */
GLfloat Rosso[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat Verde[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat Blu[] = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat Nero[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat Bianco[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat Giallo[] = {1.0f, 1.0f, 0.0f, 1.0f};
GLfloat RossoTenue[] = {0.3f, 0.1f, 0.1f, 1.0f};
GLfloat BluTenue[] = {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat GialloTenue[] = {0.6f, 0.6f, 0.0f, 1.0f};
GLfloat myDistance;
static unsigned int texture[7];  // Array of texture indices. Serve ad indicare l indice della texture

GLfloat lightPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};
// Parametri per gluPerspective
static GLfloat fovy = 80, aspect = 1, nearClipPrp = 3, farClipPrs = 100, nearClipOrt = 10, farClipOrt = 100;
// Parametri per il punto di vista
static GLfloat dist = 30, alphaxP1 = 90.0, alphazP1 = 91.0, alphaxP2 = 90.0, alphazP2 = -91.0;  // con questa rotazione siamo nella prospettiva del giocatore1 l'asseX verso di noi

static GLdouble xStart = 0.0, yStart = 0.0;  // per la rotazione con il mouse
static GLint width = 1200, height = 800;

static map<char, bool> keyState = {
    {'a', false}, {'s', false}, {'d', false}, {'w', false}, {'p', false}, {' ', false}, {'c', false},

    // hash per i tasti della tastiera true=premuto false=rilasciato
    {'t', false},  // top
    {'b', false},  // bottom
    {'l', false},  // left
    {'r', false}   // right
};

string scoreP1;
string scoreP2;
bool scoredP1 = false, scoredP2 = false, inMenu = true, insNameP1 = false, insNameP2 = false, flagWin1 = false, flagWin2 = false;
GLfloat xv = 0.30f, yv = 0.22f, zv = 0.15f;
fstream classifica;
bool player1ChangeTexture = false;
bool player2ChangeTexture = false;
bool inClassifica = false;
int indexPlayer1Texture = 5;
int indexPlayer2Texture = 3;

void writeBitmapString(void *font, string str) {
  const char *c = str.c_str();
  const char *cc;
  for (cc = c; *cc != '\0'; cc++) glutBitmapCharacter(font, *cc);
}

class Player {
 private:
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat dim;
  GLint score;
  GLfloat step;
  string name;

 public:
  Player(GLfloat d) {
    x = 0;
    y = 0;
    z = 0;
    dim = d;
    step = 0.1;
    name = "";
  }
  GLfloat getX() { return x; }
  GLfloat getY() { return y; }
  GLfloat getZ() { return z; }
  GLfloat getStep() { return step; }
  string getName() { return name; }
  void setName(string n) { name = n; }
  void setStep(GLfloat s) { step = s; }
  void setX(GLfloat x) { this->x = x; }
  void setY(GLfloat y) { this->y = y; }
  void setZ(GLfloat z) { this->z = z; }
  GLfloat getDim() { return dim; }

  void encreaseScore() { score++; }
  int getScore() { return score; }
  /*movimento player sul piano YZ*/
  void encreaseY(GLfloat dimFieldY) {
    if (y + dim / 2.0f + step <= dimFieldY / 2.0f) {  // sottraggo o sommo step altrimenti esce
      // cout<<"y:"<<y<<" dimFieldy:"<<dimFieldY/2.0f<<endl;
      y = y + step;
    }
  }
  void decreaseY(GLfloat dimFieldY) {
    if (y - dim / 2.0f - step >= -dimFieldY / 2) {
      // cout<<"y:"<<y<<" dimFieldy:"<<dimFieldY/2.0f<<endl;
      y = y - step;
    }
  }
  void encreaseZ(GLfloat dimFieldZ) {
    if (z + dim / 2.0f + step <= dimFieldZ / 2.0f) {
      // cout<<"z:"<<z<<" dim: "<<dimFieldZ/2.0f<<endl;
      z = z + step;
    }
  }
  void decreaseZ(GLfloat dimFieldZ) {
    if (z - dim / 2.0f - step >= -dimFieldZ / 2.0f) {
      // cout<<"z:"<<z<<" dim: "<<dimFieldZ/2.0f<<endl;
      z = z - step;
    }
  }
  void drawPlayer();
};

static void cubebase(void)  // costruscie una faccia del cubo, viene richiaata 6 volte da cubebase
/*specifies a side of a cube*/
{
  glLineWidth(2.5f);
  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0);
  glVertex3d(-0.5, -0.5, 0.0);

  glTexCoord2f(0.0, 1.0);
  glVertex3d(-0.5, 0.5, 0.0);

  glTexCoord2f(1.0, 1.0);
  glVertex3d(0.5, 0.5, 0.0);

  glTexCoord2f(1.0, 0.0);
  glVertex3d(0.5, -0.5, 0.0);
  glEnd();
}

static void cube(void)  // ci serve per i parallelepipedi texturizabili
/*uses cube side to construct a cube, making use of the modelview matrix*/
{
  /*make sure we're dealing with modelview matrix*/
  glMatrixMode(GL_MODELVIEW);

  /*pushes and duplicates current matrix*/
  glPushMatrix();

  /*construct the base*/
  cubebase();

  glPushMatrix();
  /*construct side on +x axis*/
  glTranslated(0.5, 0.0, 0.5);
  glRotated(90.0, 0.0, 1.0, 0.0);
  cubebase();

  glPopMatrix();

  /*construct side on -x axis*/
  glPushMatrix();
  glTranslated(-0.5, 0.0, 0.5);
  glRotated(-90.0, 0.0, 1.0, 0.0);
  cubebase();
  glPopMatrix();

  /*construct side on +y axis*/
  glPushMatrix();
  glTranslated(0.0, 0.5, 0.5);
  glRotated(-90.0, 1.0, 0.0, 0.0);
  cubebase();
  glPopMatrix();

  /*construct side on -y axis*/
  glPushMatrix();
  glTranslated(0.0, -0.5, 0.5);
  glRotated(90.0, 1.0, 0.0, 0.0);
  cubebase();
  glPopMatrix();

  /*construct top*/

  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0);
  glVertex3d(-0.5, -0.5, 1.0);

  glTexCoord2f(1.0, 0.0);
  glVertex3d(0.5, -0.5, 1.0);

  glTexCoord2f(1.0, 1.0);
  glVertex3d(0.5, 0.5, 1.0);

  glTexCoord2f(0.0, 1.0);
  glVertex3d(-0.5, 0.5, 1.0);
  glEnd();

  glPopMatrix();

  glFlush();
}

class CVector3D {
  GLdouble x;
  GLdouble y;
  GLdouble z;

 public:
  CVector3D() {
    x = 0;
    y = 0;
    z = 0;
  }
  CVector3D(GLdouble _x, GLdouble _y, GLdouble _z) {
    x = _x;
    y = _y;
    z = _z;
  }
  GLdouble getX() { return x; }
  GLdouble getY() { return y; }
  GLdouble getZ() { return z; }
};

CVector3D GetOGLPos(int x, int y) {
  GLint viewport[4];
  GLdouble modelview[16], projection[16];
  GLfloat winX, winY, winZ;
  GLdouble posX, posY, posZ;

  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glGetIntegerv(GL_VIEWPORT, viewport);

  winX = (GLfloat)x;
  winY = (GLfloat)viewport[3] - (GLfloat)y;

  GLfloat pixel;

  // glReadPixels(GLint(winX), GLint(winY), 1, 1, GL_GREEN, GL_FLOAT, &winZ);
  glReadPixels(GLint(winX), GLint(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixel);

  gluUnProject(winX, winY, pixel, modelview, projection, viewport, &posX, &posY, &posZ);
  /*punto sulla finestra 2d z=0*/ /*punto nella scena 3d, calcola z*/
  return CVector3D(posX, posY, posZ);
}

class Field {
 private:
  GLfloat dimX, dimY, dimZ;
  Player *player1, *player2;

 public:
  Field(GLfloat dimX, GLfloat dimY, GLfloat dimZ, GLfloat dimPlayer) {
    this->dimX = dimX;
    this->dimY = dimY;
    this->dimZ = dimZ;
    player1 = new Player(dimPlayer);  // situato a +x*(dimX/2)
    player2 = new Player(dimPlayer);  // situato a -x*(dimX/2)
    player1->setX(dimX / 2.0f);       // set profondità dei giocatori. Profondità lungo l'asse x(scelta iniziale del dottore andrea cappabianca)
    player2->setX(-dimX / 2.0f);
  }
  void drawField() {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(dimX, dimY, dimZ);
    glutWireCube(1);
    glPopMatrix();

    glPushMatrix();

    glTranslated(0.0, 0.0, -dimZ / 2);
    glRotated(0.0, 0.0, 0.0, 0.0);
    glScalef(dimX, dimY, 0);

    cubebase();
    glPopMatrix();
  }

  GLfloat getDimX() { return dimX; }
  GLfloat getDimY() { return dimY; }
  GLfloat getDimZ() { return dimZ; }
  void setDimX(GLfloat dimX) { this->dimX = dimX; }
  void setDimY(GLfloat dimY) { this->dimY = dimY; }
  void setDimZ(GLfloat dimZ) { this->dimZ = dimZ; }
  Player *getPlayer(int numPlayer) {
    if (numPlayer == 1)
      return player1;
    else
      return player2;
  }
};

/*Cosi mettiamo globale solo il campo
è giusto mettere anche la palla nella classe campo*/
Field campo(40, 11, 11, 2);  // X profondità y and z facciata. Ultimo parametro dimensione giocatori nel campo

class Ball {
 private:
  GLfloat xPal, yPal, zPal;  // coordiate pallina
  GLfloat speedXact;
  GLfloat speedYact;
  GLfloat speedZact;
  GLfloat speedXprc;
  GLfloat speedYprc;
  GLfloat speedZprc;
  GLfloat speedX;
  GLfloat speedY;
  GLfloat speedZ;

  GLfloat radius;
  // qua aggiungere la texture quando capirte come cazzi si fa
 public:
  Ball(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat sx, GLfloat sy, GLfloat sz) {
    xPal = x;
    yPal = y;
    zPal = z;
    speedXprc = 0.0f;  // la ball nasce con una velocità precedente sempre nulla
    speedYprc = 0.0f;
    speedZprc = 0.0f;
    speedXact = 0.0f;  // velocità attuale
    speedYact = 0.0f;
    speedZact = 0.0f;
    speedX = sx;  // velocità della pallina
    speedY = sy;
    speedZ = sz;
    radius = r;
  }

  void setXPal(GLfloat x) { xPal = x; }
  GLfloat getXPal() { return xPal; }
  void setYPal(GLfloat y) { yPal = y; }
  GLfloat getYPal() { return yPal; }
  void setZPal(GLfloat z) { zPal = z; }
  GLfloat getZPal() { return zPal; }
  void setRadius(GLfloat r) { radius = r; }
  GLfloat getRadius() { return radius; }

  void setSpeedXYZact(GLfloat sX, GLfloat sY, GLfloat sZ) {
    speedXprc = speedXact;  // salviamo la velocità precendente prima di impostare quella attuale
    speedYprc = speedYact;
    speedZprc = speedZact;
    speedXact = sX;
    speedYact = sY;
    speedZact = sZ;
  }

  void chageSpeedvector(GLfloat sX, GLfloat sY, GLfloat sZ) {  // cambio vettore della velocità
    speedX = sX;
    speedY = sY;
    speedZ = sZ;
  }

  void setSpeedXYZ() {  // setto la velocià a quella attuale per farla muovere quando nasce inizialmente è ferma
    speedXact = speedX;
    speedYact = speedY;
    speedZact = speedZ;
  }

  GLfloat getSpeedXact() { return speedXact; }
  GLfloat getSpeedYact() { return speedYact; }
  GLfloat getSpeedZact() { return speedZact; }
  GLfloat getSpeedXprc() { return speedXprc; }
  GLfloat getSpeedYprc() { return speedYprc; }
  GLfloat getSpeedZprc() { return speedZprc; }

  static void moveBall(int);
  void moveball(int);
};

Ball *ball = new Ball(0, 0, 0, 0.3f, xv, yv, zv);

// Qui sarebbe giusto passare
void Ball::moveball(int i)  // faccio check collision con bordi e con i player
{
  glutTimerFunc(50, ball->moveBall, 0);
  xPal = xPal + speedXact;
  yPal = yPal + speedYact;
  zPal = zPal + speedZact;
  // collisione con i player
  if (xPal >= campo.getDimX() / 2 - ball->getRadius()) {
    myDistance = sqrt(pow(xPal - campo.getPlayer(1)->getX(), 2.0) + pow(yPal - campo.getPlayer(1)->getY(), 2.0) + pow(zPal - campo.getPlayer(1)->getZ(), 2.0));
    if (
        myDistance >= ball->getRadius() - 0.5 &&
        myDistance <= sqrt(pow(ball->getRadius(), 2.0) + pow(campo.getPlayer(1)->getDim() * 1.4 / 2, 2)))
      // tolleranza
      speedXact = (speedXact + 0.1) * (-1);
    else {
      // aumento score
      xPal = 0;
      yPal = 0;
      zPal = 0;
      campo.getPlayer(2)->encreaseScore();
      scoredP2 = true;
      keyState[' '] = false;
      ball->setSpeedXYZact(0.0f, 0.0f, 0.0f);
      if (campo.getPlayer(2)->getScore() == 5) {
        flagWin2 = true;
        classifica.open("classifica.txt", ios::app);

        classifica.close();
      }
    }
  }

  if (xPal <= -campo.getDimX() / 2 + ball->getRadius()) {
    myDistance = sqrt(pow(xPal - campo.getPlayer(2)->getX(), 2.0) + pow(yPal - campo.getPlayer(2)->getY(), 2.0) + pow(zPal - campo.getPlayer(2)->getZ(), 2.0));
    if (
        myDistance >= ball->getRadius() - 0.5 &&
        myDistance <= sqrt(pow(ball->getRadius(), 2.0) + pow(campo.getPlayer(2)->getDim() * 1.4 / 2, 2)))
      speedXact = (speedXact - 0.1) * (-1);
    else {
      // aumento score
      xPal = 0;
      yPal = 0;
      zPal = 0;
      campo.getPlayer(1)->encreaseScore();
      scoredP1 = true;
      keyState[' '] = false;
      ball->setSpeedXYZact(0.0f, 0.0f, 0.0f);
      if (campo.getPlayer(1)->getScore() == 5)
        flagWin1 = true;
    }
  }

  // collisione con il campo
  if (yPal + ball->getRadius() >= campo.getDimY() / 2 || yPal - ball->getRadius() <= -campo.getDimY() / 2) {
    speedYact = -speedYact;
  }
  if (zPal + ball->getRadius() >= campo.getDimZ() / 2 || zPal - ball->getRadius() <= -campo.getDimZ() / 2) {
    speedZact = -speedZact;
  }
  // cout<<speedXact<<endl;
  glutPostRedisplay();
}

void Ball::moveBall(int i) {
  ball->moveball(i);
}

// serve per caricare texture da immagini con soil
void loadExternalTextures(char *menu, char *tField, char *classifica, char *tP1, char *tP2, char *tP3) {
  int numeroTexture = 6;
  int i = 0;
  char *filenameTexture[] = {menu, tField, classifica, tP1, tP2, tP3};
  int width, height, channels;
  unsigned char *img;
  while (i < numeroTexture) {
    if (i < 3)  // menu e campo in RGB
    {
      img = SOIL_load_image(filenameTexture[i], &width, &height, &channels, SOIL_LOAD_AUTO);
      if (img != NULL) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, img);
        SOIL_free_image_data(img);
      }
    } else {  // player 1  player 2 RGBA
      img = SOIL_load_image(filenameTexture[i], &width, &height, &channels, SOIL_LOAD_AUTO);
      if (img != NULL) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, img);
        SOIL_free_image_data(img);
      }
    }
    cout << filenameTexture[i] << " ";
    cout << width << " " << height << " " << channels << endl;
    i++;
  }
}

void Player::drawPlayer() {
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Nero);
  glMaterialfv(GL_FRONT, GL_EMISSION, Verde);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);
  glPushMatrix();
  glTranslatef(x, y, z - dim / 2);  //-dim/2 per centrare i giocatori su z
  glScalef(0.01, dim, dim);         // 0.01 è lo spessore che sto trattando come costanteee
  // glutWireCube(1);
  cube();  // definisce il cubo tramite poligoni per le texture
  glPopMatrix();
}

// Tasti del mouse
GLvoid mouse(GLint button, GLint state, GLint x, GLint y) {
  if (state == GLUT_DOWN) {
    if (button == GLUT_LEFT_BUTTON) {
      /* Aggiorna la posizione del mouse salvata */
      xStart = x;
      yStart = y;
    }
  }
  if (inMenu && state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
    CVector3D v = GetOGLPos(x, y);
    if (v.getZ() + 11 >= -10 && v.getZ() + 11 < 0)  // significa che il giocatore2 vuola cambiare texture
    {
      player1ChangeTexture = true;
    }
    cout << "player1" << v.getZ() << " " << v.getZ() + 11 << endl;
    if (v.getZ() + 11 > 0)  // significa che il giocatore1 vuola cambiare texture
      player2ChangeTexture = true;
    // controllo hit riquadro per l'inserimento del primo nome
    if (x > width / 100.0f * 12.0f && x < width / 100.0f * 37.0f && height - y > height - height / 100.0f * 42.5f && height - y < height - height / 100.0f * 32.0f)
      insNameP1 = true;
    else
      insNameP1 = false;
    // controllo hit secondo riquadro
    if (x > width / 100.0f * 12.0f && x < width / 100.0f * 37.0f && height - y > height - height / 100.0f * 60.0f && height - y < height - height / 100.0f * 50.0f)
      insNameP2 = true;
    else
      insNameP2 = false;
    // controllo hit tasto start
    if (x > width / 100.0f * 38.0f && x < width / 100.0f * 63.0f && height - y > height - height / 100.0f * 94.0f && height - y < height - height / 100.0f * 85.0f)
      inMenu = false;
    else
      inMenu = true;
  }
  // controllo hit velocità X
  if (x > width / 100.0f * 40.5f && x < width / 100.0f * 47.0f && height - y > height - height / 100.0f * 75.0f && height - y < height - height / 100.0f * 67.0f) {
    if (button == 3 && xv < 1.0f)
      xv += 0.01;
    if (button == 4 && xv > 0.0f)
      xv -= 0.01;
  }
  // controllo hit velocità Y
  if (x > width / 100.0f * 52.0f && x < width / 100.0f * 58.5f && height - y > height - height / 100.0f * 75.0f && height - y < height - height / 100.0f * 67.0f) {
    if (button == 3 && yv < 1.0f)
      yv += 0.01;
    if (button == 4 && yv > 0.0f)
      yv -= 0.01;
  }
  // controllo hit velocità Z
  if (x > width / 100.0f * 63.0f && x < width / 100.0f * 69.5f && height - y > height - height / 100.0f * 75.0f && height - y < height - height / 100.0f * 67.0f) {
    if (button == 3 && zv < 1.0f)
      zv += 0.01;
    if (button == 4 && zv > 0.0f)
      zv -= 0.01;
  }
}

// Movimento del mouse
GLvoid motion(GLint x, GLint y) {
  if (keyState['p']) {
    if (x < width / 2.0f) {
      /* Aggiorna il punto di vista */
      alphaxP1 -= (GLdouble)(y - yStart);
      alphazP1 += (GLdouble)(x - xStart);
      /* Aggiorna la posizione del mouse salvata */
    } else {
      alphaxP2 -= (GLdouble)(y - yStart);
      alphazP2 += (GLdouble)(x - xStart);
    }
    xStart = x;
    yStart = y;
    glutPostRedisplay();
  }
}

// inizializzazioni
GLvoid init(GLvoid) {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-width / 2.0f, -height / 2.0f, width, height, nearClipOrt, farClipOrt);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glGenTextures(6, texture);  // carico 6 texture
  loadExternalTextures((char *)"menu.jpg", (char *)"field.png", (char *)"classifica.jpg", (char *)"player1.png", (char *)"player2.png", (char *)"player3.png");
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glEnable(GL_LIGHT0);

  glEnable(GL_NORMALIZE);
}

// Callback per la tastiera
GLvoid inputKey(GLubyte key, GLint x, GLint y) {
  switch (key) {
    case KEY_ESC:
      if (inMenu)
        exit(0);
      else {
        inMenu = !inMenu;  // vai al menu
        // keyState['p'] = true; //metti in pausa azzerando la velocità attuale
        ball->setSpeedXYZact(0.0f, 0.0f, 0.0f);
      }
      break;
    case 'c':
      if (inMenu)
        keyState['c'] = true;
      break;
    case 'a':
      if (!inMenu)
        keyState['a'] = true;
      break;
    case 'd':
      if (!inMenu)
        keyState['d'] = true;
      break;
    case 'w':
      if (!inMenu)
        keyState['w'] = true;
      break;
    case 's':
      if (!inMenu)
        keyState['s'] = true;
      break;
    case 'p':                              // tasto pause
      if (!inMenu) {                       // se non sei nel menu
        if (keyState[' ']) {               // il gioco è attivo
          keyState['p'] = !keyState['p'];  // aggiorna flag pausa
          if (keyState['p'])
            ball->setSpeedXYZact(0.0f, 0.0f, 0.0f); /*fermo la pallina*/
          else
            ball->setSpeedXYZact(ball->getSpeedXprc(), ball->getSpeedYprc(), ball->getSpeedZprc()); /*riparte la pallina alla vecchia velocità*/
        }
        cout << "p: " << keyState['p'] << endl;
      }
      break;
    case ' ':
      if (!inMenu && !keyState[' ']) {  // se sei in menu e il gioco non è attivo allora possiamo attivarlo con space
        keyState[' '] = true;
        if (keyState[' ']) {
          ball->chageSpeedvector(xv, yv, zv);
          ball->setSpeedXYZ();
        }
        cout << "start : " << keyState[' '] << endl;
      }
      if (flagWin1 || flagWin2) {  // fine partita ritorna al menu resetto i player
        inMenu = true;
        campo.getPlayer(1)->setName("");
        campo.getPlayer(2)->setName("");
        ball->setSpeedXYZact(0.0f, 0.0f, 0.0f);
      }
      flagWin1 = false;
      flagWin2 = false;
      break;
  }
  glutPostRedisplay();
}

/*movimento player2 che si muove con le frecce*/
void specialKeyInput(int key, int _x, int _y) {
  switch (key) {
    case GLUT_KEY_UP:
      keyState['t'] = true;  // top
      break;
    case GLUT_KEY_DOWN:
      keyState['b'] = true;  // bottom
      break;
    case GLUT_KEY_LEFT:
      keyState['l'] = true;  // left
      break;
    case GLUT_KEY_RIGHT:
      keyState['r'] = true;  // right
      break;
    default:
      break;
  }
  glutPostRedisplay();
}

void specialKeyUpInput(int key, int _x, int _y) {
  switch (key) {
    case GLUT_KEY_UP:
      keyState['t'] = false;  // top
      break;
    case GLUT_KEY_DOWN:
      keyState['b'] = false;  // bottom
      break;
    case GLUT_KEY_LEFT:
      keyState['l'] = false;  // left
      break;
    case GLUT_KEY_RIGHT:
      keyState['r'] = false;  // right
      break;
    default:
      break;
  }
}

void inputKeyup(unsigned char key, int x, int y) {
  switch (key) {
    case 'a':
      keyState['a'] = false;
      break;
    case 'd':
      keyState['d'] = false;
      break;
    case 'w':
      keyState['w'] = false;
      break;
    case 's':
      keyState['s'] = false;
      break;
    case 'c':
      keyState['c'] = false;
  }
  if (inMenu) {
    if (insNameP1) {
      if (key == BACKSPACE) {  // backspace per cancellare
        campo.getPlayer(1)->setName(campo.getPlayer(1)->getName().erase(campo.getPlayer(1)->getName().size() - 1));
      } else {
        campo.getPlayer(1)->setName(campo.getPlayer(1)->getName() + (char)key);
      }
    }
    if (insNameP2) {
      if (key == BACKSPACE) {  // backspace per cancellare
        campo.getPlayer(2)->setName(campo.getPlayer(2)->getName().erase(campo.getPlayer(2)->getName().size() - 1));
      } else {
        campo.getPlayer(2)->setName(campo.getPlayer(2)->getName() + (char)key);
      }
    }
    glutPostRedisplay();
  }
}

// Impostazione del punto di vista
void setView(GLfloat alphax, GLfloat alphaz, GLfloat d) {
  glTranslatef(0.0f, 0.0f, -d);
  glRotatef(-alphax, 1.0f, 0.0f, 0.0f);
  glRotatef(-alphaz, 0.0f, 0.0f, 1.0f);
}

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius) {
  int i;
  int triangleAmount = 20;  // # of triangles used to draw circle

  // GLfloat radius = 0.8f; //radius
  GLfloat twicePi = 2.0f * M_PI;

  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(x, y);  // center of circle
  for (i = 0; i <= triangleAmount; i++) {
    glVertex2f(
        x + (radius * cos(i * twicePi / triangleAmount)),
        y + (radius * sin(i * twicePi / triangleAmount)));
  }
  glEnd();
}

// Callback di display
GLvoid drawScene(GLvoid) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (!inMenu) {
    glEnable(GL_DEPTH_TEST);
    /*Impostazione vista prospettica quando siamo nel gioco*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (GLfloat)width / height, nearClipPrp, farClipPrs);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*View first player*/
    glViewport(0, 0, width * 0.5f, height * 0.75f);
    /*Label Score*/
    glPushMatrix();
    setView(0.0f, 0.0f, dist);
    glColor3f(1.0, 1.0, 1.0);
    // cout<<"w: "<<width * 0.5f<<" h: "<<height * 0.75f * 0.125f<<endl;
    glRasterPos3f(-10, 22, 0);
    if (scoredP1 == false)
      writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, campo.getPlayer(1)->getName().append(": ").append(to_string(campo.getPlayer(1)->getScore())));
    else {
      if (campo.getPlayer(1)->getScore() != 5)
        writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, "SCORED");
      if (flagWin1)
        writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, " WIN!!!");
    }
    glPopMatrix();

    glPushMatrix();
    // Posizione luce legata al punto di vistaa:
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    setView(alphaxP1, alphazP1, dist);

    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_EMISSION, Nero);
    glMaterialfv(GL_FRONT, GL_AMBIENT, RossoTenue);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Rosso);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Verde);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

    glPushMatrix();
    glTranslatef(ball->getXPal(), ball->getYPal(), ball->getZPal());
    glutSolidSphere(ball->getRadius(), 31, 31);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
    glMaterialfv(GL_FRONT, GL_EMISSION, GialloTenue);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture[indexPlayer2Texture]);
    campo.getPlayer(2)->drawPlayer();
    glBindTexture(GL_TEXTURE_2D, texture[indexPlayer1Texture]);
    campo.getPlayer(1)->drawPlayer();
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    campo.drawField();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // effetto linea che segue la palla
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blu);
    glMaterialfv(GL_FRONT, GL_EMISSION, Blu);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Rosso);
    glPushMatrix();
    glTranslatef(ball->getXPal(), 0, 0);
    glScalef(0.2, campo.getDimY(), campo.getDimZ());  // 0.2 è lo spessore che sto trattando come costante
    glutWireCube(1);
    glPopMatrix();
    // effetto cerchi sui piani YZ per aiutare a colpire la palla
    glPushMatrix();
    glTranslatef(campo.getDimX() / 2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0, 0.0, ball->getRadius());
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-campo.getDimX() / 2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0, 0.0, ball->getRadius());
    glPopMatrix();
    /* Disabilita l'illuminazione */
    glDisable(GL_LIGHTING);
    glPopMatrix();

    /*View second player-----------------------------------------------------------------------------------*/
    glViewport(width * 0.5f, 0, width * 0.5f, height * 0.75f);
    /*Label Score*/
    glPushMatrix();
    setView(0.0f, 0.0f, dist);
    glColor3f(1.0, 1.0, 1.0);
    // cout<<"w: "<<width * 0.5f<<" h: "<<height * 0.75f * 0.125f<<endl;
    glRasterPos3f(-10, 22, 0);
    if (!scoredP2)
      writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, campo.getPlayer(2)->getName().append(": ").append(to_string(campo.getPlayer(2)->getScore())));
    else {
      if (campo.getPlayer(2)->getScore() != 5)
        writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, "SCORED");
    }
    if (flagWin2) {
      writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, "WIN!!!");
    }
    glPopMatrix();

    glPushMatrix();
    // Posizione luce legata al punto di vista:
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    setView(alphaxP2, alphazP2, dist);

    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_EMISSION, Nero);
    glMaterialfv(GL_FRONT, GL_AMBIENT, RossoTenue);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Rosso);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Verde);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

    glPushMatrix();
    glTranslatef(ball->getXPal(), ball->getYPal(), ball->getZPal());
    glutSolidSphere(ball->getRadius(), 31, 31);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
    glMaterialfv(GL_FRONT, GL_EMISSION, GialloTenue);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture[indexPlayer2Texture]);
    campo.getPlayer(2)->drawPlayer();  // funzione modificata per permettere di applicare le texture
    glBindTexture(GL_TEXTURE_2D, texture[indexPlayer1Texture]);
    campo.getPlayer(1)->drawPlayer();
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture[1]);  // array di texture caricate con loadExternal()
    campo.drawField();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // effetto linea che segue la palla
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blu);
    glMaterialfv(GL_FRONT, GL_EMISSION, Blu);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Rosso);
    glPushMatrix();
    glTranslatef(ball->getXPal(), 0, 0);
    glScalef(0.2, campo.getDimY(), campo.getDimZ());  // 0.2 è lo spessore che sto trattando come costante
    glutWireCube(1);
    glPopMatrix();
    // effetto cerchi sui piani YZ per aiutare a colpire la palla
    glPushMatrix();
    glTranslatef(campo.getDimX() / 2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0, 0.0, ball->getRadius());
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-campo.getDimX() / 2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0, 0.0, ball->getRadius());
    glPopMatrix();
    /* Disabilita l'illuminazione */
    glDisable(GL_LIGHTING);
    glPopMatrix();

    /*Granangolo----------------------------------------------------------------------------------------------*/

    glViewport(0, height * 0.75f, width, height);

    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    setView(90.0f, 90.0f, 40);
    glRotatef(90.0f, 0, 0, 1);
    glScalef(0.5f, 0.5f, 0.5f);
    glTranslated(0.0f, 0.0f, -50.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos3f(60, 6, 0);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, "LEGEND");
    glRasterPos3f(55, 0, 0);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, "p = PAUSE");
    glRasterPos3f(50, -5, 0);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, "space = START");
    glRasterPos3f(48, -10, 0);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, "esc = MENU'");
    // Posizione luce legata al punto di vista:

    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_EMISSION, Nero);
    glMaterialfv(GL_FRONT, GL_AMBIENT, RossoTenue);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Rosso);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Verde);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

    glPushMatrix();
    glTranslatef(ball->getXPal(), ball->getYPal(), ball->getZPal());
    glutSolidSphere(ball->getRadius(), 31, 31);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
    glMaterialfv(GL_FRONT, GL_EMISSION, GialloTenue);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, texture[3]); //array di texture caricate con loadExternal()
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    campo.getPlayer(2)->drawPlayer();  // funzione modificata per permettere di applicare le texture
    campo.getPlayer(1)->drawPlayer();
    glBindTexture(GL_TEXTURE_2D, texture[1]);  // array di texture caricate con loadExternal()
    campo.drawField();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // effetto linea che segue la palla
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blu);
    glMaterialfv(GL_FRONT, GL_EMISSION, Blu);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Rosso);
    glPushMatrix();
    glTranslatef(ball->getXPal(), 0, 0);
    glScalef(0.2, campo.getDimY(), campo.getDimZ());  // 0.2 è lo spessore che sto trattando come costante
    glutWireCube(1);
    glPopMatrix();
    // effetto cerchi sui piani YZ per aiutare a colpire la palla
    glPushMatrix();
    glTranslatef(campo.getDimX() / 2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0, 0.0, ball->getRadius());
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-campo.getDimX() / 2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0, 0.0, ball->getRadius());
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glPopMatrix();
  } else if (inClassifica) {
    glViewport(0, 0, width, height);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, nearClipOrt, farClipOrt);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);  // prova a non abilitarlo
    setView(0.0, 0.0, dist);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(0.0f, height, 0.0f);

    glTexCoord2f(0.0, 1.0);
    glVertex3d(0.0f, 0.0f, 0.0f);

    glTexCoord2f(1.0, 1.0);
    glVertex3d(width, 0.0f, 0.0f);

    glTexCoord2f(1.0, 0);
    glVertex3d(width, height, 0.0f);
    // texture player1
    glEnd();
  } else {
    glViewport(0, 0, width, height);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, nearClipOrt, farClipOrt);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);  // prova a non abilitarlo
    setView(0.0, 0.0, dist);
    // d'attivare al click di certe aree del menù che devono essere in funzione di width e height per essere responsive al resize
    // label inserimento nome primo player
    //  IMPORTANTE lo (0,0) per questa vista è in basso a sinistra per rendere più semplice l'individuazioni di aree del menu
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(width / 100.0f * 14.0f, height - height / 100.0f * 39.2f, 0.0f);  // posizione 14% width 39.2%height
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, campo.getPlayer(1)->getName());
    // label inserimento nome secondo giocatore
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(width / 100.0f * 14.0f, height - height / 100.0f * 56.0f, 0.0f);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, campo.getPlayer(2)->getName());
    // label Xvel
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(width / 100.0f * 42.3f, height - height / 100.0f * 72.5f, 0.0f);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, to_string(xv).substr(0, 4));
    // label Yvel
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(width / 100.0f * 53.3f, height - height / 100.0f * 72.5f, 0.0f);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, to_string(yv).substr(0, 4));
    // label Zvel
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(width / 100.0f * 64.7f, height - height / 100.0f * 72.5f, 0.0f);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, to_string(zv).substr(0, 4));
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(0.0f, height, 0.0f);

    glTexCoord2f(0.0, 1.0);
    glVertex3d(0.0f, 0.0f, 0.0f);

    glTexCoord2f(1.0, 1.0);
    glVertex3d(width, 0.0f, 0.0f);

    glTexCoord2f(1.0, 0);
    glVertex3d(width, height, 0.0f);
    // texture player1
    glEnd();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // se il player1 ha deciso di cambiare texture
    if (player1ChangeTexture) {
      // suppondo che le prima 2 texture non siano usabili per il giocatore
      indexPlayer1Texture++;
      indexPlayer1Texture = 3 + indexPlayer1Texture % 3;
      cout << "indexplauer " << indexPlayer1Texture << endl;
      glBindTexture(GL_TEXTURE_2D, texture[indexPlayer1Texture]);
      player1ChangeTexture = false;
    } else
      glBindTexture(GL_TEXTURE_2D, texture[indexPlayer1Texture]);

    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(width / 2.06f, height - (height / 2.7), 10.0f);

    glTexCoord2f(0.0, 1.0);
    glVertex3d(width / 2.06f, height - (height / 1.7), 10.0f);

    glTexCoord2f(1.0, 1.0);
    glVertex3d(width / 1.58f, height - (height / 1.7), 10.0f);

    glTexCoord2f(1.0, 0);
    glVertex3d(width / 1.58f, height - (height / 2.7), 10.0f);

    glDisable(GL_BLEND);
    glEnd();

    // il player 2 decide ci cambiare texture player2
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (player2ChangeTexture) {
      indexPlayer2Texture++;
      // suppondo che le prima 2 texture non siano usabili per il giocatore
      indexPlayer2Texture = 3 + indexPlayer2Texture % 3;
      glBindTexture(GL_TEXTURE_2D, texture[indexPlayer2Texture]);
      player2ChangeTexture = false;
    } else
      glBindTexture(GL_TEXTURE_2D, texture[indexPlayer2Texture]);

    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(width / 1.35f, height - (height / 2.7), 20.0f);

    glTexCoord2f(0.0, 1.0);
    glVertex3d(width / 1.35f, height - (height / 1.7), 20.0f);

    glTexCoord2f(1.0, 1.0);
    glVertex3d(width / 1.13, height - (height / 1.7), 20.0f);

    glTexCoord2f(1.0, 0);
    glVertex3d(width / 1.13, height - (height / 2.7), 20.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glPopMatrix();
  }

  glutPostRedisplay();
  glutSwapBuffers();
}

/*funzione idle per continuare gli spostamenti fin quando i tasti sono premuti*/
void idle() {
  if (!keyState['p']) {
    // idle per i tasti asdw
    if (keyState['a'])
      campo.getPlayer(1)->decreaseY(campo.getDimY());
    if (keyState['s'])
      campo.getPlayer(1)->decreaseZ(campo.getDimZ());
    if (keyState['d'])
      campo.getPlayer(1)->encreaseY(campo.getDimZ());
    if (keyState['w'])
      campo.getPlayer(1)->encreaseZ(campo.getDimZ());
    if (keyState['c'])
      inClassifica = true;
    else
      inClassifica = false;
    if (keyState['t'])
      campo.getPlayer(2)->encreaseZ(campo.getDimZ());
    if (keyState['b'])
      campo.getPlayer(2)->decreaseZ(campo.getDimZ());
    if (keyState['l'])
      campo.getPlayer(2)->encreaseY(campo.getDimY());
    if (keyState['r'])
      campo.getPlayer(2)->decreaseY(campo.getDimY());
    glutPostRedisplay();
  }
}

void resize(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (inMenu)
    gluPerspective(fovy, (GLfloat)w / h, nearClipPrp, farClipPrs);
  else
    glOrtho(-width / 2.0f, -height / 2.0f, width, height, nearClipOrt, farClipOrt);
  width = w;
  height = h;
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void labelPopUp(int endLoop) {
  static int i = 0;
  glutTimerFunc(50, labelPopUp, endLoop);
  if (i > endLoop) {
    if (scoredP1)
      scoredP1 = false;
    if (scoredP2)
      scoredP2 = false;
    // if(flagWin1) flagWin1 = false;
    // if(flagWin2) flagWin2 = false;
    i = 0;
  }
  i++;
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(X_POS, Y_POS);
  glutCreateWindow("Pong3d");
  glutReshapeFunc(resize);
  glutIgnoreKeyRepeat(1);  // serve a ignorare la ripetizione delle callback dei tasti da tastiera quando sono "tenuti giù" perchè li ho gestito separatamente la pressione e il rilascio
  // attiva callback freccette
  // pressione
  glutSpecialFunc(specialKeyInput);
  // rilascio
  glutSpecialUpFunc(specialKeyUpInput);

  // attiva callback tasti "asdw"
  // pressione
  glutKeyboardFunc(inputKey);
  // rilascio
  glutKeyboardUpFunc(inputKeyup);
  //
  glutIdleFunc(idle);

  glutDisplayFunc(drawScene);

  glutTimerFunc(50, labelPopUp, 50);
  glutTimerFunc(50, ball->moveBall, 0);

  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glewExperimental = GL_TRUE;
  glewInit();
  init();
  glutMainLoop();
  return (0);
}
