//
//  Game.cpp
//  GL2
//
//  Created by Brian Jones on 5/1/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#include "game.h"
#include <knu/window.h>
#include <knu/mathlibrary5.hpp>
#include <functional>
#include <iostream>
#include <string>



using namespace knu;
using namespace knu::math;

Game::Game()
{
	InitializeGraphics();
	InitializeAudio();
	general_setup();
	lastTimePoint = currentTimePoint = std::chrono::high_resolution_clock::now();
}

#ifdef __APPLE__
#pragma mark - Graphics section
#endif
void Game::general_setup()
{
	// opengl stuff
    GetWindowResize(windowWidth, windowHeight);
	clearDepthVal = 1.0f;
	clearColorVal[0] = 0.4f; clearColorVal[1] = 0.253f; clearColorVal[2] = 0.732f; 
	clearColorVal[3] = 1.0f;
	glEnable(GL_DEPTH_TEST);

	// A vertex array object is now needed in desktop opengl core profile
}

void Game::update(std::chrono::milliseconds delta)
{
    
}

void Game::draw()
{
	glClearBufferfv(GL_DEPTH, 0, &clearDepthVal);
	glClearBufferfv(GL_COLOR, 0, clearColorVal);


}


void Game::execute()
{
	while (Window::IsWindowActive())
	{
		currentTimePoint = std::chrono::high_resolution_clock::now();
		update(std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - lastTimePoint));
		draw();
		Window::SwapBuffers();
		lastTimePoint = currentTimePoint;
	}
}


// Private fun
void Game::InitializeGraphics()
{
	windowWidth = 1024;
	windowHeight = 768;
	int glMajorVersion = MAJOR_VERSION;
	int glMinorVersion = MINOR_VERSION;
	int depthBits = 32;
	int stencilBits = 0;
	int msaaSamples = 0;
	bool fullscreenMode = false;
	std::string windowTitle = "OpenGL Application";
    bool debugContext = false;
	
	Window::Initialize();
	Window::Create(windowWidth, windowHeight, glMajorVersion, glMinorVersion, depthBits,
		stencilBits, msaaSamples, fullscreenMode, windowTitle, debugContext);
	Window::RegisterKeyboardCallback(std::bind(&Game::GetKeyboardInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	Window::RegisterMouseButtonCallback(std::bind(&Game::GetMouseInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
	Window::RegisterWindowResizeCallback(std::bind(&Game::GetWindowResize, this, std::placeholders::_1, std::placeholders::_2));

#ifdef WIN32
	glDebugMessageCallback(&debug_output1, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
    
}

void Game::GetKeyboardInput(int keyIdentifier, int scanCode, int action, int modifier)
{

}

// Updated
void Game::GetMouseInput(int button, int buttonState, int modifiers, double xPos, double yPos)
{

}

void Game::GetWindowResize(int newWidth, int newHeight)
{
	windowWidth = newWidth;
	windowHeight = newHeight;
	glViewport(0, 0, windowWidth, windowHeight);
}

void KNU_STDCALL debug_output1(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, void *userParam)
{
	std::string msg = std::string("source: ") + std::to_string(source)
		+ "\n type: " + std::to_string(type)
		+ "\n id: " + std::to_string(id)
		+ "\n severity: " + std::to_string(severity)
		+ "\n " + std::string(message) + "\n";

#ifdef WIN32
	OutputDebugStringA(msg.c_str());
#endif
	std::cerr << msg << std::endl;
}

#ifdef __APPLE__
#pragma mark - Audio section
#endif

void Game::InitializeAudio()
{

}