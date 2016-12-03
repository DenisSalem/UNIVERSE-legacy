# VORONOI NOISE

Ce PoC génére une heightmap construire à l'aide de plusieurs diagrammes de Voronoï.
La surface ainsi généré est affiché dans une fenêtre OpenGL.


## Prérequis

Pour compiler et lancer ce PoC vous devez installer [glfw](http://www.glfw.org/).

Le mieux est d'installer cette librairie depuis les dépots
officiels de votre distribution.

Votre carte graphique doit également pouvoir supporter
OpenGL 3.3 et GLSL 3.3 au minimum. Pour vous en assurer 
rien de plus simple! Dans un terminal tapez

	$ glxinfo | grep "version"

Normalement ce devrait être le cas, mais assurez vous d'avoir
la suite d'utilitaires Mesa installé. Cette suite contient
notamment les commandes glxinfo et glxgears.

Inutile de préciser qu'il faut idéalement que vos pilotes
soient à jour.

## Compilation


Avant de compiler il faut télécharger et installer [gl3w](http://github.com/skaslev/gl3w), un chargeur d'extentions OpenGL.

Le PoC est fournit avec un installateur qui va cloner le
dépot et créer et installer les fichiers nécessaires. Pour
ça vous aurez besoin de python version 2.6 au minimum.

Avec les priviléges root, dans un terminal et dans le répertoire du PoC

	$ ./gl3wInstaller.sh

Les en-tête de gl3w seront installés dans votre système

	/usr/include/GL/gl3w.h
	/usr/include/GL/glcorearb.h

Un fichier source gl3w.c est également ajouté aux sources
du PoC. Pour compiler le PoC, enfin, tapez dans un terminal
en étant un utilisateur normal.

	$ make

le fichier exécutable VoronoiNoise est maintenant crée :)

Il y a deux options de compilation possible:	

	$ make crack

et 

	$ mage jelly

Chacune des options produit un exécutable qui génère une heightmap d'apparence très différente.

## Nettoyage

Pour enlever gl3w supprimez manuellement les deux fichiers

	/usr/include/GL/gl3w.h
	/usr/include/GL/glcorearb.h


## Usage

Dans un terminal tapez

	$ ./VoronoiNoise

