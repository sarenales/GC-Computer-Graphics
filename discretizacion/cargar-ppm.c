#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr)
{
 char line[40];
 int luz,zbkia;
 FILE *obj_file;
 int fd;
 
 if ((obj_file = fopen(file, "r")) == NULL) 
        {
        *dimxptr= 0;
        *dimyptr=0;
        *bufferptr = (unsigned char *)0;
        return;
        }
    /* fitxategi formatua irakurtzera */
luz =fscanf(obj_file, "%[^\n]\n", line);
if ( luz > 1) 
    {
    if ((line[0]== 'P')&&(line[1]=='6'))
        printf("formatu egokia\n");
      else
        {
        *dimxptr= 0;
        *dimyptr=0;
        *bufferptr = (unsigned char*)0;
        printf("formatuak P6 modukoa izan behar du\n");
        return;
        }
    }
    /* fitxategi neurria irakurtzera */
luz =fscanf(obj_file, "%[^\n]\n", line);
if (luz>0) 
    {
    luz = sscanf(line,"%d %d",dimxptr,dimyptr);
    if (luz == 2)
        printf("dimentsioak irakurrita: %d,%d\n",*dimxptr,*dimyptr);
      else
        {
        *dimxptr= 0;
        *dimyptr=0;
        *bufferptr = (unsigned char*)0;
        printf("dimentsioak irakurtzerakoan arazoak\n");
        return;
        }
    }
    /* fitxategitik kolore adierazpena irakurtzera */
luz =fscanf(obj_file, "%[^\n]\n", line);
if (luz>0) 
    {
    luz = sscanf(line,"%d",&zbkia);
    if (luz == 1)
        printf("kolorearen zenbaki maximoa irakurrita: %d\n",zbkia);
      else
        {
        *dimxptr= 0;
        *dimyptr=0;
        *bufferptr = (unsigned char*)0;
        printf("kolore adierazpena irakurtzerakoan arazoak\n");
        return;
        }
    }
luz=(*dimxptr)*(*dimyptr)*3;
*bufferptr = (unsigned char *)malloc(luz);
zbkia=fread(*bufferptr,1,luz,obj_file);
/*
fd = fileno(obj_file);
zbkia = read(fd, bufferptr, luz);
*/
if (zbkia != luz) 
        {
	*dimxptr= 0;
        *dimyptr=0;
        free(*bufferptr);
        *bufferptr = (void*)0;
        printf("bufferra betetzean erroreren bat...zbkia = %d, luz =%d\n",zbkia,luz);
        return;
        }
      else
        {
        printf("bufferra ondo irakurri du\n");
        return;
        }
}

/**
int main(int argc, char *argv[])
{
int dimx, dimy;
void * buferra;

load_ppm("joseba-eskiatzen.ppm", &buferra, &dimx, &dimy);
printf("irudia kargatuta!!! %d, %d dimentsioak dauzka\n",dimx,dimy);
}
**/
