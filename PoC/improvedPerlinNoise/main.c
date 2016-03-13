#include <stdio.h>
#include <stdlib.h>
#include "png.c"
// On définit une macro d'interpolation, appriori plus rapide qu'un appel de fonction.
// la formule est la suivante:  6t^5 - 15t^4 + 10t^3
#define interpolation(t) (6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t)

//  Certaines variables et tableaux sont global afin de ne pas être empilé lors de
//  l'appel de la fonction PerlinNoise2D, ce qui nous fera économiser des instructions
//  et du temps CPU.

// G est le tableau de gradient.
// Ici G comporte 8 pair de coordonnée. Dans l'implémentation amélioré G comporte
// 12 triplet de coordonnée pour pouvoir travailler dans un cube, et comme ce n'est pas
// le cas ici... :)

int G[8][2] = {
    {1,1},    {-1,1},   {1,-1},   {-1,-1},
    {1,0},    {-1,0},   {0,1},    {0,-1},
};

// Chaque vecteur gradient est référé par un indice Gn du tableau G
int G1,G2,G3,G4;

// Contient le résultat du produit scalaire de Gn avec P-Q dans une base orthonormal.
float s,t,u,v;

//Contient les coordonnées X et Y des vecteurs (P-Q) et du point P.
int Q1[2] ,Q2[2],Q3[2],Q4[2];
float p[2];

// Résultat de la macro interpolation en X et Y
float iX,iY;

// Résultat de l'interpolation en s/t et u/v
float iST;
float iUV;

// La table de permutation
int P[256] = {
 235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,
 127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,
 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
 153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185,
 74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
 187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,
 47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,
 64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
 203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,
 105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,
 142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,
 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
 156,180};


// Les coordonnées X et Y doivent être des réelles positifs, sinon nous serions en 
// dehors de notre grille. Pour cela on envoie à notre fonction les coordonnées d'un
// point de la matrix, que l'on remet ensuite à l'échelle de la façon suivante
// C = c / scale
// avec C la coordonnée mise à l'échelle et c la coordonné entiére.

int X,Y;

// La fonction prend également en paramétre un entier
// positif qui détermine la taille de notre grille. Par exemple si la matrix
// de destination à une résolution de 256² éléments et que notre entier vaut
// 128 alors alors la taille de la grille sera en fait de 2² élément, soit 3³
// noeud:
//
//
//    <-- 256 --> taille réelle de la matrix
//
//    0----1----2
//    |    |    |
//    3----1----5   Here stands the Grid. A digital frontiere...
//    |    |    |
//    6----7----8
//
//    <--- 2 ---> taille de la grille mise à l'échelle
    
double PerlinNoise2D(int x, int y, unsigned int scale) {

  // L'initialisation peut sembler alambiqué, mais c'est 
  // pour épargner au processeur des calculs inutiles.

  p[0] = (float) (x % scale) / scale;
  p[1] = (float) (y % scale) / scale;

  // Coin supérieur gauche
  Q1[0] = (int) p[0];
  Q1[1] = (int) p[1];
  
  // Coin supérieur droit
  Q2[0] = Q1[0] + 1;
  Q2[1] = Q1[1];

  // Coin inférieur droit
  Q3[0] = Q2[0];
  Q3[1] = Q1[1] + 1;

  // Coin inférieur gauche
  Q4[0] = Q1[0];
  Q4[1] = Q3[1];

  // On récupére pseudo aléatoirement les gradients.
  G1 = P[ (x +     P[ y       & 255]) & 255 ] & 7;  // Gradient supérieur gauche
  G2 = P[ (x + 1 + P[ y       & 255]) & 255 ] & 7;  // Gradient supérieur droit
  G3 = P[ (x + 1 + P[ (y + 1) & 255]) & 255 ] & 7;  // Gradient inférieur droit
  G4 = P[ (x +     P[ (y+1)   & 255]) & 255 ] & 7;  // Gradient inférieur gauche

  // On calcul le produit scalaire Gn . (P-Qn)
  // Avec P faisant référence aux coordonnées du poin stocké dans p.
  // (P étant la table de permutation)
  s = G[G1][0] * (p[0]-Q1[0]) + G[G1][1] * (p[1]-Q1[1]);
  t = G[G2][0] * (p[0]-Q2[0]) + G[G2][1] * (p[1]-Q2[1]);
  u = G[G3][0] * (p[0]-Q3[0]) + G[G3][1] * (p[1]-Q3[1]);
  v = G[G4][0] * (p[0]-Q4[0]) + G[G4][1] * (p[1]-Q4[1]);

  

  iX = interpolation(p[0]-Q1[0]);
  iY = interpolation(p[1]-Q1[1]);

  iST = s + iX * (t-s);
  iUV = v + iX * (u-v);

  return (1 + iST + iY * (iUV - iST)) * 0.5;
}

int main(int argc, char ** argv) {
  if (argc == 1) {
    printf("usage: ./improvedPerlinNoise <power of two>\n");
    return 0;
  }
  int scale = atoi(argv[1]);


// Matrix de 256² pixel simulé avec un tableau de longueur 256²
  // C'est dans ce tableau que nous allons stocker notre heightmap
  double * grid = (double *) malloc(sizeof(double) * scale * scale);
  
  int i,j;
  for(j=0; j<scale;j++) {
    for(i=0; i<scale;i++) {
      grid[ i + j*scale ] = PerlinNoise2D(i,j, scale/4);
    }
  }

  PIXEL ** matrix = (PIXEL **) malloc( scale * sizeof(PIXEL * ));
  for(j=0;j<scale;j++) {
    matrix[j] = (PIXEL *) malloc( scale * sizeof(PIXEL));
    for (i=0; i< scale;i++) {
      matrix[j][i].Alpha  = 255;
      matrix[j][i].Red    = (char) (grid[ i + j * scale ] * 255);
      matrix[j][i].Blue   = (char) (grid[ i + j * scale ] * 255);
      matrix[j][i].Green  = (char) (grid[ i + j * scale ] * 255);
    }
  }

  writePng(matrix, scale);
  return 0;
}
