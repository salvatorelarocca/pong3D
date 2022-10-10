#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
/*dimesioni e posizione finistra*/
#define WIDTH 800
#define HEIGHT 400
#define X_POS 100
#define Y_POS 100


GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f };
int mode = 1;

static int width, height;
//Parametri per gluPerspective
static GLfloat fovy = 60, aspect = 1, nearClip = 5, farClip = 100;
//Parametri per il punto di vista
static GLfloat  distance = 20.5, alphax1 = 0.0, alphaz1 = 0.0, alphax2 = 180, alphaz2 = 180;
//Parametri per il movimento del mouse
static GLint		movimento = 0;
static GLdouble	xStart = 0.0, yStart = 0.0;
//Angolo di rotazione per la luce
static GLfloat lightAngle = 0.0f;

int xPal, yPal, zPal;
int speedX = 1;
int speedY = 1;
int speedZ = 1;



/* Definizione di colori per proprieta' dei materiali */
GLfloat Rosso[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat Verde[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat Blu[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat Nero[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat Bianco[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat Giallo[] = { 1.0f, 1.0f, 0.0f, 1.0f };
GLfloat RossoTenue[] = { 0.3f, 0.1f, 0.1f, 1.0f };
GLfloat BluTenue[] = { 0.1f, 0.1f, 0.3f, 1.0f };
GLfloat GialloTenue[] = { 0.6f, 0.6f, 0.0f, 1.0f };

//Tasti del mouse
GLvoid mouse(GLint button, GLint state, GLint x, GLint y)
{
	static GLint buttons_down = 0;
	if (state == GLUT_DOWN)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:/* Per spostare il punto di vista */
			movimento = 1;
			/* Aggiorna la posizione del mouse salvata */
			xStart = x;
			yStart = y;
			break;
		case GLUT_RIGHT_BUTTON:/* Per modificare l'angolo di rotazione della luce */
			movimento = 2;
			/* Aggiorna la posizione del mouse salvata */
			xStart = x;
			yStart = y;
			break;
		}
	}
	else
		movimento = 0;
}

static void DrawCircle(GLfloat bx, GLfloat by, GLfloat bz, GLfloat r, int hit_dir)
{
  GLfloat x, y, z, d;

  glPushMatrix();

  glTranslatef(bx, by, bz);

  if (hit_dir == 0)
    glRotatef(90.0, 0.0, 1.0, 0.0);
  else if (hit_dir == 1)
    glRotatef(90.0, 1.0, 0.0, 0.0);

  y = -r;
  d = float(r) / 5.0;

  while (y <= r)
    {
      x = sqrt((double)(r * r - (y * y)));
      glBegin(GL_LINES);
      glVertex3f(-x, y, 0.0);
      glVertex3f(+x, y, 0.0);
      glEnd();

      glBegin(GL_LINES);
      glVertex3f(y, -x, 0.0);
      glVertex3f(y, +x, 0.0);
      glEnd();

      y += d;
    }

  glPopMatrix();
}

//Movimento del mouse
GLvoid motion(GLint x, GLint y)
{
	if (x < width / 2)
	{
		alphax1 -= (GLdouble)(y - yStart);
		alphaz1 += (GLdouble)(x - xStart);
	}
	else {
		alphax2 -= (GLdouble)(y - yStart);
		alphaz2 += (GLdouble)(x - xStart);
	}
	xStart = x;
	yStart = y;

	glutPostRedisplay();
}

//Procedura per il controllo errori
void ErrorCheck(char* label)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
		printf("%s: %s\n", label, gluErrorString(error));
}

//Disegna un sistema di assi cartesiane con origine in (0,0,0)
GLvoid drawAssi(GLfloat lato)
{
	glBegin(GL_LINES);
	glColor3fv(Rosso); glVertex3f(0, 0, 0); glVertex3f(lato, 0, 0);
	glColor3fv(Verde); glVertex3f(0, 0, 0); glVertex3f(0, lato, 0);
	glColor3fv(Blu);   glVertex3f(0, 0, 0); glVertex3f(0, 0, lato);
	glEnd();
}

//inizializzazioni
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

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, 1, nearClip, farClip);
	glMatrixMode(GL_MODELVIEW);
	// Pass the size of the OpenGL window.
	width = w;
	height = h; 
}

