#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "io.h"
#include "definitions.h"

extern object3d *_selected_object;
extern object3d *caobjeto_luzect;
extern camara * _selected_camara;
extern object3d *_camara_objeto;

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

void m_foco(int f){
    int i;
    for (i = 0; i < 16; i++){
        if(f==2) {
            global_lights[f].objeto_luz[i] = _selected_object->mptr->M[i];
        }else if(f==3){
            global_lights[f].objeto_luz[i] = _selected_camara->Minv[i];
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


/**
 * Establece los parámetros del foco asociado a la cámara.
 * Está pensado para aplicarse en el caso de que las cámaras tengan distintas representaciones
 */
void foco_camara(){
    global_lights[3].position[0] = (_camara_objeto->max.x + _camara_objeto->min.x) / 2;
    global_lights[3].position[1] = (_camara_objeto->max.y + _camara_objeto->min.y) / 2;
    global_lights[3].position[2] = (_camara_objeto->max.z + _camara_objeto->min.z) / 2;
    global_lights[3].position[3] = 1.0f;

    global_lights[3].ambient[0] = 1.5f;
    global_lights[3].ambient[1] = 1.5f;
    global_lights[3].ambient[2] = 1.5f;
    global_lights[3].ambient[3] = 1.0f;

    global_lights[3].diffuse[0] = 1.5f;
    global_lights[3].diffuse[1] = 1.5f;
    global_lights[3].diffuse[2] = 1.5f;
    global_lights[3].diffuse[3] = 1.0f;

    global_lights[3].specular[0] = 1.0f;
    global_lights[3].specular[1] = 1.0f;
    global_lights[3].specular[2] = 1.0f;
    global_lights[3].specular[3] = 1.0f;

    global_lights[3].cut_off = 45.0f;

    global_lights[3].spot_direction[0] = 0;
    global_lights[3].spot_direction[1] = 0;
    global_lights[3].spot_direction[2] = -1;

    m_foco(3);

}
/**
 * Función para establecer los valores por defecto del foco asociado al objeto
 */
void foco_obj(){

    global_lights[2].ambient[0] = 1.5f;
    global_lights[2].ambient[1] = 1.5f;
    global_lights[2].ambient[2] = 1.5f;
    global_lights[2].ambient[3] = 1.0f;

    global_lights[2].diffuse[0] = 1.5f;
    global_lights[2].diffuse[1] = 1.5f;
    global_lights[2].diffuse[2] = 1.5f;
    global_lights[2].diffuse[3] = 1.0f;

    global_lights[2].specular[0] = 1.0f;
    global_lights[2].specular[1] = 1.0f;
    global_lights[2].specular[2] = 1.0f;
    global_lights[2].specular[3] = 1.0f;

    global_lights[2].cut_off = 45.0f;

    global_lights[2].spot_direction[0] = 0.0f;
    global_lights[2].spot_direction[1] = 0.0f;
    global_lights[2].spot_direction[2] = 1.0f;

}
/**
 * Inicializa los valores por defecto de las luces
 */
void inicializar_luces(){


    global_lights[0].position[0] = 1.0f;
    global_lights[0].position[1] = 1.0f;
    global_lights[0].position[2] = 0.0f;
    global_lights[0].position[3] = 1.0f;
    global_lights[0].ambient[0] = 1.2f;
    global_lights[0].ambient[1] = 1.2f;
    global_lights[0].ambient[2] = 1.2f;
    global_lights[0].ambient[3] = 1.0f;
    global_lights[0].diffuse[0] = 1.0f;
    global_lights[0].diffuse[1] = 1.0f;
    global_lights[0].diffuse[2] = 1.0f;
    global_lights[0].diffuse[3] = 1.0f;
    global_lights[0].specular[0] = 1.0f;
    global_lights[0].specular[1] = 1.0f;
    global_lights[0].specular[2] = 1.0f;
    global_lights[0].specular[3] = 1.0f;

    identity(global_lights[0].objeto_luz);
    global_lights[0].tipo_luz = BOMBILLA;
    global_lights[0].activado = 0;


    global_lights[1].position[0] = 0.0f;
    global_lights[1].position[1] = 1.0f;
    global_lights[1].position[2] = 0.0f;
    global_lights[1].position[3] = 0.0f;
    global_lights[1].ambient[0] = 1.2f;
    global_lights[1].ambient[1] = 1.2f;
    global_lights[1].ambient[2] = 1.2f;
    global_lights[1].ambient[3] = 1.0f;
    global_lights[1].diffuse[0] = 1.0f;
    global_lights[1].diffuse[1] = 1.0f;
    global_lights[1].diffuse[2] = 1.0f;
    global_lights[1].diffuse[3] = 1.0f;
    global_lights[1].specular[0] = 1.0f;
    global_lights[1].specular[1] = 1.0f;
    global_lights[1].specular[2] = 1.0f;
    global_lights[1].specular[3] = 1.0f;

    identity(global_lights[1].objeto_luz);
    global_lights[1].tipo_luz = SOL;
    global_lights[1].activado = 0;

    global_lights[2].tipo_luz = FOCO_OBJETO;
    global_lights[3].tipo_luz = FOCO_OBJETO;
    for(int i=2; i<8; i++) {
        global_lights[i].activado = 0;
        if(i>3){
            global_lights[i].tipo_luz = NONE;
        }
    }

    foco_camara();
    foco_obj();

}

