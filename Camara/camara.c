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
extern camara *_object_camara;


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
	
		point3 at, E;
		vector3 V;
		
		at.x = _selected_object->mptr->M[12];
		at.y = _selected_object->mptr->M[13];
		at.z = _selected_object->mptr->M[14];
		
		// eje de la camara
		V.x = _selected_camara->M[4];
		V.y = _selected_camara->M[5];
		V.z = _selected_camara->M[6];
		
		glLoadIdentity();
		glTranslated(at.x, at.y, at.z);
		glRotated(V.x, V.y, V.z, 0);
		glTranslated(-(at.x),-(at.y),-(at.y));
		glMultMatrixd(_selected_camara->M);
		glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->M);
		
		// vector3 obj,cam;
        
		// obj.x = _selected_object->mptr->M[12];
		// obj.y = _selected_object->mptr->M[13];
		// obj.z = _selected_object->mptr->M[14];
        
		// cam.x = _selected_camara->M[12];
		// cam.y = _selected_camara->M[13];
		// cam.z = _selected_camara->M[14];
        
		// glMatrixMode(GL_MODELVIEW);
		// glLoadIdentity();
		// gluLookAt(cam.x,cam.y,cam.z,obj.x,obj.y,obj.z,0,1,0);
		// glGetDoublev(GL_MODELVIEW_MATRIX,_selected_camara->M);
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
	
	// c->proj = (proy) malloc(sizeof (proy));
    
	c->proj.izq = -0.1;
	c->proj.der = 0.1;
	c->proj.alto = 0.1;
	c->proj.bajo = -0.1;
	c->proj.lejos = 1000.0;
	c->proj.cerca = 0.1;


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos_camara.x, pos_camara.y, pos_camara.z,
				front_cam.x, front_cam.y, front_cam.z,
				up_cam.x, up_cam.y, up_cam.z);
				
	glGetDoublev(GL_MODELVIEW_MATRIX, c->M);
	
	c->Minv[0] = c->M[0];
	c->Minv[1] = c->M[4];
    c->Minv[2] = c->M[8];
    c->Minv[3] = 0;

    c->Minv[4] = c->M[1];
    c->Minv[5] = c->M[5];
    c->Minv[6] = c->M[9];
    c->Minv[7] = 0;

    c->Minv[8] = c->M[2];
    c->Minv[9] = c->M[6];
    c->Minv[10] = c->M[10];
    c->Minv[11] = 0;

    c->Minv[12] = pos_camara.x;
    c->Minv[13] = pos_camara.y;
    c->Minv[14] = pos_camara.z;
    c->Minv[15] = 1;
	

}

void add_camara_to_list(camara *cam){
	cam->next = _first_camara;
	_first_camara = cam;
	_selected_camara = _first_camara;
}


void add_camara(){
	camara *c = (camara*) malloc(sizeof(camara));
	
	vector3 pos_cam;
	pos_cam.x = 5.0;
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

void inicial_camaras(){


    camara *aux = (camara*)malloc(sizeof(camara));

    _object_camara = (camara*)malloc(sizeof(camara));
    
	_object_camara->tipo_proyeccion = PERSPECTIVA;
	
    //_object_camara->proj = (proy*)malloc(sizeof (proy));
    _object_camara->proj.izq = -0.1;
    _object_camara->proj.der = 0.1;
    _object_camara->proj.alto= 0.1;
    _object_camara->proj.bajo = -0.1;
    _object_camara->proj.cerca = 0.1;
    _object_camara->proj.lejos = 1000.0;

    vector3 cam_pos;
    cam_pos.x = 5.0f;
    cam_pos.y = 5.0f;
    cam_pos.z = -3.0f;

    vector3 cam_front;
    cam_front.x = 0.0f;
    cam_front.y = 0.0f;
    cam_front.z = 0.0f;

    vector3 cam_up;
    cam_up.x = 0.0f;
    cam_up.y = 1.0f;
    cam_up.z = 0.0f;

    create_camara(cam_pos, cam_front, cam_up, aux);

    _first_camara = (camara*)malloc(sizeof(camara));
    _first_camara = aux;

    _selected_camara = _first_camara;
}

