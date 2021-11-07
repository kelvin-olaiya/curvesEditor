#ifndef MODELS_H
#define MODELS_S
#include "Lib.h"
#include "ModelTypes.h"
void createHermitteShape(vec4 color_top, vec4 color_bot, Shape* shape, Shape* polygonal, Shape* derivative, Shape* tangents);
void createBezierShape(vec4 color_top, vec4 color_bot, Shape* shape, Shape* polygonal, int pval);
#endif