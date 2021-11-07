#include "Hermitte.h"
#include <algorithm>

static float derivative(int i, float* t, int size, float prev, float current, float next, float Tens, float Bias, float Cont)
{
	if (i == 0) {
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (next - current) / (t[i + 1] - t[i]);
	}
	else if (i == size - 1) {
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (current - prev) / (t[i] - t[i - 1]);
	}
	else if (i % 2 == 0) {
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (current - prev) / (t[i] - t[i - 1])
			+ 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (next - current) / (t[i + 1] - t[i]);
	}
	return  0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (current - prev) / (t[i] - t[i - 1])
		+ 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (next - current) / (t[i + 1] - t[i]);
}

float dx(int i, float* t, float Tens, float Bias, float Cont, Shape* shape)
{
	return derivative(i, t, shape->controlPoints.size(),
		shape->controlPoints[std::max(0, i - 1)].x,
		shape->controlPoints[i].x,
		shape->controlPoints[std::min((int)shape->controlPoints.size() - 1, i + 1)].x,
		Tens, Bias, Cont);
}

float dx(int i, float* t, Shape* shape, Shape* derivative)
{
	return derivative == NULL
		|| derivative->controlPoints.size() != shape->controlPoints.size()
		? dx(i, t, 0, 0, 0, shape) : dx(i, t,
			derivative->controlPoints[i].x, // Tens
			derivative->controlPoints[i].y, // Bias
			derivative->controlPoints[i].z, // Cont,
			shape);
}

float dy(int i, float* t, float Tens, float Bias, float Cont, Shape* shape)
{
	return derivative(i, t, shape->controlPoints.size(),
		shape->controlPoints[std::max(0,i-1)].y,
		shape->controlPoints[i].y,
		shape->controlPoints[std::min((int)shape->controlPoints.size() - 1, i + 1)].y,
		Tens, Bias, Cont);
}

float dy(int i, float* t, Shape* shape, Shape* derivative)
{
	return derivative == NULL 
		|| derivative->controlPoints.size() != shape->controlPoints.size() 
		? dy(i, t, 0, 0, 0, shape) : dy(i, t,
			derivative->controlPoints[i].x, // Tens
			derivative->controlPoints[i].y, // Bias
			derivative->controlPoints[i].z, // Cont,
			shape);
}

void hermitteInterpolation(float* t, Shape* shape, Shape* derivative, vec4 color_top, vec4 color_bottom, int pval)
{
	float passoTg = 1.0 / (pval - 1);
	float tg, tgmap, ampiezza;
	int i;
	int is = 0; // indice dell'intervallo

	shape->vertices.clear();
	shape->colors.clear();
	for (tg = 0; tg <= 1; tg += passoTg) {
		/* Individuare l'invervallo [t_i, t_is+1] a cui appartiene tgb */
		if (tg > t[is + 1]) is++;
		ampiezza = (t[is + 1] - t[is]);
		tgmap = (tg - t[is]) / ampiezza;
		float x = shape->controlPoints[is].x * PHI0(tgmap)
			+ dx(is, t, shape, derivative) * PHI1(tgmap) * ampiezza
			+ shape->controlPoints[is + 1].x * PSI0(tgmap)
			+ dx(is + 1, t, shape, derivative) * PSI1(tgmap) * ampiezza;

		float y = shape->controlPoints[is].y * PHI0(tgmap)
			+ dy(is, t, shape, derivative) * PHI1(tgmap) * ampiezza
			+ shape->controlPoints[is + 1].y * PSI0(tgmap)
			+ dy(is + 1, t, shape, derivative) * PSI1(tgmap) * ampiezza;

		shape->vertices.push_back(vec3(x, y, 0));
		shape->colors.push_back(color_top);
	}
}

void hermitteInterpolation(float* t, Shape* shape, vec4 color_top, vec4 color_bottom, int pval)
{
	hermitteInterpolation(t, shape, NULL, color_top, color_bottom, pval);
}

void hermitteInterpolation(float* t, Shape* shape, Shape* derivative, Shape* tangents, vec4 color_top, vec4 color_bottom, int pval)
{
	hermitteInterpolation(t, shape, derivative, color_top, color_bottom, pval);
	float parT = 1 / 50.0;
	tangents->controlPoints.clear();
	for (int i = 0; i < shape->controlPoints.size(); i++)
	{
		/* E1 */
		float e1x = shape->controlPoints[i].x + parT * dx(i, t, shape, derivative);
		float e1y = shape->controlPoints[i].y + parT * dy(i, t, shape, derivative);
		/* E2 */
		float e2x = shape->controlPoints[i].x - parT * dx(i, t, shape, derivative);
		float e2y = shape->controlPoints[i].y - parT * dy(i, t, shape, derivative);
		/* Inserisco gli estremi delle tangenti */
		tangents->controlPoints.push_back(vec3(e1x, e1y, 0));
		tangents->controlPoints.push_back(vec3(e2x, e2y, 0));
		/* Setto i colori di questo */
		tangents->controlPointsColors.push_back(vec4(0, 0, 1, 1));
		tangents->controlPointsColors.push_back(vec4(0, 0, 1, 1));
	}

}