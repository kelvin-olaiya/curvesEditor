#include "Lib.h"
#ifndef MODEL_TYPES_H
#define MODEL_TYPES_H

typedef struct {
	string name;
	GLuint vao; //Vertex Array Object
	GLuint geometryVBO; //Geometry Vertex Buffer Object
	GLuint colorVBO; //Color Vertex Buffer Object
	int numTriangles;
	/* To be interpolated with Hermitte */
	vector<vec3> controlPoints; 
	vector<vec4> controlPointsColors;
	/* To be directly drawn */
	vector<vec3> vertices;
	vector<vec4> colors;
	// Number of vertices
	int numVertices;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 model;
	// Vertex and Fragment Shader choice
	int vShaderIndex;
	int fShaderIndex;
} Shape;

#endif // !MODEL_TYPES_H
