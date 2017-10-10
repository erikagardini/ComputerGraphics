/*
 * ConnectDots.c
 *
 *     This program draws straight lines connecting dots placed with mouse clicks.
 *
 *
 * Usage:
 *   Left click to place a control point.
 *		Maximum number of control points allowed is currently set at 64.
 *	 Press "f" to remove the first control point
 *	 Press "l" to remove the last control point.
 *	 Press escape to exit.
 */

#include <stdlib.h>
#include <GLUT/glut.h>
#include <stdio.h>
#include <math.h>

#define MAX_CV 64
#define GAP 0.01

/* prototypes */
void removeFirstPoint ();
void removeLastPoint ();
void addNewPoint (float x, float y, float z);
void deCasteljau(float t, float* result);
void adaptiveSubdivision(float INPUT[MAX_CV][3]);
bool allInRange(float points[MAX_CV][3]);
void deCasteljauAdaptive(float t, float c1[MAX_CV][3], float c2[MAX_CV][3], float points[MAX_CV][3]);

/* global variables */
float CV[MAX_CV][3];
int numCV = 0;
int flag = 0;

bool trovato = false;
int pos = -1;

// Window size in pixels
int WindowHeight;
int WindowWidth;

/* function definitions */

void keyboard (unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'f':
            removeFirstPoint ();
            glutPostRedisplay ();
            break;
        case 'l':
            removeLastPoint ();
            glutPostRedisplay ();
            break;
        case 27:						   // Escape key
            exit (0);
            break;
        case '1':
            flag = 0;
            glutPostRedisplay();
            break;
        case '2':
            flag = 1;
            glutPostRedisplay();
            break;
        case '3':
            flag = 2;
            glutPostRedisplay();
            break;
    }
}

void removeFirstPoint ()
{
    int i;
    if (numCV > 0)
    {
        // Remove the first point, slide the rest down
        numCV--;
        for (i = 0; i < numCV; i++)
        {
            CV[i][0] = CV[i + 1][0];
            CV[i][1] = CV[i + 1][1];
            CV[i][2] = CV[i + 1][2];
        }
    }
}

// Left button presses place a control point.
void mouse (int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        float xPos = ((float) x) / ((float) (WindowWidth - 1));
        float yPos = ((float) y) / ((float) (WindowHeight - 1));
        float zPos = 0;
        yPos = 1.0f - yPos;			   // Flip value since y position is from top row.
        addNewPoint (xPos, yPos, zPos);
        glutPostRedisplay (); //Forza la riesecuzione della callback display
    }
    //Parte4 esercitazione
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        float xPos = ((float) x) / ((float) (WindowWidth - 1));
        float yPos = ((float) y) / ((float) (WindowHeight - 1));
        float zPos = 0;
        yPos = 1.0f - yPos;
        
        for(int i = 0; i < numCV; i++) {
            if(CV[i][0] <= (xPos + 0.01) && CV[i][0] >= (xPos - 0.01) && CV[i][1] <= (yPos + 0.01) && CV[i][1] >= (yPos - 0.01) && CV[i][2] == zPos) {
                trovato = true;
                pos = i;
            }
        }
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        trovato = false;
        pos = -1;
    }
    //Fine parte4 esercitazione
}

//Parte4 esercitazione
void mouseMotion(int x, int y) {
    if(trovato) {
        float xPos = ((float) x) / ((float) (WindowWidth - 1));
        float yPos = ((float) y) / ((float) (WindowHeight - 1));
        float zPos = 0;
        yPos = 1.0f - yPos;
        CV[pos][0] = xPos;
        CV[pos][1] = yPos;
        CV[pos][2] = zPos;
        glutPostRedisplay();
    }
}
//Fine parte4 esercitazione

// Add a new point to the end of the list.
// Remove the first point in the list if too many points.
void removeLastPoint ()
{
    if (numCV > 0)
        numCV--;
}


// Add a new point to the end of the list.
// Remove the first point in the list if too many points.
void addNewPoint (float x, float y, float z)
{
    if (numCV >= MAX_CV)
        removeFirstPoint ();
    
    CV[numCV][0] = x;
    CV[numCV][1] = y;
    CV[numCV][2] = z;
    //Aggiungere una terza dimensione
    numCV++;
}

