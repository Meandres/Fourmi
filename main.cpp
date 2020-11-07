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
unsigned char image[1024*1024*3];
int anglex=-90,angley=90,x,y,xold,yold;
int im_larg, im_haut;
// permet la rotation de la camera
float rot_x = 0, rot_y = 0, rot_z = 0;
// permet le zoom de la camera
float vue = 0;
// toutes les valeurs des formes crees
float diametre_pate = 0.05, longueur_pate = 1.5, diametre_corps = 0.7, longueur_abdomen = 3, rot_patte = 0;
float diametre_tete = 1, longueur_mandibule = 0.6, rot_mandibule = 0;
bool inverse_rot_patte = false, inverse_rot_mandibule = false, anim_pates=false;
float diametre_antenne = 0.04, longueur_antenne = 1;
GLuint texture;

void affichage();
void clavier(unsigned char touche,int x,int y);
void souris(int boutton, int etat,int x,int y);
void sourismouv(int x,int y);
void redim(int l,int h);
void loadJpegImage(char *fichier);
void affiche_abdomen(int T, int F, float r, float g, float b); //pour afficher un abdomen de la couleur dont les composants sont spécifiés en argument
void affiche_abdomen(int T, int F, int indiceTex); //pour afficher un abdomen en utilisant la texture d'indice spécifié en parametre dans la liste des textures chargées
void affiche_patte();
void affiche_ensemble_pattes();
void affiche_corps();
void affiche_tete();
void idle();
void lumieres();
void idle_anim();
void anim_deplacement();
void directions(int touche, int x , int y);

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
  loadJpegImage("./abdomen.jpg");
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glEnable(GL_TEXTURE_2D);

  /* Mise en place des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutMouseFunc(souris);
  glutMotionFunc(sourismouv);
  glutReshapeFunc(redim);
  glutIdleFunc(idle_anim);
  glutSpecialFunc(directions);

  /* Entree dans la boucle principale glut */
  glutMainLoop();
  return 0;
}

void affichage()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /*glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-5+vue,5-vue,-5+vue, 5-vue, -20, 20);
  gluLookAt(5.0, 5.0, 5.0, 0, 0, 0, 0.0, 1.0, 0.0);*/
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-10+vue,10-vue,-10+vue, 10-vue, -50, 50);
  gluLookAt(0.0, 0.0, 0.0, sin(rot_x), tan(rot_y)*2, -cos(rot_x), 0.0, 1.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotatef(angley, 0.0, 0.0, 1.0);
  glRotatef(anglex, 1.0, 0.0, 0.0);

    //glBegin(GL_QUADS) ;glTexCoord2f(0,0) ;glVertex2f(0,0) ;glTexCoord2f(1,0) ;glVertex2f(1,0) ;glTexCoord2f(1,1) ;glVertex2f(1,1) ;glTexCoord2f(0,1) ;glVertex2f(0,1) ;glEnd() ;

    glDisable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1);

    glPushMatrix();
    //corps
    affiche_corps();
    glPushMatrix();
    glTranslatef(0,0,diametre_corps*1.5);
    affiche_abdomen(20, 20, 0.5, 1, 1);
    glPopMatrix();

    //pattes
    glEnable(GL_TEXTURE_2D);
    affiche_ensemble_pattes();
    glDisable(GL_TEXTURE_2D);

    //cou
    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.4f); //gris
    glTranslatef(0, 0, (-diametre_corps)*1.3);
    glutSolidSphere(diametre_tete/2, 10, 10);
    glPopMatrix();

    //Tete
    glPushMatrix();
    glTranslatef(diametre_tete/2, 0, (-diametre_corps)*2.5);
    affiche_tete();
    glPopMatrix();

    glPopMatrix();
/*
  //Repère
    //axe x en rouge
    glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
    	glVertex3f(0, 0, 0);
    	glVertex3f(10, 0, 0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    	glColor3f(0.0,1.0,0.0);
    	glVertex3f(0, 0, 0);
    	glVertex3f(0, 10, 0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0,0.0,1.0);
    	glVertex3f(0, 0, 0);
    	glVertex3f(0, 0, 10);
    glEnd();

*/
    lumieres();
  glutSwapBuffers();

}

