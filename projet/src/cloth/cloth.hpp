#pragma once


#include "cgp/cgp.hpp"
#include "../environment.hpp"

#include <vector>

// Stores the buffers representing the cloth vertices
struct cloth_structure
{    
    // Buffers are stored as 2D grid that can be accessed as grid(ku,kv)
    cgp::grid_2D<cgp::vec3> position;  
    cgp::grid_2D<cgp::vec3> velocity;  
    cgp::grid_2D<cgp::vec3> force;
    cgp::grid_2D<cgp::vec3> normal;

    // Also stores the triangle connectivity used to update the normals
    cgp::numarray<cgp::uint3> triangle_connectivity;

    int lenght_x;
    int lenght_y;

    cgp::opengl_texture_image_structure texture;
    
    void initialize(int N_samples_edge, std::vector<vec3> pos, int x_lenght, int y_lenght);  // Initialize a square flat cloth
    void update_normal();       // Call this function every time the cloth is updated before its draw
    int N_samples_x() const;      // Number of vertex along x dimension of the grid
    int N_samples_y() const;      // Number of vertex along y dimension of the grid
};


// Helper structure and functions to draw a cloth
// ********************************************** //
struct cloth_structure_drawable
{
    cgp::mesh_drawable drawable;

    void initialize(int N_sample_edge, int x_length, int y_length);
    void update(cloth_structure const& cloth);
};

void draw(cloth_structure_drawable const& cloth_drawable, environment_generic_structure const& environment);
void draw_wireframe(cloth_structure_drawable const& cloth_drawable, environment_generic_structure const& environment);
