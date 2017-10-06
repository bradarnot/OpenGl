/*
 *  Homework 3
 *
 *  Author: Bradley Arnot
 *
 *  Key bindings:
 *  arrows     Change view angle
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=0;         //  Azimuth of view angle
int ph=90;         //  Elevation of view angle
int axes = 1;     //  Show axes
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
int mode=0;       //  Projection mode


double dt = 0.25; // First Person movement speed
int fth = 0;      // First Person camera rotation
int fph = 0;
double Ex = 0;    // First Person camera coordiantes
double Ey = 0;
double Ez = 0;


//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,0,0);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}


/**
*  Draws a cylinder
*  at x, y, z
*  with radius r
*  with length l
*/

static void cylinder(double x,double y,double z,double r, double l)
{
   const int d=5;
   int th;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   //glScaled(r,r,r);

   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
      glColor3f(Sin(th)*Sin(th), 0, Cos(th)*Cos(th));
      glVertex3d(r*Cos(th), -l/2, r*Sin(th));
      glVertex3d(r*Cos(th), l/2, r*Sin(th));
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

/**
*  Draws a cone
*  at x, y, z
*  with radius r
*  with length l
*/

static void cone(double x,double y,double z,double r, double l)
{
   const int d=5;
   int th;

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);

   // Cone face
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=d)
   {
      glColor3f(Sin(th)*Sin(th), 0, Cos(th)*Cos(th));
      glVertex3d(r*Cos(th), -l/2, r*Sin(th));
      glVertex3d(0, l/2, 0);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

/**
*  Draws a circle
*  at x, y, z
*  with radius r
*/

static void circle(double x, double y, double z, double r)
{
   const int d=5;
   int th;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Circle fan
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 0);
   for (th=0;th<=360;th+=d)
   {
      glColor3f(Sin(th)*Sin(th), Cos(th)*Sin(th), Cos(th)*Cos(th));
      glVertex3d(Cos(th), 0, Sin(th));
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}


/**
*   Draws a solid 3D ring
*   at x, y, z
*   with outer radius r
*   with inner radius r-thickness
*   with depth l
*/

static void ring(double x, double y, double z, double r, double l, double thickness) {
   const double d = 5;
   double th;

   cylinder(x, y, z, r, l);
   cylinder(x, y, z, r-thickness, l);

   //  top ring
   glBegin(GL_QUAD_STRIP);
   glVertex3d(x+r, y+(l/2), z);
   glVertex3d(x+r-thickness, y+(l/2), z);
   for (th=0;th<=360;th+=d)
   {
      glColor3f(Sin(th)*Sin(th), Cos(th)*Sin(th), Cos(th)*Cos(th));
      glVertex3d(r*Cos(th), y+(l/2), z+r*Sin(th));
      glVertex3d((r-thickness)*Cos(th), y+(l/2), z+(r-thickness)*Sin(th));
   }
   glEnd();

   //  bottom ring
   glBegin(GL_QUAD_STRIP);
   glVertex3d(x+r, y+(-l/2), z);
   glVertex3d(x+r-thickness, y+(-l/2), z);
   for (th=0;th<=360;th+=d)
   {
      glColor3f(Sin(th)*Sin(th), Cos(th)*Sin(th), Cos(th)*Cos(th));
      glVertex3d(x+r*Cos(th), y+(-l/2), z+r*Sin(th));
      glVertex3d(x+(r-thickness)*Cos(th), y+(-l/2), z+(r-thickness)*Sin(th));
   }
   glEnd();
}


/**
*  Draws a trident
*  at position x, y, z
*  with scale s
*  with rotation th over x and ph over y
*/
static void trident(double x, double y, double z, double s, double th, double ph) {
   const double rstaff = 0.1;
   const double lstaff = 3;
   const double lcone = 0.4;
   const double ringthick = 0.02;
   const double dring = 0.05;
   const double lbase = 0.5;
   const double tbase = 0.05;
   const double lprong = 1;
   const double oprong = 0.05;

   glTranslated(x, y, z);
   glRotated(ph, 0, 1, 0);
   glRotated(th, 1, 0, 0);
   glScaled(s,s,s);


   //  Save transformation
   glPushMatrix();

   //staff
   cylinder(0, 0, 0, rstaff, lstaff);
   circle(0, -(lstaff/2), 0, rstaff);
   cone(0, (lstaff/2)+(lcone/2), 0, rstaff, lcone);
   ring(0, (lstaff/2), 0, rstaff+ringthick, dring, ringthick);

   //base
   cube(0, (lstaff/2)+ringthick+tbase,0, lbase, tbase, tbase,0);

   //middle prong
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, (lstaff/2)+lprong, 0);
   glColor3f(0,0,1);
   glVertex3d(0,(lstaff/2),rstaff);
   glColor3f(0,1,0);
   glVertex3d(-rstaff,(lstaff/2),0);
   glColor3f(1,0,0);
   glVertex3d(0,(lstaff/2),-rstaff);
   glColor3f(1,0,1);
   glVertex3d(rstaff,(lstaff/2),0);
   glColor3f(0,0,1);
   glVertex3d(0,(lstaff/2),rstaff);
   glEnd();


   //side prongs
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d((lbase)+oprong, (lstaff/2)+lprong-0.1, 0);
   glColor3f(0,0,1);
   glVertex3d((lbase),(lstaff/2)+2*tbase,-tbase);
   glColor3f(0,1,0);
   glVertex3d((lbase)-2*tbase,(lstaff/2)+2*tbase,-tbase);
   glColor3f(1,0,0);
   glVertex3d((lbase)-2*tbase,(lstaff/2)+2*tbase,tbase);
   glColor3f(1,0,1);
   glVertex3d((lbase),(lstaff/2)+2*tbase,tbase);
   glColor3f(0,0,1);
   glVertex3d((lbase),(lstaff/2)+2*tbase,-tbase);
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   glVertex3d((-lbase)-oprong, (lstaff/2)+lprong-0.1, 0);
   glColor3f(0,0,1);
   glVertex3d((-lbase),(lstaff/2)+2*tbase,-tbase);
   glColor3f(0,1,0);
   glVertex3d((-lbase)+2*tbase,(lstaff/2)+2*tbase,-tbase);
   glColor3f(1,0,0);
   glVertex3d((-lbase)+2*tbase,(lstaff/2)+2*tbase,tbase);
   glColor3f(1,0,1);
   glVertex3d((-lbase),(lstaff/2)+2*tbase,tbase);
   glColor3f(0,0,1);
   glVertex3d((-lbase),(lstaff/2)+2*tbase,-tbase);
   glEnd();

   //  Undo transformations
   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   if (mode)
   {
      double Cx, Cy, Cz;  
      if(mode==2) {
         // First Person navigation
         Cx = (Ex)+Sin(fth)*Cos(fph);
         Cy = (Ey)+         Sin(fph);
         Cz = (Ez)-Cos(fth)*Cos(fph);
         gluLookAt(Ex,Ey,Ez, Cx,Cy,Cz, 0,Cos(ph),0);
      } else {
         // Perspective navigation
         double PEx = -2*dim*Sin(th)*Cos(ph);
         double PEy = +2*dim        *Sin(ph);
         double PEz = +2*dim*Cos(th)*Cos(ph);

         Cx = 0;
         Cy = 0;
         Cz = 0;
         gluLookAt(PEx,PEy,PEz, Cx,Cy,Cz, 0,Cos(ph),0);
      }
      //gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   } else {
      // Set view angles in orthoganol mode
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   
   trident(0,0,0,1,0,0);
   ring(0,0.4,0,1,0.1,0.1);
   ring(0,0.2,0,1,0.1,0.1);
   ring(0,0,0,1,0.1,0.1);
   ring(0,-0.2,0,1,0.1,0.1);
   ring(0,-0.4,0,1,0.1,0.1);

   ring(0,0.4,0,0.75,0.1,0.1);
   ring(0,0.2,0,0.75,0.1,0.1);
   ring(0,0,0,0.75,0.1,0.1);
   ring(0,-0.2,0,0.75,0.1,0.1);
   ring(0,-0.4,0,0.75,0.1,0.1);

   ring(0,0.4,0,0.5,0.1,0.1);
   ring(0,0.2,0,0.5,0.1,0.1);
   ring(0,0,0,0.5,0.1,0.1);
   ring(0,-0.2,0,0.5,0.1,0.1);
   ring(0,-0.4,0,0.5,0.1,0.1);

   ring(0,0.4,0,0.25,0.1,0.1);
   ring(0,0.2,0,0.25,0.1,0.1);
   ring(0,0,0,0.25,0.1,0.1);
   ring(0,-0.2,0,0.25,0.1,0.1);
   ring(0,-0.4,0,0.25,0.1,0.1);

   // Inner Cubes
   double cubesize = 0.5;
   cube(-2,0,-2, cubesize,cubesize,cubesize, 0);
   cube(-2,0,2, cubesize,cubesize,cubesize, 0);
   cube(2,0,-2, cubesize,cubesize,cubesize, 0);
   cube(2,0,2, cubesize,cubesize,cubesize, 0);

   cube(-2,1,0, cubesize,cubesize,cubesize, 0);
   cube(0,1,2, cubesize,cubesize,cubesize, 0);
   cube(0,1,-2, cubesize,cubesize,cubesize, 0);
   cube(2,1,0, cubesize,cubesize,cubesize, 0);

   cube(-2,-1,0, cubesize,cubesize,cubesize, 0);
   cube(0,-1,2, cubesize,cubesize,cubesize, 0);
   cube(0,-1,-2, cubesize,cubesize,cubesize, 0);
   cube(2,-1,0, cubesize,cubesize,cubesize, 0);
   
   //  White
   glColor3f(1,1,1);

   double len = 1.0;

   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
   if(mode==2)
      Print("Angle:%d,%d  Projection:%s",fth,fph,mode?((mode==2)?"First Person":"Perceptive"):"Orthogonal");
   else
      Print("Angle:%d,%d  Projection:%s",th,ph,mode?((mode==2)?"First Person":"Perceptive"):"Orthogonal");
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT) {
      if (mode==2) {
         //th += 5;
         Ex += dt*Cos(fth);
         Ez += dt*Sin(fth);
      } else {
         th += 5;
      }
   }
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT) {
      if (mode==2) {
         //th -= 5;
         Ex -= dt*Cos(fth);
         Ez -= dt*Sin(fth);
      } else {
         th -= 5;
      }
   }
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP) {
      if (mode==2) {
         //ph += 5;
         Ex += dt*Sin(fth);
         Ez -= dt*Cos(fth);
      } else {
         ph += 5;
      }
   }
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN) {
      if (mode==2) {
         //ph -= 5;
         Ex -= dt*Sin(fth);
         Ez += dt*Cos(fth);
      } else {
         ph -= 5;
      }
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;


   

   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0') {
      th = 0;
      if(mode == 2)
         ph = 0;
      else
         ph = 90;
   }
   else if (ch == 'A')
      axes = 1-axes;
   else if (ch == 'm' || ch == 'M') {
      mode = (mode+1)%3;
      if(mode==2){
         ph=0;
         th=0;
      } else {
         ph=90;
         th=0;
      }
   }
   else if (ch == 'n' || ch == 'N') {
      mode = (mode+2)%3;
      if(mode==2){
         ph=0;
         th=0;
      } else {
         ph=90;
         th=0;
      }
   }
   else if (ch == 'w') {
      fph += 2;
   }
   else if (ch == 's') {
      fph -= 2;
   }
   else if (ch == 'd') {
      fth += 2;
   }
   else if (ch == 'a') {
      fth -= 2;
   }
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if ((ch == '+' || ch == '=') && ch<179)
      fov++;
   //else if (ch == 'w' || ch == 'W')
   //   forward += 1;


   fth %= 360;
   fph %= 360;

   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   const double dim=2.5;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();

   Project();
}


/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Assignment 4: Bradley Arnot");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   Ez = 2*dim;
   glutMainLoop();
   return 0;
}
