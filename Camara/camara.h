#include <GL/glu.h>
#include "definitions.h"

void default_cameras();
void add_camara_mode_obj(object3d *obj);
void matriz_inversa(camara *c);
void modo_analisis(int x, int y);
void centre_camera_to_obj(object3d *obj);
void create_camara(vector3 pos_camara, vector3 front_cam, vector3 up_cam,camara *c);
void add_camara_to_list(camara *cam);
void add_camara();
