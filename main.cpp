#include <cstdio>
#include <cstdlib>
#include <GL/freeglut.h>
#include <jpeglib.h>
#include <jerror.h>
#include <cmath>

#ifdef __WIN32
#pragma comment (lib, "jpeg.lib")
#endif

class Point{
    public :
	//coordonnées x, y et z du point
	double x;
	double y;
	double z;
	// couleur r, v et b du point
	float r;
	float g;
	float b;
};

char presse;
int anglex=30,angley=20,x,y,xold,yold, angRotTerreSol=0, angRotTerreMM=0, angRotLuneTerre=0;
int im_larg, im_haut;


void affichage();
void clavier(unsigned char touche,int x,int y);
void souris(int boutton, int etat,int x,int y);
void sourismouv(int x,int y);
void redim(int l,int h);
void loadJpegImage(char *fichier, unsigned char* image);
void affiche_sphere(float rad, float r, float g, float b, int M, int P);
void affiche_abdomen(int T, int F, float r, float g, float b); //pour afficher un abdomen de la couleur dont les composants sont spécifiés en argument
void affiche_abdomen(int T, int F, int indiceTex); //pour afficher un abdomen en utilisant la texture d'indice spécifié en parametre dans la liste des textures chargées
void idle();

int main(int argc,char **argv)

{
  unsigned char *imLune, *imTerre, *imSoleil;
  /* Chargement de la texture */
  //loadJpegImage("./lune.jpg", imLune);
  //loadJpegImage("./terre.jpg", imTerre);
  //loadJpegImage("./soleil.jpg", imSoleil);

  /* Creation de la fenetre OpenGL */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1000,1000);
  glutCreateWindow("Systeme Solaire");

  /* Initialisation de l'etat d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  /* Mise en place de la projection perspective */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70.0,1,1.0,50.0);
  glMatrixMode(GL_MODELVIEW);

  /* Parametrage du placage de textures */
  /*glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  unsigned int texIds[3];
  glGenTextures(3, texIds);
  glBindTexture(GL_TEXTURE_2D, texIds[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, imLune);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glBindTexture(GL_TEXTURE_2D, texIds[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, imSoleil);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glBindTexture(GL_TEXTURE_2D, texIds[2]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, imTerre);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEnable(GL_TEXTURE_2D);*/

  /* Mise en place des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutMouseFunc(souris);
  glutMotionFunc(sourismouv);
  glutReshapeFunc(redim);
  glutIdleFunc(idle);

  /* Entr�e dans la boucle principale glut */
  glutMainLoop();
  return 0;
}



