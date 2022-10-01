//	Program developed by Silvia Arenales Munoz
//	
//	Informatika Fakultatea
//	Euskal Herriko Unibertsitatea
//	http://www.ehu.eus/if
//
// to compile it: gcc dibujar-puntos.c -lGL -lGLU -lglut
// I have made my own file to compile it ./compi
//
// This example shows :the use of GL_POINTS
//


#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "cargar-triangulo.h"


extern void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr);
unsigned char *bufferra;
int dimx,dimy;
int num_triangles;
hiruki *triangulosptr;
int indice = 0;
float alfa,beta,gama;


unsigned char * color_textura(float u, float v)
{
	int desplx = dimx * u;		// columna	
	int desply = dimy *(1-v);	// fila
	
	return(bufferra + ((desplx*dimx)+desply)*3 );
}

static void establecer_orden_altura(punto *hptr, punto *lptr, punto *mptr)
{ 		
	punto aux;
	
	if((*hptr).y < (*lptr).y ){
		aux = *lptr;
		*lptr = *hptr;
		*hptr = aux;
	}	
	if((*lptr).y <(*mptr).y){
		aux = *mptr;
		*mptr = *lptr;
		*lptr = aux;
	}
	if((*hptr).y < (*lptr).y){
		aux = *lptr;
		*lptr = *hptr;
		*hptr = aux;
	}
	if((*lptr).y > (*mptr).y){
		aux = *mptr;
		*mptr = *lptr;
		*lptr = aux;
	}
}

static void calcular_corte(int alt, punto *p1ptr, punto *p2ptr, punto *corteptr)
{
	corteptr->y = alt;
	corteptr->x = ((p2ptr->x - p1ptr->x)/(p2ptr->y - p1ptr->y))*(alt - p1ptr->y)+(p1ptr->x);
	corteptr->u = ((p2ptr->u - p1ptr->u)/(p2ptr->y - p1ptr->y))*(alt - p1ptr->y)+(p1ptr->u);
	corteptr->v = ((p2ptr->v - p1ptr->v)/(p2ptr->y - p1ptr->y))*(alt - p1ptr->y)+(p1ptr->v);
}

void calcularbaricentro(punto *p1ptr,punto *p2ptr,punto *p3ptr,float i,float j,float *alfa,float *beta,float *gama){
	
	int x1,y1,x2,y2,x3,y3;
	
	x1 = p1ptr->x; y1 = p1ptr->y;
	x2 = p2ptr->x; y2 = p2ptr->y;
	x3 = p3ptr->x; y3 = p3ptr->y;

	int d;
	
	
	d = x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2;
	
	*alfa = ((x3-x2)*j+(y2-y3)*i+x3*y3-x3*y2)/d;
	*beta = ((x1-x3)*j+(y3-y1)*i+x3*y1-x1*y3)/d;
	*gama = ((x2-x1)*j+(y1-y2)*i+x1*y2-x2*y1)/d;
}


void calcularUV (punto *p1ptr, punto *p2ptr, punto *p3ptr, float *u, float *v){
	
	float u1,v1,u2,v2,v3,u3;
	
	u1 = p1ptr->u; v1 = p1ptr->v;
	u2 = p2ptr->u; v2 = p2ptr->v;
	u3 = p3ptr->u; v3 = p3ptr->v;
	
	*u = alfa*u1 + beta*u2 + gama*u3;
	*v = alfa*v1 + beta*v2 + gama*v3;
}

static void dibujar_seccion(punto *corte1, punto *corte2, punto *h, punto *m, punto *l)
{
	unsigned char* colorv;
	unsigned char r, g, b;
	float j;
	float u, v;
	punto aux;
	
	
	// Ordeno los puntos de cortes 
	
	if(corte1->x > corte2->x){
		aux=*corte1;
		*corte1=*corte2;
		*corte2=aux;
	}
	
	for (j=(corte1->x);j<=(corte2->x);j++)
	{
		// calculamos las coordenadas baricentricas y asi poder hallar el color de la textura
		calcularbaricentro(h,m,l,j,corte2->y,&alfa,&beta,&gama); 
		calcularUV(h,m,l,&u,&v);
		colorv = color_textura(u,v);
			r = colorv[0];
			g = colorv[1];
			b = colorv[2];  
		
		glBegin( GL_POINTS );
			glColor3ub(r,g,b);
			glVertex3f(j,corte1->y,0.);		// Dos dimensiones
		glEnd();
	}

}

