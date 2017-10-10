/*  CG LAB2
Gestione interattiva di una scena 3D mediante controllo
da mouse e da tastiera. I modelli geometrici in scena 
sono primitive GLU e mesh poligonali in formato *.m

 * INPUT: file .m contenente la mesh a triangoli:
 *        Vertex  NUM  x y z 
 *                .......
 *        Normal  NUM  x y z 
 *                .......
 *        Face    NUM f1 f2 f3 
 *                .......
 * OUPUT: visualizzazione della mesh in una finestra OpenGL
 *
 *
 * NB: i vertici di ogni faccia sono in verso orario
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "v3d.h"

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#define MAX_V 10000 /* max number of vertices allowed in the mesh model */
#define M_PI 3.141592653589793
#define NFILE 3

int 	listname[NFILE];

int  	wireframe; /* controls the visualization of primitives via glPolygonMode */                
int     orpro;     /* controls the type of projection via gluPerspective and glOrtho */
int     cull;      /* toggles backface culling via glEnable( GL_CULL_FACE ) and glDisable( GL_CULL_FACE ); */
int     mater;     /* controls the material associated with the model via glMaterial */
int     shading;   /* controls the shading model via glShadeModel */

GLfloat fovy; /* angolo del punto di vista */
GLfloat angle[3]; /* rotazione degli assi WCS*/

//Parte3 e 4

int indexObjectToMode = 0;
float matrix[NFILE][16] = {{0.64, 0.76, 0.0, 0.0, -0.76, 0.64, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.3, 1.4, 2.2, 1.0},
                           {1.0, 0.0, 0.0, 0.0, 0.0, 0.64, 0.76, 0.0, 0.0, -0.76, 0.64, 0.0, 0.5, 1.3, 0.1, 1.0},
                           {0.64, 0.0, -0.76, 0.0, 0.0, 1.0, 0.0, 0.0, 0.76, 0.0, 0.64, 0.0, 0.3, 0.4, 0.2, 1.0}};

