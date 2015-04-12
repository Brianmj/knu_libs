#ifndef KNU_WINDOW2
#define KNU_WINDOW2


#include <stdexcept>
#include <functional>
#include <string>

#ifdef WIN32
#pragma comment(lib, "sdl2.lib")
#pragma comment(lib, "sdl2main.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#undef main
#include <SDL_opengl.h>
#endif

#ifdef __APPLE__
#include <SDL2/SDL.h>
#endif

class Window
{
	SDL_Window *window;
	SDL_GLContext context;
	bool quit;
	std::function < void(SDL_Event *)> event_callback;


private:
	void process_event(SDL_Event *event)
	{
		if (event_callback)
			event_callback(event);

	//	if (event->type == SDL_KEYDOWN)
	//	if (event->key.keysym.sym == SDLK_ESCAPE)
	//		set_quit(true);
	}
public:
	Window():
		window(nullptr),
		context(0),
		quit(false)
	{
		unsigned initFlags = SDL_INIT_EVENTS | SDL_INIT_TIMER |	SDL_INIT_VIDEO;
		if (0 != SDL_Init(initFlags))
		{
			std::string error = SDL_GetError();
			throw (std::runtime_error("Unable to initialize SDL: " + error));
		}
	}

	Window(int width, int height, int majorVersion, int minorVersion, bool debug, int depthBits = 24, int stencilBits = 0, unsigned flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL) :
		Window()
	{
		create(width, height, majorVersion, minorVersion, debug, depthBits, stencilBits, flags);
	}

	~Window()
	{
		SDL_GL_MakeCurrent(window, 0);
		SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void create(int width, int height, int majorVersion, int minorVersion, bool debug, int depthBits = 24, int stencilBits = 0, unsigned flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL)
	{	
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBits);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencilBits);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);
	
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		if (debug)
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		window = SDL_CreateWindow("OpenGL Application", 50, 50, width, height, flags);
		//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        context = SDL_GL_CreateContext(window);
        
        int requestedMajorVersion;
        int requestedMinorVersion;
        
        if(0 == context)
        {
            std::string error = SDL_GetError();
            throw std::runtime_error(error);
        }
		
		SDL_GL_MakeCurrent(window, context);
        
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &requestedMajorVersion);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &requestedMinorVersion);
        

#ifdef WIN32
		glewExperimental = true;
		GLenum val = glewInit();
#endif
		
	}
	
	void set_quit(bool val) 
	{
		quit = val; 
	}

	void set_event_callback(std::function < void(SDL_Event *)> c)
	{
		event_callback = c;
	}

	inline void poll_events()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			process_event(&event);
		}
	}

	inline bool is_active() const
	{
		return false == quit;
	}
	
	inline void swap_buffers()
	{
		SDL_GL_SwapWindow(window);
	}
    
    void set_window_title(std::string title)
    {
        SDL_SetWindowTitle(window, title.c_str());
    }

	void set_swap_val(int val /*0 for immediate refresh, 1 for synchronization with display*/)
	{
		SDL_GL_SetSwapInterval(val);
	}

	void get_window_size(int &width, int &height)
	{
		SDL_GetWindowSize(window, &width, &height);
	}
};

#endif