static void dibujar_triangulo(hiruki *t)
{
	punto hptr, mptr, lptr;
	punto c1, c2;
	int altura;
	int i;
	
	hptr = t->p1; 
	mptr = t->p2; 
	lptr = t->p3;
	
	// Se ordena los puntos respecto hptr y demanera descendente. hptr (high) >> mptr (medium) >> lptr (low)
	establecer_orden_altura(&hptr, &lptr, &mptr);

	int xh = hptr.x, yh = hptr.y;
	int xl = lptr.x, yl = lptr.y;
	int xm = mptr.x, ym = mptr.y;


	// CONTROL DE CASOS:
	
	
 	if(xh == xl && xh == xm){
		// Caso 1: si los puntos estan sobrepuestos
		if(yh == yl && yl==ym){
			printf("Caso (1) de puntos sobrepuestos.\n");
			glBegin(GL_POINTS);
				glColor3ub(0,0,0);
				glVertex2d(hptr.x, hptr.y);
			glEnd();
		}
		else{	
			// Caso 2: si los puntos estan en la misma linea en vertical (eje y) 
			printf("Caso (2) los puntos estan en la misma linea vertical.\n");
			glBegin(GL_POINTS);
				for (i=hptr.y; i>= lptr.y; i--){ 
					glColor3ub(0,0,0);
					glVertex2d(hptr.x, i);
				}
			glEnd();	
		}
	}
	// Caso 3: estan en la misma linea en horizontal (eje x)
	else if(yh == yl && yl==ym){
		printf("Caso (3) los puntos estan la misma linea horizontal.\n");
		glBegin(GL_LINES);	
			glColor3ub(0,0,0);
			glVertex2f(hptr.x, hptr.y);
			glVertex2f(lptr.x, lptr.y);
		glEnd();
	}
	
	// Caso 4: si dos estan en la misma altura y el tercero no. 
	else if(xh==xm && xm==xl && yh!=yl){
		printf("Caso (4) si dos estan en la misma altura y el tercero no. Caso vertical. \n");
		glBegin(GL_POINTS);
			for (i=hptr.y; i>= lptr.y; i--){ 
				glColor3ub(0,0,0);
				glVertex2d(hptr.x, i);
			}
		glEnd();
	} 

	
	// Caso 5: triangulo con coordenadas diferentes
	
	else{
		// parte superior del triangulo
		printf("Caso (7) triangulo con coordenadas diferentes.\n");
		for(altura = yh; altura > ym; altura--)
		{		
			calcular_corte(altura, &hptr, &mptr, &c1);	// info corte 1
			calcular_corte(altura, &hptr, &lptr, &c2);	// info corte 2
			if(c1.x < c2.x)
				dibujar_seccion(&c1, &c2, &hptr, &mptr, &lptr);
			else
				dibujar_seccion(&c1, &c2, &hptr, &mptr, &lptr);
		}
		// parte inferior
		for(altura = ym; altura >= yl; altura--)
		{
			calcular_corte(altura, &mptr, &lptr, &c1);
			calcular_corte(altura, &hptr, &lptr, &c2);
			if(c1.x < c2.x)
				dibujar_seccion(&c1, &c2, &hptr, &mptr, &lptr);
			else
				dibujar_seccion(&c1, &c2, &hptr, &mptr, &lptr);
		}
	}
	
	
}

static void marraztu(void)
{
	// borramos lo que haya...
	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(0.0, 500.0, 0.0, 500.0,-250.0, 250.0);

	// pero hay que llamar a la función que dibuja un triangulo con la textura mapeada:
	printf("vamos a dibujar el triangulo numero (%d)\n", indice);
	dibujar_triangulo(&triangulosptr[indice]);
	glFlush();
}



// This function will be called whenever the user pushes one key
static void teklatua (unsigned char key, int x, int y)
{
	switch(key)
		{
		case 13:// <ENTER>
			printf ("ENTER: siguiente triángulo.\n");
			if(indice < num_triangles-1){
				indice++;
			}else{
				printf ("Ya hemos leido todo el fichero, empezamos de nuevo.\n");
				indice = 0;
			}
			marraztu;
			break;
		case 27:  // <ESC>
			exit( 0 );
			break;
		default:
			printf("%d %c\n", key, key );
		}

	// The screen must be drawn to show the new triangle
	glutPostRedisplay();
}


int main(int argc, char** argv)
{

	printf("This program draws points in the viewport \n");
	printf("Press <ESC> to finish\n");
	glutInit(&argc,argv);
	glutInitDisplayMode ( GLUT_RGB );
	glutInitWindowSize ( 500, 500 );
	glutInitWindowPosition ( 100, 100 );
	glutCreateWindow( "GL_POINTS" );
	
	printf("Voy a dibujar\n");
	glutDisplayFunc( marraztu );
	
	printf("Listo, teclea!\n");
	glutKeyboardFunc( teklatua );

	/* we put the information of the texture in the buffer pointed by bufferra. The dimensions of the texture are loaded into dimx and dimy */ 
    load_ppm("minisilvia.ppm", &bufferra, &dimx, &dimy);
	
    cargar_triangulos(&num_triangles, &triangulosptr);
	
	glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );

	glutMainLoop();

	return 0;   
}
