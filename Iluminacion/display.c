#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>

#include "definitions.h"
#include "camara.h"
#include "iluminacion.h"

/** EXTERNAL VARIABLES **/

extern GLdouble _window_ratio;
extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern object3d *_first_object;
extern object3d *_selected_object;
extern object3d *_camara_objeto;
extern object3d *light_object;


extern camara *_first_camara;
extern camara *_selected_camara;
extern camara *_object_camara;

extern int modo;         
extern int proyeccion;                  
extern int referencia ; 

extern int luz;
extern int _selected_light; 
extern iluminacion_objetos global_lights[8]; 
extern material *mat_camara, *mat_selec, *mat_foco;

void obtenerMCSR(GLdouble *M, GLdouble *MCSR);

/**
 * función para calcular el producto escalar entre la cámara y un polígono
 * @param uno punto del polígono
 * @param normal vector normal del polígono
 * @param mo  matriz de objeto del polígono
 * @param mc  matriz de la cámara
 * @return
 */
GLdouble producto_escalar(point3 uno, vector3 normal, GLdouble *mo, GLdouble *mc){

    vector3 n, co, cero;
    GLdouble m[16];
    obtenerMCSR(m, mo);

    n.x = mc[12] * m[0] + mc[13] * m[4] + mc[14] * m[8] + m[12];
    n.y = mc[12] * m[1] + mc[13] * m[5] + mc[14] * m[9] + m[13];
    n.z = mc[12] * m[2] + mc[13] * m[6] + mc[14] * m[10] + m[14];

    co.x = n.x - uno.x;
    co.y = n.y - uno.y;
    co.z = n.z - uno.z;



    return (co.x * normal.x) + (co.y * normal.y) + (co.z * normal.z);
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
    MCSR[12]=(-1) * (M[0]*M[12]+M[1]*M[13]+M[2]*M[14]);
    MCSR[13]=(-1) * (M[4]*M[12]+M[5]*M[13]+M[6]*M[14]);
    MCSR[14]=(-1) * (M[8]*M[12]+M[9]*M[13]+M[10]*M[14]);
    MCSR[15]=1;
}



int poligono_delantero(object3d *o,face f){
	/*First we get the camera location*/
	vector3 e;
	e.x = _selected_camara->M[12]; 
	e.y = _selected_camara->M[13];
	e.z = _selected_camara->M[14];
	
	/*Then, we pass the camera location to the object reference system*/
	GLdouble csr_objeto[16];
	obtenerMCSR(_selected_object->mptr->M,csr_objeto);
	e.x = e.x*csr_objeto[0] + e.y*csr_objeto[1] + e.z*csr_objeto[2];
	e.y = e.x*csr_objeto[4] + e.y*csr_objeto[5] + e.z*csr_objeto[6];
	e.z = e.x*csr_objeto[8] + e.y*csr_objeto[9] + e.z*csr_objeto[10];
	
	/*Now we calculate the vector between the camera and the face*/
	
	vector3 v;
	vertex p_obj = o->vertex_table[f.vertex_table[0]];
	v.x = p_obj.coord.x - e.x;
	v.y = p_obj.coord.y - e.y;
	v.z = p_obj.coord.z - e.z;
	
	/*Finally we calculate the escalar product of the normal and the v vector to see the angle between them*/
	
	GLdouble escalar = v.x * f.normal.x + v.y * f.normal.y + v.z * f.normal.z;
	
	if(escalar < 0){
		return 0;
	}
	else{
		return 1;
	}
}

/**
 * @brief Function to draw the axes
 */
void draw_axes()
{
    /*Draw X axis*/
    glColor3f(KG_COL_Y_AXIS_R,KG_COL_Y_AXIS_G,KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(KG_MAX_DOUBLE,0,0);
    glVertex3d(-1*KG_MAX_DOUBLE,0,0);
    glEnd();
    /*Draw Y axis*/
    glColor3f(KG_COL_Y_AXIS_R,KG_COL_Y_AXIS_G,KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,KG_MAX_DOUBLE,0);
    glVertex3d(0,-1*KG_MAX_DOUBLE,0);
    glEnd();
    /*Draw Z axis*/
    glColor3f(KG_COL_Z_AXIS_R,KG_COL_Z_AXIS_G,KG_COL_Z_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,0,KG_MAX_DOUBLE);
    glVertex3d(0,0,-1*KG_MAX_DOUBLE);
    glEnd();
}


/**
 * @brief Callback reshape function. We just store the new proportions of the window
 * @param width New width of the window
 * @param height New height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    /*  Take care, the width and height are integer numbers, but the ratio is a GLdouble so, in order to avoid
     *  rounding the ratio to integer values we need to cast width and height before computing the ratio */
    _window_ratio = (GLdouble) width / (GLdouble) height;
}


