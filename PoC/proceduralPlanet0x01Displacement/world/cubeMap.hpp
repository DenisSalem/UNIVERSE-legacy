#ifndef _CUBEMAP_
#define _CUBEMAP_

class CubeMap {

  public:
    CubeMap(int LOD);

    // Un tableau en contenant six autres, simulant une matrice et contenant chacune de
    // nos faces du cube.
    float * faces[6];

    // max et min correspondent à la hauteur minimal et maximal sur tout le cube.
    float max;
    float min;

  private:
    // Le tableau simulant une matrice contenant le tampon de base que l'on avait
    // déjà utilisé dans le PoC du StampNoise
    float * stamp;

    // La taille de chaque faces.
    int scale;

    // Pour une face donnée il faut lui associer des faces voisines.
    // Chacun des quatres pointeur contient donc une références vers une face voisine.
    float * top;
    float * left;
    float * right;
    float * bottom;

    void Stamp();
    void Noise(int scale, int offsetX, int offsetY, int faceId);
    void MergeEdges(int faceId);
};

#endif
