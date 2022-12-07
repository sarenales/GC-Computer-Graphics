#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>

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

int movimiento = 1; // rota = 0 ; trasladar = 1; escalar = 2;

int i;
float E, at, Vup;   // E = posicion de la camara; 
                    // at = punto donde mira la camara; 
                    // Vvp = vector verticalidad



void get_matriz_objeto(GLdouble* m1, vector3* e,GLdouble* m2){
    for(int i = 0; i < 3; i++){
    	for (int j = 0; j < 3;j++){
    			m2[i+4*j] = m1[4*i+j];
    	}
    }
    m2[3] = 0;
    m2[7] = 0;
    m2[11] = 0;
    m2[12] = e->x;
    m2[13] = e->y;
    m2[14] = e->z;
    m2[15] = 1;
}
void cargar_obj_camara(camara *cam){
    object3d auxiliar_object;
    read_wavefront("camara.obj", &auxiliar_object);

    /*There is not error treatment because camara.obj will always be there*/

    cam->face_table = auxiliar_object.face_table;
    cam->num_faces = auxiliar_object.num_faces;
    cam->vertex_table = auxiliar_object.vertex_table;
    cam->num_vertices = auxiliar_object.num_vertices;
    cam->max = auxiliar_object.max;
    cam->min = auxiliar_object.min;

}