/**
 * @brief Callback display function
 
void display(void) {
    GLint v_index, v, f, v_aux;
    object3d *aux_obj = _first_object;
    camara *aux_camara = _first_camara;
    GLdouble MCSR[16];
	camara *cama;

	establecer matriz de proyección
	
	establecer matriz de cambio de sistema de referencia en MODELVIEW
	
	establecer parametros de las fuentes de iluminación (posiciones, direcciones, intensidades...) 
	
	dibujar objetos
		para cada objeto hay que establecer el material del que se compone
		para cada poligono o vertice hay que establecer el vector normal
		para cada objeto hay que dibujar sus poligonos con sus vertices)
		

	glutSwapBuffers();


	 /*When the window is wider than our original projection plane we extend the plane in the X axis*/
  
	/*
	    
    if ((_ortho_x_max - _ortho_x_min) / (_ortho_y_max - _ortho_y_min) < _window_ratio) {
        /* New width 
        GLdouble wd = (_ortho_y_max - _ortho_y_min) * _window_ratio;
        /* Midpoint in the X axis 
        GLdouble midpt = (_ortho_x_min + _ortho_x_max) / 2;
        /*Definition of the projection
        glOrtho(midpt - (wd / 2), midpt + (wd / 2), _ortho_y_min, _ortho_y_max, _ortho_z_min, _ortho_z_max);
    } else {/* In the opposite situation we extend the Y axis 
        /* New height 
        GLdouble he = (_ortho_x_max - _ortho_x_min) / _window_ratio;
        /* Midpoint in the Y axis 
        GLdouble midpt = (_ortho_y_min + _ortho_y_max) / 2;
        
        glOrtho(_ortho_x_min, _ortho_x_max, midpt - (he / 2), midpt + (he / 2), _ortho_z_min, _ortho_z_max);
    }

	*/    
    /* Clear the screen 
    glClear(GL_COLOR_BUFFER_BIT);
    /* Define the projection 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	if(modo == CAMARAOBJETO){
		cama = _object_camara;
	}else{
		cama = _selected_camara;
	}

	// /* Clear the screen 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // /* Define the projection *
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	/*When the window is wider than our original projection plane we extend the plane in the X axis*
    if(modo == CAMARAOBJETO){
		glFrustum(_object_camara->proj.izq,
			  _object_camara->proj.der,
			  _object_camara->proj.bajo,
			  _object_camara->proj.alto,
			  _object_camara->proj.cerca,
			  _object_camara->proj.lejos);
	}else if(_selected_camara->tipo_proyeccion == PERSPECTIVA){
		glFrustum(_selected_camara->proj.izq,
			  _selected_camara->proj.der,
			  _selected_camara->proj.bajo,
			  _selected_camara->proj.alto,
			  _selected_camara->proj.cerca,
			  _selected_camara->proj.lejos);
	}else{
		glOrtho(_selected_camara->proj.izq,
			  _selected_camara->proj.der,
			  _selected_camara->proj.bajo,
			  _selected_camara->proj.alto,
			  _selected_camara->proj.cerca,
			  _selected_camara->proj.lejos);
	}
	
	
		if(proyeccion == PERSPECTIVA) { 	// perspectiva
        glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 100.0);
    }
    else if(proyeccion == PARALELO){ 	// paralela
        glOrtho(-5.0, 5.0, -5.0, 5.0, 0.0, 100.0);
    }
	
	if((_selected_object!=NULL) && (modo==OBJETO )){
        obtenerMCSR(_selected_object->mptr->M, MCSR);
    }
	if((_selected_object!=NULL) && (modo==CAMARAOBJETO)){
		obtenerMCSR(_object_camara->M, MCSR);
	}
    if ((_selected_object!=NULL) && (_selected_camara!=NULL) && (modo==CAMARA)){
        obtenerMCSR(_selected_camara->M, MCSR);
    }	

    /* Now we start drawing the object *
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if(modo != CAMARAOBJETO){
		glLoadMatrixd(_selected_camara->M);
	}else
		glLoadMatrixd(_object_camara->M);
    
    /*First, we draw the axes*
    draw_axes();

   
    
    /*Now each of the objects in the list*
    while(aux_obj != 0) {
        if(modo != CAMARAOBJETO || aux_obj != _selected_object) {
			/* Select the color, depending on whether the current object is the selected one or not *
			if (aux_obj == _selected_object){
				glColor3f(KG_COL_SELECTED_R,KG_COL_SELECTED_G,KG_COL_SELECTED_B);
			}else{
				glColor3f(KG_COL_NONSELECTED_R,KG_COL_NONSELECTED_G,KG_COL_NONSELECTED_B);
			}

            /* Draw the object; for each face create a new polygon with the corresponding vertices *
            glPushMatrix();
			
			glLoadMatrixd(MCSR);
			glMultMatrixd(aux_obj->mptr->M);
			dibujar los objetos
			
			es_visible(object3d *optr, aux_obj->num_faces, punto3 *pptr)
			
			for (f = 0; f < aux_obj->num_faces; f++) {
				
				v_aux = aux_obj->face_table[f].vertex_table[0];
				
				if (producto_escalar(aux_obj->vertex_table[v_aux].coord, aux_obj->face_table[f].normal,
                                     aux_obj->mptr->M, cama->Minv) > 0.0) {
	
					glBegin(GL_POLYGON);
					
					for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
						
						v_index = aux_obj->face_table[f].vertex_table[v];
						
						glVertex3d(aux_obj->vertex_table[v_index].coord.x,
								aux_obj->vertex_table[v_index].coord.y,
								aux_obj->vertex_table[v_index].coord.z);
					}
					
				}
			}
            
			glPopMatrix();
		}
        aux_obj = aux_obj->next; 
    }
	
	while(aux_camara != 0){
		if(modo == CAMARAOBJETO || aux_camara != _selected_camara){
			glPushMatrix();
			glMultMatrixd(aux_camara->Minv);
					for (f = 0; f < _camara_objeto->num_faces; f++) {	
						v_aux = _camara_objeto->face_table[f].vertex_table[0];
						
						if (producto_escalar(_camara_objeto->vertex_table[v_aux].coord, _camara_objeto->face_table[f].normal,
											 _camara_objeto->mptr->M, cama->Minv) > 0.0) {
			
							glBegin(GL_POLYGON);
							
							for (v = 0; v < _camara_objeto->face_table[f].num_vertices; v++) {
								
								v_index = _camara_objeto->face_table[f].vertex_table[v];
								
								glVertex3d(_camara_objeto->vertex_table[v_index].coord.x,
										_camara_objeto->vertex_table[v_index].coord.y,
										_camara_objeto->vertex_table[v_index].coord.z);
							}
							glEnd();
						}
					}
			glPopMatrix();
		}
		aux_camara = aux_camara->next; 
	}
	/* Do the actual drawing *
	glFlush();
	glutSwapBuffers();
	
}
*/


