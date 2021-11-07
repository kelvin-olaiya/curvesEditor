#pragma once
#include "Lib.h"
#include "ModelTypes.h"
#ifndef HERMITTE_H
#define HERMITTE_H
#define PHI0(t)  (2.0*t*t*t-3.0*t*t+1)
#define PHI1(t)  (t*t*t-2.0*t*t+t)
#define PSI0(t)  (-2.0*t*t*t+3.0*t*t)
#define PSI1(t)  (t*t*t-t*t)

float dx(int i, float* t, float Tens, float Bias, float Cont, Shape* shape);
float dx(int i, float* t, Shape* shape, Shape* derivative);
float dy(int i, float* t, float Tens, float Bias, float Cont, Shape* shape);
float dy(int i, float* t, Shape* shape, Shape* derivative);
void hermitteInterpolation(float* t, Shape* shape, vec4 color_top, vec4 color_bottom, int pval);
void hermitteInterpolation(float* t, Shape* shape, Shape* derivative, vec4 color_top, vec4 color_bottom, int pval);
void hermitteInterpolation(float* t, Shape* shape, Shape* derivative, Shape* tangents, vec4 color_top, vec4 color_bottom, int pval);

#endif // !HERMITTE_H

