#include "Models.h"
#include "Hermitte.h"
#include "CGInit.h"

void createHermitteShape(vec4 color_top, vec4 color_bot, Shape* shape, Shape* polygonal, Shape* derivative, Shape* tangents)
{
	polygonal->controlPoints = shape->controlPoints;
	polygonal->controlPointsColors = shape->controlPointsColors;

	if (polygonal->controlPoints.size() > 1)
	{
		float* t = new float[shape->controlPoints.size()];
		float step = 1.0 / (float)(shape->controlPoints.size() - 1);
		for (int i = 0; i < shape->controlPoints.size(); i++)
		{
			t[i] = (float)i * step;
		}
		hermitteInterpolation(t, shape, derivative, tangents, color_top, color_bot, 142);
		shape->numVertices = shape->vertices.size();
	}
}

void createBezierShape(vec4 color_top, vec4 color_bot, Shape* shape, Shape* polygonal, int pval)
{
	polygonal->controlPoints = shape->controlPoints;
	polygonal->controlPointsColors = shape->controlPointsColors;
	shape->vertices.clear();
	shape->colors.clear();
	if (polygonal->controlPoints.size() > 1)
	{
		float step = 1.0 / (float)(shape->controlPoints.size() - 1);
		float passoTg = 1.0 / (float)(pval - 1);
		for (float tg = 0; tg <= 1; tg += passoTg) { // De Casteljau Algorithm
			vector<vec3> c = shape->controlPoints;
			for (int j = 1; j < shape->controlPoints.size(); j++) {
				for (int i = 0; i < shape->controlPoints.size() - j; i++)
				{
					c[i] = vec3((1 - tg) * c[i].x + tg * c[i + 1].x,
						(1 - tg) * c[i].y + tg * c[i + 1].y, 0);
				}
			}
			shape->vertices.push_back(vec3(c[0].x, c[0].y, 0.0));
			shape->colors.push_back(color_top);
		}
		shape->numVertices = shape->vertices.size();
	}
}


void createThickLineShape(Shape* line, Shape* strip, float width)
{
	/* glLineWidth > 1 is not supported by core profile contexts (e.g. on macOS),
	   so the line is drawn as a triangle strip of the given width */
	strip->vertices.clear();
	strip->colors.clear();
	int n = line->vertices.size();
	for (int i = 0; i < n; i++)
	{
		// Direction of the line at the vertex, from its neighbours
		vec2 dir = vec2(line->vertices[i < n - 1 ? i + 1 : i] - line->vertices[i > 0 ? i - 1 : i]);
		vec2 normal = length(dir) > 0 ? normalize(vec2(-dir.y, dir.x)) * (width / 2) : vec2(0);
		strip->vertices.push_back(line->vertices[i] + vec3(normal, 0));
		strip->vertices.push_back(line->vertices[i] - vec3(normal, 0));
		strip->colors.push_back(line->colors[i]);
		strip->colors.push_back(line->colors[i]);
	}
	strip->numVertices = strip->vertices.size();
}
