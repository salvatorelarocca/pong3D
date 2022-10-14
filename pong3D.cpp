#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include<map>
#include <SOIL/SOIL.h>
#include<time.h>
#include<string>

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
GLfloat myDistance;
static unsigned int texture[2]; // Array of texture indices. Serve ad indicare l indice della texture 


GLfloat lightPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};
// Parametri per gluPerspective
static GLfloat fovy = 80, aspect = 1, nearClip = 3, farClip = 100;
// Parametri per il punto di vista
static GLfloat dist = 30, alphaxP1 = 90.0, alphazP1 = 91.0, alphaxP2 = 90.0, alphazP2 = -91.0; //con questa rotazione siamo nella prospettiva del giocatore1 l'asseX verso di noi
static GLdouble xStart = 0.0, yStart = 0.0;//per la rotazione con il mouse
static GLint width = 1200, height = 800;
static map<char, bool> keyState = {{'a', false}, {'s', false}, {'d', false}, {'w', false}, {'p', false}, {' ', false},
{'t', false}, {'b', false}, {'l', false}, {'r', false}}; //hash per i tasti della tastiera t=top b=bottom l=left r=right true=premuto false=rilasciato
string scoreP1;
string scoreP2;

void writeBitmapString(void* font, string str) {
    const char* c = str.c_str();
    const char* cc;
    for (cc = c; *cc != '\0'; cc++) glutBitmapCharacter(font, *cc);
}

