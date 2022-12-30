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

object3d * _first_object = 0;               /*List of objects*/
object3d * _selected_object = 0;            /*Object currently selected*/
object3d * _camara_objeto = 0;

camara *_first_camara = 0;
camara *_selected_camara = 0;
camara *_object_camara = 0;

elemM *m1 = 0;
elemM *m2 = 0;


int referencia = GLOBAL;                    /*global, local --> para obj*/
int modo = OBJETO;                          /*obj, cam, camaraobj, iluminacion*/
int movimiento = TRASLADAR; 
int proyeccion = PERSPECTIVA;               /* perspectiva, paralela*/
int vista = VUELO;                          /* vuelo, analisis --> para cam*/




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
    _selected_camara->M[14] = 10;
    _first_camara=_selected_camara;
    inicial_camaras(_selected_camara);
    
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

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // queremos poligonos opacos
    
    glEnable(GL_DEPTH_TEST); // activar el test de profundidad (Z-buffer)


    // if (flat_smooth) 
        // glShadeModel(GL_SMOOTH);  // hacen falta los vectores normales de cada vertice
    // else 
        // glShadeModel(GL_FLAT);  // basta con vector normal del poligono
    


    /* set the callback functions */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);          // atiende a las teclas centrales
    glutSpecialFunc(keyboardspecial);    // atiende a las teclas especiales

    /* this initialization has to be AFTER the creation of the window */
    initialization();
    //load_presentation();
    //default_cameras();

    /* start the main loop */
    glutMainLoop();
    return 0;
}
