#include <iostream>
#include "planetFactory.hpp"

PlanetFactory::PlanetFactory(int LOD) {
  this->cubeScale = (2 << (LOD - 1));
  this->vertexSize = pow(this->cubeScale, 2);
  this->indexSize = this->vertexSize * 2 - (this->cubeScale + 2);

  float step = 1.0 / (this->cubeScale-1);
        
  // Certaines valeurs serons réutilisés plusieurs fois de suite,
  // on évite donc de répéter chaque calcules ou déréférencement.
  float v1,v2,v3;
  
  std::cout << "Initiate Cube with " << this->vertexSize * 6 << " vertices.\n";
  std::cout << "Initiate Cube with " << this->indexSize << " indices.\n";
	
  for (int i = 0; i < 6; i++) {
    this->vertex[i] = new glm::vec3[this->vertexSize];
  }

  this->index = new short int[this->indexSize];
  
  // On peut substantiellement réduire le nombre de calcule en précalculant les rayons de chaques vertex pour une face donnée.
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

      // Pour éviter de déréférencer 500 milles fois on stock le rayon courant une bonne fois pour toute.
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

  // create indices
  bool reverse = false;

  int x = 0;
  int y = 0;
  int i = 0;
  while (true) {
    this->index[ i ]     = this->cubeScale * y     + x; 
    this->index[ i + 1]  = this->cubeScale * (y+1) + x;
    i+=2;
    if (x == this->cubeScale -1 || (x == 0 && i > 2)) {
      if(y == this->cubeScale -2) {
        break;
      }
      reverse = !reverse;
      y++;
      this->index[ i ]     = this->cubeScale * y + x; 
      this->index[ i + 1]  = this->cubeScale * y + x;
      this->index[ i + 2]  = this->cubeScale * (y+1)+x;
      i+=3;
    }
    if (reverse) {
      x--;
    }
    else {
      x++;
    }
  }
}