class Player
{
private:
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat dim;
  GLint score;
  GLfloat step;
  string name;

public:
  Player(GLfloat d)
  {
    x = 0;
    y = 0;
    z = 0;
    dim = d;
    step = 0.1;
    name = "no_name";
  }
  GLfloat getX() { return x; }
  GLfloat getY() { return y; }
  GLfloat getZ() { return z; }
  GLfloat getStep() { return step; }
  string getName(){ return name; }
  void setName(string n){ name = n; }
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
    if (z + dim +step <= dimFieldZ/2.0f)
    {
      cout<<"z:"<<z<<" dim: "<<dimFieldZ/2.0f<<endl;
      z = z + step;
    }
  }
  void decreaseZ(GLfloat dimFieldZ)
  {
    if (z -step >= -dimFieldZ/2.0f)
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
Field campo(40, 11, 11, 2); // X profondità y and z facciata. Ultimo parametro dimensione giocatori nel campo



class Ball
{
private:
  GLfloat xPal, yPal, zPal; // coordiate pallina
  GLfloat speedX;
  GLfloat speedY;
  GLfloat speedZ;
  
  GLfloat radius;
  // qua aggiungere la texture quando capirte come cazzi si fa
public:
  Ball(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat sx, GLfloat sy, GLfloat sz)
  {
    xPal = x;
    yPal = y;
    zPal = z;
    speedX = sx;
    speedY = sy;
    speedZ = sz;
    radius = r;
  }

  Ball(GLfloat x, GLfloat y, GLfloat z, GLfloat r)
  {
    srand(time(NULL));
    xPal = x;
    yPal = y;
    zPal = z;
    speedX = 0.0f;
    speedY = 0.0f;
    speedZ = 0.0f;
    radius = r;
  }

  void setXPal(GLfloat x) { xPal = x; }
  GLfloat getXPal() { return xPal; }
  void setYPal(GLfloat y) { yPal = y; }
  GLfloat getYPal() { return yPal; }
  void setZPal(GLfloat z) { zPal = z; }
  GLfloat getZPal() { return zPal; }
  void setRadius(GLfloat r ) { radius = r;}
  GLfloat getRadius(){ return radius; }

  void setSpeedXYZ(GLfloat sX, GLfloat sY, GLfloat sZ){
    speedX = sX;
    speedY = sY;
    speedZ = sZ;
  }

  GLfloat getSpeedX(){ return speedX; }
  GLfloat getSpeedY(){ return speedY; }
  GLfloat getSpeedZ(){ return speedZ; }

  static void moveBall(int);
  void moveball(int);
};
Ball *ball = new Ball(0, 0, 0, 0.3f, 0.4f, 0.2f ,0.3f);

//Qui sarebbe giusto passare
void Ball::moveball(int i) // faccio check collision con bordi e con i player
{
  glutTimerFunc(50, ball->moveBall, 0);
  xPal = xPal + speedX;
  yPal = yPal + speedY;
  zPal = zPal + speedZ;
  // collisione con i player
  if (xPal >= campo.getDimX()/2-ball->getRadius())
  {
      myDistance=sqrt(pow(xPal-campo.getPlayer(1)->getX(),2.0)+pow(yPal-campo.getPlayer(1)->getY(),2.0)+pow(zPal-campo.getPlayer(1)->getZ(),2.0));
        if(myDistance >= ball->getRadius()-0.1 && myDistance <= sqrt(   pow(ball->getRadius(),2.0) + pow(campo.getPlayer(1)->getDim()*1.4/2,2) ) )
                                        //tolleranza
          speedX = -speedX; 
        else
        {
      // aumento score
          xPal = 0;
          yPal = 0;
          zPal = 0;
          campo.getPlayer(2)->encreseScore();
        }
  }

  if (xPal <= -campo.getDimX()/2+ball->getRadius())
  {
      myDistance=sqrt(pow(xPal-campo.getPlayer(2)->getX(),2.0)+pow(yPal-campo.getPlayer(2)->getY(),2.0)+pow(zPal-campo.getPlayer(2)->getZ(),2.0));
        if(myDistance >= ball->getRadius()-0.1 && myDistance <= sqrt(   pow(ball->getRadius(),2.0) + pow(campo.getPlayer(2)->getDim()*1.4/2,2) ) )
          speedX = -speedX; 
        else
        {
      // aumento score
          xPal = 0;
          yPal = 0;
          zPal = 0;
          campo.getPlayer(1)->encreseScore();
        }
        
  }
  cout<<"mydistance"<<myDistance<<endl<<endl;
  // collisione con il campo
  if (yPal+ball->getRadius() >= campo.getDimY()/2 || yPal-ball->getRadius() <= -campo.getDimY()/2)
  {
    speedY = -speedY;
  }
  if (zPal+ball->getRadius() >= campo.getDimZ()/2 || zPal-ball->getRadius() <= -campo.getDimZ()/2)
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
  char* filenameTexture[] = {"a.png","fish.png"}; //warning ma non errore e li legge 
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
    glTranslatef(x, y, z-dim/2); //-dim/2 per centrare i giocatori su z 
    glScalef(0.01, dim, dim); //0.01 è lo spessore che sto trattando come costanteee
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
  if(keyState['p']){
    if(x<width/2.0f){
      /* Aggiorna il punto di vista */
      alphaxP1 -= (GLdouble)(y - yStart);
      alphazP1 += (GLdouble)(x - xStart);
      /* Aggiorna la posizione del mouse salvata */
    }else{
      alphaxP2 -= (GLdouble)(y - yStart);
      alphazP2 += (GLdouble)(x - xStart);
    }
    xStart = x;
    yStart = y;
    glutPostRedisplay();
  }
}

// Disegna un sistema di assi cartesiane con origine in (0,0,0)
GLvoid drawAxis(GLfloat lato)
{
  glBegin(GL_LINES);
  glColor3fv(Rosso);
  glVertex3f(0, 0, 0);
  glVertex3f(lato, 0, 0);
  glColor3fv(Verde);
  glVertex3f(0, 0, 0);
  glVertex3f(0, lato, 0);
  glColor3fv(Blu);
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
  gluPerspective(fovy, (GLfloat)width/height, nearClip, farClip);
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
  static GLfloat oldSpeedX, oldSpeedY, oldSpeedZ;
  switch (key)
  {
  case KEY_ESC:
    exit(0);
  case 'a':
    keyState['a'] = true;
    break;
  case 'd':
    keyState['d'] = true;
    break;
  case 'w':
    keyState['w'] = true;
    break;
  case 's':
    keyState['s'] = true;
    break;
  case 'p': //tasto pause
    keyState['p'] = !keyState['p'];
    if(keyState['p']){
      oldSpeedX = ball->getSpeedX(); /*salvo la velocità*/
      oldSpeedY = ball->getSpeedY();
      oldSpeedZ = ball->getSpeedZ();
      ball->setSpeedXYZ(0.0f, 0.0f, 0.0f); /*fermo la pallina*/
    }else{
      ball->setSpeedXYZ(oldSpeedX, oldSpeedY, oldSpeedZ); /*riparte la pallina alla vecchia velocità*/
    }
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
    break;
  case GLUT_KEY_DOWN:
    keyState['b'] = true; //bottom
    break;
  case GLUT_KEY_LEFT:
    keyState['l'] = true; //left
    break;
  case GLUT_KEY_RIGHT:
    keyState['r'] = true; //right
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
    break;
  case GLUT_KEY_DOWN:
    keyState['b'] = false; //bottom
    break;
  case GLUT_KEY_LEFT:
    keyState['l'] = false; //left
    break;
  case GLUT_KEY_RIGHT:
    keyState['r'] = false; //right
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
void setView(GLfloat alphax, GLfloat alphaz, GLfloat d)
{
  glTranslatef(0.0f, 0.0f, -d);
  glRotatef(-alphax, 1.0f, 0.0f, 0.0f);
  glRotatef(-alphaz, 0.0f, 0.0f, 1.0f);
}

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
	int i;
	int triangleAmount = 20; //# of triangles used to draw circle
	
	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * M_PI;
	
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(i = 0; i <= triangleAmount; i++){ 
			glVertex2f(
		            x + (radius * cos(i *  twicePi / triangleAmount)), 
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}

// Callback di display
GLvoid drawScene(GLvoid)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /*View first player*/
  glViewport(0, 0, width * 0.5f, height * 0.75f);

  /*Label Score*/
  glPushMatrix();
    setView(0.0f, 0.0f, dist);
    glColor3f(1.0, 1.0, 1.0);
    //cout<<"w: "<<width * 0.5f<<" h: "<<height * 0.75f * 0.125f<<endl;
    glRasterPos3f(-10,22,0);
    writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, campo.getPlayer(1)->getName().append(": ").append(to_string(campo.getPlayer(1)->getScore())));
  glPopMatrix();

  glPushMatrix();
    setView(alphaxP1, alphazP1, dist);
    // Posizione luce legata al punto di vista:
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    drawAxis(20.0f);

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
      glBindTexture(GL_TEXTURE_2D, texture[1]); //array di texture caricate con loadExternal()
	      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	      campo.getPlayer(2)->drawPlayer();  //funzione modificata per permettere di applicare le texture
        campo.getPlayer(1)->drawPlayer();
      glBindTexture(GL_TEXTURE_2D, texture[0]); //array di texture caricate con loadExternal()
	      campo.drawField();
    glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  //effetto linea che segue la palla
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blu);
  glMaterialfv(GL_FRONT, GL_EMISSION, Blu);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Rosso);
  glPushMatrix();
    glTranslatef(ball->getXPal(), 0, 0);
    glScalef(0.2, campo.getDimY(), campo.getDimZ()); //0.2 è lo spessore che sto trattando come costante
    glutWireCube(1);
  glPopMatrix();
  //effetto cerchi sui piani YZ per aiutare a colpire la palla
  glPushMatrix();
    glTranslatef(campo.getDimX()/2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0,0.0,ball->getRadius());
  glPopMatrix();

  glPushMatrix();
    glTranslatef(-campo.getDimX()/2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0,0.0,ball->getRadius());
  glPopMatrix();
  /* Disabilita l'illuminazione */
glDisable(GL_LIGHTING);
glPopMatrix();


/*View second player-----------------------------------------------------------------------------------*/
glViewport(width*0.5f, 0, width*0.5f, height*0.75f);
/*Label Score*/
glPushMatrix();
  setView(0.0f, 0.0f, dist);
  glColor3f(1.0, 1.0, 1.0);
  //cout<<"w: "<<width * 0.5f<<" h: "<<height * 0.75f * 0.125f<<endl;
  glRasterPos3f(-10,22,0);
  writeBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, campo.getPlayer(2)->getName().append(": ").append(to_string(campo.getPlayer(2)->getScore())));
