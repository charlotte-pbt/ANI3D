#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "cloth/cloth.hpp"
#include "simulation/simulation.hpp"

using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = false;
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
	cgp::mesh_drawable pin_fixed_position;

	// Display of the clothesline
	cgp::mesh_drawable line;
	cgp::mesh_drawable left_pole;
	cgp::mesh_drawable right_pole;
	cgp::hierarchy_mesh_drawable hierarchy_clothesline;

	// Display of th little clothesline
	cgp::mesh_drawable little_line;
	cgp::mesh_drawable little_left_pole;
	cgp::mesh_drawable little_right_pole;
	cgp::hierarchy_mesh_drawable hierarchy_little_clothesline;

	// Display of the fan
	cgp::mesh_drawable fan_base;
	cgp::mesh_drawable fan_base_head;
	cgp::mesh_drawable fan_grid;
	cgp::mesh_drawable fan_propellers;
	cgp::hierarchy_mesh_drawable hierarchy_fan;
	std::pair<float, float> hierarchy_fan_position = { 0.0f, 0.0f };
	// Fan rotation speed
	float rotation_speed = 0.00f;
	float new_rotation_speed = 0.00f;

	// Cloth related structures
	simulation_parameters parameters;          // Stores the parameters of the simulation (time step, wind settings)


	// On clothesline in front of the fan

	cloth_structure clothF1;                     // The values of the position, velocity, forces, etc, stored as a 2D grid
	cloth_structure_drawable cloth_drawableF1;   // Helper structure to display the cloth as a mesh
	constraint_structure constraintF1;           // Handle the parameters of the constraints (fixed vertices, floor, pin, fan collision)
	cloth_structure clothF2;                     
	cloth_structure_drawable cloth_drawableF2;   
	constraint_structure constraintF2;           
	cloth_structure clothF3;                     
	cloth_structure_drawable cloth_drawableF3;   
	constraint_structure constraintF3;   

	// On clothesline right of the fan

	cloth_structure clothR1; 
	cloth_structure_drawable cloth_drawableR1;
	constraint_structure constraintR1;     
	cloth_structure clothR2;
	cloth_structure_drawable cloth_drawableR2;
	constraint_structure constraintR2;
	cloth_structure clothR3;
	cloth_structure_drawable cloth_drawableR3;
	constraint_structure constraintR3;

	// On clothesline left of the fan

	cloth_structure clothL1;
	cloth_structure_drawable cloth_drawableL1;
	constraint_structure constraintL1;
	cloth_structure clothL2;
	cloth_structure_drawable cloth_drawableL2;
	constraint_structure constraintL2;
	cloth_structure clothL3;
	cloth_structure_drawable cloth_drawableL3;
	constraint_structure constraintL3;
	cloth_structure clothL4;
	cloth_structure_drawable cloth_drawableL4;
	constraint_structure constraintL4;
	cloth_structure clothL5;
	cloth_structure_drawable cloth_drawableL5;
	constraint_structure constraintL5;

	// On little clothesline (behind the fan)

	cloth_structure clothLC1;                    
	cloth_structure_drawable cloth_drawableLC1;   
	constraint_structure constraintLC1;           

                   

	// Helper variables
	bool simulation_running = true;   // Boolean indicating if the simulation should be computed


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void initialize_cloths();
	void initialize_cloth(int N_sample, cloth_structure &cloth, cloth_structure_drawable &cloth_drawable, constraint_structure &constraint, std::vector<vec3> pos, float x_lenght, float y_lenght); // Recompute the cloth from scratch

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





