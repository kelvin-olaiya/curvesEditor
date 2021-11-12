#include "ShaderMaker.h"
#include "CGInit.h"
#include "Hermitte.h"
#include "Models.h"
#include <fstream>
/*
* DEFINE CONSTANTS
*/
#define INTERPOLATE_HERMITTE 1
#define INTERPOLATE_BEZIER 2

#define INSERT_MODE 0
#define EDIT_MODE 1
#define DELETE_MODE 2

#define TOGGLE_TANGENTS 1
#define TOGGLE_EDIT_DER_PARAMETERS 2
#define TOGGLE_POLYGONAL 3

#define EXPORT_POINTS 3
/*
* GLOBAL SCOPE
*/
static unsigned int programId;
/*
 * Viewport size
 */
int width = 1280;
int height = 768;
/* Rendering matrices */
mat4 projectionMatrix = ortho(0.0f, float(width), 0.0f, float(height));
GLint modelMatrixLocation, projectionMatrixLocation;
/* vertex and fragment choices */
unsigned int vertexShaderChoice, fragmentShaderChoice;

/* Shape vector */
static vector<Shape> Scene;

/* Mouse click position */
vec2 mouseClickPosition;

/* Selected point */
int selectedPoint = -1;

/* Menu struct */
typedef struct {
	int interpolationMethod;
	int interactionMode;
	bool editDerivativeParameters;
	bool showPolygonal;
	bool showTangents;
} Controls;

/* Variables */
Shape curve, polygonal, derivative, tangents;
Controls flags;

void getUniformLocations(unsigned int* programId) 
{
	projectionMatrixLocation = glGetUniformLocation(*programId, "Projection");
	modelMatrixLocation = glGetUniformLocation(*programId, "Model");

	vertexShaderChoice = glGetUniformLocation(*programId, "vShaderChoice");
	fragmentShaderChoice = glGetUniformLocation(*programId, "fShaderChoice");
}

void keyPressEvent(unsigned char key, int x, int y)
{
	if (selectedPoint > -1 && flags.editDerivativeParameters) {
		switch (key)
		{
		case 't':
			derivative.controlPoints[selectedPoint].x -= 1;
			break;
		case 'T':
			derivative.controlPoints[selectedPoint].x += 1;
			break;
		case 'b':
			derivative.controlPoints[selectedPoint].y -= 1;
			break;
		case 'B':
			derivative.controlPoints[selectedPoint].y += 1;
			break;
		case 'c':
			derivative.controlPoints[selectedPoint].z -= 1;
			break;
		case 'C':
			derivative.controlPoints[selectedPoint].z += 1;
			break;
		default:
			break;
		}
	}
	glutPostRedisplay();
}

double distance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void mouseClick(int button, int state, int x, int y)
{
	glutPostRedisplay();
	mouseClickPosition = vec2((float)x, (float)height - y);
	if (state == GLUT_DOWN)
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			// flags.editDerivativeParameters = false;
			if (flags.interactionMode == INSERT_MODE)
			{
				curve.controlPoints.push_back(vec3(mouseClickPosition, 0.0));
				curve.controlPointsColors.push_back(vec4(1, 0, 0, 1));
				derivative.controlPoints.push_back(vec3(0, 0, 0)); // Tens-Bias-Cont
			}
			else if (flags.interactionMode == DELETE_MODE)
			{
				curve.controlPoints.pop_back();
				curve.controlPointsColors.pop_back();
				derivative.controlPoints.pop_back();
			}
			else if (flags.interactionMode == EDIT_MODE || flags.editDerivativeParameters)
			{
				int index = 0, tollerance = 10;
				double minDist = distance(curve.controlPoints[0].x, curve.controlPoints[0].y, mouseClickPosition.x, mouseClickPosition.y);
				// Find the vertice to with the click was nearer
				for (int i = 1; i < curve.controlPoints.size(); i++)
				{
					double dist = distance(curve.controlPoints[i].x, curve.controlPoints[i].y, mouseClickPosition.x, mouseClickPosition.y);
					if (dist < minDist) {
						minDist = dist;
						index = i;
					}
				}
				if (selectedPoint != -1) {
					curve.controlPointsColors[selectedPoint] = vec4(1.0, 0, 0, 1.0); // ripristino il colore iniziale
				}
				selectedPoint = minDist > tollerance ? -1 : index;
				if (selectedPoint != -1) {
					curve.controlPointsColors[selectedPoint] = vec4(0, 0, 1.0, 1.0);
				}
			}
			break;
		default:
			break;
		}
	}
}

void mouseMotion(int x, int y)
{
	if (flags.interactionMode == EDIT_MODE && selectedPoint > -1)
	{
		curve.controlPoints[selectedPoint].x = x;
		curve.controlPoints[selectedPoint].y = height -y;
	}
	glutPostRedisplay();
}

void resize(GLsizei w, GLsizei h)
{
	float worldAspectRatio = (float)(width) / (float)(height);
	/* 
		Se l'aspect ratio del mondo è diversa da quella della finestra devo
		mappare in modo diverso per evitare distorsioni del disegno
	*/
	if (worldAspectRatio > w / h) 
	{
		glViewport(0, 0, w, w / worldAspectRatio);
	}
	else {
		glViewport(0, 0, h * worldAspectRatio, h);
	}
	glutPostRedisplay();
}

