#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "definitions.h"

#define MAXLINE 200
extern object3d *_camara_objeto;

/*
 * Auxiliar function to process each line of the file
 */
static int sreadint(char * lerroa, int * zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
        zenbakiak[kont++] = zbk;
    }
    return (kont);
}

static int sreadint2(char * lerroa, int * zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
	while ((*s != ' ')&&(*s !='\0')) s++;  // jump vector normal information
        zenbakiak[kont++] = zbk;
    }
printf("%d numbers in the line\n",kont);
    return (kont);
}


/**
 * función para obtener el vector normal de una superficie dados 3 puntos ordenados
 * @param index1 indice al punto 1
 * @param index2 indice al punto 2
 * @param index3 indice al punto 3
 * @param vertex_table la tabla de vertices de donde sacar los puntos
 * @return
 */
vector3 calculate_surface_normal(int index1, int index2, int index3, vertex *vertex_table){
    vector3 normal_vector;

    vector3 u;
    u.x = vertex_table[index2].coord.x - vertex_table[index1].coord.x;
    u.y = vertex_table[index2].coord.y - vertex_table[index1].coord.y;
    u.z = vertex_table[index2].coord.z - vertex_table[index1].coord.z;

    vector3 v;
    v.x = vertex_table[index3].coord.x - vertex_table[index1].coord.x;
    v.y = vertex_table[index3].coord.y - vertex_table[index1].coord.y;
    v.z = vertex_table[index3].coord.z - vertex_table[index1].coord.z;

    normal_vector.x = (u.y * v.z) - (u.z * v.y);
    normal_vector.y = (u.z * v.x) - (u.x * v.z);
    normal_vector.z = (u.x * v.y) - (u.y * v.x);

    return normal_vector;
}
/**
 * función para calcular todos los vectores normales de un objeto 3d
 * @param obj el objeto en cuestión
 */
void normal_vectors(object3d *obj){
    GLint f, v;
    GLint index1, index2, index3, vindex;
    GLdouble norma;
    vector3 vector_normal;

    //inicializamos los vectores normales de los vertices
    for(v = 0; v<obj->num_vertices; v++){
        obj->vertex_table[v].normal = (vector3){.x = 0, .y = 0, .z = 0};
    }

    for (f = 0; f < obj->num_faces; f++){
        index1 = obj->face_table[f].vertex_table[0];
        index2 = obj->face_table[f].vertex_table[1];
        index3 = obj->face_table[f].vertex_table[2];

        vector_normal = calculate_surface_normal(index1, index2, index3, obj->vertex_table);

        obj->face_table[f].normal = vector_normal;

        norma = sqrt(pow(obj->face_table[f].normal.x, 2) +
                     pow(obj->face_table[f].normal.y, 2) +
                     pow(obj->face_table[f].normal.z, 2));

        //canonizamos los vectores
        obj->face_table[f].normal.x /= norma;
        obj->face_table[f].normal.y /= norma;
        obj->face_table[f].normal.z /= norma;

        //sumamos el vector calculado a los vectores normales de los vertices de la cara
        for(v = 0; v<obj->face_table[f].num_vertices; v++){
            vindex = obj->face_table[f].vertex_table[v];
            obj->vertex_table[vindex].normal.x += obj->face_table[f].normal.x;
            obj->vertex_table[vindex].normal.y += obj->face_table[f].normal.y;
            obj->vertex_table[vindex].normal.z += obj->face_table[f].normal.z;
        }

    }

    for(v = 0; v<obj->num_vertices; v++){
        norma = sqrt(pow(obj->vertex_table[v].normal.x, 2) +
                     pow(obj->vertex_table[v].normal.y, 2) +
                     pow(obj->vertex_table[v].normal.z, 2));

        //canonizamos los vectores
        obj->vertex_table[v].normal.x /= norma;
        obj->vertex_table[v].normal.y /= norma;
        obj->vertex_table[v].normal.z /= norma;
    }


}
/**
 * @brief Function to read wavefront files (*.obj)
 * @param file_name Path of the file to be read
 * @param object_ptr Pointer of the object3d type structure where the data will be stored
 * @return Result of the reading: 0=Read ok, 1=File not found, 2=Invalid file, 3=Empty file
 */
int read_wavefront(char * file_name, object3d * object_ptr) {
    vertex *vertex_table;
    face *face_table;
    int num_vertices = -1, num_faces = -1, count_vertices = 0, count_faces = 0;
    FILE *obj_file;
    char line[MAXLINE], line_1[MAXLINE], aux[45];
    int k;
    int i, j;
    int values[MAXLINE];


    /*
     * The function reads twice the file. In the first read the number of
     * vertices and faces is obtained. Then, memory is allocated for each
     * of them and in the second read the actual information is read and
     * loaded. Finally, the object structure is created
     */
    if ((obj_file = fopen(file_name, "r")) == NULL) return (1);
    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        i = 0;
        while (line[i] == ' ') i++;
        if ((line[0] == '#') && ((int) strlen(line) > 5)) {
            i += 2;
            j = 0;
            while (line[i] != ' ') line_1[j++] = line[i++];
            i++;
            line_1[j] = '\0';
            j = 0;
            while ((line[i] != ' ') && (line[i] != '\0'))
                aux[j++] = line[i++];
            aux[j] = 0;
            if (strcmp(aux, "vertices") == 0) {
                num_vertices = atoi(line_1);
            }
            if (strncmp(aux, "elements", 7) == 0) {
                num_faces = atoi(line_1);
            }
        } else {
            if (strlen(line) > 6) {
                if (line[i] == 'f' && line[i + 1] == ' ')
                    count_faces++;
                else if (line[i] == 'v' && line[i + 1] == ' ')
                    count_vertices++;
            }
        }
    }
    fclose(obj_file);
