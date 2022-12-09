#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "definitions.h"
#include "load_obj.h"

extern camara *_first_camara;
extern camara *_selected_camara;

extern object3d *_first_object;
extern object3d *_selected_object;

extern int referencia;
extern int modo;

extern int vista;      
extern int proyeccion;                

void get_matriz_objeto(GLdouble* m1, vector3* e,GLdouble* m2){
    for(int i = 0; i < 3; i++){
    	for (int j = 0; j < 3;j++){
    			m2[i+4*j] = m1[4*i+j];
    	}
    }
    m2[3] = 0;
    m2[7] = 0;
    m2[11] = 0;
    m2[12] = e->x;
    m2[13] = e->y;
    m2[14] = e->z;
    m2[15] = 1;
}
void cargar_obj_camara(camara *cam){
    object3d auxiliar_object;
    
    read_wavefront("camara.obj", &auxiliar_object);

    cam->face_table = auxiliar_object.face_table;
    cam->num_faces = auxiliar_object.num_faces;
    cam->vertex_table = auxiliar_object.vertex_table;
    cam->num_vertices = auxiliar_object.num_vertices;
    cam->max = auxiliar_object.max;
    cam->min = auxiliar_object.min;

}

void mirar_obj_selec(){
		vector3 obj,cam;
        
		obj.x = _selected_object->mptr->M[12];
		obj.y = _selected_object->mptr->M[13];
		obj.z = _selected_object->mptr->M[14];
        
		cam.x = _selected_camara->M[12];
		cam.y = _selected_camara->M[13];
		cam.z = _selected_camara->M[14];
        
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(cam.x,cam.y,cam.z,obj.x,obj.y,obj.z,0,1,0);
		glGetDoublev(GL_MODELVIEW_MATRIX,_selected_camara->M);
}


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
