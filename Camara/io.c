#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "definitions.h"
#include "load_obj.h"
#include "camara.h"

extern object3d * _first_object;
extern object3d * _selected_object;
extern camara *_first_camara;
extern camara *_selected_camara;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern int modo; 
extern int referencia ; 

extern int proyeccion;   
extern int vista;

extern int movimiento; // rota = 0 ; trasladar = 1; escalar = 2;

int i;

/**
 * función que calcula la distancia entre la camara y el objeto seleccionados
 * @return la distancia entre ambos
 */
GLdouble distancia_camara_objeto(){
    GLdouble px, py, pz;

    px = _selected_object->mptr->M[12] - _selected_camara->Minv[12];
    py = _selected_object->mptr->M[13] - _selected_camara->Minv[13];
    pz = _selected_object->mptr->M[14] - _selected_camara->Minv[14];

    return sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));
}

/**
 * @brief This function just prints information about the use
 * of the keys
 */
void print_help(){
    printf("KbG Irakasgaiaren Praktika. Programa honek 3D objektuak \n");
    printf("aldatzen eta bistaratzen ditu.  \n\n");
    printf("\n\n");
    
    printf("FUNCIONES PRINCIPALES \n");
    printf("<?>\t\t Visualizar esta ayuda \n");
    printf("<ESC>\t\t Salir del programa \n");
    printf("<F>\t\t Cargar un objeto\n");
    printf("<TAB>\t\t Navegar en los objetos cargados\n");
    printf("<SUPR>\t\t Borrar el objeto seleccionado\n");
    
    printf("TRANSFORMACIONES AL OBJETO SELECCIONADO \n");
    
    printf("TIPO DE TRANSFORMACION \n");
    printf("<R>\t\t Activar rotacion\n");
    printf("<T>\t\t Activar traslacion\n");
    printf("<E>\t\t Activar escalado\n");
    
    printf("SISTEMA DE REFERENCIA \n");
    printf("<G>\t\t Sistema de referencia global\n");
    printf("<L>\t\t Sistema de referencia local\n\n");
    
    printf("ELEMENTO A TRANSFORMAR \n");
    
    printf("<O> \t\t Aplicar tranformaciones al objeto seleccionado (valor por defecto)(desactiva camara y luz)\n");
    printf("<C> \t\t Aplicar transformaciones a la camara actual (desactiva objeto y luz)\n");
    
    printf("FUNCIONES DE CAMARA \n");
    printf("<K>\t\t  Intercambiar de cámara pura a cámara asociada a objeto.\n");
    printf("<k>\t\t  Cambiar de camara.");
    printf("<N,n>\t\t  Anadir una camara.\n");
    printf("<G,g>\t\t  Camara modo Analisis.\n");
    printf("<L,l>\t\t  Camara modo Vuelo.\n");
    printf("<P,p>\t\t  Cambiar de tipo de proyeccion: perspectiva/paralela .\n");

    printf("<UP> \t\t Trasladar +Y; Escalar + Y; Rotar +X\n");
    printf("<DOWN> \t\t Trasladar -Y; Escalar - Y;  Rotar -X\n");
    printf("<RIGHT> \t Trasladar +X; Escalar +X;  Rotar +Y\n");
    printf("<LEFT> \t\t Trasladar -X; Escalar -X;  Rotar -Y\n");
    printf("<AVPAG> \t Trasladar +Z; Escalar +X;  Rotar +Y\n");
    printf("<REPAG> \t Trasladar -Z; Escalar -Z; Rotar -Z\n");
    printf("<+> \t\t Escalar + en todos los ejes (caso de objetos) o aumentar volumen de visión (caso cámara)\n");
    printf("<-> \t\t Escalar -  en todos los ejes (caso de objetos) o disminuir volumen de visión (caso cámara)\n");
    printf("<Z> \t\t Deshacer cambios\n");
    printf("\n\n");
}

