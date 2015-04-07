#include "app.hpp"
#include <string>
#include <iostream>

using namespace knu::math;
using namespace knu::math::utility;

void App::general_setup()
{
	load_shaders();
	glEnable(GL_DEPTH_TEST);

}

void App::draw_scene()
{
	glClearBufferfv(GL_COLOR, 0, clearColorVal);
	glClearBufferfv(GL_DEPTH, 0, &clearDepthVal);
    
    
}

void App::update(float seconds)
{

}

void App::load_shaders()
{

}

void App::process_messages(SDL_Event *event)
{
	switch (event->type)
	{
	case SDL_WINDOWEVENT:
		{
			switch (event->window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				{
					resize(event->window.data1, event->window.data2);
				}break;

			case SDL_WINDOWEVENT_CLOSE:
				{
					window.set_quit(true);
				}break;
			}
		}break;

	case SDL_KEYDOWN:
		{
			if (event->key.keysym.sym == SDLK_ESCAPE)
				window.set_quit(true);
		}break;
            
        case SDL_QUIT:
        {
            window.set_quit(true);
        }break;
	
	}
}

void App::resize(int w, int h)
{
	glViewport(0, 0, w, h);
    defaultProjectionMatrix = make_perspective<float>(degrees_to_radians(70.0f), static_cast<float>(w) / h, 0.1f, 100.0f);
	defaultOrthographicMatrix = make_ortho<float>(0.0f, (float)w, 0.0f, (float)h, 0.01f, 1000.0f);
}

void App::get_window_size(int &w, int &h)
{
    window.get_window_size(w, h);
}

int App::window_width()
{
	int w, h;
	get_window_size(w, h);
	return w;
}

int App::window_height()
{
	int w, h;
	get_window_size(w, h);
	return h;
}
App::App():
	window(1024, 768, MAJOR_VERSION, MINOR_VERSION, false, 24, 0)
{
	window.set_event_callback(std::bind(&App::process_messages, this, std::placeholders::_1));
	clearColorVal[0] = 0.1f; clearColorVal[1] = 0.2f; clearColorVal[2] = 0.4f; clearColorVal[3] = 1.0f;
	clearDepthVal = 1.0f;
}

int App::run()
{
	lastTime = currentTime = std::chrono::steady_clock::now();
	initialize_graphics();
    general_setup();

	while (window.is_active())
	{
		currentTime = std::chrono::steady_clock::now();
		window.poll_events();

		update(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f);
		draw_scene();

        lastTime = currentTime;
		window.swap_buffers();
	}

	return 0;
}

void App::initialize_graphics()
{
#ifdef WIN32
	glDebugMessageCallback(&debug_output1, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
	int w, h; window.get_window_size(w, h);	resize(w, h);
}

void APIENTRY debug_output1(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, const void *userParam)
{
	std::string msg = std::string("source: ") + std::to_string(source) + "\n type: " + std::to_string(type)
		+ "\n id: " + std::to_string(id) + "\n severity: " + std::to_string(severity)
		+ "\n " + std::string(message) + "\n";

#ifdef WIN32
	OutputDebugStringA(msg.c_str());
#endif
	std::cerr << msg << std::endl;
}