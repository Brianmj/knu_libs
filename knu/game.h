//
//  Game.h
//  GL2
//
//  Game.h
//  GL2
//
//  Created by Brian Jones on 5/1/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef __GL2__Game__
#define __GL2__Game__

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#define KNU_STDCALL
#endif

#ifdef WIN32
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#include <Windows.h>

#define GLEW_STATIC
#include <GL/glew.h>
#define KNU_STDCALL __stdcall
#endif

#include <chrono>
#include <knu/mathlibrary5.hpp>
#include <knu/gl_utility.h>

void KNU_STDCALL debug_output1(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, void *userParam);

class Game
{
private:
	void InitializeGraphics();
	void GetKeyboardInput(int keyIdentifier, int scanCode, int action, int modifier);
	void GetMouseInput(int button, int buttonState, int modifiers, double xPos, double yPos);
	void GetWindowResize(int newWidth, int newHeight);

	void InitializeAudio();

	void update(std::chrono::milliseconds delta);
	void draw();
    
	

private:
	int windowWidth;
	int windowHeight;

	float clearDepthVal;
	float clearColorVal[4];

	std::chrono::high_resolution_clock::time_point lastTimePoint, currentTimePoint;

public:
	Game();
	void general_setup();
	void execute();
};
#endif /* defined(__GL2__Game__) */