float matrixWCS[NFILE][16] = {{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
                                                                                                                                                                                                  
float matrixOCS[NFILE][16] = {{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};

//fine parte3 e 4

GLfloat camC[3]; /* centro del sistema */
GLfloat camE[3]; /* punto di vista */
GLfloat camU[3]; /* vettore su della camera */
GLfloat lightPos[4]; /* posizione della luce */

GLfloat brass_ambient[] = {0.33, 0.22, 0.03, 1.0}, brass_diffuse[] = {0.78, 0.57, 0.11, 1.0}, brass_specular[] = {0.99, 0.91, 0.81, 1.0}, brass_shininess[] = {27.8};
GLfloat red_plastic_ambient[] = {0.0, 0.0, 0.0}, red_plastic_diffuse[] = {0.5, 0.0, 0.0}, red_plastic_specular[] = {0.7, 0.6, 0.6}, red_plastic_shininess[] = {32.0};
GLfloat emerald_ambient[] = {0.0215, 0.1745, 0.0215}, emerald_diffuse[] = {0.07568, 0.61424, 0.07568}, emerald_specular[] = {0.633, 0.727811, 0.633}, emerald_shininess[] = {76.8};
GLfloat slate_ambient[] = {0.02, 0.02, 0.02}, slate_diffuse[] = {0.02, 0.01, 0.01}, slate_specular[] = {0.4, 0.4, 0.4}, slate_shininess[] = {.78125};

GLfloat aspect = 1.0; /* rapporto larghezza-altezza della viewport */

GLUquadricObj* myReusableQuadric = 0;

/* Trackball variables */
float tbAngle = 0.0;
float tbAxis[3];

int tbDragging = 0;
float tbV[3];
float tbW[3];

//Parte2: camera motion
int indexMotion = 0;
int flagMotion = 0;
int numCV = 7;
void deCasteljau(float t, float* result);
float CV[7][3] = { {0, 20, -10}, {10, 20, -7}, {10, 20, 7}, {0, 20, 10}, {-10, 20, 7}, {-10, 20, -7}, {0, 20, -10} };
// Punti di controllo della curva di Bèzier
//fine camera motion

enum Modes
{
	MODE_INVALID,
	MODE_CHANGE_EYE_POS,
	MODE_CHANGE_REFERENCE_POS,
	MODE_CHANGE_UP_POS,
	MODE_CHANGE_LIGHT_POS,
	MODE_CHANGE_ZOOM,
	MODE_ROTATE_MODEL,

	MODE_CHANGE_CULLING,
	MODE_CHANGE_WIREFRAME,
	MODE_CHANGE_SHADING,
	MODE_CHANGE_PROJECTION,
	MODE_CHANGE_MATERIAL,

	MODE_TRANSLATE_WCS,
	MODE_ROTATE_WCS,
	MODE_TRANSLATE_OCS,
	MODE_ROTATE_OCS,
	MODE_TRANSLATE_VCS,
	MODE_ROTATE_VCS,

	MODE_PRINT_SYSTEM_STATUS,
	MODE_RESET,
	MODE_QUIT
};

enum Modes mode = MODE_CHANGE_REFERENCE_POS; /* global variable that stores the current mode */


int WindowWidth = 700;
int WindowHeight = 700;

void drawAxis( float scale, int drawLetters )
{	
	glDisable( GL_LIGHTING );
	glPushMatrix();
	glScalef( scale, scale, scale );
	glBegin( GL_LINES );

	glColor4d( 1.0, 0.0, 0.0, 1.0 );
	if( drawLetters )
	{
		glVertex3f( .8f, 0.05f, 0.0 );  glVertex3f( 1.0, 0.25f, 0.0 ); /* Letter X */
		glVertex3f( 0.8f, .25f, 0.0 );  glVertex3f( 1.0, 0.05f, 0.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 1.0, 0.0, 0.0 ); /* X axis      */


	glColor4d( 0.0, 1.0, 0.0, 1.0 );
	if( drawLetters )
	{
		glVertex3f( 0.10f, 0.8f, 0.0 );   glVertex3f( 0.10f, 0.90f, 0.0 ); /* Letter Y */
		glVertex3f( 0.10f, 0.90f, 0.0 );  glVertex3f( 0.05, 1.0, 0.0 );
		glVertex3f( 0.10f, 0.90f, 0.0 );  glVertex3f( 0.15, 1.0, 0.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 1.0, 0.0 ); /* Y axis      */


	glColor4d( 0.0, 0.0, 1.0, 1.0 );
	if( drawLetters )
	{
		glVertex3f( 0.05f, 0, 0.8f );  glVertex3f( 0.20f, 0, 0.8f ); /* Letter Z*/
		glVertex3f( 0.20f, 0, 0.8f );  glVertex3f( 0.05, 0, 1.0 );
		glVertex3f( 0.05f, 0, 1.0 );   glVertex3f( 0.20, 0, 1.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 1.0 ); /* Z axis    */

	glEnd();
	glPopMatrix();

	glEnable( GL_LIGHTING );
}

void computePointOnTrackball( int x, int y, float p[3] )
{
	float zTemp;
	//map to [-1;1]
	p[0] = (2.0f * x - WindowWidth)  / WindowWidth;
	p[1] = (WindowHeight - 2.0f * y) / WindowHeight;

	zTemp = 1.0f - (p[0]*p[0]) - (p[1]*p[1]);
	p[2] = (zTemp > 0.0f ) ? sqrt(zTemp) : 0.0;

	//printf( "p = (%.2f, %.2f, %.2f)\n", p[0], p[1], p[2] );
}

void mouse( int button, int state, int x, int y )
{
	if( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) 
	{
		tbDragging = 1;
		computePointOnTrackball( x, y, tbV );
	}
	if( button==GLUT_LEFT_BUTTON && state==GLUT_UP ) 
	{
		tbDragging = 0;
	}

}

void motion( int x, int y )
{
	float dx, dy, dz;

	computePointOnTrackball(x, y, tbW);
	if(tbDragging)
	{
		dx = tbV[0] - tbW[0];
		dy = tbV[1] - tbW[1];
		dz = tbV[2] - tbW[2];
		if (dx || dy || dz)
		{
            //parte2: trackball
			tbAngle += sqrt(dx*dx + dy*dy + dz*dz) * (180.0 / M_PI);
			tbAxis[0] += tbW[1]*tbV[2] - tbW[2]*tbV[1];
			tbAxis[1] += tbW[2]*tbV[0] - tbW[0]*tbV[2];
			tbAxis[2] += tbW[0]*tbV[1] - tbW[1]*tbV[0];
            
            tbV[0] = tbW[0];
            tbV[1] = tbW[1];
            tbV[2] = tbW[2];
            //fine trackball
		}

		//printf( "tbAngle = %.2f tbAxis = (%.2f, %.2f, %.2f)\n", tbAngle, tbAxis[0], tbAxis[1], tbAxis[2] );
	}

	glutPostRedisplay();
}

void passiveMotion( int x, int y )
{

}

void special( int key, int x, int y )
{

}

void keyboard (unsigned char key, int x, int y) 
{	
	float* pos = NULL;
	float step = 0;

	//parte2: change eye point
    if( mode == MODE_CHANGE_EYE_POS )
	{
        pos = camE;
        step = 0.1;
	}//fine change eye point
	else if( mode == MODE_CHANGE_REFERENCE_POS )
	{
		pos = camC;
		step = 0.1;
	}
	else if( mode == MODE_CHANGE_UP_POS )
	{
		pos = camU;
		step = 0.1;
	}
	else if( mode == MODE_CHANGE_LIGHT_POS )
	{
		pos = lightPos;
		step = 1.0;
	}
    //parte2: rotate model
	else if( mode == MODE_ROTATE_MODEL )
	{
        pos = angle;
        step = 1.0;
	}//fine rotate model
    //parte2: zoom in/out
    else if(mode == MODE_CHANGE_ZOOM){
        pos = &fovy;
        step = 1.0;
    }
    //fine zoom in/out
    //parte3
    else if( mode == MODE_ROTATE_WCS )
    {
        glPushMatrix();
        glLoadIdentity();
        step = 5.0;
        if(key == 'x'){
            glRotatef(step, 1, 0, 0);
        }else if(key == 'X'){
            glRotatef(-step, -1, 0, 0);
        }else if(key == 'y'){
            glRotatef(step, 0, 1, 0);
        }else if(key == 'Y'){
            glRotatef(-step, 0, 1, 0);
        }else if(key == 'z'){
            glRotatef(step, 0, 0, 1);
        }else if(key == 'Z'){
            glRotatef(-step, 0, 0, 1);
        }
        glMultMatrixf(matrixWCS[indexObjectToMode]);
        glGetFloatv(GL_MODELVIEW, matrixWCS[indexObjectToMode]);
        glPopMatrix();
    }
    else if( mode == MODE_TRANSLATE_WCS )
    {
        glPushMatrix();
        glLoadIdentity();
        step = 0.5;
        if(key == 'x'){
            glTranslatef(step, 0, 0);
        }else if(key == 'X'){
            glTranslatef(-step, 0, 0);
        }else if(key == 'y'){
            glTranslatef(0, step, 0);
        }else if(key == 'Y'){
            glTranslatef(0, -step, 0);
        }else if(key == 'z'){
            glTranslatef(0, 0, step);
        }else if(key == 'Z'){
            glTranslatef(0, 0, -step);
        }
        glMultMatrixf(matrixWCS[indexObjectToMode]);
        glGetFloatv(GL_MODELVIEW, matrixWCS[indexObjectToMode]);
        glPopMatrix();
    }
    else if( mode == MODE_ROTATE_OCS )
    {
        glPushMatrix();
        glLoadIdentity();
        step = 5.0;
        if(key == 'x'){
            glRotatef(step, 1, 0, 0);
        }else if(key == 'X'){
            glRotatef(-step, -1, 0, 0);
        }else if(key == 'y'){
            glRotatef(step, 0, 1, 0);
        }else if(key == 'Y'){
            glRotatef(-step, 0, 1, 0);
        }else if(key == 'z'){
            glRotatef(step, 0, 0, 1);
        }else if(key == 'Z'){
            glRotatef(-step, 0, 0, 1);
        }
        glMultMatrixf(matrixOCS[indexObjectToMode]);
        glGetFloatv(GL_MODELVIEW, matrixOCS[indexObjectToMode]);
        glPopMatrix();
    }
    else if( mode == MODE_TRANSLATE_OCS )
    {
        glPushMatrix();
        glLoadIdentity();
        step = 0.5;
        if(key == 'x'){
            glTranslatef(step, 0, 0);
        }else if(key == 'X'){
            glTranslatef(-step, 0, 0);
        }else if(key == 'y'){
            glTranslatef(0, step, 0);
        }else if(key == 'Y'){
            glTranslatef(0, -step, 0);
        }else if(key == 'z'){
            glTranslatef(0, 0, step);
        }else if(key == 'Z'){
            glTranslatef(0, 0, -step);
        }
        glMultMatrixf(matrixOCS[indexObjectToMode]);
        glGetFloatv(GL_MODELVIEW, matrixOCS[indexObjectToMode]);
        glPopMatrix();
    }
   
    glutPostRedisplay();
    //fine parte3
	
	if( pos != NULL )
	{
		if( key == 'x' )
            pos[0] += step;
		else if( key == 'X' )
            pos[0] -= step;
		else if( key == 'y' )
            pos[1] += step;
		else if( key == 'Y' )
            pos[1] -= step;
		else if( key == 'z' )
            pos[2] += step;
		else if( key == 'Z' )
            pos[2] -= step;
        //parte3: zoom in/out
        else if( key == 'f')
            *pos += step;
        else if( key == 'F')
            *pos -= step;
        //fine zoom in/out
        
		glutPostRedisplay();
	}

	if( key ==  27 ) //esc
		exit(1);
    
    //Parte2: camera motion
    if(key == 's'){
        flagMotion = !flagMotion;
        indexMotion = 0;
    }
    //fine camera motion
    
    //Parte3
    if(key == 'w'){
        mode = MODE_TRANSLATE_WCS;
    }else if(key == 'W'){
        mode = MODE_ROTATE_WCS;
    }else if(key == 'o'){
        mode = MODE_TRANSLATE_OCS;
    }else if(key == 'O'){
        mode = MODE_ROTATE_OCS;
    }else if(key == '1'){
        indexObjectToMode = 0;
    }else if(key == '2'){
        indexObjectToMode = 1;
    }else if(key == '3'){
        indexObjectToMode = 2;
    }
    //fine parte3
}

//Parte2: camera motion
void idle(){
    float t;
    float result[3];
    
    if(flagMotion){
        t = (GLfloat) indexMotion/100;
        deCasteljau(t, &result[0]);
        camE[0] = result[0];
        camE[1] = result[1];
        camE[2] = result[2];
        
        indexMotion++;
        if(indexMotion == 100){
            indexMotion = 0;
        }
        glutPostRedisplay();
    }
}

void deCasteljau(float t, float* result){
    int i, k;
    float coordX[numCV], coordY[numCV], coordZ[numCV];
    
    for(i = 0; i < numCV; i++){
        coordX[i] = CV[i][0];
        coordY[i] = CV[i][1];
        coordZ[i] = CV[i][2];
    }
    
    for(i = 1; i < numCV; i++){
        for(k = 0; k < numCV - i; k++){
            coordX[k] = (1-t) * coordX[k] + (t) * coordX[k+1];
            coordY[k] = (1-t) * coordY[k] + (t) * coordY[k+1];
            coordZ[k] = (1-t) * coordZ[k] + (t) * coordZ[k+1];
        }
    }
    result[0] = coordX[0];
    result[1] = coordY[0];
    result[2] = coordZ[0];
}
//fine camera motion


void drawGluSlantCylinderWithCaps( double height, double radiusBase, double radiusTop, int slices, int stacks )
{
	// First draw the cylinder
	gluCylinder( myReusableQuadric, radiusBase, radiusTop, height, slices, stacks );

	// Draw the top disk cap
	glPushMatrix();
	glTranslated(0.0, 0.0, height);
	gluDisk( myReusableQuadric, 0.0, radiusTop, slices, stacks );
	glPopMatrix();

	// Draw the bottom disk cap
	glPushMatrix();
	glRotated(180.0, 1.0, 0.0, 0.0);
	gluDisk( myReusableQuadric, 0.0, radiusBase, slices, stacks );
	glPopMatrix();

}

void display() {
    int i;
    
	if (mater==0) //ottone
	{	
		glLightfv(GL_LIGHT0, GL_AMBIENT, brass_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brass_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, brass_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, brass_shininess);
	}
	if (mater==1) //plastica rossa
	{
		glLightfv(GL_LIGHT0, GL_AMBIENT, red_plastic_ambient);
   		glMaterialfv(GL_FRONT, GL_DIFFUSE, red_plastic_diffuse);
   		glMaterialfv(GL_FRONT, GL_SPECULAR, red_plastic_specular);
   		glMaterialfv(GL_FRONT, GL_SHININESS, red_plastic_shininess);
	}
   	if (mater==2) //smeraldo
   	{	
		glLightfv(GL_LIGHT0, GL_AMBIENT, emerald_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, emerald_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, emerald_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, emerald_shininess);
	}
	if (mater==3) //slate
	{	
		glLightfv(GL_LIGHT0, GL_AMBIENT, slate_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, slate_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, slate_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, slate_shininess);
	}

	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    
    //parte2: wireframe
    if(wireframe)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    else
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); //riempie i triangolini
    //fine wireframe
    
    //parte2: shading
    if(shading)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);
    //fine shading
    
    //parte2: culling
    if (cull) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
    //fine culling
    
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //glShadeModel (GL_FLAT);

	glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity ();
    
    //parte2: proiezione
    if(orpro)
        gluPerspective(fovy, aspect, 1, 100);
    else
        glOrtho(-1.0, 1.0, -1.0, 1.0, -10, 100);
    //fine proiezione
    
	//gluPerspective( fovy, aspect, 1, 100);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt( camE[0], camE[1], camE[2], camC[0], camC[1], camC[2], camU[0], camU[1], camU[2] );

	// Trackball rotation.
	glRotatef(tbAngle, tbAxis[0], tbAxis[1], tbAxis[2]);
    
    
    //Parte2: rotate model
    glLineWidth(2);
    drawAxis( 2.0, 1 );
    glLineWidth(1);
    
	glRotatef(angle[0], 1.0, 0.0, 0.0);
	glRotatef(angle[1], 0.0, 1.0, 0.0);
	glRotatef(angle[2], 0.0, 0.0, 1.0);
    //fine rotate model
    
    for(i = 0; i < NFILE; i++){
        glPushMatrix();
        glMultMatrixf(matrixWCS[i]);
        glMultMatrixf(matrix[i]);
        //draw the mesh model
        drawAxis( 1.0, 0 );
        glMultMatrixf(matrixOCS[i]);
        glCallList(listname[i]);
        glPopMatrix();
    }
	 
	//Draw some quadrics
    //glTranslated(2.5, 0.0, 2.5);
	//gluSphere(myReusableQuadric, 1.0, 12, 12); //Palla che circonda il maiale
    
    //Cubo per ortogonale/prospettica
    //glutSolidCube(1);
    
	//glTranslated(0.0, 0.0, 1.5);
	//gluCylinder(myReusableQuadric, 0.5, 0.2, 0.5, 12, 12);

	//gluDisk(myReusableQuadric, 0.5, 1.0, 10, 10);
	//gluPartialDisk( myReusableQuadric, 0.5, 1.0, 10, 10, 0.0, 45.0);

	//drawGluSlantCylinderWithCaps( 2.0, 1.0, 0.4, 8, 8 ); 

//  glutWireIcosahedron();
//	glutWireDodecahedron();
//  glutWireTeapot(1.0); 
//	glutWireTorus(0.5, 1.0, 10, 10);
//	glutWireCone(1.0, 1.0, 10, 10);

	glutSwapBuffers();
}

void reset()
{
	angle[0] = 0.0;
	angle[1] = 0.0;
	angle[2] = 0.0;

    camE[0] = 8.8;
    camE[1] = 4.9;
    camE[2] = 9.0;

//Per vedere la differenza fra prospettica e ortogonale
//    camE[0] = 9.0;
//    camE[1] = 1.5;
//    camE[2] = 0.0;

	camC[0] = 0.0;
	camC[1] = 0.0;
	camC[2] = 0.0;

	camU[0] = 0.0;
	camU[1] = 1.0;
	camU[2] = 0.0;

	lightPos[0] =  5.0;
	lightPos[1] =  5.0;
	lightPos[2] =  5.0;
	lightPos[3] =  1.0;

	fovy = 20;
	wireframe = 1;
	cull = 0;
	mater = 1;
	orpro = 1;
	shading = 0;

	glutPositionWindow(10,10);
	glutReshapeWindow(700,700);
	glutPostRedisplay();
    
    //Per far si che la reset continui a funzionare
    //anche quando la trackball è SEMPRE attiva
    tbAngle = 0.0;
    tbAxis[0] = 0.0;
    tbAxis[1] = 0.0;
    tbAxis[2] = 0.0;
    
    indexObjectToMode = 0;
    
    for(int i = 0; i < NFILE; i++) {
        glPushMatrix();
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW, matrixWCS[0]);
        glPopMatrix();
    
        glPushMatrix();
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW, matrixOCS[0]);
        glPopMatrix();
    }
}

void loadMesh(char* path, int faces[MAX_V][3], int *nface, float vertices[MAX_V][3], int *nvert, float fnormals[MAX_V][3], float vnormals[MAX_V][3]){
    FILE* idf;
    char s[10];
    float a,b,c;
    int id, i;
    
    //apertura del file *.m
    printf("Apertura del file...\n");

    if ((idf = fopen(path, "r")) == NULL)
    {
        perror("file non trovato\n");
        exit (1);
    }
    
    while( !feof( idf ) )
    {
        fscanf(idf,"%s %d %f %f %f",s, &id, &a,&b,&c);
        switch( s[0] )
        {
            case 'V':
                vertices[*nvert][0] = a;
                vertices[*nvert][1] = b;
                vertices[*nvert][2] = c;
                (*nvert)++;
                break;
            case 'N':
                printf(" Vertex normals not used! \n");
                break;
            case 'F':
                faces[*nface][0] = (int)a - 1;
                faces[*nface][1] = (int)b - 1;
                faces[*nface][2] = (int)c - 1;
                
                //parte1 esercitazione
                float v1[3], v2[3], v3[3];
                v3dSub(vertices[(int)b-1], vertices[(int)a-1], v1);
                v3dSub(vertices[(int)c-1], vertices[(int)a-1], v2);
                v3dCross(v1, v2, v3);
                v3dNormalize(v3);
                
                fnormals[*nface][0] = v3[0];
                fnormals[*nface][1] = v3[1];
                fnormals[*nface][2] = v3[2];
                
                vnormals[(int)a-1][0] += v3[0];
                vnormals[(int)a-1][1] += v3[1];
                vnormals[(int)a-1][2] += v3[2];
                
                vnormals[(int)b-1][0] += v3[0];
                vnormals[(int)b-1][1] += v3[1];
                vnormals[(int)b-1][2] += v3[2];
                
                vnormals[(int)c-1][0] += v3[0];
                vnormals[(int)c-1][1] += v3[1];
                vnormals[(int)c-1][2] += v3[2];
                //fine parte1 esercitazione
                
                (*nface)++;
                break;
            case 'E':
                printf(" Edge not used! \n");
                break;
            default:
                printf(" ERROR: Reading file \n");
                break;
        }
    }
    
    //parte1 esercitazione
    for(i = 0; i < *nvert; i++){
        v3dNormalize(vnormals[i]);
    }
    for(i = 0; i < *nface; i++){
        v3dNormalize(fnormals[i]);
    }
    //fine parte1 esercitazione
    
    fclose(idf);
}

void init()
{
    int 	faces[MAX_V][3];    /* faces */
	float 	vertices[MAX_V][3]; /* vertices */
	float 	vnormals[MAX_V][3]; /* vertex normals */
	float   fnormals[MAX_V][3]; /* face normals */

	int i, j, k;
	int ii, ids[3];
    int nface,nvert;
    char* filePath[NFILE];
    
    filePath[0] = "/Users/erika/Desktop/MAGISTRALE/1anno2ciclo/Fondamenti di Computer Graphics/Laboratorio/lab2/lab2/data/pig.m";
    filePath[1] = "/Users/erika/Desktop/MAGISTRALE/1anno2ciclo/Fondamenti di Computer Graphics/Laboratorio/lab2/lab2/data/cactus.m";
    filePath[2] = "/Users/erika/Desktop/MAGISTRALE/1anno2ciclo/Fondamenti di Computer Graphics/Laboratorio/lab2/lab2/data/teapot.m";
    
    for ( k = 0; k < NFILE; k++){
        for( i=0 ; i<MAX_V ; i++ )
        {
            for( j=0 ; j<3 ; j++ )
            {
                fnormals[i][j] = 0.0f;
                vnormals[i][j] = 0.0f;
                faces[i][j] = 0.0f;
                vertices[i][j] = 0.0f;
            }
        }
        
        nface=0;
        nvert=0;
        
        loadMesh(filePath[k], faces, &nface, vertices, &nvert, fnormals, vnormals);
        
        // crea la display list mesh
        printf("Creazione display list .. \n");
        listname[k]=glGenLists(1);  //ID della lista e' listname
        glNewList(listname[k],GL_COMPILE); //GL_COMPILE non esegue, quindi occorre chiamare GL_CALL_LIST
        
        for( i = 0; i < nface; i++ ) {
            ids[2] = faces[i][0];
            ids[1] = faces[i][1];
            ids[0] = faces[i][2];
            
            //disegna triangoli coi vertici specificati
            glBegin( GL_TRIANGLES );
            glColor3f(1,0,0);
            for( ii = 2; ii >= 0; ii--)
            {
                glNormal3f( vnormals[ids[ii]][0], vnormals[ids[ii]][1], vnormals[ids[ii]][2] );
                glVertex3f( vertices[ids[ii]][0], vertices[ids[ii]][1], vertices[ids[ii]][2] );
            }					
            glEnd();
        }
        
        glEndList();
    }

	//crea la display list quadrica
	myReusableQuadric = gluNewQuadric();  
	gluQuadricNormals( myReusableQuadric, GLU_SMOOTH );

//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glClearColor( 1.0, 1.0, 1.0, 1.0 );

	/*
	glEnable (GL_POINT_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);	// Antialias the lines
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/
}

void print_sys_status()
{
	system("clear");
	printf("\nSystem status\n\n");
	printf("---------------------------------------------------\n");
	printf("Eye point coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",camE[0], camE[1], camE[2] );
	printf("---------------------------------------------------\n");
	printf("Reference point coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",camC[0], camC[1], camC[2] );
	printf("---------------------------------------------------\n");
	printf("Up vector coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",camU[0], camU[1], camU[2] );
	printf("---------------------------------------------------\n");
	printf("Light position coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",lightPos[0], lightPos[1], lightPos[2] );
	printf("---------------------------------------------------\n");
	printf("Axis rotation (in degree) :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",angle[0], angle[1], angle[2] );
	printf("---------------------------------------------------\n");
	printf("Field of view angle (in degree) = %.2f\n",fovy);
	printf("---------------------------------------------------\n");
	if(wireframe)
		printf("Wireframe = YES\n");
	else
		printf("Wireframe = NO\n");
    if(shading)
        printf("Shading = YES\n");
    else
        printf("Shading = NO\n");
    printf("---------------------------------------------------\n");	
}

void menu(int sel)
{
	//enum Modes oldMode = mode;
	//mode = sel;

	if( sel == MODE_CHANGE_EYE_POS || sel == MODE_CHANGE_REFERENCE_POS || sel == MODE_CHANGE_UP_POS || sel == MODE_CHANGE_LIGHT_POS || sel == MODE_ROTATE_MODEL )
		mode = sel;

    //parte2: zoom in/out
    if (sel == MODE_CHANGE_ZOOM)
        mode = sel;
    //fine zoom in/out
    
	if(sel == MODE_CHANGE_CULLING)
	{
		cull = !cull;
	} 
	if(sel == MODE_CHANGE_WIREFRAME)
	{
		wireframe = !wireframe;
	}
	if(sel == MODE_CHANGE_PROJECTION)
	{
		orpro = !orpro;
	}
	if(sel == MODE_CHANGE_SHADING)
	{
		shading = !shading;
	}
	if( sel == MODE_CHANGE_MATERIAL )
	{
		mater = (mater+1)%4; 
	}
	if(sel == MODE_RESET)
	{
		reset();
	} 
	if(sel == MODE_QUIT)
	{
		exit(0);
	}
	if(sel == MODE_PRINT_SYSTEM_STATUS)
	{
		print_sys_status();
	}

	glutPostRedisplay();
}

int main (int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( WindowWidth, WindowHeight ); 
    glutInitWindowPosition (0, 0);
    glutCreateWindow("Model Viewer ");

    glutDisplayFunc(display);
	glutSpecialFunc(special);		// frecce up,down,left,right
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);
    //Parte2: camera motion
    glutIdleFunc(idle);
    //fine camera motion

	glutCreateMenu(menu); 
	glutAddMenuEntry("Menu",-1); //-1 significa che non si vuole gestire questa riga
	glutAddMenuEntry("",-1);   
	glutAddMenuEntry("Change eye point (use x,y,z,X,Y,Z)", MODE_CHANGE_EYE_POS);
	glutAddMenuEntry("Change reference point (use x,y,z,X,Y,Z)", MODE_CHANGE_REFERENCE_POS);
	glutAddMenuEntry("Change up vector (use x,y,z,X,Y,Z)", MODE_CHANGE_UP_POS);
	glutAddMenuEntry("Change light position (use x,y,z,X,Y,Z)", MODE_CHANGE_LIGHT_POS);
	glutAddMenuEntry("Zoom (use f,F)", MODE_CHANGE_ZOOM);
	glutAddMenuEntry("Rotate Model (use x,y,z,X,Y,Z)", MODE_ROTATE_MODEL );

	glutAddMenuEntry("",-1);
	glutAddMenuEntry("Culling", MODE_CHANGE_CULLING);
	glutAddMenuEntry("Wireframe", MODE_CHANGE_WIREFRAME);
	glutAddMenuEntry("Ortographic or Prospective", MODE_CHANGE_PROJECTION);
	glutAddMenuEntry("Shading", MODE_CHANGE_SHADING);
	glutAddMenuEntry("Material", MODE_CHANGE_MATERIAL); 

	glutAddMenuEntry("",-1);   
	glutAddMenuEntry("Print system status", MODE_PRINT_SYSTEM_STATUS);    
	glutAddMenuEntry("Reset", MODE_RESET);     
	glutAddMenuEntry("Quit", MODE_QUIT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	reset();
    init();

    glutMainLoop();

	return -1;
}
