/* Esempio - Posizione delle luci e matrice di ModelView */
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define KEY_ESC	27

GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f };
int mode=1;

//Parametri per gluPerspective
static GLfloat fovy=60, aspect=1, nearClip=3, farClip=40; 
//Parametri per il punto di vista
static GLfloat  distance=20.5, alphax=0.0, alphaz=0.0;
static GLdouble	xStart = 0.0, yStart = 0.0;

//Angolo di rotazione per la luce
static GLfloat lightAngle = 0.0f;


class Pallina
{
  private:
          GLfloat xPal,yPal,zPal;  //coordiate pallina 
          GLfloat speedX;
          GLfloat speedY; 
          GLfloat speedZ;
          //qua aggiungere la texture quando capirte come cazzi si fa 
  public: 
          Pallina()
          {
            xPal = 0.0f;
            yPal = 0.0f;
            zPal = 0.0f;
            speedX = 0.2f;
            speedY = 0.2f;
            speedZ = 0.2f;
            
          }
          Pallina(GLfloat x , GLfloat y, GLfloat z, GLfloat sx, GLfloat sy, GLfloat sz)
          {
            xPal = x;
            yPal = y;
            zPal = z;
            speedX = sx;
            speedY = sy;
            speedZ = sz;

          }
          Pallina(GLfloat x , GLfloat y, GLfloat z)
          {
            xPal = x;
            yPal = y;
            zPal = z;
            speedX = 0.2f;
            speedY = 0.2f;
            speedZ = 0.2f;
          }

          void setXPal(int x) {xPal = x;}
          GLfloat getXPal() {return xPal;}
          void setYPal(int y) {yPal = y;}
          GLfloat getYPal() {return yPal;}
          void setZPal(int z) {zPal = z;}
          GLfloat getZPal() {return zPal;}

          static void moveBall(int);
          void moveball(int);

};


class Player
{
    private:
            GLfloat x;
            GLfloat y;
            GLfloat z;

            GLfloat dim; 

            GLint score;
    public:
            Player(int id, GLfloat d ) 
            {
                if (id == 1)
                {
                    x = -10;
                    y = 2;
                    z = 2;
                    dim = d;
                }
                else{
                    x = 10;
                    y = 2;
                    z = 2;
                    dim = d;

                }
            }
            int getZ(){return z;}
            int getY(){return y;}
            
            void encreaseY() {if(y+2 <= 4)  y = y+1;}
            void decreaseY() {if(y >= -2) y = y-1;}
            void encreaseZ() {if(z+2 <= 4)  z = z+1;}
            void decreaseZ() {if(z >= -2)  z= z-1;}
            GLfloat getDim() {return dim;}
            void encreseScore() {score++;}
            int getScore() {return score;}
            void drawPlayer();


};
Player player1(1,3);
Player player2(2,3);
Pallina* ball= new Pallina(0,0,0);

void Pallina::moveBall(int i)
{
  ball->moveball(i);
}


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


void Player::drawPlayer()
{
    glMaterialfv( GL_FRONT,GL_AMBIENT_AND_DIFFUSE, Nero );
    glMaterialfv( GL_FRONT, GL_EMISSION, Verde );
		glMaterialfv( GL_FRONT, GL_SPECULAR, Nero );
		glPushMatrix();
			glTranslatef(x,y,z);
            glScalef (0.5,3,3);
			glutWireCube( 1);
		glPopMatrix();
}


//Tasti del mouse
GLvoid mouse( GLint button, GLint state, GLint x, GLint y )
{

	if (state == GLUT_DOWN) 
  {
    if (button ==  GLUT_LEFT_BUTTON)
    {
		    /* Aggiorna la posizione del mouse salvata */
		    xStart = x;
		    yStart = y;
    
    }
	} 
 }

//Movimento del mouse
GLvoid motion( GLint x, GLint y )
{
      /* Aggiorna il punto di vista */
		  alphax -= (GLdouble) (y - yStart);
		  alphaz += (GLdouble) (x - xStart);
	/* Aggiorna la posizione del mouse salvata */
	xStart = x;
	yStart = y;

	glutPostRedisplay();
}


//Disegna un sistema di assi cartesiane con origine in (0,0,0)
GLvoid drawAssi( GLvoid) 
{ 
  GLfloat lato=1.0;
  GLfloat blu[]    = { 0.0f, 0.0f, 1.0f};
  GLfloat rosso[]  = { 1.0f, 0.0f, 0.0f};
  GLfloat verde[]  = { 0.0f, 1.0f, 0.0f};

  glBegin (GL_LINES); 
    glColor3fv(rosso); glVertex3f(0,0,0); glVertex3f(lato,0,0); 
    glColor3fv(verde); glVertex3f(0,0,0); glVertex3f(0,lato,0); 
    glColor3fv(blu);   glVertex3f(0,0,0); glVertex3f(0,0,lato); 
  glEnd();
}

