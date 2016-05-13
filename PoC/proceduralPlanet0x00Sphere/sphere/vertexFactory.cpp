#include <iostream>
#include "vertexFactory.hpp"

VertexFactory::VertexFactory(int LOD) {
  this->cubeScale = (2 << (LOD - 1));
  this->vertexSize = pow(this->cubeScale, 2);
  this->indexSize = this->vertexSize * 2 - ( this->cubeScale + 2);

  float step = 1.0 / (this->cubeScale-1);
        
  // Certaines valeurs serons réutilisées plusieurs fois de suite,
  // on évite donc de répéter chaque calcul ou déréférencement.
  float v1,v2,v3;
  
  std::cout << "Initiate cube with " << this->vertexSize * 6 << " vertices.\n";
  std::cout << "Initiate index buffer with " << this->indexSize << " indices.\n";

  for (int i = 0; i < ACTIVE_FACES; i++) {
    this->vertex[i] = new glm::vec3[this->vertexSize];
  }
  this->index     = new short int[this->indexSize];

  // On peut substantiellement réduire le nombre de calculs en précalculant les rayons de chaque vertex pour une face donnée.
  // On fait ensuite la correspondance entre le rayon du vertex et sa position dans la matrice qui est indépendante de la face
  // sur laquelle on travaille.
  float * radiusPerVertex = new float[this->vertexSize];
  for (int y=0; y<this->cubeScale; y++) {
    for (int x=0; x<this->cubeScale; x++) {
      v1 = -0.5 + x * step;
      v2 =  0.5 - y * step;
      radiusPerVertex[this->cubeScale*y + x] = RADIUS / sqrt( pow(v1, 2) +  pow(v2, 2) +  pow(0.5, 2) );
    }
  }
  
  // create vertices
  for (int y=0; y<this->cubeScale; y++) {
    for (int x=0; x<this->cubeScale; x++) {
      v1 = -0.5 + x * step;
      v2 =  0.5 - y * step;

      // Pour éviter de déréférencer 500 mille fois on stocke le rayon courant une bonne fois pour toute.
      v3 = radiusPerVertex[this->cubeScale*y + x];  

      this->vertex[0][this->cubeScale*y + x].x = v1 * v3;
      this->vertex[0][this->cubeScale*y + x].y = v2 * v3;
      this->vertex[0][this->cubeScale*y + x].z = 0.5 * v3;

      this->vertex[1][this->cubeScale*y + x].x = v1 * v3;
      this->vertex[1][this->cubeScale*y + x].y = v2 * v3;
      this->vertex[1][this->cubeScale*y + x].z = -0.5 * v3;

      this->vertex[2][this->cubeScale*y + x].x = v1 * v3;
      this->vertex[2][this->cubeScale*y + x].y = -0.5 * v3;
      this->vertex[2][this->cubeScale*y + x].z = v2 * v3;

      this->vertex[3][this->cubeScale*y + x].x = v1 * v3;
      this->vertex[3][this->cubeScale*y + x].y = 0.5 * v3;
      this->vertex[3][this->cubeScale*y + x].z = v2 * v3;
      
      this->vertex[4][this->cubeScale*y + x].x = 0.5 * v3;
      this->vertex[4][this->cubeScale*y + x].y = v1 * v3;
      this->vertex[4][this->cubeScale*y + x].z = v2 * v3;

      this->vertex[5][this->cubeScale*y + x].x = -0.5 * v3;
      this->vertex[5][this->cubeScale*y + x].y = v1 * v3;
      this->vertex[5][this->cubeScale*y + x].z = v2 * v3;
    }	
  }

  // Création des indices
  
  // Permet de déterminer la direction du placement des vertex.
  bool reverse = false;

  
  // Coordonnées initiales 
  int x = 0;
  int y = 0;

  // Indice
  int i = 0;

  // On travaille dans une boucle while, ça me semblait plus commode :
  // On peut contrôler conditionnellement l'incrémentation de i.
  while (true) {

    // Les vertex sont placés par deux, verticalement.
    this->index[ i ]     = this->cubeScale * y     + x; 
    this->index[ i + 1]  = this->cubeScale * (y+1) + x;

    i+=2;

    // Quand on arrive au bout d'une ligne on change de direction
    if (x == this->cubeScale -1 || (x == 0 && i > 2)) {
      // Si en plus on arrive au bout de la dernière ligne, on arrête tout.
      if(y == this->cubeScale -2) {
        break;
      }

      // On change de direction
      reverse = !reverse;
      y++;

      // On Double le vertex courant
      this->index[ i ]     = this->cubeScale * y + x; 
      this->index[ i + 1]  = this->cubeScale * y + x;

      // On place le vertex qui formera le premier segment de la ligne
      this->index[ i + 2]  = this->cubeScale * (y+1)+x;
      i+=3;
    }

    // On se déplace le long de la ligne selon la direction
    if (reverse) {
      x--;
    }
    else {
      x++;
    }
  }
}
