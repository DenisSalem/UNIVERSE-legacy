#include "stampCollector.hpp"
#include <iostream>
#include <math.h>
StampCollector::StampCollector(int LoD) {
  this->scale = 2 << LoD;
  this->area = 2 << (LoD+LoD+1);
  this->cone = 0;
}

float * StampCollector::GetCone() {
  if (this->cone == 0) {
    this->SetCone();
    return this->cone;
  }
  return this->cone;
}

void StampCollector::SetCone() {
  int x,y,X=0;
  double halfScale = this->scale / 2 ;
  double radius;
  this->cone = new float[this->area]();

  int * powersOfTwo = new int[this->scale];

  // On crée deux tables contenants les valeurs élevées à la puissance de deux.
  // On calcule ainsi n fois ces valeurs au lieu de n².
  for(x=0; x<this->scale;x++) {
    powersOfTwo[x] = (x-halfScale) * (x-halfScale);
  }

  // Plutôt que d'incrémenter d'une unité x et calculer la position du point courant 
  // dans le tableau en multipliant par scale, on incrémente directement x par scale. La formule
  // pour retrouver le point courant n'est plus 
  // 
  // x*scale +y 
  //
  // mais 
  //
  // x+y
  //
  // Il faut donc également penser à changer la valeur limite de la première boucle
  int limit = scale*scale;
  for(x=0;x<limit;x+=this->scale) {
    for(y=0;y<this->scale;y++) {
      // On calcule le rayon du cercle sur lequel se trouve le point courant.
      // Opération très TRÈS gourmante en temps CPU
      radius = sqrtl( powersOfTwo[y] + powersOfTwo[X]);
      if ( radius < halfScale ) {
        // y a plus qu'à dessiner le cône.
	this->cone[x+y] = (halfScale - radius) / (halfScale);
      }
      else {
        // Si on est en dehors du cercle, on se casse pas la tête et on affecte un zéro.
	this->cone[x+y] = 0;
      }
    }
    //Comme x est incrémenté par scale, et qu'on doit quand même accéder à notre tableau powersOfTwo...
    X++;
  }
  delete [] powersOfTwo;
}