/*Le corps principal de la fourmi*/
void affiche_corps(){
    glPushMatrix();
    glScalef(1.25, 1.5, 1);
    glutSolidSphere(diametre_corps, 20, 20);
    glPopMatrix();
}

/*Une patte de la fourmi*/
void affiche_patte(){
    glPushMatrix();
    glutSolidCylinder(diametre_pate, longueur_pate, 20, 20);
    glTranslatef(0, 0, longueur_pate);
    glRotatef(-100, 0, 1, 0);
    glutSolidCylinder(diametre_pate, 1.5*longueur_pate, 20, 20);
    glTranslatef(0, 0, 1.5*longueur_pate);
    glRotatef(100, 0, 1, 0);
    glutSolidCylinder(diametre_pate, longueur_pate/6
    , 20, 20);
    glPopMatrix();
}

/*Toutes les pates de la fourmi*/
void affiche_ensemble_pattes(){
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslatef(3*diametre_corps/6, -3*diametre_corps/4, diametre_corps/3);
    glRotatef(30+rot_patte, 1, 0, 0);
    glRotatef(rot_patte, 0, 1, 0);
    affiche_patte();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f, 1.0f, 0.0f);
    glTranslatef(3*diametre_corps/6, -3*diametre_corps/4, 0);
    glRotatef(90-rot_patte, 1, 0, 0);
    affiche_patte();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glTranslatef(3*diametre_corps/6, -3*diametre_corps/4, -diametre_corps/3);
    glRotatef(120+rot_patte, 1, 0, 0);
    glRotatef(rot_patte, 0, 1, 0);
    glRotatef(rot_patte, 0, 0, 1);
    affiche_patte();
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslatef(3*diametre_corps/6, 3*diametre_corps/4, diametre_corps/3);
    glRotatef(-30-rot_patte, 1, 0, 0);
    glRotatef(rot_patte, 0, 1, 0);
    affiche_patte();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f, 1.0f, 0.0f);
    glTranslatef(3*diametre_corps/6, 3*diametre_corps/4, 0);
    glRotatef(-90+rot_patte, 1, 0, 0);
    affiche_patte();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glTranslatef(3*diametre_corps/6, 3*diametre_corps/4, -diametre_corps/3);
    glRotatef(-120-rot_patte, 1, 0, 0);
    glRotatef(rot_patte, 0, 1, 0);
    affiche_patte();
    glPopMatrix();
}