void affichage()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  gluLookAt(20.0,20.0,20.0,  0.0,0.0,0.0,  0.0,0.0,1.0);
  glRotatef(angley,1.0,0.0,0.0);
  glRotatef(anglex,0.0,1.0,0.0);

  affiche_abdomen(10, 20, 1.0, 1.0, 1.0);

  //Repère
    //axe x en rouge
    glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(10, 0,0.0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    	glColor3f(0.0,1.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 10,0.0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0,0.0,1.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 0,10.0);
    glEnd();

  glutSwapBuffers();

}
void idle(){
    int pas=5;
    angRotTerreSol+=pas;
    angRotTerreMM+=pas;
    angRotLuneTerre+=12*pas;
    glutPostRedisplay();
}
void affiche_sphere(float rad, float r, float g, float b, int M, int P){
    Point pS[P*M];
    int fS[(P-1)*M][4];
    for(int j=0; j<P; j++){
        for(int i=0; i<M; i++){
            pS[j*M+i]={rad*cos(2*i*M_PI/(M+0.0))*cos(-M_PI/2+M_PI*j/P), rad*sin(2*i*M_PI/(M+0.0))*cos(-M_PI/2+M_PI*j/P), rad*sin(-M_PI/2+M_PI*j/P)};
        }
    }
    for(int j=0; j<P-1; j++){
        for(int i=0; i<M; i++){
            fS[j*M+i][0]=i+j*M;
            fS[j*M+i][1]=(i+1)%M+j*M;
            fS[j*M+i][2]=(i+1)%M+(j+1)*M;
            fS[j*M+i][3]=i+(j+1)*M;
        }
    }
    for(int j=0; j<P-1; j++){
        for(int i=0; i<M; i++){
            glBegin(GL_QUADS);
                glColor3f(r, g, b);
                glVertex3f(pS[fS[j*M+i][0]].x, pS[fS[j*M+i][0]].y, pS[fS[j*M+i][0]].z);
                glVertex3f(pS[fS[j*M+i][1]].x, pS[fS[j*M+i][1]].y, pS[fS[j*M+i][1]].z);
                glVertex3f(pS[fS[j*M+i][2]].x, pS[fS[j*M+i][2]].y, pS[fS[j*M+i][2]].z);
                glVertex3f(pS[fS[j*M+i][3]].x, pS[fS[j*M+i][3]].y, pS[fS[j*M+i][3]].z);
            glEnd();
        }
    }
}
/*Ces deux fonctions servent à générer et à afficher l'abdomen de la fourmi
Elles créent une primitive ressemblant grossièrement à une pomme de pin. La formule mathématique utilisée pour une vue de profil de cette primitive est f(x)=0.8*|cos(x)*(x)^(1/6)|
*/
void affiche_abdomen(int T, int F, float r, float g, float b){ //T correspond au nombres de tranches, F aux nombres de faces par tranches
    float longueur=2;
    Point pA[T*F];
    int fA[(T-1)*F][4];
    for(int j=0; j<T; j++){
        for(int i=0; i<F; i++){
            pA[j*T+i]={cos(2*i*M_PI/(F+0.0)), sin(2*i*M_PI/(F+0.0)), longueur*0.8*abs(cos(j*M_PI/(T+0.0)*pow(x, 1/6)))};
        }
    }
    for(int j=0; j<T-1; j++){
        for(int i=0; i<F; i++){
            fA[j*T+i][0]=i+j*T;
            fA[j*T+i][1]=(i+1)%T+j*T;
            fA[j*T+i][2]=(i+1)%T+(j+1)*T;
            fA[j*T+i][3]=i+(j+1)*T;
        }
    }
    for(int j=0; j<T-1; j++){
        for(int i=0; i<F; i++){
            glBegin(GL_QUADS);
                glColor3f(r, g, b);
                glVertex3f(pA[fA[j*T+i][0]].x, pA[fA[j*T+i][0]].y, pA[fA[j*T+i][0]].z);
                glVertex3f(pA[fA[j*T+i][1]].x, pA[fA[j*T+i][1]].y, pA[fA[j*T+i][1]].z);
                glVertex3f(pA[fA[j*T+i][2]].x, pA[fA[j*T+i][2]].y, pA[fA[j*T+i][2]].z);
                glVertex3f(pA[fA[j*T+i][3]].x, pA[fA[j*T+i][3]].y, pA[fA[j*T+i][3]].z);
            glEnd();
        }
    }
}

void clavier(unsigned char touche,int x,int y)
{
  switch(touche) {
  case 'l':
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glutPostRedisplay();
    break;
  case 'n':
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glutPostRedisplay();
  break;

  case 27: /* touche ESC */
    exit(0);
  default:
	  break;
  }
}

void souris(int bouton, int etat,int x,int y)
{
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_DOWN)
  {
    presse = 1;
    xold = x;
    yold=y;
  }
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_UP)
    presse=0;
}

void sourismouv(int x,int y)
  {
    if (presse)
    {
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay();
    }

    xold=x;
    yold=y;
  }

void redim(int l,int h)
{
  if (l<h)
    glViewport(0,(h-l)/2,l,l);
  else
    glViewport((l-h)/2,0,h,h);
}



void loadJpegImage(char *fichier, unsigned char* image)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
  if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier\n");
      exit(1);
    }
#endif

  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);
  im_larg=cinfo.image_width;
  im_haut=cinfo.image_height;
  image=(unsigned char *)malloc(im_haut*im_larg*3*sizeof(unsigned char));
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*cinfo.output_width*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
}