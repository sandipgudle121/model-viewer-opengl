#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <windows.h>
#include <GL/glew.h>
#include <gl/GL.h>
#include "../vmath.h"

int initialize(HWND hwnd);
bool loadModel(const char* path);
void resize(int width, int height);
void display(HDC hdc);
void update(float deltaTime);
void handleMouseWheel(short delta);
void uninitialize(void);

#endif
