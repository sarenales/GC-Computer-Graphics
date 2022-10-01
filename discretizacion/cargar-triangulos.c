#include <stdio.h>
#include <malloc.h>
#define MAXLINE 200

typedef struct punto
{
float x, y, z, u,v;
} punto;

typedef struct hiruki
{
punto p1,p2,p3;
} hiruki;

void cargar_triangulos(int *hkopptr, hiruki **hptrptr)
{
 FILE *obj_file;
 char line[MAXLINE];
 int i, num_triangles;

 if ((obj_file = fopen("triangles.txt", "r")) == NULL) 
        {
        *hkopptr= 0;
        return;
        }
 num_triangles=0;
 while (fscanf(obj_file, "\n%[^\n]", line) > 0) 
	{
        if (line[0] == 't')  // triangulo!
		{
                 num_triangles++;
		}
	}
 fclose(obj_file);
 *hkopptr= num_triangles;
 *hptrptr = (hiruki *)malloc(num_triangles * sizeof (hiruki));
 
 obj_file = fopen("triangles.txt", "r");

 i=0;
 while (fscanf(obj_file, "\n%[^\n]", line) > 0) 
	{
        if (line[0] == 't')  // triangulo!
		{
		sscanf(line + 2, "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f", &((*hptrptr)[i].p1.x),&((*hptrptr)[i].p1.y),&((*hptrptr)[i].p1.z),
		                                                   &((*hptrptr)[i].p1.u),&((*hptrptr)[i].p1.v),
		                                                   &((*hptrptr)[i].p2.x),&((*hptrptr)[i].p2.y),&((*hptrptr)[i].p2.z),
		                                                   &((*hptrptr)[i].p2.u),&((*hptrptr)[i].p2.v),
		                                                   &((*hptrptr)[i].p3.x),&((*hptrptr)[i].p3.y),&((*hptrptr)[i].p3.z),
		                                                   &((*hptrptr)[i].p3.u),&((*hptrptr)[i].p3.v));
                i++;
		}
	}
 fclose(obj_file);
}

/*
void main(int argc, char*argv[])
{
int num_triangles,i; 
hiruki *tptr;

cargar_triangulos(&num_triangles, &tptr);
for (i=0; i<num_triangles; i++)
	{
	printf("t: (%.1f, %.1f, %.1f) (%.1f, %.1f)", tptr[i].p1.x, tptr[i].p1.y, tptr[i].p1.z, tptr[i].p1.u, tptr[i].p1.v);
	printf("   (%.1f, %.1f, %.1f) (%.1f, %.1f)", tptr[i].p2.x, tptr[i].p2.y, tptr[i].p2.z, tptr[i].p2.u, tptr[i].p2.v);
	printf("   (%.1f, %.1f, %.1f) (%.1f, %.1f)\n", tptr[i].p3.x, tptr[i].p3.y, tptr[i].p3.z, tptr[i].p3.u, tptr[i].p3.v);
	}
}

*/