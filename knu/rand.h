//
//  rand.h
//  RandomEngine
//
//  Created by Brian Jones on 8/26/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef RandomEngine_rand_h
#define RandomEngine_rand_h

#include <functional>
#include <random>
#include <limits>

namespace knu
{
    class Rand_int
    {
        std::function<int()> f;
    public:
        Rand_int():
        f(std::bind(std::uniform_int_distribution<int>(std::numeric_limits<int>::min(),
                                                       std::numeric_limits<int>::max()),
                    std::default_random_engine((unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count())))
        {
            
        }
        
        Rand_int(int min, int max):
        f(std::bind(std::uniform_int_distribution<int>(min, max),
                    std::default_random_engine((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count())))
        {
            
        }
        
        int operator()()
        {
            return f();
        }
    };
    
    template<typename real_type>
    class Rand_real
    {
        std::function<real_type()> f;
        
    public:
        Rand_real():
        f(std::bind(std::uniform_real_distribution<real_type>(std::numeric_limits<real_type>::min(),
                                                       std::numeric_limits<real_type>::max()),
                    std::default_random_engine((unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count())))
        {
            
        }
        
        Rand_real(real_type min, real_type max):
        f(std::bind(std::uniform_real_distribution<real_type>(min, max),
                    std::default_random_engine((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count())))
        {
            
        }

        real_type operator()()
        {
            return f();
        }

    };
}

#endif
