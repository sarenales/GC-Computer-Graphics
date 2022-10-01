
typedef struct punto
{
float x, y, z, u,v;
} punto;

typedef struct hiruki
{
punto p1,p2,p3;
} hiruki;

void cargar_triangulos(int *hkopptr, hiruki **hptrptr);
