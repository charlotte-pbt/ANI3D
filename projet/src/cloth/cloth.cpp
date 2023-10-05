#include "cloth.hpp"

using namespace cgp;


void cloth_structure::initialize(int N_samples_edge_arg, std::vector<vec3> pos, int x_lenght, int y_lenght)
{
    lenght_x = x_lenght;
    lenght_y = y_lenght;

    assert_cgp(N_samples_edge_arg > 3, "N_samples_edge=" + str(N_samples_edge_arg) + " should be > 3");

    position.clear();
    normal.clear();
    velocity.clear();
    force.clear();

    position.resize(N_samples_edge_arg, N_samples_edge_arg);
    normal.resize(N_samples_edge_arg, N_samples_edge_arg);
    velocity.resize(N_samples_edge_arg, N_samples_edge_arg);
    force.resize(N_samples_edge_arg, N_samples_edge_arg);
    

    mesh const cloth_mesh = mesh_primitive_grid(pos[0], pos[1], pos[2], pos[3], N_samples_edge_arg, N_samples_edge_arg);
    position = grid_2D<vec3>::from_buffer(cloth_mesh.position, N_samples_edge_arg, N_samples_edge_arg);
    normal = grid_2D<vec3>::from_buffer(cloth_mesh.normal, N_samples_edge_arg, N_samples_edge_arg);
    triangle_connectivity = cloth_mesh.connectivity;
}

void cloth_structure::update_normal()
{
    normal_per_vertex(position.data, triangle_connectivity, normal.data);
}

int cloth_structure::N_samples_x() const
{
    return position.dimension.x;
}

int cloth_structure::N_samples_y() const
{
    return position.dimension.y;
}



void cloth_structure_drawable::initialize(int N_samples_edge, int x_length, int y_length)
{
    mesh const cloth_mesh = mesh_primitive_grid({x_length,y_length,0}, {x_length,0,0}, {0,0,0}, {0,y_length,0}, N_samples_edge, N_samples_edge);

    drawable.clear();
    drawable.initialize_data_on_gpu(cloth_mesh);
    drawable.material.phong.specular = 0.0f;
    opengl_check;
}


void cloth_structure_drawable::update(cloth_structure const& cloth)
{    
    drawable.vbo_position.update(cloth.position.data);
    drawable.vbo_normal.update(cloth.normal.data);
}

void draw(cloth_structure_drawable const& cloth_drawable, environment_generic_structure const& environment)
{
    draw(cloth_drawable.drawable, environment);
}
void draw_wireframe(cloth_structure_drawable const& cloth_drawable, environment_generic_structure const& environment)
{
    draw_wireframe(cloth_drawable.drawable, environment);
}