void mirar_obj_selec(){
		vector3 obj,cam;
		obj.x = _selected_object->mptr->M[12];
		obj.y = _selected_object->mptr->M[13];
		obj.z = _selected_object->mptr->M[14];
        
		cam.x = _selected_camara->M[12];
		cam.y = _selected_camara->M[13];
		cam.z = _selected_camara->M[14];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(cam.x,cam.y,cam.z,obj.x,obj.y,obj.z,0,1,0);
		glGetDoublev(GL_MODELVIEW_MATRIX,_selected_camara->M);
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
                    auxiliar_object->mptr=(elemM*) malloc(sizeof(elemM)); // malloc devuelve un puntero. Un void pointer¿?

                    // cargue identidad en model view
                    glMatrixMode(GL_MODELVIEW);  // applies subsequent matrix operations to the projection matiz stack
                    glLoadIdentity();            // carga la matriz identidad
                    glGetDoublev(GL_MODELVIEW_MATRIX, auxiliar_object->mptr->M); // asocia la matriz a auxiliar
                    auxiliar_object->mptr->sigPtr = 0;     // no hay siguiente matriz por ahora
                    printf("%s\n",KG_MSSG_FILEREAD);
                break;
            }
        break;
      
        case 'N':
        case 'n': // anadir camara
            auxiliar_camara = malloc( sizeof(camara) );
            vector3* e = malloc( sizeof(vector3) );
            e->x = 10;
            e->y = 15;
            e->z = 10;
            elemM* m1 = malloc( sizeof(elemM) );
            elemM* m2 = malloc( sizeof(elemM) );
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(e->x,e->y,e->z,0,0,0,0,1,0);
            glGetDoublev(GL_MODELVIEW_MATRIX,m1->M);
            get_matriz_objeto(m1->M,e,m2->M);

            auxiliar_camara->M = m1->M;
            auxiliar_camara->M = m2->M;
            
            auxiliar_camara->projection[0] = -0.1;
            auxiliar_camara->projection[1] = 0.1;
            auxiliar_camara->projection[2] = -0.1;
            auxiliar_camara->projection[3] = 0.1;
            auxiliar_camara->projection[4] = 0.1;
            auxiliar_camara->projection[5] = 10000;
            
            cargar_obj_camara(auxiliar_camara);
            
            auxiliar_camara->next = _first_camara;
            _first_camara = auxiliar_camara;
            _selected_camara = _first_camara;
            
            if(vista == ANALISIS){
                mirar_obj_selec();
            }
		break;
        
        
        
        
        
        case 9: /* <TAB> */
            // seleccionar siguiente obj o camara
        if (_selected_object!=NULL){
            _selected_object = _selected_object->next;
            /*The selection is circular, thus if we move out of the list we go back to the first element*/
            if (_selected_object == 0)
                _selected_object = _first_object;
            if(vista == ANALISIS)
                mirar_obj_selec();
        }else 
            printf("No hay ningun objeto cargado\n");
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
                    mirar_obj_selec();
                }
            }else{
                printf("No se puede borrar nada! \n");
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
                printf("No hay ningun objeto cargado.\n");
            
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
                printf("No hay ningun objeto cargado.\n");
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
            if (modo == CAMARA){
                if(vista != ANALISIS){
                    vista = ANALISIS;
                    printf("Vista de la camara: Analisis\n");
                    mirar_obj_selec();
                }
            }
            else if(modo == OBJETO){
                if(referencia != GLOBAL){
                    referencia = GLOBAL;
                    printf("Sistema de referencia: Global\n");
                }
            }
    	break;
            
        case 'L':
        case 'l':
            if (modo == CAMARA){
                if(vista != VUELO){
                    vista = VUELO;
                    printf("Vista de la camara: Vuelo\n");
                }
            }
            else if (modo == OBJETO){
                if(referencia != LOCAL){
                    referencia = LOCAL;
                    printf("Sistema de referencia: Local\n");
                }
            }
    	break;
        
        
        
        
        
        case 'Z':
        case 'z': //  (Z,z) Deshacer
            if(_selected_object != NULL)
                if(_selected_object->mptr->sigPtr != NULL)
                _selected_object->mptr = _selected_object->mptr->sigPtr;
            printf("Deshaciendo...\n");
        break;
        
        
        
        
        /* Intercambiar de camara*/
        case 'k': // Cambiar de camara entre camaras puras
            if (_selected_camara!=NULL){
                _selected_camara = _selected_camara->next;
                if (_selected_camara == 0) _selected_camara = _first_camara;
                if(vista == ANALISIS)
                    mirar_obj_selec();
            }else 
                printf("No hay ninguna camara cargada\n");
        break;

        case 'K': // intercambiar de camara pura a camara asociada a obj y viceversa
            // visualizar desde el objeto
            if (_selected_camara!=NULL || _selected_object!=NULL){
                if(modo != CAMARAOBJETO){
                    modo = CAMARAOBJETO;
                    printf("Punto de vista del objeto.\n");
                }
                else{
                    modo = CAMARA;
                    printf("Modo Camara. \n");
                }
            }
        break;
        
        
        
        
        case 'p':
        case 'P':
            if (_selected_camara!=NULL || _selected_object != NULL){
                proyeccion = 1 - proyeccion;
                if(proyeccion){
                    printf("Modo de vista: Perspectiva\n");
                    _selected_camara->projection[0] = -0.1;
                    _selected_camara->projection[1] = 0.1;
                    _selected_camara->projection[2] = -0.1;
                    _selected_camara->projection[3] = 0.1;
                    _selected_camara->projection[4] = 0.1;
                    _selected_camara->projection[5] = 10000;
                }
                else{
                    printf("Modo de vista: Paralelo\n");
                    _selected_camara->projection[0] = _ortho_x_min;
                    _selected_camara->projection[1] = _ortho_x_max;
                    _selected_camara->projection[2] = _ortho_y_min;
                    _selected_camara->projection[3] = _ortho_y_max;
                    _selected_camara->projection[4] = 0;
                    _selected_camara->projection[5] = _ortho_z_max;
                }
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
        
        if(modo == OBJETO) // visualise obj
        {
            m = _selected_object->mptr;
            if(referencia == 0)         // Referencia LOCAL
               glLoadMatrixd(m->M);
            else
               glLoadIdentity();
        }
        else if(modo == CAMARA) // visualice camara
        {
            if(referencia == 0)         // Referencia LOCAL
                glLoadMatrixd(_selected_camara->M);
            else
               glLoadIdentity();
        }
            switch (key) {
                case 101: // UP  Trasladar +Y; Escalar + Y; Rotar +X 
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,1,0,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,1,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,1.1,1);
                    
                    printf("UP \n");
                    break;
                    
                case 103: // DOWN Trasladar -Y; Escalar - Y;  Rotar -X 
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,-1,0,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,-1,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,0.9,1);
                
                    printf("DOWN \n");
                    break;
                case 102: // RIGHT Trasladar +X; Escalar +X;  Rotar +Y 
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,1,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(1,0,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1.1,1,1);
                    

                    printf("RIGHT \n");
                    break;
                case 100: // LEFT  Trasladar -X; Escalar -X;  Rotar -Y 
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,-1,0);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(-1,0,0);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(0.9,1,1);
                    
                    printf("LEFT \n");
                    break;
                case 105: // AVPAG Trasladar +Z; Escalar +Z;  Rotar +Z             
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,0,1);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,0,1);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,1,1.1);
                    
                    printf("AVPAG \n");
                    break;
                    
                case 104: // REPAG Trasladar -Z; Escalar - Z;  Rotar -Z 
                    if(movimiento == 0)         // rotar     (0)
                        glRotated(10,0,0,-1);
                    else if(movimiento == 1)    // trasladar (1)
                        glTranslated(0,0,-1);
                    else if(movimiento == 2)    // escalar   (2)
                        glScaled(1,1,0.9);
                    
                    printf("REPAG \n");
                    break;
            }
            if(modo == OBJETO) // visualise obj
            {
                if(referencia == 1)         // Referencia GLOBAL** modo vuelo
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
                if(referencia == 1)         // Referencia GLOBAL** modo vuelo
                    glMultMatrixd(_selected_camara->M);
            
                glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camara->M);    
            
                glutPostRedisplay();
            }
        }    
}  


