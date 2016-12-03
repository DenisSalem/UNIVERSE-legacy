#include <stdio.h>
#include <stdlib.h>
#include "png.h"

// On définit une macro d'interpolation, à priori plus rapide qu'un appel de fonction.
// la formule est la suivante:  6t^5 - 15t^4 + 10t^3
#define interpolation(t) (6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t)

//  Certaines variables et tableaux sont globaaux afin de ne pas être empilés lors de
//  l'appel de la fonction PerlinNoise2D, ce qui nous fera économiser des instructions
//  et du temps CPU.

// G est le tableau de gradient.
// Ici G comporte 8 paires de coordonnées. Dans l'implémentation améliorée G comporte
// 12 triplets de coordonnées pour pouvoir travailler dans un cube, et comme ce n'est pas
// le cas ici... :)
int G[8][2] = {
    {1,1},    {-1,1},   {1,-1},   {-1,-1},
    {1,0},    {-1,0},   {0,1},    {0,-1},
};

// Chaque vecteur gradient est référé par un indice Gn du tableau G
int G1,G2,G3,G4;

// Contient le résultat du produit scalaire de Gn avec P-Q dans une base orthonormale.
double s,t,u,v;

//Contient les coordonnées X et Y des points Qn et du point P.
int Q1[2] ,Q2[2],Q3[2],Q4[2];
double p[2];

// Résultat de la macro d'interpolation en X et Y, tmp permet de stocker P-Q avant d'être
// injecté dans la macro.
double iX,iY,tmp;

// Résultat de l'interpolation horizontal en st et uv
double iST;
double iUV;

// La table de permutations. En l'état, l'algorithme produira toujours le même terrain 
// pseudo-aléatoire. Pour obtenir un vrai terrain pseudo-aléatoire différent à chaque
// lancement du programme il faudrait changer ou désordonner cette table de permutations
// avant d'entrer dans la boucle principale ou est appelée notre fonction de bruit.
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


// Les coordonnées x et y doivent être des nombres réels positifs, sinon nous serions en 
// dehors de notre grille. Pour cela on envoie à notre fonction les coordonnées d'un
// point de la matrice, que l'on remet ensuite à l'échelle de la façon suivante
// C = c / scale
// avec C la coordonnée mise à l'échelle et c la coordonné entière d'origine.

// La fonction prend également en paramétre un entier -l'octave-
// positif qui détermine la taille de notre grille. Par exemple si la matrice
// de destination à une résolution de 256² éléments et que notre entier vaut
// 128 alors alors la taille de la grille sera en fait de 2² éléments, soit 3²
// noeuds :
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

  // L'initialisation peut sembler alambiquée, mais c'est 
  // pour épargner au processeur des calculs inutiles.

  p[0] = (double) x / scale;
  p[1] = (double) y / scale;

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
  // Pour éviter la répétition de motifs fractals à chaque octave 
  // l'indice final dépend de l'octave courant
  G1 = P[ (Q1[0] + P[ Q1[1] & 255] + scale) & 255 ] & 7;  // Gradient supérieur gauche
  G2 = P[ (Q2[0] + P[ Q2[1] & 255] + scale) & 255 ] & 7;  // Gradient supérieur droit
  G3 = P[ (Q3[0] + P[ Q3[1] & 255] + scale) & 255 ] & 7;  // Gradient inférieur droit
  G4 = P[ (Q4[0] + P[ Q4[1] & 255] + scale) & 255 ] & 7;  // Gradient inférieur gauche

  // On calcule le produit scalaire Gn . (P-Qn)
  // Avec P faisant référence aux coordonnées du point stocké dans p.
  // (P étant la table de permutations)
  s = G[G1][0] * (p[0]-Q1[0]) + G[G1][1] * (p[1]-Q1[1]);
  t = G[G2][0] * (p[0]-Q2[0]) + G[G2][1] * (p[1]-Q2[1]);
  u = G[G3][0] * (p[0]-Q3[0]) + G[G3][1] * (p[1]-Q3[1]);
  v = G[G4][0] * (p[0]-Q4[0]) + G[G4][1] * (p[1]-Q4[1]);

  
  // On interpole en x sur le segment st et uv
  tmp = p[0]-Q1[0];
  iX = interpolation(tmp);

  iST = s + iX * (t-s);
  iUV = v + iX * (u-v);

  // On interpole y sur le segment iST et iUV
  tmp = p[1]-Q1[1];

  iY = interpolation(tmp);

  // On retourne le résultat normalisé.
  return (1 + iST + iY * (iUV - iST)) * 0.5;
}

int main(int argc, char ** argv) {
  if (argc == 1) {
    printf("usage: ./improvedPerlinNoise <power of two>\n");
    return 0;
  }
  int scale = atoi(argv[1]);


  // Matrice de 256 pixels² simulé avec un tableau de longueur 256²
  // C'est dans ce tableau que nous allons stocker notre heightmap
  double * grid = (double *) malloc(sizeof(double) * scale * scale);
  
  // i et j correspondent respectivement aux axes x et y.
  // k correspond à l'octave courrante.
  int i,j,k;

  float min=2,max=0;

  // Selon le type de texture on peut ne pas utiliser de coef ou
  // l'utiliser différemment. Mais l'idée ici est de diminuer
  // l'influence du bruit à mesure que la fréquence augmente.
  double coef = 1.0; 

  for(j=0; j<scale;j++) {
    for(i=0; i<scale;i++) {
      coef = 1.0;
      grid[ i + j*scale] = 0;
      for(k=scale/2;k>=1; k/=2) {
        grid[ i + j*scale ] += PerlinNoise2D(i,j, k) * coef;
        coef /= 2.0;
      }
      if (min > grid[ i + j*scale]) {
        min = grid[ i + j*scale];
      }
      if (max < grid[ i + j*scale]) {
        max = grid[ i + j*scale];
      }
    }
  }

  // Ici la texture est terminée. Il ne reste plus qu'à la normaliser en
  // vue de l'exploiter.

  double normalizedIntensity;
  PIXEL ** matrix = (PIXEL **) malloc( scale * sizeof(PIXEL * ));
  for(j=0;j<scale;j++) {
    matrix[j] = (PIXEL *) malloc( scale * sizeof(PIXEL));
    for (i=0; i< scale;i++) {
      normalizedIntensity = (grid[ i + j * scale ] - min) * ( 1.0 / (max-min) );
      matrix[j][i].Alpha  = 255;
      matrix[j][i].Red    = (char) (normalizedIntensity * 255);
      matrix[j][i].Blue   = (char) (normalizedIntensity * 255);
      matrix[j][i].Green  = (char) (normalizedIntensity * 255);
    }
  }

  writePng(matrix, scale);
  return 0;
}
