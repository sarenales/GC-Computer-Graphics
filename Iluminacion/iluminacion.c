#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "io.h"
#include "definitions.h"

extern object3d *_selected_object;
extern object3d *cam_object;
extern camara * _selected_camara;

extern int luz;
extern int _selected_light; 

extern iluminacion_objetos global_lights[8]; 

/**
 * función para obtener una matriz identidad en el formato de opengl
 * @param m matriz a convertir en identidad
 */
void identity(GLfloat *m){
    int i;
    for(i=0;i<16;i++){
        if(i%5==0){//si es multiplo de 5, en este caso, hay que poner uno para que sea identidad
            m[i]=1.0f;
        }else{
            m[i]=0.0f;
        }
    }
}

void put_light(GLint i){
	switch (i){
        case 0:
            glLightfv(GL_LIGHT0, GL_POSITION, global_lights[i].position);
            glLightfv(GL_LIGHT0, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, global_lights[i].specular);
            break;

        case 1:
            glLightfv(GL_LIGHT1, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT1, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT1, GL_SPECULAR, global_lights[i].specular);
            glLightfv(GL_LIGHT1, GL_POSITION, global_lights[i].position);
            break;

        case 2:
            glLightfv(GL_LIGHT2, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT2, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT2, GL_SPECULAR, global_lights[i].specular);
            glLightfv(GL_LIGHT2, GL_POSITION, global_lights[i].position);
            glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
            glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, global_lights[i].cut_off);
            break;

        case 3:
            glLightfv(GL_LIGHT3, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT3, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT3, GL_SPECULAR, global_lights[i].specular);
            glLightfv(GL_LIGHT3, GL_POSITION, global_lights[i].position);
            glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
            glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, global_lights[i].cut_off);

            break;

        case 4:
            glLightfv(GL_LIGHT4, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT4, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT4, GL_SPECULAR, global_lights[i].specular);
            glLightfv(GL_LIGHT4, GL_POSITION, global_lights[i].position);

            if (global_lights[i].tipo_luz == FOCO){
                glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
                glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, global_lights[i].cut_off);
            }
            break;

        case 5:
            glLightfv(GL_LIGHT5, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT5, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT5, GL_SPECULAR, global_lights[i].specular);
            glLightfv(GL_LIGHT5, GL_POSITION, global_lights[i].position);

            if (global_lights[i].tipo_luz == FOCO){
                glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
                glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, global_lights[i].cut_off);
            }
            break;

        case 6:
            glLightfv(GL_LIGHT6, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT6, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT6, GL_SPECULAR, global_lights[i].specular);
            glLightfv(GL_LIGHT6, GL_POSITION, global_lights[i].position);

            if (global_lights[i].tipo_luz == FOCO){
                glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
                glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, global_lights[i].cut_off);
            }
            break;

        case 7:
            glLightfv(GL_LIGHT7, GL_AMBIENT, global_lights[i].ambient);
            glLightfv(GL_LIGHT7, GL_DIFFUSE, global_lights[i].diffuse);
            glLightfv(GL_LIGHT7, GL_SPECULAR, global_lights[i].specular);
            glLightfv(GL_LIGHT7, GL_POSITION, global_lights[i].position);

            if (global_lights[i].tipo_luz == FOCO){
                glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
                glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, global_lights[i].cut_off);
            }
            break;

        default:
            break;
    }
}
/**
* función para añadir una luz a los espacios 5,6,7,8
*/
void add_lights(){
		GLint luzz, values;


		if(_selected_light>=0 && _selected_light<=3){
			printf("Error, ten seleccionada una luz entre la 5 y la 8\n");
			return;
		}


		printf("Elige el tipo de luz: 1 SOL, 2 BOMBILLA, 3 FOCO.\n");
		scanf("%d", &luzz);

		while (luzz < 1 || luzz > 3){
			printf("Error, elije entre 1 y 3");
			scanf("%d", &luzz);
		}

		global_lights[_selected_light].ambient[0] = 1.2f;
		global_lights[_selected_light].ambient[1] = 1.2f;
		global_lights[_selected_light].ambient[2] = 1.2f;
		global_lights[_selected_light].ambient[3] = 1.0f;

		global_lights[_selected_light].diffuse[0] = 1.0f;
		global_lights[_selected_light].diffuse[1] = 1.0f;
		global_lights[_selected_light].diffuse[2] = 1.0f;
		global_lights[_selected_light].diffuse[3] = 1.0f;

		global_lights[_selected_light].specular[0] = 1.0f;
		global_lights[_selected_light].specular[1] = 1.0f;
		global_lights[_selected_light].specular[2] = 1.0f;
		global_lights[_selected_light].specular[3] = 1.0f;

		if(luzz == 1) {
			global_lights[_selected_light].position[1] = 1.0;
			global_lights[_selected_light].position[3] = 0.0;
		}else{

			global_lights[_selected_light].position[1] = 0.0;
			global_lights[_selected_light].position[3] = 1.0;
		}
		global_lights[_selected_light].position[2] = 0.0;
		global_lights[_selected_light].position[0] = 0.0;

		identity(global_lights[_selected_light].objeto_luz);

		switch (luzz){
			case 1:
				global_lights[_selected_light].tipo_luz = SOL;
				break;

			case 2:
				global_lights[_selected_light].tipo_luz = BOMBILLA;
				break;

			case 3:
				global_lights[_selected_light].tipo_luz = FOCO;

				global_lights[_selected_light].cut_off = 45.0f;
				global_lights[_selected_light].spot_direction[0] = 0.0f;
				global_lights[_selected_light].spot_direction[1] = 0.0f;
				global_lights[_selected_light].spot_direction[2] = 1.0f;

				break;

		}

		global_lights[_selected_light].activado = 0;
		printf("luz %d preparada, enciendela para usarla\n",_selected_light+1);

}
