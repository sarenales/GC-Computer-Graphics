#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glu.h>
#include "definitions.h"

void obtenerMCSR(GLfloat *M, GLfloat *MCSR);
int poligono_delantero(object3d *o, face f);
void display(void);
void reshape(int width, int height);

#endif // DISPLAY_H
