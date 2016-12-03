#include <iostream>
#include "planetFactory.hpp"

int PlanetFactory::getCubeScale() {
  return this->cubeScale;
}

int PlanetFactory::getVertexSize() {
  return this->vertexSize;
}

int PlanetFactory::getIndexSize() {
  return this->indexSize;
}

PlanetFactory::PlanetFactory(int LOD, CubeMap * cubeMap) {
  this->cubeMap = cubeMap;
  this->cubeScale = (2 << (LOD - 1));
  this->vertexSize = pow(this->cubeScale, 2);
  this->indexSize = this->vertexSize * 2 - (this->cubeScale + 2);

  float step = 1.0 / (this->cubeScale-1);
        
  // Certaines valeurs serons réutilisés plusieurs fois de suite,
  // on évite donc de répéter chaque calcules ou déréférencement.
  float v1,v2,v3,h0,h1,h2,h3,h4,h5;
  
  std::cout << "Initiate cube with 6 * " << this->cubeScale << "² vertices...\n";
	
  for (int i = 0; i < 6; i++) {
    this->vertex[i] = new glm::vec3[this->vertexSize];
  }

  std::cout << "Initiate index buffer with " << this->indexSize << " indices...\n";
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
      v3 = radiusPerVertex[this->cubeScale*y + x]*1.25;  
      h0 = (0.9 + 0.1 * (-this->cubeMap->min+this->cubeMap->faces[0][this->cubeScale*y + x]) / this->cubeMap->max);
      h1 = (0.9 + 0.1 * (-this->cubeMap->min+this->cubeMap->faces[1][this->cubeScale*(this->cubeScale-1-y) + x]) / this->cubeMap->max);
      h2 = (0.9 + 0.1 * (-this->cubeMap->min+this->cubeMap->faces[2][this->cubeScale * (this->cubeScale-1-y) + x]) / this->cubeMap->max);
      h3 = (0.9 + 0.1 * (-this->cubeMap->min+this->cubeMap->faces[3][this->cubeScale*y + x]) / this->cubeMap->max);
      h4 = (0.9 + 0.1 * (-this->cubeMap->min+this->cubeMap->faces[4][this->cubeScale * (this->cubeScale-1-x) + y]) / this->cubeMap->max);
      h5 = (0.9 + 0.1 * (-this->cubeMap->min+this->cubeMap->faces[5][this->cubeScale*(this->cubeScale-1-x) + this->cubeScale-1-y]) / this->cubeMap->max);

      //PlusZ
      this->vertex[0][this->cubeScale*y + x].x = v1 * v3 * h0;
      this->vertex[0][this->cubeScale*y + x].y = v2 * v3 * h0;
      this->vertex[0][this->cubeScale*y + x].z = 0.5 * v3 * h0;

      //MinusZ
      this->vertex[1][this->cubeScale*y + x].x = v1 * v3 * h1;
      this->vertex[1][this->cubeScale*y + x].y = v2 * v3 * h1;
      this->vertex[1][this->cubeScale*y + x].z = -0.5 * v3 * h1;

      //PlusY
      this->vertex[2][this->cubeScale*y + x].x = v1 * v3 * h2;
      this->vertex[2][this->cubeScale*y + x].y = 0.5 * v3 * h2;
      this->vertex[2][this->cubeScale*y + x].z = v2 * v3 * h2;

      //MinusY
      this->vertex[3][this->cubeScale*y + x].x = v1 * v3 * h3;
      this->vertex[3][this->cubeScale*y + x].y = -0.5 * v3 * h3;
      this->vertex[3][this->cubeScale*y + x].z = v2 * v3 * h3;
      
      //PlusX
      this->vertex[4][this->cubeScale*y + x].x = 0.5 * v3 * h4;
      this->vertex[4][this->cubeScale*y + x].y = v1 * v3 * h4;
      this->vertex[4][this->cubeScale*y + x].z = v2 * v3 * h4;

      //MinusX
      this->vertex[5][this->cubeScale*y + x].x = -0.5 * v3 * h5;
      this->vertex[5][this->cubeScale*y + x].y = v1 * v3 * h5;
      this->vertex[5][this->cubeScale*y + x].z = v2 * v3 * h5;
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