printf("1 pasada: num vert = %d (%d), num faces = %d(%d) \n",num_vertices,count_vertices,num_faces,count_faces);
    if ((num_vertices != -1 && num_vertices != count_vertices) || (num_faces != -1 && num_faces != count_faces)) {
        printf("WARNING: full file format: (%s)\n", file_name);
        //return (2);
    }
    if (num_vertices == 0 || count_vertices == 0) {
        printf("No vertex found: (%s)\n", file_name);
        return (3);
    }
    if (num_faces == 0 || count_faces == 0) {
        printf("No faces found: (%s)\n", file_name);
        return (3);
    }

    num_vertices = count_vertices;
    num_faces = count_faces;

    vertex_table = (vertex *) malloc(num_vertices * sizeof (vertex));
    face_table = (face *) malloc(num_faces * sizeof (face));

    obj_file = fopen(file_name, "r");
    k = 0;
    j = 0;

    for (i = 0; i < num_vertices; i++)
        vertex_table[i].num_faces = 0;

    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        switch (line[0]) {
            case 'v':
            if (line[1] == ' ')  // vn not interested
		{
                sscanf(line + 2, "%lf%lf%lf", &(vertex_table[k].coord.x),
                        &(vertex_table[k].coord.y), &(vertex_table[k].coord.z));
                k++;
		}
               break;

            case 'f':
	            if (line[1] == ' ') // fn not interested
                {
                for (i = 2; i <= (int) strlen(line); i++)
                    line_1[i - 2] = line[i];
		            line_1[i-2] = '\0';
                    face_table[j].num_vertices = sreadint2(line_1, values);
                    //printf("f %d vertices\n",face_table[j].num_vertices);
                    face_table[j].vertex_table = (int *) malloc(face_table[j].num_vertices * sizeof (int));
                    for (i = 0; i < face_table[j].num_vertices; i++) {
                    face_table[j].vertex_table[i] = values[i] - 1;
                    //printf(" %d ",values[i] - 1);
                    vertex_table[face_table[j].vertex_table[i]].num_faces++;
                    }
                    //printf("\n");
                j++;
                }
              break;
        }
    }

    fclose(obj_file);

printf("2 pasada\n");

    /*
     * Information read is introduced in the structure */
    object_ptr->vertex_table = vertex_table;
    object_ptr->face_table = face_table;
    object_ptr->num_vertices = num_vertices;
    object_ptr->num_faces = num_faces;


    /*
     * The maximum and minimum coordinates are obtained **/
    object_ptr->max.x = object_ptr->vertex_table[0].coord.x;
    object_ptr->max.y = object_ptr->vertex_table[0].coord.y;
    object_ptr->max.z = object_ptr->vertex_table[0].coord.z;
    object_ptr->min.x = object_ptr->vertex_table[0].coord.x;
    object_ptr->min.y = object_ptr->vertex_table[0].coord.y;
    object_ptr->min.z = object_ptr->vertex_table[0].coord.z;

    for (i = 1; i < object_ptr->num_vertices; i++)
    {
        if (object_ptr->vertex_table[i].coord.x < object_ptr->min.x)
            object_ptr->min.x = object_ptr->vertex_table[i].coord.x;

        if (object_ptr->vertex_table[i].coord.y < object_ptr->min.y)
            object_ptr->min.y = object_ptr->vertex_table[i].coord.y;

        if (object_ptr->vertex_table[i].coord.z < object_ptr->min.z)
            object_ptr->min.z = object_ptr->vertex_table[i].coord.z;

        if (object_ptr->vertex_table[i].coord.x > object_ptr->max.x)
            object_ptr->max.x = object_ptr->vertex_table[i].coord.x;

        if (object_ptr->vertex_table[i].coord.y > object_ptr->max.y)
            object_ptr->max.y = object_ptr->vertex_table[i].coord.y;

        if (object_ptr->vertex_table[i].coord.z > object_ptr->max.z)
            object_ptr->max.z = object_ptr->vertex_table[i].coord.z;

    }
    normal_vectors(object_ptr);
    return (0);
}

void load_presentation(){
    
    _camara_objeto = (object3d *) malloc(sizeof(object3d));
    
    read_wavefront(PATHCAM, _camara_objeto);
    printf("Lee correctamente la camara inicial. \n");
}