//Impostazione del punto di vista
void ImpostaVistaFirstPlayer(GLfloat alphax, GLfloat alphaz)
{
	glTranslatef(0.0f, 0.0f, -distance);
	glRotatef(-alphax, 1.0f, 0.0f, 0.0f);
	glRotatef(-alphaz, 0.0f, 0.0f, 1.0f);
}

void ImpostaVistaSecondPlayer(GLfloat alphax, GLfloat alphaz)
{
	glTranslatef(0.0f, 0.0f, -distance);
	glRotatef(-alphax + 180, 1.0f, 0.0f, 0.0f);
	glRotatef(-alphaz, 0.0f, 0.0f, 1.0f);
}

void moveball(int i){
	glutTimerFunc(50, moveball, 0);
	xPal = xPal + speedX;
	yPal = yPal + speedY;
	zPal = zPal + speedZ;
	if (xPal >= 10 || xPal <= -10) { speedX = -speedX; }
	if (yPal >= 4 || yPal <= -4) { speedY = -speedY; }
	if (zPal >= 4 || zPal <= -4) { speedZ = -speedZ; }
	glutPostRedisplay();
}


// Callback di display
GLvoid drawScene(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*view first player*/
    DrawCircle(0.0, 0.0, 0.0, 3.0, 1);
	glViewport(0, 0, width / 2.0, height);
	glPushMatrix();
	glBegin(GL_POINTS);
	glVertex3f(0.0,0.0,10.0);
	glEnd();
	//Posizione luce legata al punto di vista:
    /* Definisce la posizione della luce 0 */
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	/* Stabilisce il punto di vista */
	ImpostaVistaFirstPlayer(alphax1, alphaz1);
	/* Disegna gli assi */
	drawAssi(50);
	/* Abilita l'illuminazione */
	glEnable(GL_LIGHTING);
	/* Emissione di default per Sfera e Toro */
	glMaterialfv(GL_FRONT, GL_EMISSION, Nero);
	/* Sfera: materiale rosso brillante, con highlight verde */
	glMaterialfv(GL_FRONT, GL_AMBIENT, RossoTenue);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Rosso);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Verde);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);
	glPushMatrix();
	glTranslatef(xPal, yPal, zPal);
	glutSolidSphere(0.4, 31, 31);
	glPopMatrix();
	/* Cubo: materiale giallo matto che emette luce.
	 * La shininess e' la stessa del toro (non e' impostata diversamente) */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
	glMaterialfv(GL_FRONT, GL_EMISSION, GialloTenue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(21, 9, 9);
	glutWireCube(1);
	glPopMatrix();
	/* Disabilita l'illuminazione */
	glDisable(GL_LIGHTING);
	glPopMatrix();
	/* Controllo errori */
	ErrorCheck((char*)"disegno");



	/*view second player*/
	glViewport(width / 2.0, 0, width / 2.0, height);
	glLoadIdentity();
	glPushMatrix();
	/* Definisce la posizione della luce 0 */
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	/* Stabilisce il punto di vista */
	ImpostaVistaSecondPlayer(alphax2, alphaz2);
	/* Disegna gli assi */
	drawAssi(50);
	/* Abilita l'illuminazione */
	glEnable(GL_LIGHTING);
	/* Emissione di default per Sfera e Toro */
	glMaterialfv(GL_FRONT, GL_EMISSION, Nero);
	/* Sfera: materiale rosso brillante, con highlight verde */
	glMaterialfv(GL_FRONT, GL_AMBIENT, RossoTenue);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Rosso);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Verde);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);
	glPushMatrix();
	glTranslatef(xPal, yPal, zPal);
	glutSolidSphere(0.4, 31, 31);
	glPopMatrix();
	/* Cubo: materiale giallo matto che emette luce.
	 * La shininess e' la stessa del toro (non e' impostata diversamente) */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
	glMaterialfv(GL_FRONT, GL_EMISSION, GialloTenue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Nero);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(21, 9, 9);
	glutWireCube(1);
	glPopMatrix();
	/* Disabilita l'illuminazione */
	glDisable(GL_LIGHTING);
	glPopMatrix();
	/* Controllo errori */
	ErrorCheck((char*)"disegno");
	glutSwapBuffers();
}



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(X_POS, Y_POS);
	glutInitWindowSize(width, height);
	glutCreateWindow("Pong [3D]");
	init();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutTimerFunc(50, moveball, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();
	return(0);
}

