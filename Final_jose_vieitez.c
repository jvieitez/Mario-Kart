/*
 *  Thunderbirds
 *
 *  Draw complex objects from vertex list.
 *  Flight simulation demonstration.
 *
 *  Key bindings:
 *  l/L        Toggle lighting on/off
 *  f/F        Toggle flight/static mode
 *  KP 4/6     Roll (static mode)
 *  KP 2/8     Pitch (static mode)
 *  KP 1/3     Yaw (static mode)
 *  <>         Power (static mode)
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx229.h"
#include "F16.h"
#include <math.h>
#import "./Mario_Kart_Jose_Ethan_final.h"
// #include "mario_kart.h"
int    fly=0;    //  Animated flight
int    axes=0;   //  Display axes
int    move=0;   //  Light movement
int    light=1;  //  Lighting
int    F16[3];   //  F16 display list
int    th=90;     //  Azimuth of view angle
int    ph=15;     //  Elevation of view angle
int    zh=0;     //  Azimuth of light
double Yl=8;     //  Elevation of light
double roll=0;   //  Roll angle
double pitch=0;  //  Pitch angle
double yaw=0;    //  Yaw angle
int    pwr=100;  //  Power setting (%)
int    fov=55;   //  Field of view (for perspective)
double asp=1;    //  Aspect ratio
double dim=9.2;  //  Size of world orig 6.5
int    box=1;    //  Draw sky
int    sky[5];   //  Sky textures
double X  = 0;   //  Location
double Y  = 0;   //  Location
double Z  = 0;   //  Location
double Dx = 1;   //  Direction
double Dy = 0;   //  Direction
double Dz = 0;   //  Direction
double Sx = 1;   //  Sideways
double Sy = 0;   //  Sideways
double Sz = 0;   //  Sideways
double Ex = 1;   //  Eye
double Ey = 1;   //  Eye
double Ez = 1;   //  Eye
double Ox = 0;   //  LookAt, make equal to x, y, z because that is the object always, same as "center"
double Oy = 0;   //  LookAt
double Oz = 0;   //  LookAt
double Ux = 1;   //  Up keeping it as 0,1,0 is normal, means up is "up" 
double Uy = 0;   //  Up
double Uz = 0;   //  Up
double driveZ = 0;
double driveX = 0;
double pitX = 0;  // Angles for the cockpit view
double pitY = 0;  // Angles for the cockpit view
double pitZ = 0;  // Angles for the cockpit view
int pit = 0;      // go into cockpit view
// float Position[4];  //= {5*Cos(zh),Yl,5*Sin(zh),1};

// for shadows
#define Dfloor  8
#define Yfloor 0.1
float N[] = {0, -1, 0}; // Normal vector for the plane
float E[] = {0, Yfloor, 0 }; // Point of the plane


// extern void DrawModel();
// PFNGLLOCKARRAYSEXTPROC     glLockArraysEXT   = NULL;
// PFNGLUNLOCKARRAYSEXTPROC   glUnlockArraysEXT = NULL;


/* Multiply the current ModelView-Matrix with a shadow-projetion matrix.
 *
 * L is the position of the light source
 * E is a point within the plane on which the shadow is to be projected.  
 * N is the normal vector of the plane.
 *
 * Everything that is drawn after this call is "squashed" down to the plane.
 */
void ShadowProjection(float L[4], float E[4], float N[4])
{
   float mat[16];
   float e = E[0]*N[0] + E[1]*N[1] + E[2]*N[2];
   float l = L[0]*N[0] + L[1]*N[1] + L[2]*N[2];
   float c = e - l;
   //  Create the matrix.
   mat[0] = N[0]*L[0]+c; mat[4] = N[1]*L[0];   mat[8]  = N[2]*L[0];   mat[12] = -e*L[0];
   mat[1] = N[0]*L[1];   mat[5] = N[1]*L[1]+c; mat[9]  = N[2]*L[1];   mat[13] = -e*L[1];
   mat[2] = N[0]*L[2];   mat[6] = N[1]*L[2];   mat[10] = N[2]*L[2]+c; mat[14] = -e*L[2];
   mat[3] = N[0];        mat[7] = N[1];        mat[11] = N[2];        mat[15] = -l;
   //  Multiply modelview matrix
   glMultMatrixf(mat);
}
/*
 *  Draw Set of Facets
 */
