#include "scene.hpp"


using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ -10, -10, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	obstacle_floor.initialize_data_on_gpu(mesh_primitive_quadrangle({ -10,-10,0 }, { -10,10,0 }, { 10,10,0 }, { 10,-10,0 }));
	obstacle_floor.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/wood.jpg");
	obstacle_floor.model.translation = { 0,0,constraint.ground_z };
	obstacle_floor.material.texture_settings.two_sided = true;

	obstacle_sphere.initialize_data_on_gpu(mesh_primitive_sphere());
	obstacle_sphere.model.translation = constraint.sphere.center;
	obstacle_sphere.model.scaling = constraint.sphere.radius;
	obstacle_sphere.material.color = { 1,0,0 };

	sphere_fixed_position.initialize_data_on_gpu(mesh_primitive_sphere());
	sphere_fixed_position.model.scaling = 0.02f;
	sphere_fixed_position.material.color = { 0,0,1 };

	// mesh_load_file_obj: read a .obj file and return a mesh structure
	mesh fan_base_mesh = mesh_load_file_obj("assets/fan_base.obj");
	fan_base.initialize_data_on_gpu(fan_base_mesh);
	fan_base.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/fan_col.png");
	fan_base.model.translation = { 2,0,constraint.ground_z};
	fan_base.model.scaling = 0.3f;
	fan_base.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2);

	mesh fan_base_head_mesh = mesh_load_file_obj("assets/fan_base_head.obj");
	fan_base_head.initialize_data_on_gpu(fan_base_head_mesh);
	fan_base_head.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/fan_col.png");
	fan_base_head.model.scaling = 0.3f;
	fan_base_head.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2);

	mesh fan_grid_mesh = mesh_load_file_obj("assets/fan_grid.obj");
	fan_grid.initialize_data_on_gpu(fan_grid_mesh);
	fan_grid.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/grid_col2.png");
	fan_grid.shader.load(project::path + "shaders/mesh_transparency/mesh_transparency.vert.glsl", project::path + "shaders/mesh_transparency/mesh_transparency.frag.glsl");
	fan_grid.material.phong = { 0.4f, 0.6f, 0, 1 };
	fan_grid.model.scaling = 0.3f;
	fan_grid.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2);

	mesh fan_propellers_mesh = mesh_load_file_obj("assets/fan_propellers.obj");
	fan_propellers.initialize_data_on_gpu(fan_propellers_mesh);
	fan_propellers.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/fan_col.png");
	fan_propellers.model.scaling = 0.3f;

	hierarchy_fan.add(fan_base, "fan_base");
	hierarchy_fan.add(fan_base_head, "fan_base_head", "fan_base",  { 2.1f,0,1.8f });
	hierarchy_fan.add(fan_grid, "fan_grid", "fan_base_head", {-0.2f, 0, 0.35f});
	hierarchy_fan.add(fan_propellers, "fan_propellers", "fan_base_head",  {-0.2f, 0, 0.35f});

	cloth_texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/cloth.jpg");
	initialize_cloths();
}

// Compute a new cloth in its initial position (can be called multiple times)
void scene_structure::initialize_cloth(int N_sample, cloth_structure &cloth1, cloth_structure_drawable &cloth_drawable1, constraint_structure &constraint1, std::vector<vec3> pos)
{
	cloth1.initialize(N_sample, pos);
	cloth_drawable1.initialize(N_sample);
	cloth_drawable1.drawable.texture = cloth_texture;
	cloth_drawable1.drawable.material.texture_settings.two_sided = true;

	constraint1.fixed_sample.clear();
	constraint1.add_fixed_position(0, 0, cloth1);
	constraint1.add_fixed_position(0, N_sample - 1, cloth1);

	std::cout << "Cloth initialized with " << N_sample << "x" << N_sample << " vertices" << std::endl;
}

void scene_structure::initialize_cloths()
{
	initialize_cloth(gui.N_sample_edge, cloth, cloth_drawable, constraint, { {-5,-5,5}, {-5,0,5}, {0,0,5}, {0,-5,5} });
	initialize_cloth(gui.N_sample_edge, cloth2, cloth_drawable2, constraint2, { {-5,5,5}, {-5,0,5}, {0,0,5}, {0,5,5} });
}


