//
//  sfml_app.cpp
//  sfml_app
//
//  Created by Brian Jones on 2/25/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#include "sfml_app.h"
#include <iostream>

SfmlApp::SfmlApp():
SfmlApp(1600, 1200) {}

SfmlApp::SfmlApp(int width, int height)
{
    sf::VideoMode vm;
    vm.width = width;
    vm.height = height;
    vm.bitsPerPixel = 32;
    window.create(vm, "SFML App");
    
    initialize();
}

void SfmlApp::initialize()
{
    window.setFramerateLimit(60);
    windowWidth = window.getSize().x;
    windowHeight = window.getSize().y;
}

void SfmlApp::render()
{
    
}

void SfmlApp::process_event()
{
    sf::Event event;
    while(window.pollEvent(event))
    {
        switch(event.type)
        {
            case sf::Event::Closed:
            {
                window.close();
            }break;
                
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Escape)
                    window.close();
            }break;
                
            case sf::Event::Resized:
            {
                windowWidth = event.size.width;
                windowHeight = event.size.height;
                
            }break;
        }
    }
}

void SfmlApp::run()
{
    while(window.isOpen())
    {
        window.clear(sf::Color::Blue);
        process_event();
        
        render();
        window.display();
    }
}