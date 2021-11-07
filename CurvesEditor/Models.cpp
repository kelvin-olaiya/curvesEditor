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
			shape->colors.push_back(vec4(1, 0, 0, 1));
		}
		shape->numVertices = shape->vertices.size();
	}
}
