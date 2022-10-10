#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace std;

#define KEY_ESC 27
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


GLfloat lightPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};
// Parametri per gluPerspective
static GLfloat fovy = 60, aspect = 1, nearClip = 3, farClip = 40;
// Parametri per il punto di vista
static GLfloat dist = 20.5, alphax = 0.0, alphaz = 0.0;
static GLdouble xStart = 0.0, yStart = 0.0;//per la rotazione con il mouse
static GLint width = 1200, height = 800;

class Player
{
private:
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat dim;
  GLint score;

public:
  Player(GLfloat d)
  {
    x = 0;
    y = 0;
    z = 0;
    dim = d;
  }
  GLfloat getX() { return x; }
  GLfloat getY() { return y; }
  GLfloat getZ() { return z; }
  void setX(GLfloat x) { this->x = x; }
  void setY(GLfloat y) { this->y = y; }
  void setZ(GLfloat z) { this->z = z; }
  GLfloat getDim() { return dim; }

  void encreseScore() { score++; }
  int getScore() { return score; }
  /*movimento player sul piano YZ*/
  void encreaseY();
  void decreaseY();
  void encreaseZ();
  void decreaseZ();

  void drawPlayer();
};

class Field
{
private:
  GLfloat dimX, dimY, dimZ;
  Player *player1, *player2;
public:
  Field(GLfloat dimX, GLfloat dimY, GLfloat dimZ, GLfloat dimPlayer)
  {
    this->dimX = dimX;
    this->dimY = dimY;
    this->dimZ = dimZ;
    player1 = new Player(dimPlayer);
    player2 = new Player(dimPlayer);
    player1->setX(dimX/2.0f);
    player2->setX(-dimX/2.0f);
  }
  void drawField()
  {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(dimX, dimY, dimZ);
    glutWireCube(1);
    glPopMatrix();
  }

  GLfloat getDimX(){ return dimX; }
  GLfloat getDimY(){ return dimY; }
  GLfloat getDimZ(){ return dimZ; }
  void setDimX(GLfloat dimX){ this->dimX = dimX; }
  void setDimY(GLfloat dimY){ this->dimY = dimY; }
  void setDimZ(GLfloat dimZ){ this->dimZ = dimZ; }
  Player* getPlayer(int numPlayer){
    if(numPlayer == 1)
      return player1;
    else
      return player2;
  }
};


/*Cosi mettiamo globale solo il campo
è giusto mettere anche la palla nella classe campo*/
Field campo(15, 10, 10, 3); // X profondità y and z facciata

void Player::encreaseY()
{
  if (y + dim/2.0f <= campo.getDimY())
    cout<<y + dim/2.0f <<" "<< campo.getDimY()<<endl;
    y = y + 1;
}
void Player::decreaseY()
{
  if (y >= -dim/2)
    y = y - 1;
}
void Player::encreaseZ()
{
  if (z + dim/2 <= campo.getDimZ())
    z = z + 1;
}
void Player::decreaseZ()
{
  if (z >= -dim/2)
    z = z - 1;
}

class Ball
{
private:
  GLfloat xPal, yPal, zPal; // coordiate pallina
  GLfloat speedX;
  GLfloat speedY;
  GLfloat speedZ;
  // qua aggiungere la texture quando capirte come cazzi si fa
public:
  Ball()
  {
    xPal = 0.0f;
    yPal = 0.0f;
    zPal = 0.0f;
    speedX = 0.2f;
    speedY = 0.2f;
    speedZ = 0.2f;
  }
  Ball(GLfloat x, GLfloat y, GLfloat z, GLfloat sx, GLfloat sy, GLfloat sz)
  {
    xPal = x;
    yPal = y;
    zPal = z;
    speedX = sx;
    speedY = sy;
    speedZ = sz;
  }
  Ball(GLfloat x, GLfloat y, GLfloat z)
  {
    xPal = x;
    yPal = y;
    zPal = z;
    speedX = 0.2f;
    speedY = 0.2f;
    speedZ = 0.2f;
  }

  void setXPal(int x) { xPal = x; }
  GLfloat getXPal() { return xPal; }
  void setYPal(int y) { yPal = y; }
  GLfloat getYPal() { return yPal; }
  void setZPal(int z) { zPal = z; }
  GLfloat getZPal() { return zPal; }

  static void moveBall(int);
  void moveball(int);
};
Ball *ball = new Ball(0, 0, 0);

//Qui sarebbe giusto passare
void Ball::moveball(int i) // faccio check collision con bordi e con i player
{
  glutTimerFunc(50, ball->moveBall, 0);
  xPal = xPal + speedX;
  yPal = yPal + speedY;
  zPal = zPal + speedZ;
  // collisione con i player
  if (xPal >= 9 || xPal <= -9)
  {
    if (campo.getPlayer(2)->getY() - 2 < yPal && campo.getPlayer(2)->getY() + 2 > yPal &&
            campo.getPlayer(2)->getZ() - 2 < zPal && campo.getPlayer(2)->getZ() + 2 > zPal ||
        (campo.getPlayer(1)->getY() - 2 < yPal && campo.getPlayer(1)->getY() + 2 > yPal &&
         campo.getPlayer(1)->getZ() - 2 < zPal && campo.getPlayer(1)->getZ() + 2 > zPal)){
      speedX = -speedX;
    }
    else
    {
      // aumento score
      xPal = 0;
      yPal = 0;
      zPal = 0;
    }
  }

  // collisione con il campo
  if (yPal >= 4 || yPal <= -4)
  {
    speedY = -speedY;
  }
  if (zPal >= 4 || zPal <= -4)
  {
    speedZ = -speedZ;
  }

  glutPostRedisplay();
}