/**
 * @brief Callback display function
 */
void display(void) {
    GLint v_index, v, f, v_aux;
    object3d *aux_obj = _first_object;
    camara *aux_camara = _first_camara;
    GLdouble MCSR[16];
	camara *camara_aux;
	
	/* Clear the screen */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	if(modo == CAMARAOBJETO)
		camara_aux = _object_camara;
	else
		camara_aux = _selected_camara;
	

	if(_selected_camara->tipo_proyeccion == PERSPECTIVA){
		glFrustum(_selected_camara->proj.izq, 
			  _selected_camara->proj.der,
			  _selected_camara->proj.bajo,
			  _selected_camara->proj.alto,
			  _selected_camara->proj.cerca,
			  _selected_camara->proj.lejos);
	}else{
		glOrtho(_selected_camara->proj.izq, 
			  _selected_camara->proj.der,
			  _selected_camara->proj.bajo,
			  _selected_camara->proj.alto,
			  _selected_camara->proj.cerca,
			  _selected_camara->proj.lejos);
	}

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	

    /*First, we draw the axes*/
    // draw_axes();
	
	if(luz == ON){
		//las fuentes de luz se representan con shadders flat
        glShadeModel(GL_FLAT);

        glMaterialdv(GL_FRONT, GL_AMBIENT, mat_foco->amb);
        glMaterialdv(GL_FRONT, GL_DIFFUSE, mat_foco->dif);
        glMaterialdv(GL_FRONT, GL_SPECULAR, mat_foco->spec);
        glMaterialdv(GL_FRONT, GL_SHININESS, mat_foco->shiny);

        for (int i = 0; i < 8; i++) {
            if (global_lights[i].tipo_luz != NONE) {
                glPushMatrix();
                glMultMatrixd(global_lights[i].objeto_luz);

                if(global_lights[i].tipo_luz == FOCO){

                    for (f = 0; f < light_object->num_faces; f++) {
                        v_aux = light_object->face_table[f].vertex_table[0];

                        

                        if (producto_escalar(light_object->vertex_table[v_aux].coord, light_object->face_table[f].normal,
                                             global_lights[i].objeto_luz, camara_aux->Minv) > 0.0) {

                            glBegin(GL_POLYGON);


                            glNormal3d(light_object->face_table[f].normal.x,
                                       light_object->face_table[f].normal.y,
                                       light_object->face_table[f].normal.z);


                            for (v = 0; v < light_object->face_table[f].num_vertices; v++) {
                                v_index = light_object->face_table[f].vertex_table[v];

                                glVertex3d(light_object->vertex_table[v_index].coord.x,
                                           light_object->vertex_table[v_index].coord.y,
                                           light_object->vertex_table[v_index].coord.z);

                            }
                            glEnd();
                        }
                    }

                }

                if(global_lights[i].activado == 1) {
                    put_light(i);
                }
                glPopMatrix();
            }
        }
	}
	
	 /*Now each of the objects in the list*/
    while(aux_obj != 0) {
		
			// Habilidad el test de profundidad
			glEnable(GL_DEPTH_TEST);
			// Aceptar el fragmento si está más cerca de la cámara que el fragmento anterior
			glDepthFunc(GL_LESS);
			
			/* Select the color, depending on whether the current object is the selected one or not */
			if (aux_obj == _selected_object){
				glColor3f(1.0, 0.0, 0.0); // Establece el color rojito
				//glColor3f(KG_COL_SELECTED_R,KG_COL_SELECTED_G,KG_COL_SELECTED_B);
			}else{
				glColor3f(KG_COL_NONSELECTED_R,KG_COL_NONSELECTED_G,KG_COL_NONSELECTED_B);
			}
			
			if(modo == CAMARAOBJETO){
				obtenerMCSR(_selected_object->mptr->M, MCSR);
				glLoadMatrixd(MCSR);
				if(aux_obj != _selected_object){
					glMultMatrixd(aux_obj->mptr->M);
					for (f = 0; f < aux_obj->num_faces; f++) {	
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glBegin(GL_POLYGON);
						for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
							v_index = aux_obj->face_table[f].vertex_table[v];
							glVertex3d(aux_obj->vertex_table[v_index].coord.x,
									aux_obj->vertex_table[v_index].coord.y,
									aux_obj->vertex_table[v_index].coord.z);
						}
						glEnd();
					}
				}
			}else{
				obtenerMCSR(_selected_camara->M, MCSR);
				glLoadMatrixd(MCSR);
				glMultMatrixd(aux_obj->mptr->M);
				for (f = 0; f < aux_obj->num_faces; f++) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);					
                        glBegin(GL_POLYGON);
                        for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
                            v_index = aux_obj->face_table[f].vertex_table[v];
                            glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                                    aux_obj->vertex_table[v_index].coord.y,
                                    aux_obj->vertex_table[v_index].coord.z);
                        }
                        glEnd();
				}				
			}
        aux_obj = aux_obj->next; 
		
	}
	
	while (aux_camara != 0) {
        /* We will draw the cameras blue */
        glColor3f(0.0,0.0,KG_COL_NONSELECTED_B);
        
        if (modo == CAMARAOBJETO){
        	glLoadMatrixd(_selected_object->mptr->M);
        	glMultMatrixd(aux_camara->M);
			/* Draw the camera; for each face create a new polygon with the corresponding vertices */
			for (f = 0; f < aux_camara->num_faces; f++) {
					glBegin(GL_POLYGON);
					for (v = 0; v < aux_camara->face_table[f].num_vertices; v++) {
						v_index = aux_camara->face_table[f].vertex_table[v];
						glVertex3d(aux_camara->vertex_table[v_index].coord.x,
							aux_camara->vertex_table[v_index].coord.y,
							aux_camara->vertex_table[v_index].coord.z);
							}
					glEnd();
                
			}
			aux_camara = aux_camara->next;
        }
        else{
			obtenerMCSR(_selected_camara->M, MCSR);
			glLoadMatrixd(MCSR);
        	/*If we are viewing from the camera we have to check not to draw the selected camera*/
        	if(aux_camara != _selected_camara){
				glMultMatrixd(aux_camara->M);
				/* Draw the camera; for each face create a new polygon with the corresponding vertices */
				for (f = 0; f < aux_camara->num_faces; f++) {
                        glBegin(GL_POLYGON);
                        for (v = 0; v < aux_camara->face_table[f].num_vertices; v++) {
                            v_index = aux_camara->face_table[f].vertex_table[v];
                            glVertex3d(aux_camara->vertex_table[v_index].coord.x,
                                    aux_camara->vertex_table[v_index].coord.y,
                                    aux_camara->vertex_table[v_index].coord.z);
                        }
                        glEnd();  
                    
				}
			}
        aux_camara = aux_camara->next;
        }
        
    }
	
    /*Do the actual drawing*/
    glFlush();
}
