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
float q,w,e;


unsigned char * color_textura(float u, float v)
{
	int desplx = dimx * u;		// columna	
	int desply = dimy *(1-v);	// fila
	
	return(bufferra + ((desplx*dimx)+desply)*3 );
}

static void establecer_orden_altura(punto *hptrptr, punto *lptrptr, punto *mptrptr){ 	// doble puntero?¿?¿?¿?¿
	
	punto aux;
//	printf("ANTES DE ORDENADOR\n xh: %f | yh: %f  \n xl: %f | yl: %f \n xm: %f | ym: %f \n",
//			(*hptrptr).x,(*hptrptr).y,(*lptrptr).x,(*lptrptr).y,(*mptrptr).x,(*mptrptr).y);
	
	
	if((*hptrptr).y < (*lptrptr).y ){
		
		aux = *lptrptr;
		//printf("h %f|l: %f \n",(*hptrptr).y , (*lptrptr).y );
		*lptrptr = *hptrptr;
		*hptrptr = aux;
		//printf("h %f|l: %f \n",(*hptrptr).y , (*lptrptr).y );
	}	
	if((*lptrptr).y <(*mptrptr).y){
		aux = *mptrptr;
		//printf("m %f|l: %f \n",(*mptrptr).y , (*lptrptr).y );
		*mptrptr = *lptrptr;
		*lptrptr = aux;
		//printf("m %f|l: %f \n",(*mptrptr).y , (*lptrptr).y );
	}
	if((*hptrptr).y < (*lptrptr).y){
		aux = *lptrptr;
		//printf("h %f|l: %f \n",(*hptrptr).y , (*lptrptr).y );
		*lptrptr = *hptrptr;
		*hptrptr = aux;
		//printf("h %f|l: %f \n",(*hptrptr).y , (*lptrptr).y );
	}
	if((*lptrptr).y > (*mptrptr).y){
		aux = *mptrptr;
		//printf("m %f|l: %f \n",(*mptrptr).y , (*lptrptr).y );
		*mptrptr = *lptrptr;
		*lptrptr = aux;
		//printf("m %f|l: %f \n",(*mptrptr).y , (*lptrptr).y );
	}
	
	
	
//	printf("ORDENADO\n xh: %f | yh: %f  \n xl: %f | yl: %f \n xm: %f | ym: %f \n",
//			(*hptrptr).x,(*hptrptr).y,(*lptrptr).x,(*lptrptr).y,(*mptrptr).x,(*mptrptr).y);
}

static void calcular_corte(int alt, punto *p1ptr, punto *p2ptr, punto *corteptr)
{
	// printf("alt: %f | p1: %f | p2: %f \n", alt, (*p1ptr).x, (*p2ptr).x);
	
	float aux;
	
	aux = (*p1ptr).x + ( alt - (*p1ptr).y) * (*p2ptr).x - (*p1ptr).x / ((*p2ptr).y - (*p1ptr).y);
	
	corteptr->x = aux;
	
	//printf("Punto de corte: x: %f, y:%f\n", (*corteptr).x,(*corteptr).y  );
	
	/* corteptr->y = y;
	p2ptr->y - p1ptr->y -> y - p1ptr.y;
	p2ptr.u-p1ptr.u-> corteptr.u - p1ptr.u; */
}

void calcularbaricentro(punto *p1,punto *p2,punto *p3,float i,float j,float *q,float *w,float *e){

	*q =((p2->x)*(p3->y)+(p3->x)*j+i*(p2->y)-(p2->x)*j-i*(p3->y)-(p3->x)*(p2->y))/((p2->x)*(p3->y)+(p3->x)*(p1->y)+(p1->x)*(p2->y)-(p2->x)*(p1->y)-(p1->x)*(p3->y)-(p3->x)*(p2->y));

	*w =(i*(p3->y)+(p3->x)*(p1->y)+(p1->x)*j-i*(p1->y)-(p1->x)*(p3->y)-(p3->x)*j)/((p2->x)*(p3->y)+(p3->x)*(p1->y)+(p1->x)*(p2->y)-(p2->x)*(p1->y)-(p1->x)*(p3->y)-(p3->x)*(p2->y));

	*e =((p2->x)*j+i*(p1->y)+(p1->x)*(p2->y)-(p2->x)*(p1->y)-(p1->x)*j-i*(p2->y))/((p2->x)*(p3->y)+(p3->x)*(p1->y)+(p1->x)*(p2->y)-(p2->x)*(p1->y)-(p1->x)*(p3->y)-(p3->x)*(p2->y));

}


void calcularUV (punto *p1, punto *p2, punto *p3, float *u, float *v){
	*u = q*(p1->u) + w*(p2->u) + e*(p3->u);
	*v = q*(p1->v) + w*(p2->v) + e*(p3->v);
}

static void dibujar_seccion(punto *corte1, punto *corte2, punto *h, punto *m, punto *l)
{
	unsigned char* colorv;
	unsigned char r, g, b;
	float j;
	float u, v;
	punto aux;
	
	if(corte1->x > corte2->x){
		aux=*corte1;
		*corte1=*corte2;
		*corte2=aux;
	}
	
	for (j=(corte1->x);j<=(corte2->x);j++)
	{
		calcularbaricentro(h,m,l,j,corte2->y,&q,&w,&e); 
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
	punto hptr, mptr, lptr; 	// *hptr, *mptr, *lptr;
	punto c1, c2;
	int altura;
	
	hptr = t->p1; 
	mptr = t->p2; 
	lptr = t->p3;
	
	// Se ordena los puntos respecto hptr y demanera descendente. hptr (high) >> mptr (medium) >> lptr (low)
	printf("vamos a ordenar.\n");
	establecer_orden_altura(&hptr, &lptr, &mptr);
	printf("establecido el orden.\n");

	int xh = hptr.x, yh = hptr.y;
	int xl = lptr.x, yl = lptr.y;
	int xm = mptr.x, ym = mptr.y;

	// altura = yh - 1 ;
	printf("xh: %d | yh: %d  \n xl: %d | yl: %d \n xm: %d | ym: %d \n",xh,yh,xl,yl,xm,ym);
	// parte superior del triangulo
	for(altura = yh; altura > ym; altura--)
	{
		//printf("vamos a calcular el corte\n");		
		calcular_corte(altura, &hptr, &mptr, &c1);	// info corte 1
		printf("punto de corte %f .\n", c1.x);
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
	
	printf("vamos a dibujar el triangulo\n");
	dibujar_triangulo(&triangulosptr[indice]);
	
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
    load_ppm("silvia.ppm", &bufferra, &dimx, &dimy);
	
    cargar_triangulos(&num_triangles, &triangulosptr);
	
	glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );

	glutMainLoop();

	return 0;   
}