/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {

    int read = 0;
    object3d *auxiliar_object = 0;
    camara *auxiliar_camara = 0;
    GLdouble wd,he,midx,midy;
    vector3* e ;
    
    char* fname;
    GLdouble matriz_rotada[16];
    elemM *m,*sig_matriz;
    
    switch (key) {
        case 'f':// Carga de objeto desde el fichero
        case 'F':
            /*Ask for file*/
            fname = malloc(sizeof (char)*128); /* Note that scanf adds a null character at the end of the vector*/
            printf("%s", KG_MSSG_SELECT_FILE);
            scanf("%s", fname);
            /*Allocate memory for the structure and read the file*/
            auxiliar_object = (object3d *) malloc(sizeof (object3d));
            read = read_wavefront(fname, auxiliar_object);
            switch (read) {
                /*Errors in the reading*/
                case 1:
                    printf("%s: %s\n", fname, KG_MSSG_FILENOTFOUND);
                break;
                case 2:
                    printf("%s: %s\n", fname, KG_MSSG_INVALIDFILE);
                break;
                case 3:
                    printf("%s: %s\n", fname, KG_MSSG_EMPTYFILE);
                break;
                /*Read OK*/
                case 0:
                    /*Insert the new object in the list*/
                    auxiliar_object->next = _first_object;
                    _first_object = auxiliar_object;
                    _selected_object = _first_object;

                     // al objeto tenemos que decirle que apunte uno nuevo
                    auxiliar_object->mptr=(elemM*) malloc(sizeof(elemM)); // malloc devuelve un puntero. Un void pointer
                    auxiliar_object->mptr->sigPtr = 0;     // no hay siguiente matriz por ahora
                                      
                    // cargue identidad en model view
                    glMatrixMode(GL_MODELVIEW);  // applies subsequent matrix operations to the projection matiz stack
                    glLoadIdentity();            // carga la matriz identidad
                    glGetDoublev(GL_MODELVIEW_MATRIX, auxiliar_object->mptr->M); // asocia la matriz a auxiliar
                    // if(modo == CAMARA && vista == ANALISIS){
                        // centre_camera_to_obj(_selected_object);  
                    // }else if(modo == CAMARAOBJETO){
                        // add_camara_mode_obj(_selected_object);
                    // }
                    
                    printf("%s\n",KG_MSSG_FILEREAD);
                break;
            }
        break;
      
        case 'N':
        case 'n': // anadir camara
            add_camara();
            if(modo == ANALISIS && _selected_camara!=NULL){
                    centre_camera_to_obj(_selected_object);
            }
		break;
        
        
        
        
        
        case 9: /* <TAB> */
            // seleccionar siguiente obj o camara
            if (_selected_object!=NULL){
               // if(modo == OBJETO || modo == CAMARAOBJETO){
                     _selected_object = _selected_object->next;
                    printf("Siguiente OBJETO... \n");
                    /*The selection is circular, thus if we move out of the list we go back to the first element*/
                    if (_selected_object == 0)
                        _selected_object = _first_object;
                    /*if(vista == ANALISIS && modo == CAMARA)
                        centre_camera_to_obj(_selected_object);
                    else if  (modo == CAMARAOBJETO){
                    
                    }*/
                //}                
            }
            else {
                printf("%s: %s\n", fname, KG_MSSG_NOOBJ);
            }
        break;


        /* Intercambiar de camara*/
        case 'k': // Cambiar de camara entre camaras puras
            if (_selected_camara!=NULL){
                _selected_camara = _selected_camara->next;
                printf("Siguiente CAMARA ... \n");
                if(modo==CAMARA){
                    if (_selected_camara == 0) 
                        _selected_camara = _first_camara;
                    if(vista == ANALISIS)
                        centre_camera_to_obj(_selected_object);          
                }
            }else 
                printf("%s: %s\n", fname, KG_MSSG_NOCAM);
        break;

        

        case 127: /* <SUPR> */
            // elimina obj seleccionado
            if(_selected_object !=0 ){
                    if (_selected_object == _first_object){
                         _first_object = _first_object->next;
                        
                        // a parte de borrar objto tambn su memoria asociada 
                        // tabla_vertices, tabla_poligonos y puntero matrices
                        for(i=0; i< _selected_object->num_faces; i++){
                            face poligono = _selected_object->face_table[i];
                            free(poligono.vertex_table);
                            (_selected_object->face_table[i]);
                        }
                        free(_selected_object);
                         _selected_object = _first_object;
                    } else {
                        auxiliar_object = _first_object;
                        while (auxiliar_object->next != _selected_object)
                            auxiliar_object = auxiliar_object->next;
                        auxiliar_object->next = _selected_object->next;
                        free(_selected_object);
                        _selected_object = auxiliar_object;
                    }
                if (vista == ANALISIS){
                    centre_camera_to_obj(_selected_object);
                }
            }else{
               printf("%s: %s\n", fname, KG_MSSG_NODELETE);
            }    
        break;
        
        
        
        
        
        case '-':
            // Reducir el volumen de visualización
            // Escalar - en todos los ejes (caso de objetos) o disminuir volumen de vision (caso camara)
            if(_selected_object!=NULL){
                if(modo == OBJETO){
                    elemM *m = _selected_object->mptr;
                    glMatrixMode(GL_MODELVIEW);
                    glLoadMatrixd(m->M);
                    glScaled(0.8,0.8,0.8);
                    glGetDoublev(GL_MODELVIEW_MATRIX, matriz_rotada);
                    sig_matriz = malloc(sizeof (elemM));
                    for(i = 0; i<16;i++)
                        sig_matriz->M[i] = matriz_rotada[i];
                    
                    sig_matriz->sigPtr = m;
                    _selected_object->mptr = sig_matriz;
                    
                }else if(modo == CAMARA){        
                    wd=(_ortho_x_max-_ortho_x_min)/KG_STEP_ZOOM;
                    he=(_ortho_y_max-_ortho_y_min)/KG_STEP_ZOOM;
                    midx = (_ortho_x_max+_ortho_x_min)/2;
                    midy = (_ortho_y_max+_ortho_y_min)/2;
                    _ortho_x_max = midx + wd/2;
                    _ortho_x_min = midx - wd/2;
                    _ortho_y_max = midy + he/2;
                    _ortho_y_min = midy - he/2;
                }
            }
            else
                printf("%s: %s\n", fname, KG_MSSG_NOOBJ);
            
        break;
        
        case '+':
            //  Incrementar el volumen de visualización
            //  Escalar + en todos los ejes (caso de objetos) o aumentar volumen de visión (caso cámara)
            /*Increase the projection plane; compute the new dimensions*/
            if(_selected_object!=NULL){
                if(modo == OBJETO){ 
                    elemM *m = _selected_object->mptr;
                    glMatrixMode(GL_MODELVIEW);
                    glLoadMatrixd(m->M);
                    glScaled(1.25,1.25,1.25);
                    glGetDoublev(GL_MODELVIEW_MATRIX, matriz_rotada);
                    sig_matriz = malloc(sizeof (elemM));
                    for(i = 0; i<16;i++)
                        sig_matriz->M[i] = matriz_rotada[i];
                    
                    sig_matriz->sigPtr = m;
                    _selected_object->mptr = sig_matriz;
                }else if(modo == CAMARA){
                    wd=(_ortho_x_max-_ortho_x_min)*KG_STEP_ZOOM;
                    he=(_ortho_y_max-_ortho_y_min)*KG_STEP_ZOOM;
                    midx = (_ortho_x_max+_ortho_x_min)/2;
                    midy = (_ortho_y_max+_ortho_y_min)/2;
                    _ortho_x_max = midx + wd/2;
                    _ortho_x_min = midx - wd/2;
                    _ortho_y_max = midy + he/2;
                    _ortho_y_min = midy - he/2;
                }
            }
            else
                printf("%s: %s\n", fname, KG_MSSG_NOOBJ);
        break;

        case '?':
            //  Visualizar la ayuda
            print_help();
            break;

        case 27: /* <ESC> */
            // Finalizar la ejecucion de la aplicacion
            exit(0);
            break;
        
        
        
        
        
        /* MODO: objeto, camara, iluminacion*/
        case 'O': 
        case 'o':
            if(modo != OBJETO){
                modo = OBJETO;
                printf("Se aplica las transformaciones al OBJETO.\n");
            }
    	break;

        case 'C': 
        case 'c':
            if(modo != CAMARA){
                modo = CAMARA;
                printf("Se aplica las transformaciones a la CAMARA.\n");
            }
        break;

        case 'I': // I,i   Aplicar transformaciones a la Iluminación selecionada (desactiva objeto y camara)
        case 'i':
            if(modo != ILUMINACION){
                modo = ILUMINACION;
                printf("Se aplica las transformaciones a la ILUMINACION.\n");
            }
        break;
        
        case 'K': // K  intercambiar de camara pura a camara asociada a obj y viceversa
            // visualizar desde el objeto
            if (_selected_object!=NULL){
                if(modo != CAMARAOBJETO){
                    printf("Modo especial, ahora visualizamos lo que el objeto seleccionado ve.\n");
                    printf("Modo CAMARAOBJETO \n");
                    printf("Pulsa K, O, (I) o C para salir. \n");
                    modo = CAMARAOBJETO;
                }
                else{
                    modo = CAMARA;
                    printf("Se aplica las transformaciones al CAMARA.\n");
                }
            }else 
                printf("%s: %s and %s \n", fname, KG_MSSG_NOOBJ, KG_MSSG_NOCAM);
        break;
        
        
        
        
        
        /* tipo de transformacion, movimiento */
        case 'R': // R,r   Activar rotación (desactiva traslación y escalado)
        case 'r':
            movimiento = 0;
            printf("Rotacion activado. \n");

            break;
        
        case 'T': // T,t    Activar traslación (desactiva rotación y escalado)
        case 't':
            movimiento = 1;
            printf("Traslacion activado. \n");
    
        break;
            
        case 'E': // E,e   Activar escalado (desactiva rotación y traslación)
        case 'e':
            movimiento = 2;
            printf("Escalado activado. \n");
        break;
        
        
        
        
        
        /* sistema de referencia, referencia || vista camara, vuelo o analisis, vista */
        case 'G':
        case 'g': 
            if(_selected_object != NULL){
                if (modo == CAMARA){
                    if(vista != ANALISIS){
                        vista = ANALISIS;
                        _selected_camara->vistacam = ANALISIS;
                        printf("Vista de la camara: ANALISIS\n");
                        centre_camera_to_obj(_selected_object);
                    }
                }
                else if(modo == OBJETO){
                    if(referencia != GLOBAL){
                        referencia = GLOBAL;
                        printf("Sistema de referencia: GLOBAL\n");
                    }
                }
            }else 
                printf("%s: %s\n", fname, KG_MSSG_NOOBJ);
            
    	break;
            
        case 'L':
        case 'l':
            if(_selected_object != NULL){
                if (modo == CAMARA){
                    if(vista != VUELO){
                        vista = VUELO;
                        _selected_camara->vistacam = VUELO;
                        printf("Vista de la camara: VUELO\n");
                    }
                }
                else if (modo == OBJETO){
                    if(referencia != LOCAL){
                        referencia = LOCAL;
                        printf("Sistema de referencia: LOCAL\n");
                    }
                }
            }else
                printf("%s: %s\n", fname, KG_MSSG_NOOBJ);
           
    	break;
        
        
        
        
        
        case 'Z':
        case 'z': //  (Z,z) Deshacer
           if(_selected_object != NULL){
                if(modo == OBJETO){
                    if(_selected_object->mptr->sigPtr != NULL)
                        _selected_object->mptr = _selected_object->mptr->sigPtr;
                    printf("Deshaciendo OBJ...\n");
                }else if(modo == CAMARA){
                    if(_selected_camara->next != NULL) //TODO cuidau
                        _selected_camara = _selected_camara->next;
                    printf("Deshaciendo CAM...\n");
                    
                }
            }
        break;
        
        
        
        
    
        
        
        case 'p':
        case 'P':
            if (_selected_object != NULL){
                proyeccion = 1 - proyeccion;
                if(_selected_camara->tipo_proyeccion == PERSPECTIVA){
                    printf("Modo de vista: Perspectiva\n");
                   /* _selected_camara->projection[0] = -0.1;
                    _selected_camara->projection[1] = 0.1;
                    _selected_camara->projection[2] = -0.1;
                    _selected_camara->projection[3] = 0.1;
                    _selected_camara->projection[4] = 0.1;
                    _selected_camara->projection[5] = 10000;*/
                    _selected_camara->proj.izq = -5.0;
                    _selected_camara->proj.der = 5.0;
                    _selected_camara->proj.alto = 5.0;
                    _selected_camara->proj.bajo = -5.0;
                    _selected_camara->proj.cerca = 0;
                    _selected_camara->proj.lejos = 1000.0;
                }
                else if(_selected_camara->tipo_proyeccion == PARALELO){
                    printf("Modo de vista: Paralelo\n");
                   /* _selected_camara->projection[0] = _ortho_x_min;
                    _selected_camara->projection[1] = _ortho_x_max;
                    _selected_camara->projection[2] = _ortho_y_min;
                    _selected_camara->projection[3] = _ortho_y_max;
                    _selected_camara->projection[4] = 0;
                    _selected_camara->projection[5] = _ortho_z_max;*/
                    _selected_camara->proj.izq = -0.1;
                    _selected_camara->proj.der = 0.1;
                    _selected_camara->proj.alto = 0.1;
                    _selected_camara->proj.bajo = -0.1;
                    _selected_camara->proj.cerca = 0.1;
                    _selected_camara->proj.lejos = 1000.0;
                }
            }else{
                printf("%s: %s \n", fname, KG_MSSG_NOOBJ);
            }
            
            
            /*
                if(modo != CAMARAOBJETO){
                _selected_camara->tipo_proyeccion = (_selected_camara->tipo_proyeccion +1)%2;
                
                if(_selected_camara->tipo_proyeccion == PARALELO){
                    printf("proyeccion paralela. \n");
                    _selected_camara->proj.izq = -5.0;
                    _selected_camara->proj.der = 5.0;
                    _selected_camara->proj.alto = 5.0;
                    _selected_camara->proj.bajo = -5.0;
                    _selected_camara->proj.cerca = 0;
                    _selected_camara->proj.lejos = 1000.0;
                }else if(_selected_camara->tipo_proyeccion == PERSPECTIVA){
                    printf("proyeccion perspectiva. \n");
                    _selected_camara->proj.izq = -0.1;
                    _selected_camara->proj.der = 0.1;
                    _selected_camara->proj.alto = 0.1;
                    _selected_camara->proj.bajo = -0.1;
                    _selected_camara->proj.cerca = 0.1;
                    _selected_camara->proj.lejos = 1000.0;
                }
                
            }
            */
        break;


        default:
            /*In the default case we just print the code of the key. This is usefull to define new cases*/
            printf("%d %c\n", key, key);
            printf("keyboard");
            break;
        }
        
        fflush(stdout); // me aseguro que borra todo
        
        /*In case we have do any modification affecting the displaying of the object, we redraw them*/
        glutPostRedisplay();
}
/**
 * @brief Callback function to control the special keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboardspecial(int key, int x, int y){
    
    char* fname = malloc(sizeof (char)*128); /* Note that scanf adds a null character at the end of the vector*/
    int read = 0;
    GLdouble wd,he,midx,midy;
    GLdouble matriz_rotada[16], m_aux[16];
    elemM *m, *sig_matriz;

    if(_selected_object!=NULL){
        
        switch(key){
            case 101: // UP
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    
                    glMatrixMode(GL_MODELVIEW);
                    if(referencia == LOCAL)
                        glLoadMatrixd(_selected_object->mptr->M);
                    else
                        glLoadIdentity();
                    
                    
                    if(movimiento == TRASLADAR){
                        glTranslated(0,1,0);
                    }else if(movimiento == ROTAR){
                        glRotated(10,1,0,0);
                    }else if(movimiento == ESCALAR){
                        glScaled(1,1.1,1);
                    }
                    
                    
                    elemM *n = (elemM*) malloc(sizeof(elemM));
                    if(referencia == LOCAL)
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M); 
                    else{
                        glMultMatrixd(_selected_object->mptr->M);
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M);          
                    }                                    
                    n->sigPtr = _selected_object->mptr;
                    _selected_object->mptr = n;
                    
                    
                }else if(modo == CAMARA){
                    glLoadMatrixd(_selected_camara->Minv);
                    if(movimiento == TRASLADAR){
                        if(vista == VUELO){
                           glTranslated(1,0,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(1,0);
                        }
                    }else if(movimiento == ROTAR){
                        if(vista == VUELO){
                           glRotated(10,1,0,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(1,0);
                        }
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.alto -= 0.01;
                        _selected_camara->proj.bajo += 0.01;
                    }
                    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->Minv);
                    matriz_inversa(_selected_camara);
                    
                }else if(modo == ILUMINACION){
                    break;
                }
                printf("UP \n");
                break;
                
                
            case 103: // DOWN
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    glMatrixMode(GL_MODELVIEW);
                    if(referencia == LOCAL)
                        glLoadMatrixd(_selected_object->mptr->M);
                    else
                        glLoadIdentity();
                    
                    
                    if(movimiento == TRASLADAR){
                        glTranslated(0,-1,0);
                    }else if(movimiento == ROTAR){
                        glRotated(10,-1,0,0);
                    }else if(movimiento == ESCALAR){
                        glScaled(1,0.9,1);
                    }
                    
                    
                    elemM *n = (elemM*) malloc(sizeof(elemM));
                    if(referencia == LOCAL)
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M); 
                    else{
                        glMultMatrixd(_selected_object->mptr->M);
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M);          
                    }                                    
                    n->sigPtr = _selected_object->mptr;
                    _selected_object->mptr = n;
                    
                }else if(modo == CAMARA){
                    glLoadMatrixd(_selected_camara->Minv);
                    if(movimiento == TRASLADAR){
                        if(vista == VUELO){
                           glTranslated(0,-1,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(-1,0);
                        }
                    }else if(movimiento == ROTAR){
                        if(vista == VUELO){
                           glRotated(10,-1,0,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(-1,0);
                        }
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.alto += 0.01;
                        _selected_camara->proj.bajo -= 0.01;
                    }
                    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->Minv);
                    matriz_inversa(_selected_camara);
                    
                }else if(modo == ILUMINACION){
                    break;
                }
                printf("DOWN \n");
                break;
                

            case 100: //LEAFT
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    glMatrixMode(GL_MODELVIEW);
                    if(referencia == LOCAL)
                        glLoadMatrixd(_selected_object->mptr->M);
                    else
                        glLoadIdentity();
                    if(movimiento == TRASLADAR){
                        glTranslated(-1,0,0);
                    }else if(movimiento == ROTAR){
                        glRotated(10,0,-1,0);
                    }else if(movimiento == ESCALAR){
                        glScaled(0.9,1,1);
                    }
                    elemM *n = (elemM*) malloc(sizeof(elemM));
                    if(referencia == LOCAL)
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M); 
                    else{
                        glMultMatrixd(_selected_object->mptr->M);
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M);          
                    }                                    
                    n->sigPtr = _selected_object->mptr;
                    _selected_object->mptr = n;
                    
                }else if(modo == CAMARA){
                    glLoadMatrixd(_selected_camara->Minv);
                    if(movimiento == TRASLADAR){
                        if(vista == VUELO){
                           glTranslated(-1,0,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(0,1);
                        }
                    }else if(movimiento == ROTAR){
                        if(vista == VUELO){
                           glRotated(10,0,-1,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(0,1);
                        }
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.alto += 0.01;
                        _selected_camara->proj.bajo -= 0.01;
                    }
                    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->Minv);
                    matriz_inversa(_selected_camara);
                    
                }else if(modo == ILUMINACION){
                    break;
                }
                printf("LEAFT \n");
                break;

            case 102: //RIGHT
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    glMatrixMode(GL_MODELVIEW);
                    if(referencia == LOCAL)
                        glLoadMatrixd(_selected_object->mptr->M);
                    else
                        glLoadIdentity();
                    if(movimiento == TRASLADAR){
                        glTranslated(1,0,0);
                    }else if(movimiento == ROTAR){
                        glRotated(10,0,1,0);
                    }else if(movimiento == ESCALAR){
                        glScaled(1.1,1,1);
                    }
                    elemM *n = (elemM*) malloc(sizeof(elemM));
                    if(referencia == LOCAL)
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M); 
                    else{
                        glMultMatrixd(_selected_object->mptr->M);
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M);          
                    }                                    
                    n->sigPtr = _selected_object->mptr;
                    _selected_object->mptr = n;
                    
                }else if(modo == CAMARA){
                    glLoadMatrixd(_selected_camara->Minv);
                    if(movimiento == TRASLADAR){
                        if(vista == VUELO){
                           glTranslated(1,0,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(0,-1);
                        }
                    }else if(movimiento == ROTAR){
                        if(vista == VUELO){
                           glRotated(10,0,1,0);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(0,-1);
                        }
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.alto += 0.01;
                        _selected_camara->proj.bajo -= 0.01;
                    }
                    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->Minv);
                    matriz_inversa(_selected_camara);
                    
                }else if(modo == ILUMINACION){
                    break;
                }
                printf("RIGHT \n");
                break;
                
            case 104: //REPAG
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    glMatrixMode(GL_MODELVIEW);
                    if(referencia == LOCAL)
                        glLoadMatrixd(_selected_object->mptr->M);
                    else
                        glLoadIdentity();
                    if(movimiento == TRASLADAR){
                        glTranslated(0,0,-1);
                    }else if(movimiento == ROTAR){
                        glRotated(10,0,0,-1);
                    }else if(movimiento == ESCALAR){
                        glScaled(1,1,0.9);
                    }
                    elemM *n = (elemM*) malloc(sizeof(elemM));
                    if(referencia == LOCAL)
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M); 
                    else{
                        glMultMatrixd(_selected_object->mptr->M);
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M);          
                    }                                    
                    n->sigPtr = _selected_object->mptr;
                    _selected_object->mptr = n;
                    
                }else if(modo == CAMARA){
                    glLoadMatrixd(_selected_camara->Minv);
                    if(movimiento == TRASLADAR){
                        if(vista == VUELO){
                           glTranslated(0,0,-1);
                        }
                        else if(vista == ANALISIS){
                            GLdouble dist = distancia_camara_objeto();
                            if (dist > 1.0){
                                glTranslated(0,0,-1);
                            }else{//para evitar error de redondeo
                                centre_camera_to_obj(_selected_object);
                            }
                        }
                    }else if(movimiento == ROTAR){
                        if(vista == VUELO){
                           glTranslated(0,0,-1);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(0,-1);
                        }
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.alto += 0.01;
                        _selected_camara->proj.bajo -= 0.01;
                    }
                    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->Minv);
                    matriz_inversa(_selected_camara);
                    
                }else if(modo == ILUMINACION){
                    break;
                }
                printf("REPAG \n");
                break;
                
                
             case 105: //AVPAG
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    glMatrixMode(GL_MODELVIEW);
                    if(referencia == LOCAL)
                        glLoadMatrixd(_selected_object->mptr->M);
                    else
                        glLoadIdentity();
                    if(movimiento == TRASLADAR){
                        glTranslated(0,0,1);
                    }else if(movimiento == ROTAR){
                        glRotated(10,0,0,1);
                    }else if(movimiento == ESCALAR){
                        glScaled(1,1,1.1);
                    }
                    elemM *n = (elemM*) malloc(sizeof(elemM));
                    if(referencia == LOCAL)
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M); 
                    else{
                        glMultMatrixd(_selected_object->mptr->M);
                        glGetDoublev(GL_MODELVIEW_MATRIX, n->M);          
                    }                                    
                    n->sigPtr = _selected_object->mptr;
                    _selected_object->mptr = n;
                    
                }else if(modo == CAMARA){
                    glLoadMatrixd(_selected_camara->Minv);
                    if(movimiento == TRASLADAR){
                        if(vista == VUELO){
                           glTranslated(0,0,1);
                        }
                        else if(vista == ANALISIS){
                            GLdouble dist = distancia_camara_objeto();
                            if (dist < 100.0){ //no permitimos que se aleje en exceso
                                glTranslated(0,0,1);
                            }else{//para evitar error de redondeo
                                centre_camera_to_obj(_selected_object);
                            }
                        }
                    }else if(movimiento == ROTAR){
                        if(vista == VUELO){
                           glRotated(10,0,0,1);
                        }
                        else if(vista == ANALISIS){
                            modo_analisis(0,-1);
                        }
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.alto += 0.01;
                        _selected_camara->proj.bajo -= 0.01;
                    }
                    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->Minv);
                    matriz_inversa(_selected_camara);
                    
                }else if(modo == ILUMINACION){
                    break;
                }
                printf("AVPAG \n");
                break;
                
      
            
                glutPostRedisplay();
        }  
    }
    
        
        // si el objeto es la camara, miramos desde dentro del objeto
        // if(modo == CAMARAOBJETO){
            // add_camara_mode_obj(_selected_camara);
        // }
        
        //glutPostRedisplay();
        
        // if(modo == OBJETO || modo == CAMARAOBJETO) // visualise obj
        // {
            // m = _selected_object->mptr;
            
            // if(referencia == LOCAL){         // Referencia LOCAL
               // glLoadMatrixd(m->M);
            // }if(referencia ==GLOBAL){        // referencia GLOBAL
               // glLoadIdentity();
            // }
        // }
        // else if(modo == CAMARA) // visualice camara
        // {
            // if(vista == VUELO)         // Referenciavista modo vuelo
                // glLoadMatrixd(_selected_camara->M);
            // if(vista == ANALISIS){          // modo analisis 
               // glLoadIdentity();
               // mirar_obj_selec();
            // }
        // }
        
        
        // switch (key) {
            // case 101: // UP  Trasladar +Y; Escalar + Y; Rotar +X 
                // if(movimiento == ROTAR)         // rotar     (0)
                    // glRotated(10,1,0,0);
                // else if(movimiento == TRASLADAR)    // trasladar (1)
                    // glTranslated(0,1,0);
                // else if(movimiento == ESCALAR)    // escalar   (2)
                    // glScaled(1,1.1,1);
                
                // printf("UP \n");
                // break;
                
            // case 103: // DOWN Trasladar -Y; Escalar - Y;  Rotar -X 
                // if(movimiento == ROTAR)         // rotar     (0)
                    // glRotated(10,-1,0,0);
                // else if(movimiento == TRASLADAR)    // trasladar (1)
                    // glTranslated(0,-1,0);
                // else if(movimiento == ESCALAR)    // escalar   (2)
                    // glScaled(1,0.9,1);
            
                // printf("DOWN \n");
                // break;
            // case 102: // RIGHT Trasladar +X; Escalar +X;  Rotar +Y 
                // if(movimiento == ROTAR)         // rotar     (0)
                    // glRotated(10,0,1,0);
                // else if(movimiento == TRASLADAR)    // trasladar (1)
                    // glTranslated(1,0,0);
                // else if(movimiento == ESCALAR)    // escalar   (2)
                    // glScaled(1.1,1,1);
                

                // printf("RIGHT \n");
                // break;
            // case 100: // LEFT  Trasladar -X; Escalar -X;  Rotar -Y 
                // if(movimiento == ROTAR)         // rotar     (0)
                    // glRotated(10,0,-1,0);
                // else if(movimiento == TRASLADAR)    // trasladar (1)
                    // glTranslated(-1,0,0);
                // else if(movimiento == ESCALAR)    // escalar   (2)
                    // glScaled(0.9,1,1);
                
                // printf("LEFT \n");
                // break;
            // case 105: // AVPAG Trasladar +Z; Escalar +Z;  Rotar +Z             
                // if(movimiento == ROTAR)         // rotar     (0)
                    // glRotated(10,0,0,1);
                // else if(movimiento == TRASLADAR)    // trasladar (1)
                    // glTranslated(0,0,1);
                // else if(movimiento == ESCALAR)    // escalar   (2)
                    // glScaled(1,1,1.1);
                
                // printf("AVPAG \n");
                // break;
                
            // case 104: // REPAG Trasladar -Z; Escalar - Z;  Rotar -Z 
                // if(movimiento == ROTAR)         // rotar     (0)
                    // glRotated(10,0,0,-1);
                // else if(movimiento == TRASLADAR)    // trasladar (1)
                    // glTranslated(0,0,-1);
                // else if(movimiento == ESCALAR)    // escalar   (2)
                    // glScaled(1,1,0.9);
                
                // printf("REPAG \n");
                // break;
        // }
        // if(modo == OBJETO || modo == CAMARAOBJETO) // visualise obj
        // {
            // if(referencia == GLOBAL)         // Referencia GLOBAL
                // glMultMatrixd(m->M);
                        
            // glGetDoublev(GL_MODELVIEW_MATRIX, matriz_rotada); 
            
            // sig_matriz = malloc(sizeof (elemM));            
            // for(i =0; i<16; i++)
                // sig_matriz->M[i] = matriz_rotada[i];
                        
            // sig_matriz->sigPtr = m;
            // _selected_object->mptr = sig_matriz;
                        
            // glutPostRedisplay();
        // }
        // else if(modo == CAMARA)
        // {
            // if(vista == ANALISIS){
                // //glMultMatrixd(_selected_camara->M);
                // //centre_camera_to_obj(_selected_object);
                // mirar_obj_selec();
                // modo_analisis(_selected_camara, _selected_object);
            // }
            // glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->M);    
        
            // glutPostRedisplay();
        // }
}  


