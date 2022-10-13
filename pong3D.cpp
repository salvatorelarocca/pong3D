#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include<map>
#include <SOIL/SOIL.h>
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

static unsigned int texture[2]; // Array of texture indices. Serve ad indicare l indice della texture 


GLfloat lightPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};
// Parametri per gluPerspective
static GLfloat fovy = 60, aspect = 1, nearClip = 3, farClip = 40;
// Parametri per il punto di vista
static GLfloat dist = 20.5, alphax = 90.0, alphaz = 91.0; //con questa rotazione siamo nella prospettiva del giocatore1 l'asseX verso di noi
static GLdouble xStart = 0.0, yStart = 0.0;//per la rotazione con il mouse
static GLint width = 1200, height = 800;
static map<char, bool> keyState = {{'a', false}, {'s', false}, {'d', false}, {'w', false}, 
{'t', false}, {'b', false}, {'l', false}, {'r', false}}; //hash per i tasti della tastiera t=top b=bottom l=left r=right true=premuto false=rilasciato

class Player
{
private:
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat dim;
  GLint score;
  GLfloat step;

public:
  Player(GLfloat d)
  {
    x = 0;
    y = 0;
    z = 0;
    dim = d;
    step = 0.1;
  }
  GLfloat getX() { return x; }
  GLfloat getY() { return y; }
  GLfloat getZ() { return z; }
  GLfloat getStep() { return step; }
  void setStep(GLfloat s){ step = s; }
  void setX(GLfloat x) { this->x = x; }
  void setY(GLfloat y) { this->y = y; }
  void setZ(GLfloat z) { this->z = z; }
  GLfloat getDim() { return dim; }

  void encreseScore() { score++; }
  int getScore() { return score; }
  /*movimento player sul piano YZ*/
  void encreaseY(GLfloat dimFieldY)
  {
    if (y + dim/2.0f + step <= dimFieldY/2.0f){ //sottraggo o sommo step altrimenti esce
      cout<<"y:"<<y<<" dimFieldy:"<<dimFieldY/2.0f<<endl;
      y = y + step;
    }
  }
  void decreaseY(GLfloat dimFieldY)
  {
    if (y - dim/2.0f - step >= -dimFieldY/2){
      cout<<"y:"<<y<<" dimFieldy:"<<dimFieldY/2.0f<<endl;
      y = y - step;
    }
  }
  void encreaseZ(GLfloat dimFieldZ)
  {
    if (z + dim/2.0f +step <= dimFieldZ/2.0f)
    {
      cout<<"z:"<<z<<" dim: "<<dimFieldZ/2.0f<<endl;
      z = z + step;
    }
  }
  void decreaseZ(GLfloat dimFieldZ)
  {
    if (z - dim/2.0f -step >= -dimFieldZ/2.0f)
    {
      cout<<"z:"<<z<<" dim: "<<dimFieldZ/2.0f<<endl;
      z = z - step;
    }
  }
  void drawPlayer();
};

