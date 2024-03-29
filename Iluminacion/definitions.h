#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <GL/gl.h>
#include <GL/glu.h>

/** DEFINITIONS **/

#define KG_WINDOW_TITLE                     "Practica GPO"
#define KG_WINDOW_WIDTH                     600
#define KG_WINDOW_HEIGHT                    400
#define KG_WINDOW_X                         50
#define KG_WINDOW_Y                         50

#define KG_MSSG_SELECT_FILE                 "Escribe el path del fichero objeto: "
#define KG_MSSG_FILENOTFOUND                "No existe ese fichero!!"
#define KG_MSSG_INVALIDFILE                 "Ha habido algun problema con el fichero ..."
#define KG_MSSG_EMPTYFILE                   "Este fichero esta vacio."
#define KG_MSSG_FILEREAD                    "Problema al leer el fichero."
#define KG_MSSG_NOOBJ                       "No hay ningun objeto cargado."
#define KG_MSSG_NODELETE                    "No se puede borrar nada!!"
#define KG_MSSG_NOCAM                       "No hay mas camaras cargas..."
#define KG_MSSG_NOMODECAM                   "No se encuentra en modo camara."

#define KG_STEP_MOVE                        5.0f
#define KG_STEP_ROTATE                      10.0f
#define KG_STEP_ZOOM                        0.75
#define KG_STEP_CAMERA_ANGLE                5.0f

#define KG_ORTHO_X_MIN_INIT                -5
#define KG_ORTHO_X_MAX_INIT                 5
#define KG_ORTHO_Y_MIN_INIT                -5
#define KG_ORTHO_Y_MAX_INIT                 5
#define KG_ORTHO_Z_MIN_INIT                -100
#define KG_ORTHO_Z_MAX_INIT                 10000

#define KG_COL_BACK_R                       0.30f
#define KG_COL_BACK_G                       0.30f
#define KG_COL_BACK_B                       0.30f
#define KG_COL_BACK_A                       1.00f

#define KG_COL_SELECTED_R                   1.00f
#define KG_COL_SELECTED_G                   0.75f
#define KG_COL_SELECTED_B                   0.00f

#define KG_COL_NONSELECTED_R                1.00f
#define KG_COL_NONSELECTED_G                1.00f
#define KG_COL_NONSELECTED_B                1.00f

#define KG_COL_X_AXIS_R                     1.0f
#define KG_COL_X_AXIS_G                     0.0f
#define KG_COL_X_AXIS_B                     0.0f

#define KG_COL_Y_AXIS_R                     0.0f
#define KG_COL_Y_AXIS_G                     1.0f
#define KG_COL_Y_AXIS_B                     0.0f

#define KG_COL_Z_AXIS_R                     0.0f
#define KG_COL_Z_AXIS_G                     0.0f
#define KG_COL_Z_AXIS_B                     1.0f

#define KG_MAX_DOUBLE                       10E25

// referencia
#define GLOBAL                              0
#define LOCAL                               1

// modo
#define OBJETO                              0
#define CAMARA                              1
#define CAMARAOBJETO                        2
#define ILUMINACION                         3

// proyeccion
#define PERSPECTIVA                         0
#define PARALELO                            1

// vista
#define VUELO                               0
#define ANALISIS                            1

// movimiento
#define ROTAR                               0
#define TRASLADAR                           1
#define ESCALAR                             2

// iluminacion
#define ON                                  1
#define OFF                                 0

#define NONE                               -1
#define SOL                                 0
#define BOMBILLA                            1
#define FOCO                                2
#define FOCO_OBJETO                         3

#define FLAT                                0
#define SMOOTH                              1

#define PATHCAM                             "camaraprincipal.obj"
#define LIGHTPATH                           "lightsource.obj"


/** STRUCTURES **/

/****************************
 * Structure to store the   *
 * coordinates of 3D points *
 ****************************/
typedef struct {
    GLdouble x, y, z;
} point3;

