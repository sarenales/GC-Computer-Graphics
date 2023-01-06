#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "definitions.h"
#include "load_obj.h"
#include "camara.h"
#include "iluminacion.h"

extern object3d * _first_object;
extern object3d * _selected_object;
extern camara *_first_camara;
extern camara *_selected_camara;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern int modo; 
extern int referencia ; 

extern int luz;
extern int _selected_light; 
extern iluminacion_objetos global_lights[8]; 
extern material *mat_camara, *mat_selec, *mat_foco;

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
                    
                    if(modo == CAMARA && vista == ANALISIS){
                        centre_camera_to_obj(_selected_object);  
                    }else if(modo == CAMARAOBJETO){
                        add_camara_mode_obj(_selected_object);
                    }
                    
                    printf("%s\n",KG_MSSG_FILEREAD);
                break;
            }
        break;
      
        case 'N':
        case 'n': // anadir camara
            if(modo == CAMARA){
                add_camara();
                if(modo == ANALISIS && _selected_camara!=NULL){
                        centre_camera_to_obj(_selected_object);
                }
                printf("Se ha anadido otra camara. \n");
            }else {
                printf("%s\n", KG_MSSG_NOMODECAM);
            }
            
		break;
        
        
        
        
        /* Intercambiar entre objetos */
        case 9: /* <TAB> */
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
        case 'k': 
            if (_selected_camara!=NULL){
                if(modo==CAMARA){
                    if(_selected_camara->next == 0){
                        _selected_camara = _first_camara;
                    }else{
                        _selected_camara = _selected_camara->next;
                    }
                    printf("Siguiente CAMARA ... \n");
                    if(vista == ANALISIS){
                        centre_camera_to_obj(_selected_object);          
                    }
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
                if(_selected_object->mptr->sigPtr != NULL)
                    _selected_object->mptr = _selected_object->mptr->sigPtr;
                printf("Deshaciendo movimiento del objeto ... \n");
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


        case 'a':
        case 'A':
            if(modo != ILUMINACION &&  luz == ON){
                printf("Elemento ha cambiado a iluminacion\n");
                modo = ILUMINACION;
                if(movimiento == ESCALAR){
                    movimiento = TRASLADAR;
                    printf("El escalado no existe en iluminacion, se ha cambiado a traslacion \n");
                }
            }else if(luz == OFF){
                printf("Activa la iluminacion primero.\n");
            }
            
        break;
        
        case '0':
            add_lights();
            break;
        
        case '1':
        if(_selected_light != 0 && luz == ON) {
            _selected_light = 0;
            printf("BOMBILLA seleccionada.\n");
        }
        break;

        case '2':
            if(_selected_light != 1 && luz == ON) {
                _selected_light = 1;
                printf("SOL seleccionado.\n");
            }
        break;

        case '3':
            if(_selected_light != 2 && luz == ON) {
                _selected_light = 2;
                printf("FOCO-OBJETO seleccionado.\n");
            }
        break;

        case '4':
            if(_selected_light != 3 && luz == ON) {
                _selected_light = 3;
                printf("FOCO-CÁMARA seleccionado.\n");
            }
        break;

        case '5':
            if(_selected_light != 4 && luz == ON) {
                _selected_light = 4;
                printf("Seleccionada LUZ 5.\n");
            }
        break;

        case '6':
            if(_selected_light != 5 && luz == ON) {
                _selected_light = 5;
                printf("Seleccionada LUZ 6.\n");
            }
        break;

        case '7':
            if(_selected_light != 6 && luz == ON) {
                _selected_light = 6;
                printf("Seleccionada LUZ 7.\n");
            }
        break;

        case '8':
            if(_selected_light != 7 && luz == ON) {
                _selected_light = 7;
                printf("Seleccionada LUZ 8.\n");
            }
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
    GLdouble matriz_rotada[16];
    elemM *m, *sig_matriz;
    if(_selected_object!=NULL){
        glMatrixMode(GL_MODELVIEW);
        
        if(modo == OBJETO || modo == CAMARAOBJETO) // visualise obj
        {
            m = _selected_object->mptr;
            
            if(referencia == LOCAL){         // Referencia LOCAL
               glLoadMatrixd(m->M);
            }if(referencia ==GLOBAL){        // referencia GLOBAL
               glLoadIdentity();
            }
        }
        else if(modo == CAMARA) // visualice camara
        {
            if(vista == VUELO)         // Referenciavista modo vuelo
                glLoadMatrixd(_selected_camara->M);
            if(vista == ANALISIS){          // modo analisis 
               glLoadIdentity();
               //mirar_obj_selec();
            }
        }
        
        if(vista == ANALISIS && movimiento != ROTAR){
            movimiento = ROTAR;
            printf("En modo analisis no se puede trasladar, no escalar. Solo Rotar\n");
            printf("Se ha cambiado a modo rotacion. \n");
        }
        
        switch (key) {
            case 101: // UP  Trasladar +Y; Escalar + Y; Rotar +X 
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,1,0,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,1,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,1.1,1);
                }else if(modo == CAMARA){
                    if(vista == VUELO){
                        if(movimiento == ROTAR ) {
                           glRotated(10,1,0,0);
                        }else if(movimiento == TRASLADAR){
                            glTranslated(0,1,0);
                        }else if(movimiento == ESCALAR){
                            _selected_camara->proj.alto -= 0.01;
                            _selected_camara->proj.bajo += 0.01;
                        }                             
                    }else{
                        // modo_analisis(1,0);
                        glLoadIdentity();
                        
                        
                        glTranslated(_selected_object->mptr->M[12], 
                                    _selected_object->mptr->M[13],
                                    _selected_object->mptr->M[14]);
                        
                        glRotated(10,-1,0,0);
                        
                        
                        glTranslated(-(_selected_object->mptr->M[12]), 
                                    -(_selected_object->mptr->M[13]),
                                    -(_selected_object->mptr->M[14]));
                        
                        glMultMatrixd(_selected_camara->M);
                        
                        
                        
                        glLoadMatrixd(_selected_camara->Minv);
                        
                        glTranslated(_selected_object->mptr->M[12], 
                                    _selected_object->mptr->M[13],
                                    _selected_object->mptr->M[14]);
                        
                        glRotated(10,-1,0,0);
                        
                        glTranslated(-(_selected_object->mptr->M[12]), 
                                    -(_selected_object->mptr->M[13]),
                                    -(_selected_object->mptr->M[14]));
                            
                    }
                }
                printf("UP \n");
                break;
                
            case 103: // DOWN Trasladar -Y; Escalar - Y;  Rotar -X 
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,-1,0,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,-1,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,0.9,1);
                }else if(modo == CAMARA){
                    if(vista == VUELO){
                        if(movimiento == ROTAR ){
                            glRotated(10,-1,0,0);
                        }else if(movimiento == TRASLADAR){
                            glTranslated(0,-1,0);
                        }else{
                            _selected_camara->proj.alto += 0.01;
                            _selected_camara->proj.bajo -= 0.01;
                        }
                    }else{
                        modo_analisis(-1,0);
                    }
                }
                printf("DOWN \n");
                break;
                
            case 102: // RIGHT Trasladar +X; Escalar +X;  Rotar +Y 
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,1,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(-1,0,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1.1,1,1);
                }else if(modo == CAMARA){
                    if(vista == VUELO){
                        if(movimiento == ROTAR){
                            glRotated(10,0,1,0);
                        }else if(movimiento == TRASLADAR){
                            glTranslated(1,0,0);
                        }else{
                            _selected_camara->proj.izq -= 0.01;
                            _selected_camara->proj.der += 0.01;
                        }
                    }else if(vista == ANALISIS){
                        modo_analisis(0,-1);
                    }
                }
                
                printf("RIGHT \n");
                break;
                
                
            case 100: // LEFT  Trasladar -X; Escalar -X;  Rotar -Y 
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,-1,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(1,0,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(0.9,1,1);
                }else if(modo == CAMARA){
                    if(vista == VUELO){
                        if(movimiento == ROTAR){
                            glRotated(10,0,-1,0);
                        }else if(movimiento == TRASLADAR){
                            glTranslated(-1,0,0);
                        }else{
                            _selected_camara->proj.izq += 0.01;
                            _selected_camara->proj.der -= 0.01;
                        }
                    }else{
                        modo_analisis(0,1);
                    }
                }
                
                printf("LEFT \n");
                break;
                
                
            case 105: // AVPAG Trasladar +Z; Escalar +Z;  Rotar +Z   
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,0,1);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,0,1);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,1,1.1);
                }else if(modo == CAMARA){
                    if(movimiento == TRASLADAR){
                        if(vista == ANALISIS){
                            // para no pasarnos
                            GLdouble dist = distancia_camara_objeto();
                            if(dist > 1.0){
                                glTranslated(0,0,1);
                            }else{
                                centre_camera_to_obj(_selected_object);
                            }
                        }else{
                            glTranslated(0,0,1);
                        }
                    }else if(movimiento == ROTAR){
                        glRotated(10,0,0,1);
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.cerca -= 0.01;
                        _selected_camara->proj.lejos -= 0.01;
                    }
                }                
                printf("AVPAG \n");
                break;
                
            case 104: // REPAG Trasladar -Z; Escalar - Z;  Rotar -Z 
                if(modo == OBJETO || modo == CAMARAOBJETO){
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,0,-1);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,0,-1);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,1,0.9);
                }else if(modo == CAMARA){
                    if(movimiento == TRASLADAR){
                        if(vista == ANALISIS){
                            // para no pasarnos
                            GLdouble dist = distancia_camara_objeto();
                            if(dist < 100.0){
                                glTranslated(0,0,-1);
                            }else{
                                centre_camera_to_obj(_selected_object);
                            }
                        }else{
                            glTranslated(0,0,-1);
                        }
                    }else if(movimiento == ROTAR){
                        glRotated(10,0,0,-1);
                    }else if(movimiento == ESCALAR){
                        _selected_camara->proj.cerca += 0.01;
                        _selected_camara->proj.lejos += 0.01;
                    }
                } 
                printf("REPAG \n");
                break;

                case GLUT_KEY_F1:
                    if(luz==ON) {
                        switch (global_lights[0].activado) {
                            case 0:
                                global_lights[0].activado = 1;
                                glEnable(GL_LIGHT0);
                                printf("BOMBILLA encendida\n");
                                break;
                            case 1:
                                global_lights[0].activado = 0;
                                glDisable(GL_LIGHT0);
                                printf("BOMBILLA apagada\n");
                                break;
                        }
                    }else{
                        printf("primero activa la iluminacion\n");
                    }
                    break;

                case GLUT_KEY_F2:
                    if(luz==ON) {
                        switch (global_lights[1].activado) {
                            case 0:
                                global_lights[1].activado = 1;
                                glEnable(GL_LIGHT1);
                                printf("SOL encendido\n");
                                break;
                            case 1:
                                global_lights[1].activado = 0;
                                glDisable(GL_LIGHT1);
                                printf("SOL apagado\n");
                                break;
                        }
                    }else{
                        printf("primero activa la iluminación\n");
                    }
                    break;

                case GLUT_KEY_F3:
                    if(luz == ON && _selected_object!=0) {
                        switch (global_lights[2].activado) {
                            case 0:
                                global_lights[2].activado = 1;
                                glEnable(GL_LIGHT2);
                                printf("FOCO-OBJETO encendido\n");
                                break;
                            case 1:
                                global_lights[2].activado = 0;
                                glDisable(GL_LIGHT2);
                                printf("FOCO-OBJETO apagado\n");
                                break;
                        }
                    }else{
                        printf("primero activa la iluminación o carga un objeto\n");
                    }
                    break;

                case GLUT_KEY_F4:
                    if(luz==ON) {
                        switch (global_lights[3].activado) {
                            case 0:
                                global_lights[3].activado = 1;
                                glEnable(GL_LIGHT3);
                                printf("FOCO-CÁMARA encendido\n");
                                break;
                            case 1:
                                global_lights[3].activado = 0;
                                glDisable(GL_LIGHT3);
                                printf("FOCO-CÁMARA apagado\n");
                                break;
                        }
                    }else{
                        printf("primero activa la iluminación\n");
                    }
                    break;

                case GLUT_KEY_F5:
                    if(luz==ON) {
                        if (global_lights[4].tipo_luz != NONE) {
                            switch (global_lights[4].activado) {
                                case 0:
                                    global_lights[4].activado = 1;
                                    glEnable(GL_LIGHT4);
                                    printf("luz 5 encendida\n");
                                    break;
                                case 1:
                                    global_lights[4].activado = 0;
                                    glDisable(GL_LIGHT4);
                                    printf("luz 5 apagada\n");
                                    break;
                            }
                        } else {
                            printf("primero incializa la luz 5\n");
                        }
                    }else{
                        printf("primero activa la iluminación\n");
                    }
                    break;

                case GLUT_KEY_F6:
                    if(luz == ON) {
                        if (global_lights[5].tipo_luz != NONE) {
                            switch (global_lights[5].activado) {
                                case 0:
                                    global_lights[5].activado = 1;
                                    glEnable(GL_LIGHT5);
                                    printf("luz 6 encendida\n");
                                    break;
                                case 1:
                                    global_lights[5].activado = 0;
                                    glDisable(GL_LIGHT5);
                                    printf("luz 6 apagada\n");
                                    break;
                            }
                        } else {
                            printf("primero incializa la luz 6\n");
                        }
                    }else{
                        printf("primero activa la iluminación\n");
                    }
                    break;

                case GLUT_KEY_F7:
                    if(luz == ON) {
                        if (global_lights[6].tipo_luz != NONE) {
                            switch (global_lights[6].activado) {
                                case 0:
                                    global_lights[6].activado = 1;
                                    glEnable(GL_LIGHT6);
                                    printf("luz 7 encendida\n");
                                    break;
                                case 1:
                                    global_lights[6].activado = 0;
                                    glDisable(GL_LIGHT6);
                                    printf("luz 7 apagada\n");
                                    break;
                            }
                        } else {
                            printf("primero incializa la luz 7\n");
                        }
                    }else{
                        printf("primero activa la iluminación\n");
                    }
                    break;

                case GLUT_KEY_F8:
                    if(luz == ON) {
                        if (global_lights[7].tipo_luz != NONE) {
                            switch (global_lights[7].activado) {
                                case 0:
                                    global_lights[7].activado = 1;
                                    glEnable(GL_LIGHT7);
                                    printf("luz 8 encendida\n");
                                    break;
                                case 1:
                                    global_lights[7].activado = 0;
                                    glDisable(GL_LIGHT7);
                                    printf("luz 8 apagada\n");
                                    break;
                            }
                        } else {
                            printf("primero incializa la luz 8\n");
                        }
                    }else{
                        printf("primero activa la iluminación\n");
                    }
                    break;

                case GLUT_KEY_F9:
                    if(luz == OFF){
                        luz = ON;
                        glEnable(GL_LIGHTING);
                        printf("Iluminación ON\n");
                    }else{
                        luz = OFF;
                        glDisable(GL_LIGHTING);
                        printf("Iluminación OFF\n");
                        if(modo == ILUMINACION){
                            modo = OBJETO;
                            printf("Elemento cambiado a objeto debido a que la iluminación ha sido desactivada\n");
                        }
                    }
                    break;

                case GLUT_KEY_F12:
                    if(luz == ON) {
                        _selected_object->shade = (_selected_object->shade + 1) % 2;
                    }
                    break;

                
        }
        if(modo == OBJETO || modo == CAMARAOBJETO) // visualise obj
        {
            if(referencia == GLOBAL)         // Referencia GLOBAL
                glMultMatrixd(m->M);
                        
            glGetDoublev(GL_MODELVIEW_MATRIX, matriz_rotada); 
            
            sig_matriz = malloc(sizeof (elemM));            
            for(i =0; i<16; i++)
                sig_matriz->M[i] = matriz_rotada[i];
                        
            sig_matriz->sigPtr = m;
            _selected_object->mptr = sig_matriz;
                        
            glutPostRedisplay();
        }
        else if(modo == CAMARA)
        {
            if(vista == ANALISIS)       
                glMultMatrixd(_selected_camara->M);
        
            glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->M);    
        
            glutPostRedisplay();
        }
    }    
}  