static void cubebase(void) //costruscie una faccia del cubo, viene richiaata 6 volte da cubebase
/*specifies a side of a cube*/
{
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

static void cube(void)  //ci serve per i parallelepipedi texturizabili
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
    player1 = new Player(dimPlayer); //situato a +x*(dimX/2)
    player2 = new Player(dimPlayer); //situato a -x*(dimX/2)
    player1->setX(dimX/2.0f); //set profondità dei giocatori. Profondità lungo l'asse x(scelta iniziale del dottore andrea cappabianca)
    player2->setX(-dimX/2.0f);
  }
  void drawField()
  {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(dimX, dimY, dimZ);
    glutWireCube(1);
    glPopMatrix();

    glPushMatrix();

 	glTranslated(0.0, 0.0, -dimZ/2);
	glRotated(0.0, 0.0, 0.0, 0.0);
   glScalef(dimX, dimY,0);
   
  
	cubebase();
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
Field campo(20, 11, 11, 2); // X profondità y and z facciata. Ultimo parametro dimensione giocatori nel campo



class Ball
{
private:
  GLfloat xPal, yPal, zPal; // coordiate pallina
  GLfloat speedX;
  GLfloat speedY;
  GLfloat speedZ;
  GLfloat raggio;
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
    raggio = 0.2f;
  }
  Ball(GLfloat x, GLfloat y, GLfloat z, GLfloat sx, GLfloat sy, GLfloat sz)
  {
    xPal = x;
    yPal = y;
    zPal = z;
    speedX = sx;
    speedY = sy;
    speedZ = sz;
    raggio = 0.2f;
  }
  Ball(GLfloat x, GLfloat y, GLfloat z)
  {
    xPal = x;
    yPal = y;
    zPal = z;
    speedX = 0.2f;
    speedY = 0.2f;
    speedZ = 0.2f;
    raggio = 0.2f;
  }

  void setXPal(int x) { xPal = x; }
  GLfloat getXPal() { return xPal; }
  void setYPal(int y) { yPal = y; }
  GLfloat getYPal() { return yPal; }
  void setZPal(int z) { zPal = z; }
  GLfloat getZPal() { return zPal; }
  void setRaggio(int r ) {raggio=r;}
  GLfloat getRaggio(){return raggio;}

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
  if (xPal >= campo.getDimX()/2 || xPal <= -campo.getDimX()/2)
  {
    if (campo.getPlayer(2)->getY() < yPal && campo.getPlayer(2)->getY() + campo.getPlayer(2)->getDim() > yPal &&
            campo.getPlayer(2)->getZ()< zPal && campo.getPlayer(2)->getZ() + campo.getPlayer(2)->getDim() > zPal ||
        (campo.getPlayer(1)->getY() < yPal && campo.getPlayer(1)->getY() + campo.getPlayer(2)->getDim() > yPal &&
         campo.getPlayer(1)->getZ() < zPal && campo.getPlayer(1)->getZ() + campo.getPlayer(2)->getDim() > zPal)){
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
  if (yPal+ball->getRaggio() >= campo.getDimY()/2 || yPal-ball->getRaggio() <= -campo.getDimY()/2)
  {
    speedY = -speedY;
  }
  if (zPal+ball->getRaggio() >= campo.getDimZ()/2 || zPal-ball->getRaggio() <= -campo.getDimZ()/2)
  {
    speedZ = -speedZ;
  }

  glutPostRedisplay();
}

void Ball::moveBall(int i)
{
  ball->moveball(i);
}


//serve per caricare texture da immagini con soil
void loadExternalTextures()			
{
  
  int numeroTexture = 2;
  int i = 0;
  char* filenameTexture[] = {"a.jpg","fish.png"}; //warning ma non errore e li legge 
	int width, height, channels;
	unsigned char *img;
  while(i<numeroTexture)
  {
	  img = SOIL_load_image
	  (
		  filenameTexture[i],
		  &width, &height, &channels,
		  SOIL_LOAD_AUTO
	  );
	  if (img != NULL) 
    {
	  	glBindTexture(GL_TEXTURE_2D, texture[i]);
      // Costruisce le MipMap, eventualmente scalando l'immagine, e specifica le texture 
		  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height,
			GL_RGB, GL_UNSIGNED_BYTE, img);

		  SOIL_free_image_data(img);
	  }
    i++;
  }
}






void Player::drawPlayer()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Nero);
  glMaterialfv(GL_FRONT, GL_EMISSION, Verde);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);
  glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.01, dim, dim); //0.01 è lo spessore che sto trattando come costante
    //glutWireCube(1);
    cube(); // definisce il cubo tramite poligoni per le texture 
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



  //carico texture esterne con soil
  glGenTextures(1, texture); 
  loadExternalTextures();
  // Specify how texture values combine with current surface color values. 
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);




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
    keyState['a'] = true;
    campo.getPlayer(1)->decreaseY(campo.getDimY());
    break;
  case 'd':
    keyState['d'] = true;
    campo.getPlayer(1)->encreaseY(campo.getDimZ());
    break;
  case 'w':
    keyState['w'] = true;
    campo.getPlayer(1)->encreaseZ(campo.getDimZ());
    break;
  case 's':
    keyState['s'] = true;
    campo.getPlayer(1)->decreaseZ(campo.getDimZ());
    break;
  }
  glutPostRedisplay();
}

