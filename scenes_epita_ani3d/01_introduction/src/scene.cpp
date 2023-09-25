#include "scene.hpp"


using namespace cgp;

void scene_structure::initialize()
{
	

	// Basic set-up
	// ***************************************** //
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	

	// Initialize the shapes of the scene
	// ***************************************** //

	// Create a mesh structure (here a cube)
	mesh cube_mesh = mesh_primitive_cube(/*center*/{ 0,0,0 }, /*edge length*/ 1.0f);
	// a mesh is simply a container of vertex data (position,normal,color,uv) and triangle index
	// the mesh data are stored on the CPU memory - they will need to be sent to the GPU memory before being drawn


	// Initialize a "mesh_drawable" from a mesh structure
	//   - mesh : store buffer of data (vertices, indices, etc) on the CPU. The mesh structure is convenient to manipulate in the C++ code but cannot be displayed directly (data is not on GPU).
	//   - mesh_drawable : store VBO associated to elements on the GPU + associated uniform parameters. A mesh_drawable can be displayed using the function draw(mesh_drawable, environment). It only stores the indices of the buffers on the GPU - the buffer of data cannot be directly accessed in the C++ code through a mesh_drawable.
	//   Note: A mesh_drawable can be initialized from a mesh structure using the syntax
	//    [meshDrawableName].initalize_data_on_gpu( [meshName] );
	cube.initialize_data_on_gpu(cube_mesh);
	cube.material.color = { 1,1,0 };  // set the color of the cube using (R,G,B) \in [0,1] - this color information is send as uniform parameter to the shader when display is called.
	cube.model.translation = { 1,1,0 }; // set the position of the cube - translation applied as uniform parameter to the shader when display is called.



	// Same process for the ground which is a plane
	float z_floor = -0.51f;
	float L = 3;
	mesh ground_mesh = mesh_primitive_quadrangle({ -L,-L,z_floor }, { L,-L,z_floor }, { L,L,z_floor }, { -L,L,z_floor });
	ground.initialize_data_on_gpu(ground_mesh);
	ground.texture.load_and_initialize_texture_2d_on_gpu(project::path +"assets/checkboard.png");

	mesh sphere_mesh = mesh_primitive_sphere();
	sphere.initialize_data_on_gpu(sphere_mesh);
	sphere.model.scaling = 0.2f; // coordinates are multiplied by 0.2 in the shader
	sphere.model.translation = {1,2,0}; // coordinates are offseted by {1,2,0} in the shader
	sphere.material.color = { 1,0.5f,0.5f }; // sphere will appear red (r,g,b components in [0,1])

	// mesh_load_file_obj: read a .obj file and return a mesh structure
	mesh camel_mesh = mesh_load_file_obj("assets/camel.obj");

	// Classical initialization of a mesh_drawable structure
	camel.initialize_data_on_gpu(camel_mesh);

	// Adjusting size and position of the shape
	camel.model.scaling = 0.5f;
	camel.model.translation = { -1,1,0.5f };

	cone.initialize_data_on_gpu( mesh_primitive_cone(0.1f, 0.2f) );
	cone.material.color = { 0,1,0 };

	cylinder.initialize_data_on_gpu(mesh_primitive_cylinder(0.05f, { 0,0,0 }, { 0,0,0.3f }));
	cylinder.material.color = { 0.55,0.27,0.07 };

	const int N_cone = 60;
	for (int k = 0; k < N_cone; ++k)
	{
		float x = rand_interval(-2, 2);
		float y = rand_interval(-2, 2);

		for(int i=0; i<cone_positions.size(); ++i)
		{
			if (norm(cone_positions[i] - vec3{ x,y,-0.3 }) < 0.2f)
			{
				x = rand_interval(-2, 2);
				y = rand_interval(-2, 2);
				i = 0;
			}
		}

		cone_positions.push_back({ x,y,-0.3 });

		cylinder_positions.push_back({ x,y,-0.51 });
	}
}



// This function is constantly called at every frame
void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	// the general syntax to display a mesh is:
	//   draw(mesh_drawableName, environment);
	//     Note: scene is used to set the uniform parameters associated to the camera, light, etc. to the shader
	/*draw(cube, environment);
	draw(sphere, environment);
	draw(camel, environment);*/

	draw(ground, environment);

	for (int k = 0; k < cone_positions.size(); ++k)
	{
		cone.model.translation = cone_positions[k];
		draw(cone, environment);

		cylinder.model.translation = cylinder_positions[k];
		draw(cylinder, environment);

		if (gui.display_wireframe) 
		{
			draw_wireframe(cone, environment);
			draw_wireframe(cylinder, environment);
		}
	}

	if (gui.display_wireframe) 
	{
		draw_wireframe(ground, environment);
	}

	/*
	if (gui.display_wireframe) 
	{
		draw_wireframe(ground, environment);
		draw_wireframe(sphere, environment);
		draw_wireframe(cube, environment);
		draw_wireframe(camel, environment);
	}*/
}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	//camera_control.action_mouse_click(environment.camera);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
	
}