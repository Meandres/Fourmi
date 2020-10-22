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
float diametre_pate=0.1, longueur_pate=2, diametre_corps=1, longueur_abdomen=2;

void affichage();
void clavier(unsigned char touche,int x,int y);
void souris(int boutton, int etat,int x,int y);
void sourismouv(int x,int y);
void redim(int l,int h);
void loadJpegImage(char *fichier, unsigned char* image);
void affiche_abdomen(int T, int F, float r, float g, float b); //pour afficher un abdomen de la couleur dont les composants sont spécifiés en argument
void affiche_abdomen(int T, int F, int indiceTex); //pour afficher un abdomen en utilisant la texture d'indice spécifié en parametre dans la liste des textures chargées
void affiche_pate();
void affiche_corps();
void idle();

int main(int argc,char **argv)

{

  /* Creation de la fenetre OpenGL */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1000,1000);
  glutCreateWindow("Fourmi");

  /* Initialisation de l'etat d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  /* Mise en place de la projection perspective */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1,1.0,50.0);
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

  gluLookAt(15.0,15.0,15.0,  0.0,0.0,0.0,  0.0,0.0,1.0);
  glRotatef(angley,1.0,0.0,0.0);
  glRotatef(anglex,0.0,1.0,0.0);
    glColor3f(1, 1, 1);

    glPushMatrix();
    affiche_corps();

    glPushMatrix();
    glColor3f(1.0f, 0.0f, 1.0f);//magenta
    glTranslatef(3*diametre_corps/4, 3*diametre_corps/4, 0);
    glRotatef(35, 0, 1, 0);
    glRotatef(35, 0, 0, 1);
    affiche_pate();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5f, 1.0f, 1.0f); //cyan
    glTranslatef(0, 3*diametre_corps/4, 0);
    glRotatef(35, 0, 0, 1);
    affiche_pate();
    glPopMatrix();

    glPushMatrix();
    glColor4f(1.0f, 1.0f, 0.0f, 0.0f); //jaune
    glTranslatef(3*diametre_corps/4, -3*diametre_corps/4, 0);
    glRotatef(35, 0, 0, 1);
    glRotatef(-35, 0, 1, 0);
    affiche_pate();
    glPopMatrix();

    glPopMatrix();

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
void affiche_corps(){
    glPushMatrix();
    glScalef(1.25, 1.5, 1);
    glutSolidSphere(diametre_corps, 20, 20);
    glPopMatrix();
}
void affiche_pate(){
    glPushMatrix();
    glutSolidCylinder(diametre_pate, longueur_pate, 20, 20);
    glTranslatef(0, 0, longueur_pate);
    glRotatef(120, 0, 1, 0);
    glutSolidCylinder(diametre_pate, 1.5*longueur_pate, 20, 20);
    glPopMatrix();
}
/*Ces deux fonctions servent à générer et à afficher l'abdomen de la fourmi
Elles créent une primitive ressemblant grossièrement à une pomme de pin. La formule mathématique utilisée pour une vue de profil de cette primitive est f(x)=0.8*|cos(x)*(x)^(1/6)|
*/
void affiche_abdomen(int T, int F, float r, float g, float b){ //T correspond au nombres de tranches, F aux nombres de faces par tranches
    Point pA[T*F];
    int fA[(T-1)*F][4];
    for(int j=0; j<T; j++){
        for(int i=0; i<F; i++){
            pA[j*T+i]={0.8*abs(cos(j*M_PI/(2*T+0.0)*pow(x, 1/6)))*cos(2*i*M_PI/(F+0.0)), 0.8*abs(cos(j*M_PI/(2*T+0.0)*pow(x, 1/6)))*sin(2*i*M_PI/(F+0.0)), j*longueur_abdomen/(F+0.0)};
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
    glPushMatrix();
    glTranslatef(0, 0, -1/2);
    glutSolidSphere(0.8, 30, 30);
    glPopMatrix();
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
