
void CubeMap::Stamp(float * matrix, int scale) {
  int x,y,X=0;
  double halfScale = scale / 2 ;
  double radius;

  int * powersOfTwo = (int *) malloc( sizeof(int) * scale);

  // On crée deux tables contenants les valeurs élevées à la puissance de deux.
  // On calcul ainsi n fois ces valeurs au lieu de n².
  for(x=0; x<scale;x++) {
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
  for(x=0;x<limit;x+=scale) {
    for(y=0;y<scale;y++) {
      // On calcule le rayon du cercle sur lequel se trouve le point courant.
      // Opération très TRÈS gourmante en temps CPU
      radius = sqrtl( powersOfTwo[y] + powersOfTwo[X]);
      if ( radius < halfScale ) {
        // y a plus qu'à dessiner le cône.
	matrix[x+y] = (halfScale - radius) / (halfScale);
      }
      else {
        // Si on est en dehors du cercle, on se casse pas la tête et on affecte un zero.
	matrix[x+y] = 0;
      }
    }
    //Comme x est incrémenté par scale, et qu'on doit quand même accéder à notre tableau powersOfTwo...
    X++;
  }
  free(powersOfTwo);
}

void CubeMap::Noise( HEIGHTMAP * heightmap, int scale, int offsetX, int offsetY, int faceId) {
  if (scale == 1) {
    return;
  }

  char seed = getRandom() & 3;
  char doIt = getRandom() & 1;
  float sign = getRandom() & 2 ? 1.0 : -1.0;
  int halfScale = scale >> 1;
  int randX = - halfScale + getRandom() % (scale);
  int randY = - halfScale + getRandom() % (scale);
  int inc = heightmap->scale / scale;
  int stampIndex;
  int faceIndex;
  int stampX=0,stampY=0;
	

  float * dest;

  // Quand le tampon est contenu exactement à l'intérieur d'une face
  if ((randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale) && (randX+offsetX >= 0 && randX+offsetX+scale-1< heightmap->scale)) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( heightmap->stamp[stampIndex] != 0) {
	  faceIndex = (y+randY+offsetY) * heightmap->scale + x+randX+offsetX;
	  heightmap->faces[faceId][ faceIndex] += sign * heightmap->stamp[stampIndex] / inc;
	  if (heightmap->max < heightmap->faces[faceId][ faceIndex]) {
	    heightmap->max = heightmap->faces[faceId][ faceIndex ];
	  }
	  if (heightmap->min <= 0 && heightmap->min > heightmap->faces[faceId][ faceIndex ]) {
	    heightmap->min = heightmap->faces[faceId][ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += (inc * inc);
    }
  } 

  // Quand le tampon dépasse sur l'axe X
  else if ( !(randX+offsetX >= 0 && randX+offsetX+scale-1 < heightmap->scale) && ( randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale  )) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( heightmap->stamp[stampIndex] != 0) {
	  if (randX+offsetX+x < 0) {
	    dest = heightmap->left;
	    if (faceId == 1) {
	      faceIndex = ( heightmap->scale - (y+randY+offsetY) ) * heightmap->scale - (x+randX+offsetX)-1;
	    }
	    else if (faceId == 2) {
	      faceIndex = -(x+randX+offsetX + 1) * heightmap->scale +(y+randY+offsetY);
	    }
	    else if (faceId == 3) {
	      faceIndex = ( heightmap->scale + x+randX+offsetX + 1) * heightmap->scale + heightmap->scale - (y+randY+offsetY);
	    }
	    else if (faceId == 4) {
	      faceIndex = (y+randY+offsetY) * heightmap->scale + heightmap->scale + (x+randX+offsetX);
	    }
	    else if (faceId == 5) {
	      faceIndex = ( heightmap->scale - (y+randY+offsetY)) * heightmap->scale - (x+randX+offsetX) - 1;
	    }
	    else {
	      faceIndex = (y+randY+offsetY) * heightmap->scale + heightmap->scale + (x+randX+offsetX);
	    }
	  }
	  else if ( randX+offsetX+x >= heightmap->scale ) {
	    dest = heightmap->right;
	    if (faceId == 1) {
	      faceIndex = (heightmap->scale - (y+randY+offsetY)) * heightmap->scale + heightmap->scale - ((x+randX+offsetX) - heightmap->scale) - 1;
	    }
	    else if (faceId == 2) {
	      faceIndex = ((x+randX+offsetX) - heightmap->scale)* heightmap->scale + heightmap->scale - (y+randY+offsetY);
	    }
	    else if (faceId == 3) {
	      faceIndex = ( heightmap->scale - ((x+randX+offsetX) - heightmap->scale) - 1)* heightmap->scale + (y+randY+offsetY);
	    }
	    else if (faceId == 4) {
	      faceIndex = ( heightmap->scale - (y+randY+offsetY) ) * heightmap->scale + heightmap->scale - ((x+randX+offsetX) - heightmap->scale) - 1;
	    }
	    else if (faceId == 5) {
	      faceIndex = (y+randY+offsetY) * heightmap->scale + (x+randX+offsetX) - heightmap->scale;
            }
	    else {
	      faceIndex = (y+randY+offsetY) * heightmap->scale + x+randX+offsetX - heightmap->scale;
	    }
	  }
	  else {
	    dest = heightmap->faces[faceId];
	    faceIndex = ((y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
	  }
	  dest[ faceIndex] += sign * heightmap->stamp[stampIndex] / inc ;
	  if (heightmap->max < dest[ faceIndex]) {
	    heightmap->max = dest[ faceIndex ];
	  }
	  if (heightmap->min <= 0 && heightmap->min > dest[ faceIndex ]) {
	    heightmap->min = dest[ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += inc * inc;
    }
  }
  // Quand le tampon dépasse sur l'axe Y
  else if ((randX+offsetX >= 0 && randX+offsetX+scale-1 < heightmap->scale) && !( randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale  )) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( heightmap->stamp[stampIndex] != 0) {
	  if (randY+offsetY+y < 0) {
	    dest = heightmap->top;
	    if ( faceId == 4) {
	      faceIndex = ( heightmap->scale + (y+randY+offsetY)) + heightmap->scale * ( heightmap->scale - (x+randX+offsetX));
	    }
	    else if ( faceId == 5) {
	      faceIndex = (x+randX+offsetX) * heightmap->scale - 1 - y-randY-offsetY;
	    }
	    else {
	      faceIndex = (heightmap->scale + (y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
	    }
	  }
	  else if (randY+offsetY+y >= heightmap->scale) {
	    dest = heightmap->bottom;
	    if ( faceId == 4 ) {
	      faceIndex = - (y+randY+offsetY) - 1 + (heightmap->scale * ( scale - x+randX+offsetX) );
	    }
	    else if(faceId == 5) {
	      faceIndex = (y+randY+offsetY - heightmap->scale) + heightmap->scale * (heightmap->scale -x-randX-offsetX);
	    }
	    else {
	      faceIndex = (y+randY+offsetY - heightmap->scale) * heightmap->scale + x+randX+offsetX;
	    }
	  }
	  else {
	    dest = heightmap->faces[faceId];
	    faceIndex = ((y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
	  }
	  dest[ faceIndex] += sign * heightmap->stamp[stampIndex] / inc ;
	  if (heightmap->max < dest[ faceIndex]) {
	    heightmap->max = dest[ faceIndex ];
	  }
	  if (heightmap->min <= 0 && heightmap->min > dest[ faceIndex ]) {
	    heightmap->min = dest[ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += (inc * inc);
    }
  }

  UNIVERSE_NOISE_1(heightmap, halfScale, offsetX, 		offsetY,		faceId);
  UNIVERSE_NOISE_1(heightmap, halfScale, offsetX, 		offsetY+halfScale,	faceId);
  UNIVERSE_NOISE_1(heightmap, halfScale, offsetX+halfScale, 	offsetY+halfScale,	faceId);
  UNIVERSE_NOISE_1(heightmap, halfScale, offsetX+halfScale, 	offsetY,		faceId);
}
