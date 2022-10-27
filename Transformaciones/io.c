#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>

extern object3d * _first_object;
extern object3d * _selected_object;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

int movimiento = 0; // rota = 0 ; trasladar = 1; escalar = 2;
int referencia = 0; // 0 = local & 1 = global


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
    printf("<CTRL + +>\t Aumentar el campo de vision\n");
    printf("<CTRL + ->\t Reducir el campo de vision\n\n");
    
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
    printf("<I> \t\t Aplicar transformaciones a la iluminacion seleccionada (desactiva objeto y camara)\n");
    
    printf("<UP> \t\t Trasladar +Y; Escalar + Y; Rotar +X\n");
    printf("<DOWN> \t\t Trasladar -Y; Escalar - Y;  Rotar -X\n");
    printf("<RIGHT> \t Trasladar +X; Escalar +X;  Rotar +Y\n");
    printf("<LEFT> \t\t Trasladar -X; Escalar -X;  Rotar -Y\n");
    printf("<AVPAG> \t Trasladar +Z; Escalar +X;  Rotar +Y\n");
    printf("<REPAG> \t Trasladar -Z; Escalar -Z; Rotar -Z\n");
    printf("<+> \t\t Escalar + en todos los ejes\n");
    printf("<-> \t\t Escalar - en todos los ejes\n");
    printf("<Z> \t Deshacer cambios\n");
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
    GLdouble wd,he,midx,midy;
    int i;
    char* fname;
    GLfloat matriz_rotada[16];
    
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
                    glLoadIdentity();
                    glGetDoublev(GL_MODELVIEW_MATRIX, auxiliar_object->mptr->M);
                    auxiliar_object->mptr->sigPtr = 0;     // no hay siguiente matriz por ahora

                    printf("%s\n",KG_MSSG_FILEREAD);
                    break;
            }
        break;
        case 9: /* <TAB> */
            // seleccionar siguiente obj
            _selected_object = _selected_object->next;
            /*The selection is circular, thus if we move out of the list we go back to the first element*/
            if (_selected_object == 0) _selected_object = _first_object;
            else printf("No hay ningun objeto cargado\n");
            break;

        case 127: /* <SUPR> */
            // elimina obj seleccionado
            /*Erasing an object depends on whether it is the first one or not*/
            if (_selected_object == _first_object){
                /*To remove the first object we just set the first as the current's next*/
                _first_object = _first_object->next;
                
                // a parte de borrar objto tambn su memoria asociada 
                // tabla_vertices, tabla_poligonos y puntero matrices
                
                for( i=0; i< _selected_object->num_faces; i++){
                    face poligono = _selected_object->face_table[i];
                    free(poligono.vertex_table);
                    (_selected_object->face_table[i]);
                }
                
                /*Once updated the pointer to the first object it is save to free the memory*/
                free(_selected_object);
                /*Finally, set the selected to the new first one*/
                _selected_object = _first_object;
            } else {
                /*In this case we need to get the previous element to the one we want to erase*/
                auxiliar_object = _first_object;
                while (auxiliar_object->next != _selected_object)
                    auxiliar_object = auxiliar_object->next;
                /*Now we bypass the element to erase*/
                auxiliar_object->next = _selected_object->next;
                /*free the memory*/
                free(_selected_object);
                /*and update the selection*/
                _selected_object = auxiliar_object;
            }
            break;

        case '-':
            // Reducir el volumen de visualización
            // Escalar - en todos los ejes (caso de objetos) o disminuir volumen de vision (caso camara)
           // if (glutGetModifiers() == GLUT_ACTIVE_CTRL){
           
            if(movimiento = 2){
                elemM *m = _selected_object->mptr;
                glMatrixMode(GL_MODELVIEW);
                glLoadMatrixf(m->M);
                glScalef(0.9,0.9,0.9);
                glGetFloatv(GL_MODELVIEW_MATRIX,matriz_rotada);
                
                elemM *sig_matriz = malloc(sizeof (elemM));
                for(int i = 0; i < 16; i++){
                    sig_matriz->M[i] = matriz_rotada[i];
                }
                sig_matriz->sigPtr = m;
                _selected_object->mptr = sig_matriz;
                
            }else{ 
                /*Increase the projection plane; compute the new dimensions*/
                wd=(_ortho_x_max-_ortho_x_min)/KG_STEP_ZOOM;
                he=(_ortho_y_max-_ortho_y_min)/KG_STEP_ZOOM;
                /*In order to avoid moving the center of the plane, we get its coordinates*/
                midx = (_ortho_x_max+_ortho_x_min)/2;
                midy = (_ortho_y_max+_ortho_y_min)/2;
                /*The the new limits are set, keeping the center of the plane*/
                _ortho_x_max = (midx + wd)/2;
                _ortho_x_min = (midx - wd)/2;
                _ortho_y_max = (midy + he)/2;
                _ortho_y_min = (midy - he)/2;
            }
            
                
            break;

        case '+':
            //  Incrementar el volumen de visualización
            
            if(movimiento = 2){
                elemM *m = _selected_object->mptr;
                glMatrixMode(GL_MODELVIEW);
                glLoadMatrixf(m->M);
                glScalef(1.1,1.1,1.1);
                glGetFloatv(GL_MODELVIEW_MATRIX,matriz_rotada);
                
                elemM *sig_matriz = malloc(sizeof (elemM));
                for(int i = 0; i < 16; i++){
                    sig_matriz->M[i] = matriz_rotada[i];
                }
                sig_matriz->sigPtr = m;
                _selected_object->mptr = sig_matriz;
            }
            else{
                //  Escalar + en todos los ejes (caso de objetos) o aumentar volumen de visión (caso cámara)
                /*Increase the projection plane; compute the new dimensions*/
                wd=(_ortho_x_max+_ortho_x_min)*KG_STEP_ZOOM;
                he=(_ortho_y_max+_ortho_y_min)*KG_STEP_ZOOM;
                /*In order to avoid moving the center of the plane, we get its coordinates*/
                midx = (_ortho_x_max+_ortho_x_min)/2;
                midy = (_ortho_y_max+_ortho_y_min)/2;
                /*The the new limits are set, keeping the center of the plane*/
                _ortho_x_max = (midx + wd)/2;
                _ortho_x_min = (midx - wd)/2;
                _ortho_y_max = (midy + he)/2;
                _ortho_y_min = (midy - he)/2;
            }
            
            
            break;

        case '?':
            //  Visualizar la ayuda
            print_help();
            break;

        case 27: /* <ESC> */
            // Finalizar la ejecucion de la aplicacion
            exit(0);
            break;
        
        // tipo de transformacion 
       

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

        // sistema de referencia
        case 'G': // G,g Activar transformaciones en el sistema de referencia del mundo (transformaciones globales)
        case 'g':
            referencia = 1;
            printf("Sistema de referencia: GLOBAL \n");
            break;
        case 'L': // L,l Activar transformaciones en el sistema de referencia local del objeto (objeto 3D, cámara o luces) )
        case 'l':
            referencia = 0;
                printf("Sistema de referencia: LOCAL \n");
            break;

        // elemento a transformar (en todo momento debe haber algun elemento transformable.)
        case 'O': // O,o  Aplicar transformaciones al objeto seleccionado (valor por defecto) (desactiva cámara y luz)
        case 'o':
            break;

        case 'C': // C,c   Aplicar transformaciones a la cámara actual (desactiva objeto y luz)
        case 'c':
            break;

        case 'I': // I,i   Aplicar transformaciones a la Iluminación selecionada (desactiva objeto y camara)
        case 'i':
            break;

        case 'Z':
        case 'z': //  (Z,z) Deshacer
            // if (glutGetModifiers() == GLUT_ACTIVE_CTRL){

            // }
            // break;


        default:
            /*In the default case we just print the code of the key. This is usefull to define new cases*/
            printf("%d %c\n", key, key);
            printf("keyboard");
        }
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
    object3d *auxiliar_object = 0;
    GLdouble wd,he,midx,midy;
    GLdouble matriz_rotada[16];
    elemM *m;
    
    switch (key) {
        case 101: // UP  Trasladar +Y; Escalar + Y; Rotar +X 
            
            m = _selected_object->mptr;
            
            glMatrixMode(GL_MODELVIEW);
            
            if(referencia == 0)         // Referencia LOCAL
                glLoadMatrixf(m->M);
            else
                glLoadIdentity();
            
            
            if(movimiento == 0)         // rotar     (0)
                glRotatef(10,1,0,0);
            else if(movimiento == 1)    // trasladar (1)
                glTranslatef(0,1,0);
            else if(movimiento == 2)    // escalar   (2)
                glScalef(1,1.1,1);
            
            if(referencia == 1)         // Referencia GLOBAL
                glLoadMatrixf(m->M);
            
            glGetDoublev(GL_MODELVIEW, matriz_rotada);
            
            
            elemM *sig_matriz = malloc(sizeof (elemM));
            
            for(int i =0; i<16; i++)
                sig_matriz->M[i] = matriz_rotada[i];
            
            sig_matriz->sigPtr = m;
            _selected_object->next->mptr = sig_matriz;
            break;
            
        case 103: // DOWN Trasladar -Y; Escalar - Y;  Rotar -X 
            break;
        case 102: // RIGHT Trasladar +X; Escalar +X;  Rotar +Y 
            break;
        case 100: // LEFT  Trasladar -X; Escalar -X;  Rotar -Y 
            break;
        case 105: // AVPAG Trasladar +Z; Escalar +Z;  Rotar +Z 
            break;
        case 104: // REPAG Trasladar -Z; Escalar - Z;  Rotar -Z 
            break;
        default:
            /*In the default case we just print the code of the key. This is usefull to define new cases*/
            printf("%d %c\n", key, key);
            printf("keyboard");
    }
    glutPostRedisplay();
}

