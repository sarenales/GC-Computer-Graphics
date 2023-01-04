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
extern int modo;         
extern int proyeccion;                  
extern int referencia ; 





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

 
    if( proyeccion== 0) { // perspectiva
        glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 100.0);
    }
    else{ // paralela
        glOrtho(-5.0, 5.0, -5.0, 5.0, 0.0, 100.0);
    }


    

    if((_selected_object!=NULL) && (modo==0)){
        obtenerMCSR(_selected_object->mptr->M, MCSR);
    }
    if ((_selected_object!=NULL) && (_selected_camara!=NULL) && (modo==1)){
        obtenerMCSR(_selected_camara->M, MCSR);
    }


    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glLoadMatrixd(MCSR);
    
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