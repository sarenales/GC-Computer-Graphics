#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "display.h"
#include "io.h"
#include "definitions.h"
#include "load_obj.h"
#include "camara.h"

/** GLOBAL VARIABLES **/

GLdouble _window_ratio;                     /*Control of window's proportions */
GLdouble _ortho_x_min,_ortho_x_max;         /*Variables for the control of the orthographic projection*/
GLdouble _ortho_y_min ,_ortho_y_max;        /*Variables for the control of the orthographic projection*/
GLdouble _ortho_z_min,_ortho_z_max;         /*Variables for the control of the orthographic projection*/

object3d * _first_object= 0;                /*List of objects*/
object3d * _selected_object = 0;            /*Object currently selected*/
camara *_first_camara=0;
camara *_selected_camara=0;

elemM *m1=0;
elemM *m2=0;


int referencia = 0;                         /*global, local --> para obj*/
int modo = 0;                               /*obj, cam, camaraobj, iluminacion*/


int proyeccion=0;                           /* perspectiva, paralela*/
int vista=0;                                /* vuelo, analisis --> para cam*/




/** GENERAL INITIALIZATION **/
void initialization (){

    /*Initialization of all the variables with the default values*/
    _ortho_x_min = KG_ORTHO_X_MIN_INIT;
    _ortho_x_max = KG_ORTHO_X_MAX_INIT;
    _ortho_y_min = KG_ORTHO_Y_MIN_INIT;
    _ortho_y_max = KG_ORTHO_Y_MAX_INIT;
    _ortho_z_min = KG_ORTHO_Z_MIN_INIT;
    _ortho_z_max = KG_ORTHO_Z_MAX_INIT;

    _window_ratio = (GLdouble) KG_WINDOW_WIDTH / (GLdouble) KG_WINDOW_HEIGHT;

    /*Definition of the background color*/
    glClearColor(KG_COL_BACK_R, KG_COL_BACK_G, KG_COL_BACK_B, KG_COL_BACK_A);

    _selected_camara =(camara*) malloc(sizeof(camara));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
     glGetDoublev(GL_MODELVIEW_MATRIX,_selected_camara->M);
     _first_camara=_selected_camara;
    
    
    // _first_camara =(camara*) malloc(sizeof(camara));
    // printf("MAIN llega aqui 1\n");
   
    // vector3* a;
    // a->x = 5;
    // a->y = 5;
    // a->z = 5;
    
    // m1 =(elemM*) malloc( sizeof(elemM));
    // m2 =(elemM*) malloc( sizeof(elemM));

    // printf("MAIN llega aqui 2\n");
    
    
    
    //printf("llega aqui 2.1\n");
    // glLoadIdentity();
    // printf("MAIN no le gusta el glulookat \n");
    // gluLookAt(a->x,a->y,a->z,0,0,0,0,1,0);
    // gluLookAt(5,5,5,0,0,0,0,1,0);
    // printf("MAIN llega aqui 3:  le gusta el glulookat \n");
    
    // glGetDoublev(GL_MODELVIEW_MATRIX,_selected_camara->M);
    // _first_camara=_selected_camara;
    // printf("MAIN llega aqui 4\n");
    
    // get_matriz_objeto(m1->M, a, m2->M);
    // printf("MAIN llega aqui 5\n");
    
    // _selected_camara->projection[0] = -0.1;
    // _selected_camara->projection[1] = 0.1;
    // _selected_camara->projection[2] = -0.1;
    // _selected_camara->projection[3] = 0.1;
    // _selected_camara->projection[4] = 0.1;
    // _selected_camara->projection[5] = 1000;
    // printf("MAIN llega aqui 6\n");
    
    // cargar_obj_camara(_selected_camara);
    // _selected_camara->next = 0;
    // _selected_camara=_first_camara;
    
    /*Definition of the method to draw the objects*/
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

    

}


/** MAIN FUNCTION **/
int main(int argc, char** argv) {

    /*First of all, print the help information*/
    print_help();

    /* glut initializations */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(KG_WINDOW_WIDTH, KG_WINDOW_HEIGHT);
    glutInitWindowPosition(KG_WINDOW_X, KG_WINDOW_Y);
    glutCreateWindow(KG_WINDOW_TITLE);

    /* set the callback functions */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);          // atiende a las teclas centrales
    glutSpecialFunc(keyboardspecial);    // atiende a las teclas especiales

    /* this initialization has to be AFTER the creation of the window */
    initialization();

    /* start the main loop */
    glutMainLoop();
    return 0;
}