void Ball::moveBall(int i)
{
  ball->moveball(i);
}


void Player::drawPlayer()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Nero);
  glMaterialfv(GL_FRONT, GL_EMISSION, Verde);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);
  glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.01, dim, dim); //0.01 è lo spessore che sto trattando come costante
    glutWireCube(1);
  glPopMatrix();
}

// Tasti del mouse
GLvoid mouse(GLint button, GLint state, GLint x, GLint y)
{
  if (state == GLUT_DOWN)
  {
    if (button == GLUT_LEFT_BUTTON)
    {
      /* Aggiorna la posizione del mouse salvata */
      xStart = x;
      yStart = y;
    }
  }
}

// Movimento del mouse
GLvoid motion(GLint x, GLint y)
{
  /* Aggiorna il punto di vista */
  alphax -= (GLdouble)(y - yStart);
  alphaz += (GLdouble)(x - xStart);
  /* Aggiorna la posizione del mouse salvata */
  xStart = x;
  yStart = y;
  glutPostRedisplay();
}

// Disegna un sistema di assi cartesiane con origine in (0,0,0)
GLvoid drawAxis(GLfloat lato)
{
  GLfloat blu[] = {0.0f, 0.0f, 1.0f};
  GLfloat rosso[] = {1.0f, 0.0f, 0.0f};
  GLfloat verde[] = {0.0f, 1.0f, 0.0f};
  glBegin(GL_LINES);
  glColor3fv(rosso);
  glVertex3f(0, 0, 0);
  glVertex3f(lato, 0, 0);
  glColor3fv(verde);
  glVertex3f(0, 0, 0);
  glVertex3f(0, lato, 0);
  glColor3fv(blu);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, lato);
  glEnd();
}

// inizializzazioni
GLvoid init(GLvoid)
{
  /* Stabilisce il colore dello sfondo */
  glClearColor(0.0, 0.0, 0.0, 1.0);

  /* Uso depth buffer */
  glEnable(GL_DEPTH_TEST);

  /* Matrice di proiezione */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, 1, nearClip, farClip);
  /* Matrice di ModelView */
  glMatrixMode(GL_MODELVIEW);

  /* Abilitazione luce 0 */
  glEnable(GL_LIGHT0);

  /* Normalizzazione delle normali */
  glEnable(GL_NORMALIZE);
}

// Callback per la tastiera
GLvoid inputKey(GLubyte key, GLint x, GLint y)
{
  switch (key)
  {

  case KEY_ESC:
    exit(0);
  case 'a':
    campo.getPlayer(1)->encreaseY();
    break;
  case 'd':
    campo.getPlayer(1)->decreaseY();
    break;
  case 'w':
    campo.getPlayer(1)->encreaseZ();
    break;
  case 's':
    campo.getPlayer(1)->decreaseZ();
    break;
  }
  glutPostRedisplay();
}

void specialKeyInput(int key, int _x, int _y)
{
  switch (key)
  {
  case GLUT_KEY_UP:
    campo.getPlayer(2)->encreaseZ();
    break;
  case GLUT_KEY_DOWN:
    campo.getPlayer(2)->decreaseZ();
    break;
  case GLUT_KEY_LEFT:
    campo.getPlayer(2)->decreaseY();
    break;
  case GLUT_KEY_RIGHT:
    campo.getPlayer(2)->encreaseY();
    break;
  default:
    break;
  }
  // glutPostRedisplay();
}

// Impostazione del punto di vista
void setView(GLfloat alphax, GLfloat alphaz)
{
  glTranslatef(0.0f, 0.0f, -dist);
  glRotatef(-alphax, 1.0f, 0.0f, 0.0f);
  glRotatef(-alphaz, 0.0f, 0.0f, 1.0f);
}


// Callback di display
GLvoid drawScene(GLvoid)
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  // Posizione luce legata al punto di vista:
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  setView(alphax, alphaz);
  drawAxis(20.0f);

  glEnable(GL_LIGHTING);

  /* Emissione di default per Sfera e Toro */
  glMaterialfv(GL_FRONT, GL_EMISSION, Nero);

  /* Sfera: materiale rosso brillante, con highlight verde */
  glMaterialfv(GL_FRONT, GL_AMBIENT, RossoTenue);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, Rosso);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Verde);
  glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);

  glPushMatrix();
  glTranslatef(ball->getXPal(), ball->getYPal(), ball->getZPal());
  glutSolidSphere(0.4, 31, 31); // ball.draw
  glPopMatrix();

  /* Cubo: materiale giallo matto che emette luce.
   * La shininess e' la stessa del toro (non e' impostata diversamente) */
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
  glMaterialfv(GL_FRONT, GL_EMISSION, GialloTenue);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);

  /*Draw fiels*/
  campo.drawField();

  campo.getPlayer(2)->drawPlayer();
  campo.getPlayer(1)->drawPlayer();

  /* Disabilita l'illuminazione */
  glDisable(GL_LIGHTING);

  glPopMatrix();

  glutSwapBuffers();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
	glutInitWindowPosition(X_POS, Y_POS);
  glutCreateWindow("Pong3d");
  init();
  glutKeyboardFunc(inputKey);
  glutSpecialFunc(specialKeyInput);

  glutDisplayFunc(drawScene);
  glutTimerFunc(50, ball->moveBall, 0);

  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glutMainLoop();
  return (0);
}
