#ifndef GALAXY_HPP
#define GALAXY_HPP
#include "Particul.hpp"

struct Galaxy{
    Vec3 massCenter; //center of mass of the galaxy
    Particuls particules; //list of particules in the galaxy
};

#endif