void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);


	// Elements of the scene: Obstacles (floor, sphere), and fixed position
	// ***************************************** //
	
	draw(obstacle_floor, environment);
	//draw(obstacle_sphere, environment);
	for (auto const& c : constraint.fixed_sample)
	{
		sphere_fixed_position.model.translation = c.second.position;
		draw(sphere_fixed_position, environment);
	}

	timer.update();

	int speed = 5;
	if (gui.speed1)
		speed = 5;
	else if (gui.speed2)
		speed = 7;
	else if (gui.speed3)
		speed = 9;

	float rotation_speed = 0.5f;
	if (gui.rotation_speed1)
		rotation_speed = 0.5f;
	else if (gui.rotation_speed2)
		rotation_speed = 1.2f;
	else if (gui.rotation_speed3)
		rotation_speed = 1.8f;

	hierarchy_fan["fan_propellers"].transform_local.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, speed * timer.t);
	hierarchy_fan["fan_base_head"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, std::sin(-rotation_speed * timer.t));

	hierarchy_fan.update_local_to_global_coordinates();
	draw(hierarchy_fan, environment);
	
	// Simulation of the cloth
	// ***************************************** //
	int const N_step = 1; // Adapt here the number of intermediate simulation steps (ex. 5 intermediate steps per frame)
	for (int k_step = 0; simulation_running == true && k_step < N_step; ++k_step)
	{
		// Update the forces on each particle
		simulation_compute_force(cloth, parameters);
		simulation_compute_force(cloth2, parameters);

		// One step of numerical integration
		simulation_numerical_integration(cloth, parameters, parameters.dt);
		simulation_numerical_integration(cloth2, parameters, parameters.dt);

		// Apply the positional (and velocity) constraints
		simulation_apply_constraints(cloth, constraint);
		simulation_apply_constraints(cloth2, constraint2);

		// Check if the simulation has not diverged - otherwise stop it
		bool const simulation_diverged = simulation_detect_divergence(cloth);
		bool const simulation_diverged2 = simulation_detect_divergence(cloth2);
		if (simulation_diverged || simulation_diverged2) {
			std::cout << "\n *** Simulation has diverged ***" << std::endl;
			std::cout << " > The simulation is stoped" << std::endl;
			simulation_running = false;
		}
	}


	// Cloth display
	// ***************************************** //

	// Prepare to display the updated cloth
	cloth.update_normal();        // compute the new normals
	cloth_drawable.update(cloth); // update the positions on the GPU

	cloth2.update_normal();        // compute the new normals
	cloth_drawable2.update(cloth2); // update the positions on the GPU

	// Display the cloth
	draw(cloth_drawable, environment);
	if (gui.display_wireframe)
		draw_wireframe(cloth_drawable, environment);
		
	draw(cloth_drawable2, environment);
	if (gui.display_wireframe)
		draw_wireframe(cloth_drawable2, environment);

}

void scene_structure::display_gui()
{
	bool reset = false;

	ImGui::Text("Display");
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	ImGui::Checkbox("Texture Cloth", &cloth_drawable.drawable.material.texture_settings.active);

	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("Simulation parameters");
	ImGui::SliderFloat("Time step", &parameters.dt, 0.0001f, 0.02f, "%.4f", 2.0f);
	ImGui::SliderFloat("Stiffness", &parameters.K, 0.2f, 50.0f, "%.3f", 2.0f);
	ImGui::SliderFloat("Wind magnitude", &parameters.wind.magnitude, 0, 60, "%.3f", 2.0f);
	ImGui::SliderFloat("Damping", &parameters.mu, 1.0f, 30.0f);
	ImGui::SliderFloat("Mass", &parameters.mass_total, 0.2f, 5.0f, "%.3f", 2.0f);

	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("Speed");
	ImGui::Checkbox("1", &gui.speed1);
	if (gui.speed1)
    {
        gui.speed2 = false;
        gui.speed3 = false;
    }
	ImGui::SameLine();
    ImGui::Checkbox("2", &gui.speed2);
	if (gui.speed2)
    {
        gui.speed1 = false;
        gui.speed3 = false;
    }
	ImGui::SameLine();
    ImGui::Checkbox("3", &gui.speed3);
	if (gui.speed3)
    {
        gui.speed1 = false;
        gui.speed2 = false;
    }
    
	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("Rotation Speed");
	ImGui::Checkbox("1 ", &gui.rotation_speed1);
	if (gui.rotation_speed1)
    {
        gui.rotation_speed2 = false;
        gui.rotation_speed3 = false;
    }
	ImGui::SameLine();
    ImGui::Checkbox("2 ", &gui.rotation_speed2);
	if (gui.rotation_speed2)
    {
        gui.rotation_speed1 = false;
        gui.rotation_speed3 = false;
    }
	ImGui::SameLine();
    ImGui::Checkbox("3 ", &gui.rotation_speed3);
	if (gui.rotation_speed3)
    {
        gui.rotation_speed1 = false;
        gui.rotation_speed2 = false;
    }

	ImGui::Spacing(); ImGui::Spacing();

	reset |= ImGui::SliderInt("Cloth samples", &gui.N_sample_edge, 4, 80);

	ImGui::Spacing(); ImGui::Spacing();
	reset |= ImGui::Button("Restart");
	if (reset) {
		initialize_cloths();
		simulation_running = true;
	}
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