/*La tête de la fourmi*/
void affiche_tete(){
    glPushMatrix();
    glutSolidSphere(diametre_tete, 6, 6);

    glColor3f(0.2f, 0.2f, 0.2f); //gris sombre
    //Les yeux
    glPushMatrix();
    glTranslatef(diametre_tete/1.8, - diametre_tete/2, - diametre_corps/3);
    glutSolidSphere(diametre_tete/3, 5, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(diametre_tete/1.8, diametre_tete/2, - diametre_corps/3);
    glutSolidSphere(diametre_tete/3, 5, 10);
    glPopMatrix();

    glColor3f(0.6f, 0.3f, 0.0f); //orange
    //Les mandibules
    glPushMatrix();
    glTranslatef(0, - diametre_tete/2.5, - diametre_corps - diametre_tete/4);
    glRotatef(210 - rot_mandibule/2, 0, 1, 0);
    glRotatef(-40 + rot_mandibule, 1, 0, 0);
    glutSolidSphere(diametre_tete/5, 10, 10);
    glutSolidCone(diametre_tete/6, longueur_mandibule, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, diametre_tete/2.5, - diametre_corps - diametre_tete/4);
    glRotatef(210 - rot_mandibule/2, 0, 1, 0);
    glRotatef(40 - rot_mandibule, 1, 0, 0);
    glutSolidSphere(diametre_tete/5, 10, 10);
    glutSolidCone(diametre_tete/6, longueur_mandibule, 10, 10);
    glPopMatrix();

    //Les antennes
    glPushMatrix();
    glTranslatef(longueur_antenne/4, diametre_tete/2, - diametre_corps/1.2);
    glRotatef(190, 1, 0, 0);
    glRotatef(50, 0, 1, 0);
    glutSolidCylinder(diametre_antenne, longueur_antenne, 10, 10);
    glTranslatef(0, 0, longueur_antenne);
    glRotatef(-60, 0, 1, 0);
    glutSolidCylinder(diametre_antenne, 1.3*longueur_antenne, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(longueur_antenne/4, -diametre_tete/2, - diametre_corps/1.2);
    glRotatef(170, 1, 0, 0);
    glRotatef(50, 0, 1, 0);
    glutSolidCylinder(diametre_antenne, longueur_antenne, 10, 10);
    glTranslatef(0, 0, longueur_antenne);
    glRotatef(-60, 0, 1, 0);
    glutSolidCylinder(diametre_antenne, 1.3*longueur_antenne, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

/*Cette fonction sert à générer et à afficher l'abdomen de la fourmi
Elle crée une primitive ressemblant grossièrement à une pomme de pin. La formule mathématique utilisée pour une vue de profil de cette primitive est f(x)=0.8*|cos(x)*(x)^(1/6)|
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
    glEnable(GL_TEXTURE_2D);
    for(int j=0; j<T-1; j++){
        for(int i=0; i<F; i++){
            glBegin(GL_QUADS);
                //glColor3f(r, g, b);
                glTexCoord2f(i/(F+0.0), j/(T-1.0));
                glVertex3f(pA[fA[j*T+i][0]].x, pA[fA[j*T+i][0]].y, pA[fA[j*T+i][0]].z);
                glTexCoord2f((i+1)/(F+0.0), j/(T-1.0));
                glVertex3f(pA[fA[j*T+i][1]].x, pA[fA[j*T+i][1]].y, pA[fA[j*T+i][1]].z);
                glTexCoord2f((i+1)/(F+0.0), (j+1)/(T-1.0));
                glVertex3f(pA[fA[j*T+i][2]].x, pA[fA[j*T+i][2]].y, pA[fA[j*T+i][2]].z);
                glTexCoord2f((i+1)/(F+0.0), (j+1)/(T-1.0));
                glVertex3f(pA[fA[j*T+i][3]].x, pA[fA[j*T+i][3]].y, pA[fA[j*T+i][3]].z);
            glEnd();
        }
    }
    glPushMatrix();
    glTranslatef(0, 0, -1/2);
    glutSolidSphere(0.8, 30, 30);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/*Utilisation d'une lumière diffuse*/
void lumieres()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    GLfloat diffuse_lum[] = {1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse_lum);

    GLfloat coordonnees_lum [] = {0.0, 5.0, -8.0};
    glLightfv(GL_LIGHT1,GL_POSITION,coordonnees_lum);

}

/*Animation activable des pates*/
void anim_deplacement()
{
    if (inverse_rot_patte)
        rot_patte -= 1.0;
    else
        rot_patte += 1.0;

    if (rot_patte > 20 && !inverse_rot_patte)
        inverse_rot_patte = true;
    else if (rot_patte < 0 && inverse_rot_patte)
        inverse_rot_patte = false;
    //glutPostRedisplay();
}

/*Animation automatique des mandibules*/
void idle_anim()
{
    if (inverse_rot_mandibule)
        rot_mandibule -= 2.0;
    else
        rot_mandibule += 2.0;

    if (rot_mandibule > 40 && !inverse_rot_mandibule)
        inverse_rot_mandibule = true;
    else if (rot_mandibule < 0 && inverse_rot_mandibule)
        inverse_rot_mandibule = false;
    if(anim_pates){
        anim_deplacement();
    }
    glutPostRedisplay();
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
  case 'Z' : vue += 1;
    glutPostRedisplay();
    break; /*zoom arrière*/
  case 'z' : vue -= 1;
    glutPostRedisplay();
    break;  /*zoom avant*/
  case 32 : /*touche Espace */
      if(!anim_pates)
            anim_pates=true;
      else
            anim_pates=false;
    break;
  case 27 : /* touche ESC */
    exit(0);
  default:
	  break;
  }
}

void directions(int touche, int x , int y)
{
	switch (touche) {
		case GLUT_KEY_UP : rot_y += 0.25;
			break;
		case GLUT_KEY_DOWN : rot_y -= 0.25;
			break;
		case GLUT_KEY_LEFT : rot_x -= 0.25;
			break;
		case GLUT_KEY_RIGHT : rot_x += 0.25;
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
void loadJpegImage(char *fichier)
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
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*1024*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
}
