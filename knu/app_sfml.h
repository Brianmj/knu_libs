//
//  sfml_app.h
//  sfml_app
//
//  Created by Brian Jones on 2/25/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#ifndef __sfml_app__sfml_app__
#define __sfml_app__sfml_app__

#include <stdio.h>
#include <SFML/Graphics.hpp>

class SfmlApp
{
    sf::RenderWindow window;
    int windowWidth, windowHeight;
    
private:
    void initialize();
    void render();
    void process_event();
public:
    SfmlApp();
    SfmlApp(int width, int height);
    void run();
};

#endif /* defined(__sfml_app__sfml_app__) */
