#pragma once
#include "Lib.h"
#include "ModelTypes.h"

#ifndef CG_INIT_H
#define CG_INIT_H
void createVerticesVaoVector(Shape* fig);
void createControlPointVaoVector(Shape* fig);
void initializeShader(unsigned int* programId);
#endif