/*****************************
 * Structure to store the    *
 * coordinates of 3D vectors *
 *****************************/
typedef struct {
    GLdouble x, y, z;
} vector3;

/****************************
 * Structure to store the   *
 * colors in RGB mode       *
 ****************************/
typedef struct {
    GLfloat r, g, b;
} color3;

/****************************
 * Structure to store       *
 * objects' vertices         *
 ****************************/
typedef struct {
    point3 coord;                       /* coordinates,x, y, z */
    GLint num_faces;                    /* number of faces that share this vertex */
    vector3 normal;                     /* vector normal del vector*/
} vertex;

/****************************
 * Structure to store       *
 * objects' faces or        *
 * polygons                 *
 ****************************/
typedef struct {
    GLint num_vertices;                 /* number of vertices in the face */
    GLint *vertex_table;                /* table with the index of each vertex */
    vector3 normal;                     /* the normal vecto of a poligon */
} face;

/****************************
 * Structure of the matrix  *
 * 16 elems and a pointer   *
 ****************************/
typedef struct elemM{
    GLfloat M[16];
    struct elemM *sigPtr;
} elemM;

typedef struct proy{
  //  GLfloat angulo;
    GLfloat cerca; // near
    GLfloat lejos; // far 
    GLfloat bajo; // bottom
    GLfloat alto; // top 
    GLfloat izq; //left
    GLfloat der; //right
}proy;


typedef struct camara{
    GLfloat M[16];                     /* the csr matrix of the camera*/
    GLfloat Mobj[16];                  /* the matrix of the obj asociated */
    GLfloat Minv[16];
    int tipo_proyeccion;                /* projection type */
    GLint num_vertices;                 /* number of vertices in the camara*/
    vertex *vertex_table;               /* table of vertices */
    GLint num_faces;                    /* number of faces in the camara */
    face *face_table;                   /* table of faces */
    int vistacam;                          /* tipo de camara: pura o vuelo*/
    point3 min;                         /* coordinates' lower bounds */
    point3 max;                         /* coordinates' bigger bounds */
    struct camara *next;
    proy proj;
} camara;

/****************************
 * Structure of the matrix  *
 *  source of the material  *
 ****************************/
typedef struct material{
    GLfloat amb[4];    /*ambiente RGB*/
    GLfloat dif[4];    /*diffuse RGB*/
    GLfloat spec[4];   /*specular RGB*/
    GLfloat pos[4];     /*position */
    GLfloat shiny[1];     /*shiniess*/
} material;

/****************************
 * Structure to store a     *
 * pile of 3D objects       *
 ****************************/
struct object3d{
    GLint num_vertices;                 /* number of vertices in the object*/
    vertex *vertex_table;               /* table of vertices */
    GLint num_faces;                    /* number of faces in the object */
    face *face_table;                   /* table of faces */
    point3 min;                         /* coordinates' lower bounds */
    point3 max;                         /* coordinates' bigger bounds */
    elemM *mptr;
    GLint shade;                        /* shade property */
    material *material_light;     /* material of the object */
    struct object3d *next;              /* next element in the pile of objects */
};

typedef struct object3d object3d;

/****************************
 * Structure of the matrix  *
 *  source of light         *
 ****************************/
typedef struct iluminacion_objetos{
    GLfloat diffuse[4];    // color del material cuando esta iluminao directamente
    GLfloat ambient[4];    // color del material cuando esta en sombra
    GLfloat specular[4];   // colo del reflejo cuando esta iluminao directamente (ex: cuando una manzana se le da mucha luz brilla)
    GLfloat position[4];   // cantidad de brillo especular del material
    GLint activado;
    GLint tipo_luz;     /* none, sol, bombilla, foco o focoobjeto*/
    GLfloat cut_off;
    GLfloat objeto_luz[16];   
    GLfloat spot_direction[3];
} iluminacion_objetos;



#endif // DEFINITIONS_H
