#ifndef MODELS_H
#define MODELS_H
#include "Lib.h"
#include "ModelTypes.h"
void createHermitteShape(vec4 color_top, vec4 color_bot, Shape* shape, Shape* polygonal, Shape* derivative, Shape* tangents);
void createBezierShape(vec4 color_top, vec4 color_bot, Shape* shape, Shape* polygonal, int pval);
void createThickLineShape(Shape* line, Shape* strip, float width);
#endif