void display (void)
{
    int i;
    //float t, result[3];
    
    glClear (GL_COLOR_BUFFER_BIT);
    
    // Draw the line segments
    glColor3f (1.0f, 0.0f, 0.8f);	   // Reddish/purple lines - si definisce il colore della linea
    glEnable (GL_LINE_STIPPLE); //Parmette di abilitare il tipo di linea, in questo caso tratteggiata
    glLineStipple (1, 0x00FF);

    //Appena l'untente inserisce il secondo punto allora occorre inziare a disegnare la polilinea
    if (numCV > 1)
    {
        glBegin (GL_LINE_STRIP);
        for (i = 0; i < numCV; i++)
        {
            //Se si lavora con tre coordinate basta questo, altrimenti occorre aggiungere un'altra
            glVertex2f (CV[i][0], CV[i][1]);
        }
        glEnd ();
        
        glDisable (GL_LINE_STIPPLE); //Si disabilitano gli attributi della linea
        // Draw the interpolated points second.
        glColor3f (0.0f, 0.0f, 0.0f);	   // Draw points in black
        
        glEnable(GL_MAP1_VERTEX_3);
        glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, numCV, &CV[0][0]);
    
        glBegin(GL_LINE_STRIP);
        
        if(flag == 0){
            //Parte1 esercitazione
            for(i=0; i <= 100; i++){
                glEvalCoord1f((GLfloat) i / 100.0);
            }
        }else if(flag == 1){
            //Parte2 esercitazione
            float t, result[3];
            for(i = 0; i <= 100; i++) {
                t = (GLfloat) i/100;
                deCasteljau(t, &result[0]);
                glVertex3f(result[0], result[1], result[2]);
            }
        }else if(flag == 2){
            /*Parte3 esercitazione*/
            adaptiveSubdivision(CV);
        }
        
        glEnd();
    }
    
    glBegin (GL_POINTS);
    for (i = 0; i < numCV; i++)
    {
        glVertex2f (CV[i][0], CV[i][1]);
    }
    glEnd ();
    
    //Permette di evadere l'evento display così da vederne immediatamente i risultati
    glFlush ();
}
//
////Parte3 esercitazione
void adaptiveSubdivision(float INPUT[MAX_CV][3]){
    int i;
    float c1[MAX_CV][3], c2[MAX_CV][3], c2rev[MAX_CV][3];
    
    if(allInRange(INPUT)){
        glVertex3f(INPUT[0][0], INPUT[0][1], INPUT[0][2]);
        glVertex3f(INPUT[numCV-1][0], INPUT[numCV-1][1], INPUT[numCV-1][2]);
    }else{
        deCasteljauAdaptive(0.5, c1, c2, INPUT);
        adaptiveSubdivision(c1);
        for(i=0; i < numCV; i++){
            c2rev[i][0] = c2[numCV-1-i][0];
            c2rev[i][1] = c2[numCV-1-i][1];
            c2rev[i][2] = 0;
        }
        adaptiveSubdivision(c2rev);
    }
}
//
////Parte3 esercitazione
bool allInRange(float points[MAX_CV][3]){
    int i;
    float a, b, c, distance;
    float x0, y0, x1, y1;
    
    x0 = points[0][0];
    y0 = points[0][1];
    x1 = points[numCV-1][0];
    y1 = points[numCV-1][1];
    
    a = y0 - y1;
    b = x1 - x0;
    c = (y1 - y0)*x0 - (x1 - x0)*y1;
    
    for(i = 0; i < numCV; i++){
        distance = fabsf(a*points[i][0] + b*points[i][1] + c) / sqrtf(powf(a,2) + powf(b, 2));
        if(distance >= GAP)
            return false;
    }
    return true;
}

//Parte2 esercitazione
void deCasteljau(float t, float* result){
    int i, k;
    float coordX[numCV], coordY[numCV];
    
    for(i = 0; i < numCV; i++){
        coordX[i] = CV[i][0];
        coordY[i] = CV[i][1];
    }
    
    for(i = 1; i < numCV; i++){
        for(k = 0; k < numCV - i; k++){
            coordX[k] = (1-t) * coordX[k] + (t) * coordX[k+1];
            coordY[k] = (1-t) * coordY[k] + (t) * coordY[k+1];
        }
    }
    result[0] = coordX[0];
    result[1] = coordY[0];
    result[2] = 0;
}

////Parte3 esercitazione
void deCasteljauAdaptive(float t, float c1[MAX_CV][3], float c2[MAX_CV][3], float points[MAX_CV][3]){
    int i, k;
    float temp[MAX_CV][3];
    
    for(i = 0; i < numCV; i++){
        temp[i][0] = points[i][0];
        temp[i][1] = points[i][1];
        temp[i][2] = 0;
    }
    
    c1[0][0] = points[0][0];
    c1[0][1] = points[0][1];
    c1[0][2] = 0;
    
    c2[0][0] = points[numCV-1][0];
    c2[0][1] = points[numCV-1][1];
    c2[0][2] = 0;
    
    for(i = 1; i < numCV; i++){
        for(k = 0; k < numCV - i; k++){
            temp[k][0] = (1-t) * temp[k][0] + (t) * temp[k+1][0];
            temp[k][1] = (1-t) * temp[k][1] + (t) * temp[k+1][1];
        }
        c1[i][0] = temp[0][0];
        c1[i][1] = temp[0][1];
        c1[i][2] = 0;
        
        c2[i][0] = temp[k-1][0];
        c2[i][1] = temp[k-1][1];
        c2[i][2] = 0;
    }
    
}

void initRendering ()
{
    glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
    
    // Make big points and wide lines.  (This may be commented out if desired.)
    glPointSize (6);
    glLineWidth (1);
    
    // The following commands should induce OpenGL to create round points and
    //  antialias points and lines.  (This is implementation dependent unfortunately, and
    //  may slow down rendering considerably.)
    //  You may comment these out if you wish.
    glEnable (GL_POINT_SMOOTH);
    glEnable (GL_LINE_SMOOTH);
    glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
    glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);	// Antialias the lines
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
} 

void reshape (int w, int h) 
{
    WindowHeight = (h > 1) ? h : 2;
    WindowWidth = (w > 1) ? w : 2;
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (0.0f, 1.0f, 0.0f, 1.0f);	// Always view [0,1]x[0,1].
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
} 

int main (int argc, char **argv) 
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    
    initRendering ();
    
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutKeyboardFunc (keyboard);
    glutMouseFunc (mouse);
    
    glutMotionFunc(mouseMotion);
    
    glutMainLoop ();
    return 0;						   // This line is never reached
}

