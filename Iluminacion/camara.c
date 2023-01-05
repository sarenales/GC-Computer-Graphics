#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "definitions.h"
#include "load_obj.h"
#include "display.h"

extern camara *_first_camara;
extern camara *_selected_camara;
extern camara *_object_camara;


extern object3d *_first_object;
extern object3d *_selected_object;

extern int referencia;
extern int modo;

extern int vista;      
extern int proyeccion; 

/**
 * obtenemos la matriz de cambio de sistema de referencia a partir de la de objeto
 * @param c la camara
 */
void matriz_inversa(camara *c){
    c->M[0] = c->Minv[0];
    c->M[4] = c->Minv[1];
    c->M[8] = c->Minv[2];
    c->M[12] = (-1)*(c->Minv[12] * c->Minv[0] +
                  c->Minv[13] * c->Minv[1] +
                  c->Minv[14] * c->Minv[2]);

    c->M[1] = c->Minv[4];
    c->M[5] = c->Minv[5];
    c->M[9] = c->Minv[6];
    c->M[13] = (-1)*(c->Minv[12] * c->Minv[4] +
                 c->Minv[13] * c->Minv[5] +
                 c->Minv[14] * c->Minv[6]);

    c->M[2] = c->Minv[8];
    c->M[6] = c->Minv[9];
    c->M[10] = c->Minv[10];
    c->M[14] = (-1)*(c->Minv[12] * c->Minv[8] +
                 c->Minv[13] * c->Minv[9] +
                 c->Minv[14] * c->Minv[10]);

    c->M[3] = 0;
    c->M[7] = 0;
    c->M[11] = 0;
    c->M[15] = 1;
}               


void add_camera_mode_obj(object3d *obj)
{

    _object_camara->Minv[0] = obj->mptr->M[0];
    _object_camara->Minv[1] = obj->mptr->M[1];
    _object_camara->Minv[2] = obj->mptr->M[2];
    _object_camara->Minv[3] = 0;

    _object_camara->Minv[4] = obj->mptr->M[4];
    _object_camara->Minv[5] = obj->mptr->M[5];
    _object_camara->Minv[6] = obj->mptr->M[6];
    _object_camara->Minv[7] = 0;

    _object_camara->Minv[8] = obj->mptr->M[8];
    _object_camara->Minv[9] = obj->mptr->M[9];
    _object_camara->Minv[10] = (-1) * (obj->mptr->M[10]);
    _object_camara->Minv[11] = 0;

    _object_camara->Minv[12] = obj->mptr->M[12];
    _object_camara->Minv[13] = obj->mptr->M[13];
    _object_camara->Minv[14] = obj->mptr->M[14];
    _object_camara->Minv[15] = 1;


    matriz_inversa(_object_camara);
	//obtenerMCSR(_object_camara->M, _object_camara->Minv);
	
}


void modo_analisis(int x, int y){

    if(_selected_object != 0) {
        GLdouble px, py, pz, distance;

        px = _selected_object->mptr->M[12] - _selected_camara->Minv[12];
        py = _selected_object->mptr->M[13] - _selected_camara->Minv[13];
        pz = _selected_object->mptr->M[14] - _selected_camara->Minv[14];

        distance = sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(_selected_camara->Minv);
        glTranslated(0, 0, -distance);
        glRotated(8.0d, -x, -y, 0);
        glTranslated(0, 0, distance);
        glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->Minv);

        matriz_inversa(_selected_camara);
    }

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
	c->proj.lejos = 10000.0;
	c->proj.cerca = 0.1;


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*gluLookAt(pos_camara.x, pos_camara.y, pos_camara.z,
				front_cam.x, front_cam.y, front_cam.z,
				up_cam.x, up_cam.y, up_cam.z);
	*/
	glGetDoublev(GL_MODELVIEW_MATRIX, c->M);
	
	obtenerMCSR(c->M, c->Minv);
	
	// c->Minv[0] = c->M[0];
	// c->Minv[1] = c->M[4];
    // c->Minv[2] = c->M[8];
    // c->Minv[3] = 0;

    // c->Minv[4] = c->M[1];
    // c->Minv[5] = c->M[5];
    // c->Minv[6] = c->M[9];
    // c->Minv[7] = 0;

    // c->Minv[8] = c->M[2];
    // c->Minv[9] = c->M[6];
    // c->Minv[10] = c->M[10];
    // c->Minv[11] = 0;

    // c->Minv[12] = pos_camara.x;
    // c->Minv[13] = pos_camara.y;
    // c->Minv[14] = pos_camara.z;
    // c->Minv[15] = 1;
	

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
	pos_cam.z = 0.0;
	
	
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
 * Obtenemos la matriz de objeto para una nueva cámara
 * @param obj el objeto
 */
void add_camara_mode_obj(object3d *obj)
{

    _object_camara->Minv[0] = obj->mptr->M[0];
    _object_camara->Minv[1] = obj->mptr->M[1];
    _object_camara->Minv[2] = obj->mptr->M[2];
    _object_camara->Minv[3] = 0;

    _object_camara->Minv[4] = obj->mptr->M[4];
    _object_camara->Minv[5] = obj->mptr->M[5];
    _object_camara->Minv[6] = obj->mptr->M[6];
    _object_camara->Minv[7] = 0;

    _object_camara->Minv[8] = obj->mptr->M[8];
    _object_camara->Minv[9] = obj->mptr->M[9];
    _object_camara->Minv[10] = (-1)* (obj->mptr->M[10]);
    _object_camara->Minv[11] = 0;

    _object_camara->Minv[12] = obj->mptr->M[12];
    _object_camara->Minv[13] = obj->mptr->M[13];
    _object_camara->Minv[14] = obj->mptr->M[14];
    _object_camara->Minv[15] = 1;


    matriz_inversa(_object_camara);
}

/**
 * Método para inicializar todas las cámaras
 */
void default_cameras(){


    camara *aux = (camara*)malloc(sizeof(camara));

    _object_camara = (camara*) malloc(sizeof(camara));
    _object_camara->tipo_proyeccion = PERSPECTIVA;
	
   // _object_camara->proj = (proy*)malloc(sizeof (proy));
    _object_camara->proj.izq = (-1)*(0.1);
    _object_camara->proj.der = 0.1;
    _object_camara->proj.alto = 0.1;
    _object_camara->proj.bajo = (-1)*(0.1);
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
