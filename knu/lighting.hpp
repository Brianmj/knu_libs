//
//  lighting.hpp
//  Assimp Test
//
//  Created by Brian Jones on 2/1/14.
//  Copyright (c) 2014 Brian Jones. All rights reserved.
//

#ifndef KNU_LIGHTING
#define KNU_LIGHTING

#include <knu/mathlibrary5.hpp>

namespace knu
{
    namespace graphics
    {
        struct SimpleLight
        {
            knu::math::v4f lightPosition;
            knu::math::v4f lightColor;
        };
        
        struct Light0
        {
            knu::math::v4f lightPosition;
            knu::math::v4f ambientColor;
            knu::math::v4f diffuseColor;
        };
        
        class IlluminationModel
        {
            public:
            static SimpleLight simple_light()
            {
                SimpleLight s;
                s.lightPosition = knu::math::v4f(200.0f, 200.0f, 200.0f, 1.0f);
                s.lightColor = knu::math::v4f(0.9998f, 0.978f, 0.9663f, 1.0f);
                return s;
            }
            
            static Light0 light0()
            {
                Light0 ld0;
                ld0.lightPosition = knu::math::v4f(100.0f, 100.0f, 100.0f, 1.0f);
                ld0.ambientColor = knu::math::v4f(0.994f, 0.84f, 0.993f, 1.0f);
                ld0.diffuseColor = knu::math::v4f(0.994f, 0.83f, 0.933f, 1.0f);
                return ld0;
            }
        };
    }
}


#endif  // KNU_LIGHTING