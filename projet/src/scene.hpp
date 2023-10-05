#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "cloth/cloth.hpp"
#include "simulation/simulation.hpp"

using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
	int N_sample_edge = 20;  // number of samples of the cloth (the total number of vertices is N_sample_edge^2)

	bool speed1 = true;
	bool speed2 = false;
	bool speed3 = false;

	bool rotation_speed1 = true;
	bool rotation_speed2 = false;
	bool rotation_speed3 = false;
};

// The structure of the custom scene
struct scene_structure : scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	cgp::timer_basic timer;

	// Display of the obstacles and constraints
	cgp::mesh_drawable obstacle_floor;
	cgp::mesh_drawable obstacle_sphere;
	cgp::mesh_drawable sphere_fixed_position;
	cgp::mesh_drawable fan_base;
	cgp::mesh_drawable fan_base_head;
	cgp::mesh_drawable fan_grid;
	cgp::mesh_drawable fan_propellers;
	cgp::hierarchy_mesh_drawable hierarchy_fan;

	// Cloth related structures
	cloth_structure cloth;                     // The values of the position, velocity, forces, etc, stored as a 2D grid
	cloth_structure_drawable cloth_drawable;   // Helper structure to display the cloth as a mesh
	simulation_parameters parameters;          // Stores the parameters of the simulation (stiffness, mass, damping, time step, etc)
	constraint_structure constraint;           // Handle the parameters of the constraints (fixed vertices, floor and sphere)

	// Cloth 2
	cloth_structure cloth2;                     // The values of the position, velocity, forces, etc, stored as a 2D grid
	cloth_structure_drawable cloth_drawable2;   // Helper structure to display the cloth as a mesh
	constraint_structure constraint2;           // Handle the parameters of the constraints (fixed vertices, floor and sphere)

	// Helper variables
	bool simulation_running = true;   // Boolean indicating if the simulation should be computed


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void initialize_cloths();
	void initialize_cloth(int N_sample, cloth_structure &cloth, cloth_structure_drawable &cloth_drawable, constraint_structure &constraint, std::vector<vec3> pos, int x_lenght, int y_lenght); // Recompute the cloth from scratch

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