/*movimento player2 che si muove con le frecce*/
void specialKeyInput(int key, int _x, int _y)
{
  switch (key)
  {
  case GLUT_KEY_UP:
    keyState['t'] = true; //top
    campo.getPlayer(2)->encreaseZ(campo.getDimZ());
    break;
  case GLUT_KEY_DOWN:
    keyState['b'] = true; //bottom
    campo.getPlayer(2)->decreaseZ(campo.getDimZ());
    break;
  case GLUT_KEY_LEFT:
    keyState['l'] = true; //left
    campo.getPlayer(2)->encreaseY(campo.getDimY()); //specchiato rispetto al player1
    break;
  case GLUT_KEY_RIGHT:
    keyState['r'] = true; //right
    campo.getPlayer(2)->decreaseY(campo.getDimY());
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void specialKeyUpInput(int key, int _x, int _y){
  switch (key)
  {
  case GLUT_KEY_UP:
    keyState['t'] = false; //top
    campo.getPlayer(2)->encreaseZ(campo.getDimZ());
    break;
  case GLUT_KEY_DOWN:
    keyState['b'] = false; //bottom
    campo.getPlayer(2)->decreaseZ(campo.getDimZ());
    break;
  case GLUT_KEY_LEFT:
    keyState['l'] = false; //left
    campo.getPlayer(2)->encreaseY(campo.getDimY()); //specchiato rispetto al player1
    break;
  case GLUT_KEY_RIGHT:
    keyState['r'] = false; //right
    campo.getPlayer(2)->decreaseY(campo.getDimY());
    break;
  default:
    break;
  }
}

void inputKeyup(unsigned char key, int x, int y){
   switch (key)
  {
  case KEY_ESC:
    exit(0);
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
  }
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
  glutSolidSphere(ball->getRaggio(), 31, 31); // ball.draw
  glPopMatrix();

  /* Cubo: materiale giallo matto che emette luce.
   * La shininess e' la stessa del toro (non e' impostata diversamente) */
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
  glMaterialfv(GL_FRONT, GL_EMISSION, GialloTenue);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);

  /*Draw fiels*/
  

//texture applicata al player ma si puo fare anche direttamente nel drawplayer 
glPushMatrix();
 glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture[1]); //array di texture caricate con loadExternal()
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	campo.getPlayer(2)->drawPlayer();  //funzione modificata per permettere di applicare le texutre
  campo.getPlayer(1)->drawPlayer();
   glBindTexture(GL_TEXTURE_2D, texture[0]); //array di texture caricate con loadExternal()
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  campo.drawField();

 glPopMatrix();
 glDisable(GL_TEXTURE_2D);
  


  //effetto linea che segue la palla
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blu);
  glMaterialfv(GL_FRONT, GL_EMISSION, Blu);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Blu);
  glPushMatrix();
    glTranslatef(ball->getXPal(), 0, 0);
    glScalef(0.2, campo.getDimY(), campo.getDimZ()); //0.2 è lo spessore che sto trattando come costante
    glutWireCube(1);
  glPopMatrix();


  /* Disabilita l'illuminazione */
  glDisable(GL_LIGHTING);

  glPopMatrix();

  glutSwapBuffers();
}





/*funzione idle per continuare gli spostamenti fin quando i tasti sono premuti*/
void idle(){
  //idle per i tasti asdw   
  if(keyState['a'])
    campo.getPlayer(1)->decreaseY(campo.getDimY());
  if(keyState['s'])
    campo.getPlayer(1)->decreaseZ(campo.getDimZ());
  if(keyState['d'])
    campo.getPlayer(1)->encreaseY(campo.getDimZ());
  if(keyState['w'])
    campo.getPlayer(1)->encreaseZ(campo.getDimZ());

  if(keyState['t'])
    campo.getPlayer(2)->encreaseZ(campo.getDimZ());
  if(keyState['b'])
    campo.getPlayer(2)->decreaseZ(campo.getDimZ());
  if(keyState['l'])
    campo.getPlayer(2)->encreaseY(campo.getDimY());
  if(keyState['r'])
    campo.getPlayer(2)->decreaseY(campo.getDimY());
  glutPostRedisplay();
}



int main(int argc, char *argv[])
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
	glutInitWindowPosition(X_POS, Y_POS);
  glutCreateWindow("Pong3d");
  init();
  glutIgnoreKeyRepeat(1); //serve a ignorare la ripetizione delle callback dei tasti da tastiera quando sono "tenuti giù" perchè li ho gestito separatamente la pressione e il rilascio
  //attiva callback freccette
  //pressione
  glutSpecialFunc(specialKeyInput);
  //rilascio
  glutSpecialUpFunc(specialKeyUpInput);

  //attiva callback tasti "asdw"
  //pressione
  glutKeyboardFunc(inputKey);
  //rilascio
  glutKeyboardUpFunc(inputKeyup);
  //
  glutIdleFunc(idle);

  glutDisplayFunc(drawScene);
  glutTimerFunc(50, ball->moveBall, 0);

  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glutMainLoop();
  return (0);
}