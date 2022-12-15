#include <GL/glu.h>
#include "definitions.h"

void get_matriz_objeto(GLdouble* m1, vector3* e,GLdouble* m2);
void cargar_obj_camara(camara *cam);
void add_camara();
void add_camara_to_list(camara *cam);
void create_camara(vector3 pos_camara, vector3 front_cam, vector3 up_cam,camara *c);
void centre_camera_to_obj(object3d *obj);
void mirar_obj_selec();
void inicial_camaras();
