#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

#include "definitions.h"
#include "camara.h"
/** EXTERNAL VARIABLES **/

extern GLdouble _window_ratio;
extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern object3d *_first_object;
extern object3d *_selected_object;

extern camara *_first_camara;
extern camara *_selected_camara;
extern camara *_object_camara;

extern int modo;         
extern int proyeccion;                  
extern int referencia ; 




/**
 * @brief Function to calculate de matrix Mcsr of the camera
 */
void obtenerMCSR(GLdouble *M, GLdouble *MCSR){
    MCSR[0]=M[0];
    MCSR[1]=M[4];
    MCSR[2]=M[8];
    MCSR[3]=0;
    MCSR[4]=M[1];
    MCSR[5]=M[5];
    MCSR[6]=M[9];
    MCSR[7]=0;
    MCSR[8]=M[2];
    MCSR[9]=M[6];
    MCSR[10]=M[10];
    MCSR[11]=0;
    MCSR[12]=-(M[0]*M[12]+M[1]*M[13]+M[2]*M[14]);
    MCSR[13]=-(M[4]*M[12]+M[5]*M[13]+M[6]*M[14]);
    MCSR[14]=-(M[8]*M[12]+M[9]*M[13]+M[10]*M[14]);
    MCSR[15]=1;
}


GLdouble producto_escalar(point3 uno, vector3 normal, GLfloat *mo, GLfloat *mc){

    vector3 n, co, cero;
    GLfloat m[16];
    matriz_csr(m, mo);

    n.x = mc[12] * m[0] + mc[13] * m[4] + mc[14] * m[8] + m[12];
    n.y = mc[12] * m[1] + mc[13] * m[5] + mc[14] * m[9] + m[13];
    n.z = mc[12] * m[2] + mc[13] * m[6] + mc[14] * m[10] + m[14];

    co.x = n.x - uno.x;
    co.y = n.y - uno.y;
    co.z = n.z - uno.z;



    return (co.x * normal.x) + (co.y * normal.y) + (co.z * normal.z);
}

int poligono_delantero(object3d *o,face f){
	/*First we get the camera location*/
	vector3 e;
	e.x = _selected_camara->M[12]; 
	e.y = _selected_camara->M[13];
	e.z = _selected_camara->M[14];
	
	/*Then, we pass the camera location to the object reference system*/
	GLdouble csr_objeto[16];
	obtenerMCSR(_selected_object->mptr->M,csr_objeto);
	e.x = e.x*csr_objeto[0] + e.y*csr_objeto[1] + e.z*csr_objeto[2];
	e.y = e.x*csr_objeto[4] + e.y*csr_objeto[5] + e.z*csr_objeto[6];
	e.z = e.x*csr_objeto[8] + e.y*csr_objeto[9] + e.z*csr_objeto[10];
	
	/*Now we calculate the vector between the camera and the face*/
	
	vector3 v;
	vertex p_obj = o->vertex_table[f.vertex_table[0]];
	v.x = p_obj.coord.x - e.x;
	v.y = p_obj.coord.y - e.y;
	v.z = p_obj.coord.z - e.z;
	
	/*Finally we calculate the escalar product of the normal and the v vector to see the angle between them*/
	
	GLdouble escalar = v.x * f.normal.x + v.y * f.normal.y + v.z * f.normal.z;
	
	if(escalar < 0){
		return 0;
	}
	else{
		return 1;
	}
}

/**
 * @brief Function to draw the axes
 */
void draw_axes()
{
    /*Draw X axis*/
    glColor3f(KG_COL_X_AXIS_R,KG_COL_X_AXIS_G,KG_COL_X_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(KG_MAX_DOUBLE,0,0);
    glVertex3d(-1*KG_MAX_DOUBLE,0,0);
    glEnd();
    /*Draw Y axis*/
    glColor3f(KG_COL_Y_AXIS_R,KG_COL_Y_AXIS_G,KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,KG_MAX_DOUBLE,0);
    glVertex3d(0,-1*KG_MAX_DOUBLE,0);
    glEnd();
    /*Draw Z axis*/
    glColor3f(KG_COL_Z_AXIS_R,KG_COL_Z_AXIS_G,KG_COL_Z_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,0,KG_MAX_DOUBLE);
    glVertex3d(0,0,-1*KG_MAX_DOUBLE);
    glEnd();
}


/**
 * @brief Callback reshape function. We just store the new proportions of the window
 * @param width New width of the window
 * @param height New height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    /*  Take care, the width and height are integer numbers, but the ratio is a GLdouble so, in order to avoid
     *  rounding the ratio to integer values we need to cast width and height before computing the ratio */
    _window_ratio = (GLdouble) width / (GLdouble) height;
}

/**
 * @brief Callback display function
 */
void display(void) {
    GLint v_index, v, f;
    object3d *aux_obj = _first_object;
    camara *aux_camara = _first_camara;
    GLdouble MCSR[16];

    /* Clear the screen */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

 
    if( proyeccion == PERSPECTIVA) { // perspectiva
        glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 100.0);
    }
    else if(proyeccion == PARALELO){ // paralela
        glOrtho(-5.0, 5.0, -5.0, 5.0, 0.0, 100.0);
    }


    

    if((_selected_object!=NULL) && (modo==OBJETO || modo==CAMARAOBJETO)){
        obtenerMCSR(_selected_object->mptr->M, MCSR);
    }
	if((_selected_object!=NULL) && (modo==OBJETO || modo==CAMARAOBJETO)){
		obtenerMCSR();
	}
    if ((_selected_object!=NULL) && (_selected_camara!=NULL) && (modo==CAMARA)){
        obtenerMCSR(_selected_camara->M, MCSR);
    }


    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glLoadMatrixd(MCSR);
	if(modo != CAMARAOBJETO){
		glLoadMatrixd(_selected_camara->M);
	}else
		glLoadMatrixd(_object_camara->M);
    
    /*First, we draw the axes*/
    draw_axes();

   
    
    /*Now each of the objects in the list*/
    while(aux_obj != 0) {
        
        /* Select the color, depending on whether the current object is the selected one or not */
        if (aux_obj == _selected_object){
            glColor3f(KG_COL_SELECTED_R,KG_COL_SELECTED_G,KG_COL_SELECTED_B);
        }else{
            glColor3f(KG_COL_NONSELECTED_R,KG_COL_NONSELECTED_G,KG_COL_NONSELECTED_B);
        }


        /* Draw the object; for each face create a new polygon with the corresponding vertices */
  
        glLoadMatrixd(MCSR);
        glMultMatrixd(aux_obj->mptr->M);
        // dibujar los objetos
        
        // es_visible(object3d *optr, aux_obj->num_faces, punto3 *pptr)
        
        for (f = 0; f < aux_obj->num_faces; f++) {
            glBegin(GL_POLYGON);
            for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
                v_index = aux_obj->face_table[f].vertex_table[v];
                glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                        aux_obj->vertex_table[v_index].coord.y,
                        aux_obj->vertex_table[v_index].coord.z);
            }
            glEnd();
        }
        aux_obj = aux_obj->next; 
    }
    /*Do the actual drawing*/
    glFlush();
}
