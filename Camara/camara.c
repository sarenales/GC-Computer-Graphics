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


void centre_camera_to_obj(object3d *obj){
	vector3 camera_pos = (vector3) { 
		.x = _selected_camara->Minv[12], 
		.y = _selected_camara->Minv[13], 
		.z = _selected_camara->Minv[14] 
	};
    vector3 camera_front = (vector3) { 
		.x = obj->mptr->M[12], 
		.y = obj->mptr->M[13], 
		.z = obj->mptr->M[14] 
	};
    vector3 camera_up = (vector3) { 
		.x = 0, 
		.y = 1, 
		.z = 0 
	};


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z,
              camera_front.x, camera_front.y, camera_front.z,
              camera_up.x, camera_up.y, camera_up.z);

    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->M);

    _selected_camara->Minv[0] = _selected_camara->M[0];
    _selected_camara->Minv[1] = _selected_camara->M[4];
    _selected_camara->Minv[2] = _selected_camara->M[8];
    _selected_camara->Minv[3] = 0;

    _selected_camara->Minv[4] = _selected_camara->M[1];
    _selected_camara->Minv[5] = _selected_camara->M[5];
    _selected_camara->Minv[6] = _selected_camara->M[9];
    _selected_camara->Minv[7] = 0;

    _selected_camara->Minv[8] = _selected_camara->M[2];
    _selected_camara->Minv[9] = _selected_camara->M[6];
    _selected_camara->Minv[10] = _selected_camara->M[10];
    _selected_camara->Minv[11] = 0;

    _selected_camara->Minv[12] = camera_pos.x;
    _selected_camara->Minv[13] = camera_pos.y;
    _selected_camara->Minv[14] = camera_pos.z;
    _selected_camara->Minv[15] = 1;


}


void create_camara(vector3 pos_camara, vector3 front_cam, vector3 up_cam,camara *c){

	c->next = 0;
	c->tipo_proyeccion = PERSPECTIVA;
	
	c->proyeccion = (proyeccion*) malloc(sizeof (proyeccion));
	c->proyeccion-> izq = -0.1;
	c->proyeccion-> der = 0.1;
	c->proyeccion-> alto = 0.1:
	c->proyeccion-> bajo = -0.1;
	c->proyeccion-> lejos = 1000.0;
	c->proyeccion-> cerca = 0.1;


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos_camara.x, pos_camara.y, pos_camara.z
				front_cam.x, front_cam.y, front_cam.z
				up_cam.x, up_cam.y, up_cam.z);
				
	glGetDoublev(GL_MODELVIEW_MATRIX, c->M);
	
	
	

}

void add_camera(){
	camara *c = (camara*) malloc(sizeof(camara));
	
	vector3 pos_cam;
	pos_cam.x = 5.0:
	pos_cam.y = 5.0;
	pos_cam.z = -3.0;
	
	
	vector3 front_cam;
	front_cam.x = 0.0;
	front_cam.y = 0.0;
	front_cam.z = 0.0;
	
	vector3 up_cam;
	up_cam.x = 0.0;
	up_cam.y = 1.0;
	up_cam.z = 0.0;
	
	create_camara(pos_cam, front_cam, up_cam, c);
	
	add_camara_to_list(c);
	
	
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
