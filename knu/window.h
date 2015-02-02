//
//  glwindow.h
//  GL2
//
//  Created by Brian Jones on 4/29/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef GL2_glwindow_h
#define GL2_glwindow_h


#include <stdexcept>
#include <functional>
#include <string>

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GLFW/glfw3.h>
const int MAJOR_VERSION = 4;
const int MINOR_VERSION = 1;

#elif defined(WIN32)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
const int MAJOR_VERSION = 4;
const int MINOR_VERSION = 4;
#endif

#include <string>

namespace knu
{
	class Window
	{
	private:
		Window() {}
		Window(const Window &);

		GLFWwindow *windowHandle;

		int windowWidth;
		int windowHeight;

		std::function<void(int, int, int, int)>   keyboardCallback;
		// mousebuttoncallback has changed in glfw3
		std::function<void(int, int, int, double, double)>   mouseButtonCallback;
		std::function<void(int, int)>   windowResizeCallback;


	private:

		static void RegisterCallbacks()
		{
			glfwSetKeyCallback(Instance()->windowHandle, &KBCallback);
			glfwSetMouseButtonCallback(Instance()->windowHandle, &MBCallback);
			glfwSetWindowSizeCallback(Instance()->windowHandle, WindowResizeCallback);

		}

		static void KBCallback(GLFWwindow *glfwWindow, int keyIdentifier, int scanCode, int action, int modifier)
		{
			if (Instance()->keyboardCallback)    // Make sure keyboardCallback is set
				Instance()->keyboardCallback(keyIdentifier, scanCode, action, modifier);
		}

		static void MBCallback(GLFWwindow *window, int buttonIdentifier, int action, int modifiers)
		{
			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);
			if (Instance()->mouseButtonCallback)
				Instance()->mouseButtonCallback(buttonIdentifier, action, modifiers, xPos, yPos);
		}

		static void WindowResizeCallback(GLFWwindow *glfwWindow, int width, int height)
		{
			Instance()->windowWidth = width;
			Instance()->windowHeight = height;

			if (Instance()->windowResizeCallback)
				Instance()->windowResizeCallback(width, height);
		}

	public:
		inline static Window *Instance() { static Window window; return &window; }
		static void Initialize() { Instance(); if (!glfwInit()) throw std::runtime_error("Could not initialize GLFW"); }
		static void Shutdown() { glfwTerminate(); }

		bool static Create(int width, int height, int majorVersion = MAJOR_VERSION, int minorVersion = MINOR_VERSION, int depthBits = 32, int stencilBits = 0, int msaaSamples = 0, bool fullscreen = false, std::string windowTitle = "OpenGL", bool debugContext = false)
		{

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_DEPTH_BITS, depthBits);
			glfwWindowHint(GLFW_STENCIL_BITS, stencilBits);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext);
			glfwWindowHint(GLFW_SAMPLES, msaaSamples);
			Instance()->windowHandle = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
            
			if (!Instance()->windowHandle)
				return false;

			glfwMakeContextCurrent(Instance()->windowHandle);

#ifdef WIN32
			glewExperimental = true;
			glewInit();

			int isDebugContext = glfwGetWindowAttrib(Window::Instance()->windowHandle, GLFW_OPENGL_DEBUG_CONTEXT);

			if (isDebugContext)
			{
				
			}
#endif
            int requestedMajorVersion = glfwGetWindowAttrib(Window::Instance()->windowHandle, GLFW_CONTEXT_VERSION_MAJOR);
            
            int requestedMinorVersion = glfwGetWindowAttrib(Window::Instance()->windowHandle, GLFW_CONTEXT_VERSION_MINOR);
            
            if((majorVersion != requestedMajorVersion) || (minorVersion != requestedMinorVersion))
            {
                
            }

			glfwGetWindowSize(Instance()->windowHandle, &Instance()->windowWidth, &Instance()->windowHeight);
            glfwSetWindowSize(Instance()->windowHandle, Instance()->windowWidth, Instance()->windowHeight);
            glfwSetWindowPos(Instance()->windowHandle, 0, 0);
            glViewport(0, 0, Instance()->windowWidth, Instance()->windowHeight);
			RegisterCallbacks();

			return true;
		}

		template<typename CallbackIntInt>
		static void RegisterKeyboardCallback(CallbackIntInt kbcb)
		{
			Instance()->keyboardCallback = kbcb;
		}

		template<typename CallbackIntInt>
		static void RegisterMouseButtonCallback(CallbackIntInt mbcb)
		{
			Instance()->mouseButtonCallback = mbcb;
		}

		template<typename CallbackIntInt>
		static void RegisterWindowResizeCallback(CallbackIntInt wrcb)
		{
			Instance()->windowResizeCallback = wrcb;
		}

		inline static void WindowSize(int &windowWidth, int &windowHeight) { windowWidth = Instance()->windowWidth; windowHeight = Instance()->windowHeight; }
		inline static bool IsWindowActive() { return !glfwWindowShouldClose(Instance()->windowHandle) && !glfwGetKey(Instance()->windowHandle, GLFW_KEY_ESCAPE); }
		inline static void SwapBuffers() { glfwSwapBuffers(Instance()->windowHandle); glfwPollEvents(); }

	};
}

#endif
