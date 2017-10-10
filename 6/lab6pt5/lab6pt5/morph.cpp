#include "/Users/erika/Desktop/MAGISTRALE/1anno2ciclo/Fondamenti di Computer Graphics/Laboratorio/06/common/common.h"

GLuint         program;
GLint          timeParam;
GLint          vertices_two_location;
GLint          color_two_location; // PARTE1

const GLfloat vertices_one[3][2] = {{0.0, 0.0},{0.5,1.0},{1.0, 0.0}};

const GLfloat vertices_two[3][2] = {{0.0, 1.0},{0.5,0.0},{1.0, 1.0}};
const GLfloat colors_two[3][3]  = {{1.0,1.0,0.0},{0.0,1.0,0.0},{1.0,0.0,1.0}}; //PARTE1

const GLfloat vertices_three[4][2] = {{0.0, 0.0},{1.0,0.0},{0.5,1.0},{0.5, 1.0}}; //PARTE1
const GLfloat vertices_four[4][2] = {{0.0, 0.0},{1.0,0.0},{1.0,1.0},{0.0, 1.0}}; //PARTE1
const GLfloat colors_three[3][3]  = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}}; //PARTE1

const GLfloat vertices_five[4][2] = {{0.0, 0.0},{1.0,0.0},{1.0,1.0},{0.0, 1.0}}; //PARTE1
const GLfloat vertices_six[4][2] = {{1.0, 0.0},{1.0,1.0},{0.0,1.0},{0.0, 0.0}}; //PARTE1

static void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0,0.0,0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);

    glEnable(GL_DEPTH_TEST);

    //glewInit();
    program = initShader("/Users/erika/Desktop/MAGISTRALE/1anno2ciclo/Fondamenti di Computer Graphics/Laboratorio/lab6pt5/lab6pt5/v.glsl", "/Users/erika/Desktop/MAGISTRALE/1anno2ciclo/Fondamenti di Computer Graphics/Laboratorio/lab6pt5/lab6pt5/f.glsl");

    // Setup uniform and attribute prameters
    timeParam = glGetUniformLocation(program, "time");
    
    vertices_two_location = glGetAttribLocation(program, "vertices2");
    color_two_location = glGetAttribLocation(program, "color2"); //PARTE1
	/* ----------- ADD ------------------ 
			handle additional variables if needed
	*/
}

static void draw(void)
{
    /* send elapsed time to shaders */
    
    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //DEFAULT
//    glBegin(GL_LINE_LOOP);
//    glVertexAttrib2fv(vertices_two_location, &vertices_two[0][0]);
//    glVertex2fv(vertices_one[0]);
//    glVertexAttrib2fv(vertices_two_location, &vertices_two[1][0]);
//    glVertex2fv(vertices_one[1]);
//    glVertexAttrib2fv(vertices_two_location, &vertices_two[2][0]);
//    glVertex2fv(vertices_one[2]);

//    glBegin(GL_TRIANGLES); //PARTE 1
//        /* ----------- ADD ------------------
//            instead of lines, draw a triangle
//            assign color to the vertices
//        */
//        glColor3f(1.0, 0.0, 0.0); //PARTE 1
//        glVertexAttrib2fv(vertices_two_location, &vertices_two[0][0]);
//        glVertexAttrib3fv(color_two_location, &colors_two[0][0]); //PARTE 1
//        glVertex2fv(vertices_one[0]);
//
//        glColor3f(0.0, 1.0, 0.0); //PARTE 1
//        glVertexAttrib2fv(vertices_two_location, &vertices_two[1][0]);
//        glVertexAttrib3fv(color_two_location, &colors_two[1][0]); //PARTE 1
//        glVertex2fv(vertices_one[1]);
//
//        glColor3f(0.0, 0.0, 1.0); //PARTE 1
//        glVertexAttrib2fv(vertices_two_location, &vertices_two[2][0]);
//        glVertexAttrib3fv(color_two_location, &colors_two[2][0]); //PARTE 1
//        glVertex2fv(vertices_one[2]);
//
//    glEnd();
    
    
    glBegin(GL_QUADS); //PARTE 1
    /* ----------- ADD ------------------
     instead of lines, draw a triangle
     assign color to the vertices
     */
    glColor3f(1.0, 0.0, 0.0); //PARTE 1
    glVertexAttrib2fv(vertices_two_location, &vertices_six[0][0]);
    glVertexAttrib3fv(color_two_location, &colors_two[0][0]); //PARTE 1
    glVertex2fv(vertices_five[0]);
    
    glColor3f(0.0, 1.0, 0.0); //PARTE 1
    glVertexAttrib2fv(vertices_two_location, &vertices_six[1][0]);
    glVertexAttrib3fv(color_two_location, &colors_two[1][0]); //PARTE 1
    glVertex2fv(vertices_five[1]);
    
    glColor3f(0.0, 0.0, 1.0); //PARTE 1
    glVertexAttrib2fv(vertices_two_location, &vertices_six[2][0]);
    glVertexAttrib3fv(color_two_location, &colors_two[2][0]); //PARTE 1
    glVertex2fv(vertices_five[2]);
    
    glColor3f(0.0, 0.0, 1.0); //PARTE 1
    glVertexAttrib2fv(vertices_two_location, &vertices_six[3][0]);
    glVertexAttrib3fv(color_two_location, &colors_two[2][0]); //PARTE 1
    glVertex2fv(vertices_five[3]);
    
    glEnd();
    
//    glBegin(GL_QUADS); //PARTE 1
//    /* ----------- ADD ------------------
//     instead of lines, draw a triangle
//     assign color to the vertices
//     */
//    glColor3f(1.0, 0.0, 0.0); //PARTE 1
//    glVertexAttrib2fv(vertices_two_location, &vertices_four[0][0]);
//    glVertexAttrib3fv(color_two_location, &colors_three[0][0]); //PARTE 1
//    glVertex2fv(vertices_three[0]);
//
//    glColor3f(0.0, 1.0, 0.0); //PARTE 1
//    glVertexAttrib2fv(vertices_two_location, &vertices_four[1][0]);
//    glVertexAttrib3fv(color_two_location, &colors_three[1][0]); //PARTE 1
//    glVertex2fv(vertices_three[1]);
//
//    glColor3f(0.0, 0.0, 1.0); //PARTE 1
//    glVertexAttrib2fv(vertices_two_location, &vertices_four[2][0]);
//    glVertexAttrib3fv(color_two_location, &colors_three[2][0]); //PARTE 1
//    glVertex2fv(vertices_three[2]);
//
//    glColor3f(0.0, 0.0, 1.0); //PARTE 1
//    glVertexAttrib2fv(vertices_two_location, &vertices_four[3][0]);
//    glVertexAttrib3fv(color_two_location, &colors_three[2][0]); //PARTE 1
//    glVertex2fv(vertices_three[3]);
//
//    glEnd();


    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(commonKeyboard);

    init();

    glutMainLoop();
    return 0;
}
