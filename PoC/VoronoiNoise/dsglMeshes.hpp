#if defined(DSGL_GL3W)
	#include <GL/gl3w.h>
#else
	/* INCLUDE ALTERNATIVE LIBRARY HERE */
#endif

#ifndef DSGL_MESHES_INCLUDED
#define DSGL_MESHES_INCLUDED

namespace DSGL {
	namespace Meshes {
		struct Triangle {
		  	Triangle();
			Triangle(GLfloat x, GLfloat y, GLfloat z);

			GLfloat		vertex[9];
			GLuint		index[3];
		};

		struct Quad {
		  	Quad();
			Quad(GLfloat x, GLfloat y, GLfloat z);
			
			GLfloat		vertex[12];
			GLfloat		texCoords[8];
			GLuint		index[4];
		};
	}
}

#endif