static void Facets(int k)
{
   int i,j;
   glBegin(GL_TRIANGLES);
   for (i=nFacet[k];i<nFacet[k+1];i++)
      for (j=0;j<3;j++)
      {
         glTexCoord2fv(Texture[Facet[i][2][j]]);
         glNormal3fv(Normal[Facet[i][1][j]]);
         glVertex3fv(Vertex[Facet[i][0][j]]);
      }
   glEnd();
}

/*
 *  Compile F16 display list
 */
static void CompileF16(void)
{
   float black[] = {0,0,0,1};
   int tex[2];

   //  Load textures
   tex[0] = LoadTexBMP("F16s.bmp");
   tex[1] = LoadTexBMP("F16t.bmp");

   //  Body list
   F16[0] = glGenLists(1);
   glNewList(F16[0],GL_COMPILE);
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,tex[0]);
   Facets(0);
   glDisable(GL_TEXTURE_2D);
   glEndList();

   //  Engine list
   F16[1] = glGenLists(1);
   glNewList(F16[1],GL_COMPILE);
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,tex[0]);
   Facets(1);
   glDisable(GL_TEXTURE_2D);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   glEndList();

   //  Canopy list
   F16[2] = glGenLists(1);
   glNewList(F16[2],GL_COMPILE);
   glColor4f(1,1,1,0.33);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,tex[1]);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(0);
   Facets(2);
   glDepthMask(1);
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glEndList();
}

/*
 *  Draw Flight
 */
static void DrawFlight(double x0,double y0,double z0,
                       double dx,double dy,double dz,
                       double ux,double uy,double uz)
{
   int i,k;
   //  Position of members
   double X[] = {-1.0,+1.0,+0.0,+0.0};
   double Y[] = {-0.3,+0.3,+0.0,+0.0};
   double Z[] = {+0.0,+0.0,-1.5,+1.5};
   //  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double M[16] = {X0,Y0,Z0,0 , X1,Y1,Z1,0 , X2,Y2,Z2,0 , 0,0,0,1};

   //  Save current transforms
   glPushMatrix();
   //  Offset and rotate
   glTranslated(x0,y0,z0);
   glMultMatrixd(M);
   //  k=0  draw body
   //  k=1  draw engine
   //  k=2  draw canopy
   for (k=0;k<3;k++)
      //  Draw 1st F16 which is above ground
      for (i=1;i<2;i++)
      {
         glPushMatrix();
         glTranslated(X[i],Y[i],Z[i]);
         glRotated(yaw,0,1,0);
         glRotated(pitch,0,0,1);
         glRotated(roll,1,0,0);
         if (k==1)
    {
            float power[] = {0.01*pwr,0.01*pwr,0.01*pwr,1};
            glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,power);
    }
         glCallList(F16[k]);
         glPopMatrix();
      }
   //  Undo transformations
   glPopMatrix();
}

/* 
 *  Draw sky box
 */
