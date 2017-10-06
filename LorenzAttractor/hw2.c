/*
 *  Assignment 2: Bradley Arnot
 *
 *  Displays lorenz attractor
 *
 *  Key bindings:
 *  s,r,b  Set which parameter to change
 *  +/-    Increase/decrease parameter
 *  arrows Change view angle
 *  ESC    Exit
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

#ifndef max
   #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

//  Globals
// lorenz parameters
double s  = 10;
double b  = 2.6666;
double r  = 28;
int th=0;       // Azimuth of view angle
int ph=0;       // Elevation of view angle
double dim=50;   // Dimension of orthogonal box
int mode=0;    // Tracks which parameter is being changed
char modec = 's';    //tracks the parameter name that is being changed
double t = 0;


/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
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
 *  Display the scene
 */
void display()
{
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();


   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   
   //glEnable(GL_DEPTH_TEST);
   //  Draw 10 pixel yellow points
   //glColor3f(0,0,1);
   glPointSize(10);
   glBegin(GL_LINE_STRIP);

   

   int i;
   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;

   // Stores max values of each coordinate component
   double mx = 0;
   double my = 0;
   double mz = 0;
   /*  Time step  */
   double dt = 0.001;

   glVertex3d(x,y,z);
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<50000;i++)
   {
      //calculate speed and new coordinate point
      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      t += dt;
      glColor3f(abs(sin(t)),abs(cos(t)),abs(tan(t)));
      glVertex3d(x,y,z);
      mx = max(mx, x);
      my = max(my, y);
      mz = max(mz, z);
   }

   // Change orthogonal box dimension based on how big lorenz attractor is
   dim = max(mx, max(my, mz)) + 2;

   glEnd();
   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(20,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,20,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,20);
   glEnd();
   //  Label axes
   glRasterPos3d(20,0,0);
   Print("X");
   glRasterPos3d(0,20,0);
   Print("Y");
   glRasterPos3d(0,0,20);
   Print("Z");
   //  Display parameters
   glWindowPos2i(5,5);
   double val = 0;
   if (mode == 0)
      val = s;
   if (mode == 1)
      val = b;
   if (mode == 2)
      val = r;
   Print("View Angle=%d,%d  %c=%.2f",th,ph,modec,val);
   //  Flush and swap
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   // = or + to increase parameter value
   else if (ch == '+' || ch == '=')
   {
      if (mode == 0)
         s += 0.1;
      else if (mode == 1)
         b += 0.1;
      else if (mode == 2)
         r += 0.1;
   }
   // - to decrease parameter value
   else if (ch == '-')
   {
      if (mode == 0)
         s -= 0.1;
      else if (mode == 1)
         b -= 0.1;
      else if (mode == 2)
         r -= 0.1;
   // s to start changing value of s
   } else if (ch == 's') {
      mode = 0;
      modec = ch;
   }
   // b to start changing value of b
   else if (ch == 'b') {
      mode = 1;
      modec = ch;
   }
   // r to start changing value of r
   else if (ch == 'r') {
      mode = 2;
      modec = ch;
   }
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Assignment 2: Bradley Arnot");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
  //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