//inizializzazioni
GLvoid init( GLvoid )
{
  /* Stabilisce il colore dello sfondo */
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

  /* Uso depth buffer */
	glEnable( GL_DEPTH_TEST );

  /* Matrice di proiezione */
  glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( fovy, 1, nearClip, farClip );
	/* Matrice di ModelView */
  glMatrixMode( GL_MODELVIEW );

  /* Abilitazione luce 0 */
	glEnable( GL_LIGHT0 );

  /* Normalizzazione delle normali */
  glEnable(GL_NORMALIZE);
}

//Callback per la tastiera 
GLvoid inputKey( GLubyte key, GLint x, GLint y ) 
{ 
   switch (key) 
   { 
      
      case KEY_ESC: exit(0); 
      case 'a': player1.encreaseY(); break;
      case 'd': player1.decreaseY(); break;
      case 'w': player1.encreaseZ(); break;
      case 's': player1.decreaseZ(); break;
   }
   glutPostRedisplay(); 
} 


void specialKeyInput(int key, int _x, int _y) 
{
    switch (key) 
    {
        case GLUT_KEY_UP:   player2.encreaseZ(); break;
        case GLUT_KEY_DOWN: player2.decreaseZ(); break;
        case GLUT_KEY_LEFT: player2.decreaseY(); break;
        case GLUT_KEY_RIGHT: player2.encreaseY(); break;
        default: break;
    } 
    //glutPostRedisplay();
}



//Impostazione del punto di vista
void ImpostaVista( GLfloat alphax, GLfloat alphaz)
{
  glTranslatef( 0.0f, 0.0f, -distance);
	glRotatef( -alphax, 1.0f, 0.0f, 0.0f);
	glRotatef( -alphaz, 0.0f, 0.0f, 1.0f);
}





void Pallina::moveball(int i)   // faccio check collision con bordi e con i player
{

    glutTimerFunc(50,ball->moveBall,0);
    
    xPal=xPal+speedX;  
    yPal=yPal+speedY;
    zPal=zPal+speedZ;

    //collisione con i player
    if(xPal >= 9 || xPal <= -9) 
    {
        if( player2.getY()-2 < yPal && player2.getY()+2 > yPal && 
            player2.getZ()-2 < zPal && player2.getZ()+2> zPal
            ||
            (player1.getY()-2 < yPal && player1.getY()+2 > yPal && 
            player1.getZ()-2 < zPal && player1.getZ()+2> zPal
             )
          )
                speedX = -speedX;
        else
        {
           //aumento score
            xPal=0;
            yPal=0;
            zPal=0;
        }
       
    }

    //collisione con il campo 
    if(yPal >= 4 || yPal <= -4) {speedY = -speedY;}
    if(zPal >= 4 || zPal <= -4) {speedZ = -speedZ;}

    glutPostRedisplay();
}










// Callback di display
GLvoid drawScene( GLvoid )
{

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glPushMatrix();

    //Posizione luce legata al punto di vista:
    glLightfv(GL_LIGHT0,GL_POSITION,lightPosition);
    ImpostaVista(alphax,alphaz);    
    drawAssi();
             
    glEnable( GL_LIGHTING );

    /* Emissione di default per Sfera e Toro */
		glMaterialfv( GL_FRONT, GL_EMISSION, Nero );

		/* Sfera: materiale rosso brillante, con highlight verde */
		glMaterialfv( GL_FRONT, GL_AMBIENT,  RossoTenue );
		glMaterialfv( GL_FRONT, GL_DIFFUSE,  Rosso );
		glMaterialfv( GL_FRONT, GL_SPECULAR, Verde );
		glMaterialf(  GL_FRONT, GL_SHININESS, 128.0f ); 

		glPushMatrix();
			glTranslatef(ball->getXPal(), ball->getYPal(), ball->getZPal() );
			glutSolidSphere( 0.4, 31, 31 ); //ball.draw
		glPopMatrix();


		/* Cubo: materiale giallo matto che emette luce.
		 * La shininess e' la stessa del toro (non e' impostata diversamente) */
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Giallo);
		glMaterialfv( GL_FRONT, GL_EMISSION, GialloTenue );
		glMaterialfv( GL_FRONT, GL_SPECULAR, Nero );
		glPushMatrix();
			glTranslatef( 0.0f, 0.0f, 0.0f );
            glScalef (21,9,9);
			glutWireCube( 1);
		glPopMatrix();
    
        player2.drawPlayer();
        player1.drawPlayer();

		/* Disabilita l'illuminazione */
    glDisable( GL_LIGHTING );

	glPopMatrix();

	glutSwapBuffers();
}

int main( int argc, char *argv[] )
{
	glutInit( &argc, argv );

	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
  glutInitWindowPosition( 800, 600 );
	glutInitWindowSize( 1000, 800);
	glutCreateWindow( "Pong3d" );
	init();
	glutKeyboardFunc( inputKey );
	glutSpecialFunc(specialKeyInput);

	glutDisplayFunc( drawScene );
    glutTimerFunc(50,ball->moveBall,0);

 	glutMouseFunc( mouse );
	glutMotionFunc( motion );

	glutMainLoop();
  return(0);
}