void exportPoints()
{
	fstream fout;
	fout.open("output.txt", ios::out);
	if (!fout)
	{
		cout << "Impossibile scrivere su file" << endl;
		return;
	}
	fout << "CURVE CONTROL POINTS" << endl;
	fout << "--------------------" << endl;
	for (int i = 0; i < curve.controlPoints.size(); i++)
	{
		vec3 point = curve.controlPoints[i];
		fout << point.x << "," << point.y << "," << point.z << endl;
	}
	fout << "--------------------" << endl;
	for (int i = 0; i < derivative.controlPoints.size(); i++)
	{
		vec3 point = derivative.controlPoints[i];
		fout << point.x << "," << point.y << "," << point.z << endl;
	}
	fout << "--------------------" << endl;
	fout.close();
}

void interactionSubMenuFunction(int selection)
{
	flags.interactionMode = selection;
}

void hermitteSubMenuFunction(int selection)
{
	flags.interpolationMethod = INTERPOLATE_HERMITTE;
	flags.showPolygonal = false;
	if (selection == TOGGLE_TANGENTS) {
		flags.showTangents = !flags.showTangents;
	}
	else if (selection == TOGGLE_EDIT_DER_PARAMETERS)
	{
		flags.editDerivativeParameters = !flags.editDerivativeParameters;
	}
}

void bezierSubMenuFunction(int selection)
{
	flags.interpolationMethod = INTERPOLATE_BEZIER;
	flags.showTangents = false;
	flags.editDerivativeParameters = false;
	if (selection == TOGGLE_POLYGONAL)
	{
		flags.showPolygonal = !flags.showPolygonal;
	}
}

void mainMenuFunction(int selection)
{
	switch (selection)
	{
	case EXPORT_POINTS:
		exportPoints();
		break;
	default:
		break;
	}
}

void buildMainMenu()
{
	int interactionSubMenu = glutCreateMenu(interactionSubMenuFunction);
	glutAddMenuEntry("Inserisci", INSERT_MODE);
	glutAddMenuEntry("Sposta", EDIT_MODE);
	glutAddMenuEntry("Elimina", DELETE_MODE);

	int hermitteSubMenu = glutCreateMenu(hermitteSubMenuFunction);
	glutAddMenuEntry("Calcola interpolante - Hermitte", 0);
	glutAddMenuEntry("Toggle modifica tangenti", TOGGLE_EDIT_DER_PARAMETERS);
	glutAddMenuEntry("Toggle visualizzazione tangenti", TOGGLE_TANGENTS);

	int bezierSubMenu = glutCreateMenu(bezierSubMenuFunction);
	glutAddMenuEntry("Calcola approssimante - Bezier", 0);
	glutAddMenuEntry("Toggle poligono di controllo", TOGGLE_POLYGONAL);

	int mainMenu = glutCreateMenu(mainMenuFunction);
	glutAddSubMenu("Modalita interazione", interactionSubMenu);
	glutAddSubMenu("Hermitte", hermitteSubMenu);
	glutAddSubMenu("Bezier", bezierSubMenu);
	glutAddMenuEntry("Export", EXPORT_POINTS);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void initializer() {}

void drawScene(void)
{
	/* Maps the object coordinates to a portion of the screen */
	glViewport(0, 0, width, height);
	float time = glutGet(GLUT_ELAPSED_TIME);
	/* Make sure that stencil buffer contains only zeros */
	glClearStencil(0);
	glClearColor(0.0, 1.0, 0.0, 1.0); // sfondo
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));
	/*-------------------------------- START HERE ---------------------------------------*/
	vec4 col_bottom = vec4{ 0.5451, 0.2706, 0.0745, 1.0000 };
	vec4 col_top = vec4{ 1.0,0.4980, 0.0353,1.0000 };
	if (flags.interpolationMethod == INTERPOLATE_HERMITTE)
	{
		createHermitteShape(col_top, col_bottom, &curve, &polygonal, &derivative, &tangents);
	}
	else if (flags.interpolationMethod == INTERPOLATE_BEZIER)
	{
		createBezierShape(col_top, col_bottom, &curve, &polygonal, 140);
	}
	createVerticesVaoVector(&curve);
	createControlPointVaoVector(&polygonal);

	curve.model = mat4(1.0);
	glPointSize(6.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(curve.model));

	if (polygonal.controlPoints.size() > 1)
	{
		glBindVertexArray(curve.vao);
		glDrawArrays(GL_LINE_STRIP, 0, curve.vertices.size());
		glBindVertexArray(0);
	}
	// Draw control points
	glBindVertexArray(polygonal.vao);
	glDrawArrays(GL_POINTS, 0, polygonal.controlPoints.size());
	glBindVertexArray(0);

	// Draw polygonal
	if (flags.showPolygonal)
	{
		glBindVertexArray(polygonal.vao);
		glDrawArrays(GL_LINE_STRIP, 0, polygonal.controlPoints.size());
	}
	if (flags.showTangents)
	{
		createControlPointVaoVector(&tangents);
		glBindVertexArray(tangents.vao);
		glDrawArrays(GL_LINES, 0, tangents.controlPoints.size());
	}
	/*-----------------------------------------------------------------------------------*/
	glBindVertexArray(0);
	glutSwapBuffers();
}

void update(int value)
{
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	flags.interpolationMethod = INTERPOLATE_HERMITTE;
	flags.interactionMode = INSERT_MODE;
	flags.showPolygonal = false;
	flags.editDerivativeParameters = false;
	flags.showTangents = false;
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Scena OpenGL");
	
	glutDisplayFunc(drawScene);
	//glutTimerFunc(66, update, 0);
	glutKeyboardFunc(keyPressEvent);
	//glutReshapeFunc(resize);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glewExperimental = GL_TRUE;
	glewInit();
	initializeShader(&programId);
	getUniformLocations(&programId);
	buildMainMenu();
	/*
	* Initialization
	*/
	initializer();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}