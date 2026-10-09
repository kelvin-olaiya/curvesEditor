#include "ShaderMaker.h"
#include "CGInit.h"
#include "Hermitte.h"
#include "Models.h"
#include <fstream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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
static GLFWwindow* window;
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

/* Set on right click, the menu popup is opened during the next frame */
bool openMainMenu = false;

/* Menu struct */
typedef struct {
	int interpolationMethod;
	int interactionMode;
	bool editDerivativeParameters;
	bool showPolygonal;
	bool showTangents;
} Controls;

/* Variables */
Shape curve, polygonal, derivative, tangents, thickCurve;
Controls flags;

void getUniformLocations(unsigned int* programId) 
{
	projectionMatrixLocation = glGetUniformLocation(*programId, "Projection");
	modelMatrixLocation = glGetUniformLocation(*programId, "Model");

	vertexShaderChoice = glGetUniformLocation(*programId, "vShaderChoice");
	fragmentShaderChoice = glGetUniformLocation(*programId, "fShaderChoice");
}

void keyPressEvent(GLFWwindow* window, unsigned int key)
{
	if (ImGui::GetIO().WantCaptureKeyboard) return;
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
}

double distance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void mouseClick(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	mouseClickPosition = vec2((float)x, (float)height - y);
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_RIGHT:
			openMainMenu = true;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
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

void mouseMotion(GLFWwindow* window, double x, double y)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) return;
	if (flags.interactionMode == EDIT_MODE && selectedPoint > -1)
	{
		curve.controlPoints[selectedPoint].x = x;
		curve.controlPoints[selectedPoint].y = height -y;
	}
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

void drawMainMenu()
{
	if (openMainMenu)
	{
		ImGui::OpenPopup("mainMenu");
		openMainMenu = false;
	}
	if (!ImGui::BeginPopup("mainMenu")) return;

	if (ImGui::BeginMenu("Modalita interazione"))
	{
		if (ImGui::MenuItem("Inserisci", nullptr, flags.interactionMode == INSERT_MODE)) interactionSubMenuFunction(INSERT_MODE);
		if (ImGui::MenuItem("Sposta", nullptr, flags.interactionMode == EDIT_MODE)) interactionSubMenuFunction(EDIT_MODE);
		if (ImGui::MenuItem("Elimina", nullptr, flags.interactionMode == DELETE_MODE)) interactionSubMenuFunction(DELETE_MODE);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Hermitte"))
	{
		if (ImGui::MenuItem("Calcola interpolante - Hermitte")) hermitteSubMenuFunction(0);
		if (ImGui::MenuItem("Toggle modifica tangenti", nullptr, flags.editDerivativeParameters)) hermitteSubMenuFunction(TOGGLE_EDIT_DER_PARAMETERS);
		if (ImGui::MenuItem("Toggle visualizzazione tangenti", nullptr, flags.showTangents)) hermitteSubMenuFunction(TOGGLE_TANGENTS);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Bezier"))
	{
		if (ImGui::MenuItem("Calcola approssimante - Bezier")) bezierSubMenuFunction(0);
		if (ImGui::MenuItem("Toggle poligono di controllo", nullptr, flags.showPolygonal)) bezierSubMenuFunction(TOGGLE_POLYGONAL);
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Export")) mainMenuFunction(EXPORT_POINTS);

	ImGui::EndPopup();
}

void initializer() {}

void drawScene(void)
{
	/* Maps the object coordinates to a portion of the screen (framebuffer size differs from window size on HiDPI screens) */
	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glViewport(0, 0, framebufferWidth, framebufferHeight);
	float time = glfwGetTime() * 1000;
	/* Make sure that stencil buffer contains only zeros */
	glClearStencil(0);
	glClearColor(0.0, 0.0, 0.0, 1.0); // sfondo
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));
	/*-------------------------------- START HERE ---------------------------------------*/
	vec4 col_bottom = vec4{ 0.5451, 0.2706, 0.0745, 1.0000 };
	vec4 col_top = vec4{ 1.0, 1.0, 1.0, 1.0 }; // curve color
	if (flags.interpolationMethod == INTERPOLATE_HERMITTE)
	{
		createHermitteShape(col_top, col_bottom, &curve, &polygonal, &derivative, &tangents);
	}
	else if (flags.interpolationMethod == INTERPOLATE_BEZIER)
	{
		createBezierShape(col_top, col_bottom, &curve, &polygonal, 140);
	}
	createControlPointVaoVector(&polygonal);

	curve.model = mat4(1.0);
	glPointSize(6.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(curve.model));

	if (polygonal.controlPoints.size() > 1)
	{
		createThickLineShape(&curve, &thickCurve, 3.0);
		createVerticesVaoVector(&thickCurve);
		glBindVertexArray(thickCurve.vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, thickCurve.vertices.size());
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
}

int main(int argc, char* argv[])
{
	flags.interpolationMethod = INTERPOLATE_HERMITTE;
	flags.interactionMode = INSERT_MODE;
	flags.showPolygonal = false;
	flags.editDerivativeParameters = false;
	flags.showTangents = false;
	if (!glfwInit())
	{
		cout << "Impossibile inizializzare GLFW" << endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, "Scena OpenGL", nullptr, nullptr);
	if (!window)
	{
		cout << "Impossibile creare la finestra" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetCharCallback(window, keyPressEvent);
	glfwSetMouseButtonCallback(window, mouseClick);
	glfwSetCursorPosCallback(window, mouseMotion);

	/* Installed after our callbacks, so that ImGui chains them */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	initializeShader(&programId);
	getUniformLocations(&programId);
	/*
	* Initialization
	*/
	initializer();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		drawMainMenu();
		ImGui::Render();

		glUseProgram(programId);
		drawScene();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