glPopMatrix();

glPushMatrix();
  // Posizione luce legata al punto di vista:
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    setView(alphaxP2, alphazP2, dist);
    drawAxis(20.0f);

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
      glBindTexture(GL_TEXTURE_2D, texture[1]); //array di texture caricate con loadExternal()
	      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	      campo.getPlayer(2)->drawPlayer();  //funzione modificata per permettere di applicare le texture
        campo.getPlayer(1)->drawPlayer();
      glBindTexture(GL_TEXTURE_2D, texture[0]); //array di texture caricate con loadExternal()
	      campo.drawField();
    glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  //effetto linea che segue la palla
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blu);
  glMaterialfv(GL_FRONT, GL_EMISSION, Blu);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Rosso);
  glPushMatrix();
    glTranslatef(ball->getXPal(), 0, 0);
    glScalef(0.2, campo.getDimY(), campo.getDimZ()); //0.2 è lo spessore che sto trattando come costante
    glutWireCube(1);
  glPopMatrix();
  //effetto cerchi sui piani YZ per aiutare a colpire la palla
  glPushMatrix();
    glTranslatef(campo.getDimX()/2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0,0.0,ball->getRadius());
  glPopMatrix();

  glPushMatrix();
    glTranslatef(-campo.getDimX()/2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0,0.0,ball->getRadius());
  glPopMatrix();
  /* Disabilita l'illuminazione */
  glDisable(GL_LIGHTING);
  glPopMatrix();


  /*Granangolo----------------------------------------------------------------------------------------------*/

  glViewport(0, height*0.75f, width, height);
  glPushMatrix();
    setView(90.0f, 90.0f, 40);
    glRotatef(90.0f, 0,0,1);
    glScalef(0.5f,0.5f,0.5f);
    glTranslated(0.0f,0.0f,-50.0f);
    // Posizione luce legata al punto di vista:
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    drawAxis(100.0f);

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
      glBindTexture(GL_TEXTURE_2D, texture[1]); //array di texture caricate con loadExternal()
	      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	      campo.getPlayer(2)->drawPlayer();  //funzione modificata per permettere di applicare le texture
        campo.getPlayer(1)->drawPlayer();
      glBindTexture(GL_TEXTURE_2D, texture[0]); //array di texture caricate con loadExternal()
	      campo.drawField();
    glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  //effetto linea che segue la palla
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blu);
  glMaterialfv(GL_FRONT, GL_EMISSION, Blu);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Rosso);
  glPushMatrix();
    glTranslatef(ball->getXPal(), 0, 0);
    glScalef(0.2, campo.getDimY(), campo.getDimZ()); //0.2 è lo spessore che sto trattando come costante
    glutWireCube(1);
  glPopMatrix();
  //effetto cerchi sui piani YZ per aiutare a colpire la palla
  glPushMatrix();
    glTranslatef(campo.getDimX()/2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0,0.0,ball->getRadius());
  glPopMatrix();

  glPushMatrix();
    glTranslatef(-campo.getDimX()/2.0f, ball->getYPal(), ball->getZPal());
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawFilledCircle(0.0,0.0,ball->getRadius());
  glPopMatrix();
  /* Disabilita l'illuminazione */
glDisable(GL_LIGHTING);
glPopMatrix();

  glutSwapBuffers();
}



/*funzione idle per continuare gli spostamenti fin quando i tasti sono premuti*/
void idle(){
  if(!keyState['p']){
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
}


void resize(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)w/h, nearClip, farClip);
	width = w;
	height = h;
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
}




int main(int argc, char *argv[])
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
	glutInitWindowPosition(X_POS, Y_POS);
  glutCreateWindow("Pong3d");
  glutReshapeFunc(resize);
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

  init();
  glutMainLoop();
  return (0);
}