static void Sky(double D)
{
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);

   //  Sides
   glBindTexture(GL_TEXTURE_2D,sky[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,sky[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(1.0,1); glVertex3f(-D,0,+D);
   glTexCoord2f(0.5,1); glVertex3f(+D,0,+D); //**
   glTexCoord2f(0.5,0); glVertex3f(+D,0,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,0,-D);
   glEnd();

   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius r
 */
static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,0);
   glutSolidSphere(1.0,16,16);
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void rect(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   // float Emission2[]  = {0.0,1.0,1.0,1.0};
   // glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   // glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);//Emission2);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,sky[2]);
   //  Front
   glColor3f(1,0,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   glColor3f(0,0,1);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glColor3f(1,1,0);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glColor3f(0,1,0);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glColor3f(0,1,1);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glColor3f(1,0,1);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}
static void quest(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   // float Emission2[]  = {0.0,1.0,1.0,1.0};
   // glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   // glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);//Emission2);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,sky[3]);
   //  Front
   glColor3f(.8,.8,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   glColor3f(.8,.8,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glColor3f(.8,.8,0);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glColor3f(.8,.8,0);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glColor3f(.8,.8,0);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glColor3f(.8,.8,0);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}
static void shadowquest(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   // float Emission2[]  = {0.0,1.0,1.0,1.0};
   // glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   // glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);//Emission2);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,sky[3]);
   //  Front
   glColor3f(0,0,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   glColor3f(0,0,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glColor3f(0,0,0);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glColor3f(0,0,0);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glColor3f(0,0,0);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glColor3f(0,0,0);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}
// kart drawing
static void kart(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   // float Emission2[]  = {0.0,1.0,1.0,1.0};
   // glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   // glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);//Emission2);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable textures
   // glEnable(GL_TEXTURE_2D); //*
   // glColor3f(1,1,1); //*
   // glBindTexture(GL_TEXTURE_2D,sky[2]); //*
   glVertexPointer(3, GL_FLOAT, 0, Mario_Kart_Jose_Ethan_finalVerts);
   glNormalPointer(GL_FLOAT, 0, Mario_Kart_Jose_Ethan_finalNormals); 
   glTexCoordPointer(2, GL_FLOAT, 0, Mario_Kart_Jose_Ethan_finalTexCoords);
   
   glDrawArrays(GL_TRIANGLES, 0, Mario_Kart_Jose_Ethan_finalNumVerts);
   //  Undo transformations and textures
   glPopMatrix();
   // glDisable(GL_TEXTURE_2D); //*
}
// Draw Boxes making up barrier 
void hedges() 
{

   // variables: at [(x,y,z) dimentions,(dx,dy,dz)], rotated th about the y axis
   // row on tech 
   for (int x=0;x<25;x++){
      rect(-21.8+1.70*x,0,-2.1,  .85,.6,.60, 0);
   }
   // collision with first barrier
   if ((-21.8+(1.70*25) >=X) && (X>=-21.8) && (-0.9>=Z) && (Z>=-1.8)) { 
         driveX += 1;
         driveZ += 1;
   }
   if ((-21.8+(1.70*25) >=X) && (X>=-21.8) && (-2.9>=Z) && (Z>=-3.4)) {
         driveX -= 1;
         driveZ -= 1;
   }




   // far right bottom right, vertical
   for (int x=0;x<5;x++){
      rect(-16.1-1.70*x,0,10.8,  .85,.6,.60, 0);
   }
   if ((-16.2-(1.70*4) <=X) && (X<=-16.2) && (11.2>=Z) && (Z>=9)) { 
         driveX += 1;
         driveZ += 1;
   }
   // struct box {
   //    double X1;
   //    double X2;
   //    double Z1;
   //    double Z2;
   // }
   // box CrashCoords[5];

   for (int x=0;x<25;x++){
      rect(3.5*dim-1.70*x,0,-9.5,  .85,.6,.60, 0);
   }
   // collision with second left barrier
   if ((3.5*dim >=X) && (X>=3.5*dim-(1.7*25)) && (-9.5>=Z) && (Z>=-11.0)) { // 8.9 10.1
         driveX -= 1;
         driveZ -= 1;
   }
   if ((3.5*dim >=X) && (X>=3.5*dim-(1.7*25)) && (-8.2>=Z) && (Z>=-9.15)) {
         driveX += 1;
         driveZ += 1;
   }
   // CrashCoords[0] =  {3.5*dim
   // sideways main runway



   // bottom right boxes horizontal
   for (int x=0;x<7;x++){
      rect(-15.8,0,-.9+1.70*x,  .85,.6,.60, 90);
   }
   if ((-17 <=X) && (X<=-15.9) && (-.7<=Z) && (Z<=11.3)) { // 8.9 10.1
         driveX += 1;
         driveZ += 1;
   }



   // long bottom wall
   for (int x=0;x<23;x++){
      rect((-.73*3.5*dim),0,(-.61*3.5*dim)+1.70*x,  .85,.6,.60, 90);
   }
   if ((-22.2 >=X) && (X>=-23.1) && ((-.61*3.5*dim)<=Z) && (Z<=(-.61*3.5*dim)+1.70*23)) { // 8.9 10.1
         driveX += 1;
         driveZ += 1;
   }
   if ((-23.7 >=X) && (X>=-25.4) && ((-.61*3.5*dim)<=Z) && (Z<=(-.61*3.5*dim)+1.70*23)) { // 8.9 10.1
         driveX -= 1;
         driveZ -= 1;
   }



   // far left wall vert
   for (int x=0;x<24;x++){
      rect(-23+1.70*x,0,-19.5,  .85,.6,.60, 0);
   }
   // collision
   if ((-23+1.70*24 >=X) && (X>=-24) && (-20.4<=Z) && (Z<=-19.4)) { 
         driveX -= 1;
         driveZ -= 1;
   }
   if ((-23+1.70*24 >=X) && (X>=-24) && (-20<=Z) && (Z<=-18.2)) { 
         driveX += 1;
         driveZ += 1;
   }

   // top right wall horizontal
   for (int x=0;x<16;x++){
      rect(-21.8+(1.70*25),0,-1.9+1.70*x,  .85,.6,.60, 90);
   }
   if ((18<=X) && (X<=20.0) && (-1.9<=Z) && (Z<=-1.9+1.70*16)) { // 8.9 10.1
         driveX -= 1;
         driveZ -= 1;
   }
   if ((20.9 >=X) && (X>=19.0) && (-1.9<=Z) && (Z<=-1.9+1.70*16)) { // 8.9 10.1
         driveX += 1;
         driveZ += 1;
   }
   // mid right wall horizontal
   for (int x=0;x<15;x++){
      rect(-1.3,0,(3.5*dim)-(1.70*x), .85,.6,.60, 90);
   }
   if ((-3.6<=X) && (X<=-1.85) && ((3.5*dim)-(1.70*15)<=Z) && (Z<=3.5*dim)) { // 8.9 10.1
         driveX -= 1;
         driveZ -= 1;
   }
   if ((-1.3 >=X) && (X>=-2.7) && ((3.5*dim)-(1.70*15)<=Z) && (Z<=3.5*dim)) { // 8.9 10.1
         driveX += 1;
         driveZ += 1;
   }
   // top right wall vertical
   for (int x=0;x<7;x++){
      rect(20.2-1.70*x,0,23.8,  .85,.6,.60, 0);
   }
   // collision
   if ((20.2>=X) && (X>=20.1-1.70*7) && (22.1<=Z) && (Z<=23.9)) { 
         driveX -= 1;
         driveZ -= 1;
   }
   if ((20.2>=X) && (X>=20.1-1.70*7) && (24.8>=Z) && (Z>=23.4)) { 
         driveX += 1;
         driveZ += 1;
   }
   
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=2.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClearColor(0,0.3,0.7,0);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   gluLookAt(Ex,Ey,Ez , Ox,Oy,Oz , Ux,Uy,Uz);

   //  Draw sky
   if (box) Sky(3.5*dim);

   //  Light switch
   // if (light)
   // {
   //  Translate intensity to color vectors
   float F = (light==2) ? 1 : 0.3;
   float Ambient[]   = {0.3*F,0.3*F,0.3*F,1};
   // float Diffuse[]   = {0.5*F,0.5*F,0.5*F,1};
   float Diffuse[]   = {1.0,1.0,1.0,1.0};
   float Specular[]  = {1.0*F,1.0*F,1.0*F,1};
   float white[]     = {1,1,1,1};
   //  Light direction
   float Position[]  = {10*Cos(zh),Yl,10*Sin(zh),1};
   //  Draw light position as ball (still no lighting here)
   ball(Position[0],Position[1],Position[2] , 0.1);


   //  Enable lighting with normalization
   glEnable(GL_LIGHTING);
   glEnable(GL_NORMALIZE);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   //  Enable light 0
   glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0f);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   // }
   // else
      // glDisable(GL_LIGHTING);

  
   hedges();
   DrawFlight(X,Y,Z , Dx,Dy,Dz , Ux,Uy,Uz);

   // collision detection
   // box edges
   if (X > 3.5*dim-2)
      driveX -= 1;
   if (X < -3.5*dim)
      driveX += 1;
   if (Z > 3.5*dim-2)
      driveZ -= 1;
   if (Z < -3.5*dim+2)
      driveZ += 1;
   // question mark boxes
   quest(-2.0,0.5,-.1,  .3,.3,.3, 0);
   quest(1.7,0.5,-.1,  .3,.3,.3, 0);
   quest(6.7,0.5,.1,  .3,.3,.3, 0);
   quest(-2.3,0.5,3.0,  .3,.3,.3, 0);
   quest(.8,0.5,1.1,  .3,.3,.3, 0);
   quest(-.4,0.5,2.9,  .3,.3,.3, 0);
   quest(3.6,0.5,2.9,  .3,.3,.3, 0);
   quest(4.8,0.5,1.1,  .3,.3,.3, 0);


 // ---
   // from shadows on other doc
   //  Save what is glEnabled
      // causes the stack overflow error. 
   // glPushAttrib(GL_ENABLE_BIT);
   // Shadows **

   glDisable(GL_LIGHTING);
   // glColor3f(0.3,0.3,0.3);
   //  Blended color
   // glEnable(GL_BLEND);
   // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glColor4f(0,0,0,0.4);
   //  Draw flattened scene
   glPushMatrix();
   ShadowProjection(Position,E,N);
   /*
   * This is where we insert what needs to have a shadow *****
   *
   *
   */
   // question mark boxes
   shadowquest(-2.0,0.5,-.1,  .3,.3,.3, 0);
   shadowquest(1.7,0.5,-.1,  .3,.3,.3, 0);
   shadowquest(6.7,0.5,.1,  .3,.3,.3, 0);
   shadowquest(-2.3,0.5,3.0,  .3,.3,.3, 0);
   shadowquest(.8,0.5,1.1,  .3,.3,.3, 0);
   shadowquest(-.4,0.5,2.9,  .3,.3,.3, 0);
   shadowquest(3.6,0.5,2.9,  .3,.3,.3, 0);
   shadowquest(4.8,0.5,1.1,  .3,.3,.3, 0);
    //  Draw flight of F16s
   DrawFlight(X,Y,Z , Dx,Dy,Dz , Ux,Uy,Uz);
   hedges();
   kart(X,Y+3,Z,3,3,3,0);
   // scene();
   glPopMatrix();
// ---
   /// END INSERT OF WHAT GETS A SHADOW

   //  Draw axes
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
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
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f  Roll %.1f Pitch %.1f Yaw %.1f X %.1f Y %.1f Z %.1f",th,ph,dim,roll,pitch,yaw,X,Y,Z);
   if (light) Print(" Light=%d,%.1f",zh,Yl);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine every 50ms
 */
void timer(int toggle)
{
   //  Toggle movement
   if (toggle>0)
      move = !move;
   //  Increment light position
   else // changes speed of sun
      zh = (zh+1)%360;
   //  Animate flight using Lorenz transform
   if (fly)
   {
      //  Lorenz integration parameters
      double dt = 0.003;
      double s = -1.7;
      double b = 2.66;
      double r = 50;
      //  Old vectors
      double D,Nx,Ny,Nz;
      double Dx0 = Dx;
      double Dy0 = Dy;
      double Dz0 = Dz;
      double Ux0 = Ux;
      double Uy0 = Uy;
      double Uz0 = Uz;
      //  Fix degenerate case
      if (X==0 && Y==0 && Z==0) Y = Z = 40;
      //  Update position
      // This is the equation that must change  ***
      Dx = s*(Y-X);
      // Dx = .005;
      Dy = X*(r-Z)-Y;
      // Dy = 0;
      Dz = X*Y - b*Z;
      // Dz = .05;
      // this seems to update X, Y, Z ***
      X += dt*Dx;
      Y += dt*Dy;
      Z += dt*Dz;
      //  Normalize DX
      D = sqrt(Dx*Dx+Dy*Dy+Dz*Dz);
      Dx /= D;
      Dy /= D;
      Dz /= D;
      //  Calculate sideways
      Sx  = Dy0*Dz-Dz0*Dy;
      Sy  = Dz0*Dx-Dx0*Dz;
      Sz  = Dx0*Dy-Dy0*Dx;
      //  Calculate Up
      Ux  = Dz*Sy - Dy*Sz;
      Uy  = Dx*Sz - Dz*Sx;
      Uz  = Dy*Sx - Dx*Sy;
      //  Normalize Up
      D = sqrt(Ux*Ux+Uy*Uy+Uz*Uz);
      Ux /= D;
      Uy /= D;
      Uz /= D;
      //  Eye and lookat position
      Ex = X-7*Dx;
      Ey = Y-7*Dy;
      Ez = Z-7*Dz;
      Ox = X;
      Oy = Y;
      Oz = Z;
      //  Next DX
      // This equation must also change  ***
      Nx = s*(Y-X);
      Ny = X*(r-Z)-Y;
      // Ny = 0;
      Nz = X*Y - b*Z;
      // Nx = .0005;
      // Nz = .0005;
      //  Pitch angle
      pitch = 180*acos(Dx*Dx0+Dy*Dy0+Dz*Dz0);
      //  Roll angle
      D = (Ux*Ux0+Uy*Uy0+Uz*Uz0) / (Dx*Dx0+Dy*Dy0+Dz*Dz0);
      if (D>1) D = 1;
      roll = (Nx*Sx+Ny*Sy+Nz*Sz>0?+1:-1)*960*acos(D);
      //  Yaw angle
      yaw = 0;
      //  Power setting (0-1)
      if (Dy>0.8)
         pwr = 100;
      else if (Dy>-0.2)
    pwr = 20+100*Dy;
      else
    pwr = 0;
   }
   //  Static Roll/Pitch/Yaw
   else
   {
      Ox = X = driveX; //-8.0;
      Oy = Y = 0; //-44;
      Oz = Z = driveZ;//28;

      if(pit){// determines the camera view while in cockpit mode
         pitX = X-.4*dim*Sin(yaw+90)*Cos(ph);
         pitY = Y+.5*dim        *Sin(ph);
         pitZ = Z-2*dim*Cos(yaw+90)*Cos(ph)/8;// Z +(yaw/10);

         // pitX = X - 14;
         // pitY = Y + 14;
         // pitZ = Z;
      } 
      else if (pit == 0){
         pitX = -2*dim*Sin(th)*Cos(ph);
         pitY = +2*dim        *Sin(ph);
         pitZ = +2*dim*Cos(th)*Cos(ph);
      }
      
      // Ex =  -2*dim*Sin(th)*Cos(ph);
      // Ey =  +2*dim        *Sin(ph);
      // Ez =  +2*dim*Cos(th)*Cos(ph);
      Ex = pitX;
      Ey = pitY;
      Ez = pitZ; 
      // Ox = Oy = Oz = 0; // orig
      // X = Y = Z = 0;  // orig
      Dx = 1; Dy = 0; Dz = 0;
      Ux = 0; Uy = 1; Uz = 0;
      // Ex = X-4;//7*Dx;
      // Ey = Y+4;//-7*Dy;
      // Ez = Z;//-7*Dz;



      // Ex = 1.0 -2*dim*Sin(th)*Cos(ph);
      // Ey = -5.6 +2*dim        *Sin(ph);
      // Ez = -2.0 +2*dim*Cos(th)*Cos(ph);
      
   }
   //  Set timer to go again
   if (move && toggle>=0) glutTimerFunc(50,timer,0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   // limit prevents angle from going underground
   else if (key == GLUT_KEY_UP && ph <= 170)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   // limit prevents angle from going underground
   else if (key == GLUT_KEY_DOWN && ph >= 10)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(fov,asp,dim);
   //  Update state
   timer(-1);
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
      roll = pitch = yaw = th = 0;
      ph = 5;}
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Fly
   else if (ch == 'f' || ch == 'F')
      dim = (fly = !fly) ? 30 : 3;
   //  Toggle skybox
   else if (ch == 'b' || ch == 'B')
      box = 1-box;
   //  Cycle light
   else if (ch == 'l')
      light = (light+1)%3;
   else if (ch == 'L')
      light = (light+2)%3;
   //  Toggle light movement
   else if (ch == 's' || ch == 'S')
      timer(1);
   //  Roll
   else if (ch == '4')
      roll -= 10;
   else if (ch == '6')
      roll += 10;
   //  Pitch
   else if (ch == '8')
      pitch -= 1;
   else if (ch == '2')
      pitch += 1;
   //  Yaw
   else if (ch == '1')
      yaw -= 30;
   else if (ch == '3')
      yaw += 30;
   //  Power
   else if (ch=='<' && pwr>0)
      pwr -= 10;
   else if (ch=='>' && pwr<100)
      pwr += 10;
   //  Light azimuth
   else if (ch=='[')
      zh -= 1;
   else if (ch==']')
      zh += 1;
   //  Light elevation
   else if (ch=='-')
      Yl -= 0.1;
   else if (ch=='+')
      Yl += 0.1;
   else if (ch == 'c' || ch == 'C'){
      pit = 1- pit;
   }
   // driver controls
   else if (ch == 'y' || ch == 'Y'){
      // turn if 0 to 90 deg, quadrant 4
      if (yaw >= 0 && yaw <= 90){
         driveX += 1-(yaw/90);
         driveZ -= (yaw)/90;
      }// quadrant 3
      else if (yaw > 90 && yaw <= 180){
         driveX -= ((yaw-90)/90);
         driveZ -= 1-(yaw-90)/90;
      } // quadrant 2
      else if (yaw > 180 && yaw <= 270){
         driveX -= 1-((yaw-180)/90);
         driveZ += (yaw-180)/90;
      }// quadrant 1
      else if (yaw > 270 && yaw <= 360){
         driveX += ((yaw-270)/90);
         driveZ += 1-(yaw-270)/90;
      }// turn if 0 to -90 deg, quadrant 1
      else if (yaw < 0 && yaw >= -90){
         driveX += 1+(yaw/90);
         driveZ -= (yaw)/90;
      }// quadrant 2 negative
      else if (yaw < -90 && yaw >= -180){
         driveX -= -((yaw+90)/90);
         driveZ += 1+(yaw+90)/90;
      }// quadrant 3 negative
      else if (yaw < -180 && yaw >= -270){
         driveX -= 1+((yaw+180)/90);
         driveZ -= -(yaw+180)/90;
      }// quadrant 4 negative
      else if (yaw < -270 && yaw >= -360){
         driveX += -((yaw+270)/90);
         driveZ -= 1+(yaw+270)/90;
      } // 
      // if ((3.5*dim >=X) && (X>=3.5*dim-(1.7*25)) && (-9.5>=Z) && (Z>=-11.0)) { // 8.9 10.1
      //    driveX -= 1;
      //    driveZ -= 1;

      // }
      // if ((3.5*dim >=X) && (X>=3.5*dim-(1.7*25)) && (-8.2>=Z) && (Z>=-9.15)) {
      //    driveX += 1;
      //    driveZ += 1;

      // }
      // // prevents you from driving off
      // if (X > 3.5*dim-2)
      //    driveX -= 1;
      // if (X < -3.5*dim)
      //    driveX += 1;
      // if (Z > 3.5*dim-2)
      //    driveZ -= 1;
      // if (Z < -3.5*dim+2)
      //    driveZ += 1;
      // collision
   }// Reverse
   else if (ch == 'h' || ch == 'H'){
      // driveX -= 1;
      // turn if 0 to 90 deg, quadrant 4
      if (yaw >= 0 && yaw <= 90){
         driveX -= 1-(yaw/90);
         driveZ += (yaw)/90;
      }// quadrant 3
      else if (yaw > 90 && yaw <= 180){
         driveX += ((yaw-90)/90);
         driveZ += 1-(yaw-90)/90;
      } // quadrant 2
      else if (yaw > 180 && yaw <= 270){
         driveX += 1-((yaw-180)/90);
         driveZ -= (yaw-180)/90;
      }// quadrant 1
      else if (yaw > 270 && yaw <= 360){
         driveX -= ((yaw-270)/90);
         driveZ -= 1-(yaw-270)/90;
      }// turn if 0 to -90 deg, quadrant 1
      else if (yaw < 0 && yaw >= -90){
         driveX -= 1+(yaw/90);
         driveZ += (yaw)/90;
      }// quadrant 2 negative
      else if (yaw < -90 && yaw >= -180){
         driveX += -((yaw+90)/90);
         driveZ -= 1+(yaw+90)/90;
      }// quadrant 3 negative
      else if (yaw < -180 && yaw >= -270){
         driveX += 1+((yaw+180)/90);
         driveZ += -(yaw+180)/90;
      }// quadrant 4 negative
      else if (yaw < -270 && yaw >= -360){
         driveX -= -((yaw+270)/90);
         driveZ += 1+(yaw+270)/90;
      }
   }

   else if (ch == 'g' || ch == 'G')
      yaw += 10;
   else if (ch == 'j' || ch == 'J')
      yaw -= 10;
   if (yaw >360) 
      yaw -= 360;
   if (yaw <-360) 
      yaw += 360;

   //  Reproject
   Project(fov,asp,dim);
   //  Update state
   timer(-1);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(fov,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(1400,1100);
   glutCreateWindow("F16 Thunderbirds");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   
   //  Load and Compile G16
   CompileF16();
   // DrawModel();
   // mario kart 
   // kart(X,Y+3,Z,3,3,3,0);

   //  Load skybox texture
   sky[0] = LoadTexBMP("sky0.bmp");
   sky[1] = LoadTexBMP("sky1.bmp");
   sky[2] = LoadTexBMP("crate.bmp");
   sky[3] = LoadTexBMP("question.bmp");
   // glVertexPointer(3, GL_FLOAT, 0, Mario_Kart_Jose_Ethan_finalVerts);
   // glNormalPointer(GL_FLOAT, 0, Mario_Kart_Jose_Ethan_finalNormals);
   // glTexCoordPointer(2, GL_FLOAT, 0, Mario_Kart_Jose_Ethan_finalTexCoords);
   // glDrawArrays(GL_TRIANGLES, 0, Mario_Kart_Jose_Ethan_finalNumVerts);
   //  Set timer
   timer(1);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
