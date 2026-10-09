#include "CGInit.h"
#include "ShaderMaker.h"

static void createVaoVector(GLuint* vao, GLuint* geometryVBO, vector<vec3>* points, GLuint* colorVBO, vector<vec4>* colors)
{
	//------------------------------GEOMETRY--------------------------------
	/* Release the objects created on the previous call, if any */
	glDeleteVertexArrays(1, vao);
	glDeleteBuffers(1, geometryVBO);
	glDeleteBuffers(1, colorVBO);
	/* Generate and bind (activate) a vertex array object */
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);
	/* Generate and bind (activate) a buffer array object */
	glGenBuffers(1, geometryVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *geometryVBO);
	/* Load data in vertex buffer */
	glBufferData(GL_ARRAY_BUFFER, points->size() * sizeof(vec3), points->data(), GL_STATIC_DRAW);
	/* !!! search documentation !!! */ //--> layer 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	//------------------------------COLORS--------------------------------
	glGenBuffers(1, colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *colorVBO);
	glBufferData(GL_ARRAY_BUFFER, colors->size() * sizeof(vec4), colors->data(), GL_STATIC_DRAW);
	/* !!! search documentation !!! */  //--> layer 1
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

void createVerticesVaoVector(Shape* shape)
{
	createVaoVector(&shape->vao, &shape->geometryVBO, &shape->vertices, &shape->colorVBO, &shape->colors);
}

void createControlPointVaoVector(Shape* shape)
{
	createVaoVector(&shape->vao, &shape->geometryVBO, &shape->controlPoints, &shape->colorVBO, &shape->controlPointsColors);
}

void initializeShader(unsigned int* programId)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M.glsl";

	*programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(*programId);
}