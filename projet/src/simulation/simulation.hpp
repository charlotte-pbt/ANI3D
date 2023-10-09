#pragma once

#include "cgp/cgp.hpp"
#include "../cloth/cloth.hpp"
#include "../constraint/constraint.hpp"


struct simulation_parameters
{
    float dt = 0.005f;        // time step for the numerical integration
    vec3 fan_position = { 0,0,0 }; // position of the fan
    bool fan_min_x = false;
    bool fan_max_x = false;
    bool fan_min_y = false;
    bool fan_max_y = false;

    std::vector<std::pair<vec3, vec3>> clothesline_poles = {
                                                    {{-8,-8,0}, {-8,-8,6.5f}},
                                                    {{-8,8,0}, {-8,8,6.5f}},
                                                    {{8,-8,0}, {8,-8,6.5f}},
                                                    {{8,8,0}, {8,8,6.5f}},
                                                    {{4,6,0}, {4,6,6.5f}},
                                                    {{4,2,0}, {4,2,6.5f}},
                                                    };

    std::vector<std::pair<vec3, vec3>> clothesline = {
                                                    {{-8,-8,6}, {-8,8,6}},
                                                    {{-8,-8,6}, {8,-8,6}},
                                                    {{-8,8,6}, {8,8,6}},
                                                    {{4,2,6}, {4,6,6}},
                                                    };

    //  Wind magnitude and direction
    struct {
        float magnitude = 0.0f;
        cgp::vec3 initial_direction = { -1,0,1 };
        cgp::vec3 direction = { -1,0,1 };
        cgp::vec3 source = {0, 0, 2};
    } wind;
};


// Fill the forces in the cloth given the position and velocity
void simulation_compute_force(cloth_structure& cloth, simulation_parameters const& parameters);

// Perform 1 step of a semi-implicit integration with time step dt
void simulation_numerical_integration(cloth_structure& cloth, float dt);

// Apply the constraints (fixed position, obstacles) on the cloth position and velocity
void simulation_apply_constraints(cloth_structure& cloth, constraint_structure const& constraint, simulation_parameters const& parameters);

vec3 simulation_fan_clothesline(simulation_parameters &parameters, char axis);

// Helper function that tries to detect if the simulation diverged 
bool simulation_detect_divergence(cloth_structure const& cloth);