#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glu.h>
#include "definitions.h"

void display(void);
void reshape(int width, int height);
int poligono_delanterp(object3d *o, face f);

#endif // DISPLAY_H
