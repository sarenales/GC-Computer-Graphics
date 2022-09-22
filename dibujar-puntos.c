//	Program developed by Silvia Arenales Munoz
//	
//	Informatika Fakultatea
//	Euskal Herriko Unibertsitatea
//	http://www.ehu.eus/if
//
// to compile it: gcc dibujar-puntos.c -lGL -lGLU -lglut
//
// This example shows :the use of GL_POINTS
//


#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "cargar-triangulo.h"


// texturaren informazioa

extern void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr);
unsigned char *bufferra;
int dimx,dimy;
int num_triangles;
hiruki *triangulosptr;
int indice = 0;


unsigned char * color_textura(float u, float v)
{
	int desplx = dimx * u;		// columna	
	int desply = dimy *(1-v);	// fila
	
	return(bufferra + ((desplx*dimx)+desply)*3 );
}

static void establecer_orden_altura(punto *hptrptr, punto *lptrptr, punto *mptrptr){ 	// doble puntero?¿?¿?¿?¿
	
	/* con entrada (punto *hptrptr, punto *lptrptr, punto *mptrptr)*/
	punto* aux;

	if(hptrptr-> y < lptrptr-> y ){
		aux = lptrptr;
		lptrptr = hptrptr;
		hptrptr = aux;
	}
	if(lptrptr-> y < mptrptr-> y){
		aux = mptrptr;
		mptrptr = lptrptr;
		lptrptr = aux;
	}
	if(hptrptr-> y < lptrptr-> y){
		aux = lptrptr;
		lptrptr = hptrptr;
		hptrptr = aux;
	}
	
}

static punto * calculcar_corte(int alt, punto a, punto b, punto c)
{
	c.x = a.x + (alt-a.y) * (b.x-a.x) / (b.y - a.y);
}


static void dibujar_seccion(punto *d, punto *e)
{
	unsigned char* colorvd = color_textura(d.u, d.v);
	unsigned char* colorve = color_textura(e.u, e.v);
	unsigned char rd, gd, bd, re, ge, be;
	int i, j;
	
	for (i=0;i<500;i++)
		for (j=0;j<500;j++)
		{
			rd	= colorvd[0];gd = colorvd[1];bd	= colorvd[2];     
			glBegin( GL_POINTS );
			glColor3ub(rd,gd,bd);
			glVertex3f(i,j,0.);		// Dos dimensiones
			glEnd();
		}

}

static void dibujar_triangulo(hiruki t)
{
	punto hptr, mptr, lptr;
	punto *c1, *c2;
	int altura;

	hptr = t.p1; mptr = t.p2; lptr = t.p3;

	// Se ordena los puntos respecto hptr y demanera descendente. hptr (high) >> mptr (medium) >> lptr (low)
	establecer_orden_altura(&hptr, &lptr, &mptr);

	int xhptr = hptr.x, yhptr = hptr.y;
	int xlptr = lptr.x, ylptr = lptr.y;
	int xmptr = mptr.x, ymptr = mptr.y;

	altura = yhptr;

	// parte superior del triangulo
	for(altura = hptr.y; altura > mptr.y; altura--)
	{
		calcular_corte(altura, hptr, mptr, &c1);	// info corte 1
		calcular_corte(altura, hptr, lptr, &c2);	// info corte 2
		if(c1->x < c2->x)
			dibujar_seccion(c1, c2);
		else
			dibujar_seccion(c2,c1);
	}
	// parte inferior
	for(altura = mptr.y; altura >= lptr.y; altura--)
	{
		calcular_corte(altura, mptr, lptr, &c1);
		calcular_corte(altura, hptr, lptr, &c2);
		if(c1->x < c2->x)
			dibujar_seccion(c1,c2);
		else
			dibujar_seccion(c2,c1);
	}
}

static void marraztu(void)
{
/*	float u,v;
	float i,j;
	unsigned char* colorv;
	unsigned char r,g,b;
*/
	// borramos lo que haya...
	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(0.0, 500.0, 0.0, 500.0,-250.0, 250.0);

	// por ahora dibujamos todos los pixels de la ventana de 500x500 con el color que devuelve la función color_textura
	// pero hay que llamar a la función que dibuja un triangulo con la textura mapeada:

	dibujar_triangulo(triangulosptr[indice]);
/*
	for (i=0;i<500;i++)
		for (j=0;j<500;j++)
			{
			u = i/500.0;
			v = j/500.0;
			colorv=  color_textura(u, v); // si esta función es correcta se ve la foto en la ventana
			r= colorv[0];
			g=colorv[1];
			b=colorv[2];     
		glBegin( GL_POINTS );
		glColor3ub(r,g,b);
		glVertex3f(i,j,0.);	// por ahora 2 dimensiones
		glEnd();
		}
*/
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
				indice = 0;
			}
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
    load_ppm("silvia.ppm", &bufferra, &dimx, &dimy);
	
    // cargar_triangulos(&num_triangles, &triangulosptr);

	glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );

	glutMainLoop();

	return 0;   
}
