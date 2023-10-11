#include "scene.hpp"

using namespace cgp;


void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 15, 0, 10 }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	obstacle_floor.initialize_data_on_gpu(mesh_primitive_quadrangle({ -10,-10,0 }, { -10,10,0 }, { 10,10,0 }, { 10,-10,0 }));
	obstacle_floor.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/ground.jpg", GL_REPEAT, GL_REPEAT);
	obstacle_floor.model.translation = { 0,0,constraintF1.ground_z };
	obstacle_floor.material.texture_settings.two_sided = true;

	mesh laundry_pin_mesh = mesh_load_file_obj("assets/laundry_pin.obj");
	pin_fixed_position.initialize_data_on_gpu(laundry_pin_mesh);
	pin_fixed_position.model.scaling = 0.1f;
	pin_fixed_position.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/laundry_pin_wood.jpg");
	pin_fixed_position.model.translation = { 0,0,constraintF1.ground_z};
	pin_fixed_position.model.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, Pi / 2) * rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2);

	// Clothesline

	line.initialize_data_on_gpu(mesh_primitive_cylinder(0.01f, { -7,-8,6 }, { -7,8,6 }, 10, 20, true));
	line.material.color = { 0.5f, 0.5f, 0.5f };
	left_pole.initialize_data_on_gpu(mesh_primitive_cylinder(0.1f, { -7,-8,6.5f }, { -7,-8, constraintF1.ground_z }, 10, 20, true));
	left_pole.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/wood.jpg");
	right_pole.initialize_data_on_gpu(mesh_primitive_cylinder(0.1f, { -7,8,6.5f }, { -7,8, constraintF1.ground_z }, 10, 20, true));
	right_pole.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/wood.jpg");

	hierarchy_clothesline.add(line, "line");
	hierarchy_clothesline.add(left_pole, "left_pole", "line");
	hierarchy_clothesline.add(right_pole, "right_pole", "line");

	// Little clothesline

	little_line.initialize_data_on_gpu(mesh_primitive_cylinder(0.01f, { 4,2,6 }, { 4,6,6 }, 10, 20, true));
	little_line.material.color = { 0.5f, 0.5f, 0.5f };
	little_left_pole.initialize_data_on_gpu(mesh_primitive_cylinder(0.1f, { 4,2,6.5f }, { 4,2, constraintF1.ground_z }, 10, 20, true));
	little_left_pole.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/wood.jpg");
	little_right_pole.initialize_data_on_gpu(mesh_primitive_cylinder(0.1f, { 4,6,6.5f }, { 4,6, constraintF1.ground_z }, 10, 20, true));
	little_right_pole.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/wood.jpg");

	hierarchy_little_clothesline.add(little_line, "little_line");
	hierarchy_little_clothesline.add(little_left_pole, "little_left_pole", "little_line");
	hierarchy_little_clothesline.add(little_right_pole, "little_right_pole", "little_line");

	// Load fan obj :

	mesh fan_base_mesh = mesh_load_file_obj("assets/fan_base.obj");
	fan_base.initialize_data_on_gpu(fan_base_mesh);
	fan_base.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/fan_col.png");
	fan_base.model.translation = { 0,0,constraintF1.ground_z};
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
	fan_grid.model.scaling = 0.3f;
	fan_grid.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2);

	mesh fan_propellers_mesh = mesh_load_file_obj("assets/fan_propellers.obj");
	fan_propellers.initialize_data_on_gpu(fan_propellers_mesh);
	fan_propellers.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/fan_col.png");
	fan_propellers.model.scaling = 0.3f;

	hierarchy_fan.add(fan_base, "fan_base");
	hierarchy_fan.add(fan_base_head, "fan_base_head", "fan_base",  { 0,0,1.8f });
	hierarchy_fan.add(fan_grid, "fan_grid", "fan_base_head", {-0.2f, 0, 0.35f});
	hierarchy_fan.add(fan_propellers, "fan_propellers", "fan_base_head",  {-0.2f, 0, 0.35f});

	// Cloths
	initialize_cloths();
}

// Compute a new cloth in its initial position (can be called multiple times)
void scene_structure::initialize_cloth(int N_sample, cloth_structure &cloth, cloth_structure_drawable &cloth_drawable, constraint_structure &constraint, std::vector<vec3> pos, float x_lenght, float y_lenght)
{
	cloth.initialize(N_sample, pos, x_lenght, y_lenght);
	cloth_drawable.initialize(N_sample, x_lenght, y_lenght);
	cloth_drawable.drawable.texture = cloth.texture;
	cloth_drawable.drawable.material.texture_settings.two_sided = true;

	constraint.fixed_sample.clear();
	
	constraint.add_fixed_position(0, 2, cloth);
	constraint.add_fixed_position(0, N_sample - 3, cloth);
	constraint.add_fixed_position(1, 2, cloth);
	constraint.add_fixed_position(1, N_sample - 3, cloth);
	constraint.add_fixed_position(2, 2, cloth);
	constraint.add_fixed_position(2, N_sample - 3, cloth);
}

void scene_structure::initialize_cloths()
{
	// On clothesline in front of the fan

	clothF1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/picnic.jpg", GL_REPEAT, GL_REPEAT);
	clothF1.mass_total = 0.8f;
	initialize_cloth(gui.N_sample_edge, clothF1, cloth_drawableF1, constraintF1, { {-8,-2,6}, {-8,-7,6}, {-8,-7,1.2f}, {-8,-2,1.2f} }, 5, 5);

	clothF2.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/towel.jpg", GL_REPEAT, GL_REPEAT);
	clothF2.mass_total = 0.5f;
	initialize_cloth(gui.N_sample_edge, clothF2, cloth_drawableF2, constraintF2, { {-8,7,6}, {-8,2,6}, {-8,2,4.2}, {-8,7,4.2} }, 2, 5);

	clothF3.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/blue.png", GL_REPEAT, GL_REPEAT);
	clothF3.mass_total = 0.3f;
	initialize_cloth(gui.N_sample_edge, clothF3, cloth_drawableF3, constraintF3, { {-8,1,6}, {-8,-1,6}, {-8,-1,3.2}, {-8,1,3.2} }, 3, 2);

	// On clothesline right of the fan

	clothR1.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/tartan2.jpg", GL_REPEAT, GL_REPEAT);
	clothR1.mass_total = 0.8f;
	initialize_cloth(gui.N_sample_edge, clothR1, cloth_drawableR1, constraintR1, { {-7,8,6}, {-2,8,6}, {-2,8,1.2}, {-7,8,1.2} }, 5, 5);

	clothR2.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/green.jpg", GL_REPEAT, GL_REPEAT);
	clothR2.mass_total = 0.65f;
	initialize_cloth(gui.N_sample_edge, clothR2, cloth_drawableR2, constraintR2, { {-1,8,6}, {3,8,6}, {3,8,2.2}, {-1,8,2.2} }, 4, 4);

	clothR3.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/towel.jpg", GL_REPEAT, GL_REPEAT);
	clothR3.mass_total = 0.45f;
	initialize_cloth(gui.N_sample_edge, clothR3, cloth_drawableR3, constraintR3, { {4,8,6}, {7,8,6}, {7,8,4.2}, {4,8,4.2} }, 2, 3);


	// On clothesline left of the fan

	clothL1.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/towel.jpg");
	clothL1.mass_total = 0.45f;
	initialize_cloth(gui.N_sample_edge, clothL1, cloth_drawableL1, constraintL1, { {-3,-8,6}, {-7,-8,6}, {-7,-8,4.2}, {-3,-8,4.2} }, 2, 4);

	clothL2.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/tartan.jpg");
	clothL2.mass_total = 0.3f;
	initialize_cloth(gui.N_sample_edge, clothL2, cloth_drawableL2, constraintL2, { {-2,-8,6}, {-0.5,-8,6}, {-0.5,-8,4.7}, {-2,-8,4.7} }, 1.5, 1.5);

	clothL3.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/tartan.jpg");
	clothL3.mass_total = 0.3f;
	initialize_cloth(gui.N_sample_edge, clothL3, cloth_drawableL3, constraintL3, { {0,-8,6}, {1.5,-8,6}, {1.5,-8,4.7}, {0,-8,4.7} }, 1.5, 1.5);

	clothL4.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/blue.jpg");
	clothL4.mass_total = 0.5f;
	initialize_cloth(gui.N_sample_edge, clothL4, cloth_drawableL4, constraintL4, { {2.5,-8,6}, {4,-8,6}, {4,-8,1.2}, {2.5,-8,1.2} }, 5, 1.5);

	clothL5.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/motif.jpg");
	clothL5.mass_total = 0.6f;
	initialize_cloth(gui.N_sample_edge, clothL5, cloth_drawableL5, constraintL5, { {5,-8,6}, {7,-8,6}, {7,-8,2.2}, {5,-8,2.2} }, 4, 2);

	// On little clothesline (behind the fan)

	clothLC1.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/blue.jpg");
	clothLC1.mass_total = 0.5f;
	initialize_cloth(gui.N_sample_edge, clothLC1, cloth_drawableLC1, constraintLC1, { {4,5,6}, {4,3,6}, {4,3,1.2}, {4,5,1.2} }, 5, 2);
}


void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	// Frame display
	if (gui.display_frame)
		draw(global_frame, environment);

	// Floor display
	draw(obstacle_floor, environment);



	// Clothes pins display
	// ***************************************** //
	
	// If your cloth is along the x axis, you can rotate the pins by 90° with rotate = true
	auto draw_pin = [&](constraint_structure constraint, bool rotate = false) {
		for (auto const& c : constraint.fixed_sample)
		{
			if ( c.second.ku == 0)
			{
				vec3 pin_position =  vec3(c.second.position.x, c.second.position.y, 5.8f);
				pin_fixed_position.model.translation = pin_position;
				if (rotate)
					pin_fixed_position.model.rotation = pin_fixed_position.model.rotation * rotation_transform::from_axis_angle({ 0,1,0 }, Pi/2);
				else
					pin_fixed_position.model.translation.x -= 0.008f;
					
				draw(pin_fixed_position, environment);
				pin_fixed_position.model.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, Pi / 2) * rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2);
				pin_fixed_position.model.translation = pin_position;
			}
		}
	};

	draw_pin(constraintF1);
	draw_pin(constraintF2);
	draw_pin(constraintF3);

	draw_pin(constraintR1, true);
	draw_pin(constraintR2, true);
	draw_pin(constraintR3, true);

	draw_pin(constraintL1, true);
	draw_pin(constraintL2, true);
	draw_pin(constraintL3, true);
	draw_pin(constraintL4, true);
	draw_pin(constraintL5, true);

	draw_pin(constraintLC1);



	// Clotheslines display
	// ***************************************** //

	// Clothesline in front of the fan
	hierarchy_clothesline["line"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, 0);;
	hierarchy_clothesline["line"].transform_local.translation = { -1, 0, 0 };
	hierarchy_clothesline.update_local_to_global_coordinates();
	draw(hierarchy_clothesline, environment);
	// Clothesline left of the fan
	hierarchy_clothesline["line"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, Pi / 2);
	hierarchy_clothesline["line"].transform_local.translation = { 0, -1, 0 };
	hierarchy_clothesline.update_local_to_global_coordinates();
	draw(hierarchy_clothesline, environment);
	// Clothesline right of the fan
	hierarchy_clothesline["line"].transform_local.translation = { 0, 15, 0 };
	hierarchy_clothesline.update_local_to_global_coordinates();
	draw(hierarchy_clothesline, environment);

	// Little clothesline (behind of the fan)
	draw(hierarchy_little_clothesline, environment);



	// Simulation and display of the fan
	// ***************************************** //

	timer.update();

	// Update fan and wind speed in function of the GUI
	int speed = 0;
	if (gui.speed1)
	{
		speed = 5;
		parameters.wind.magnitude = 1;
	}
	else if (gui.speed2)
	{
		speed = 7;
		parameters.wind.magnitude = 3;
	}
	else if (gui.speed3)
	{
		speed = 9;
		parameters.wind.magnitude = 5;
	}
	else
	{
		speed = 0;
		parameters.wind.magnitude = 0;
	}

	// Update rotation fan speed in function of the GUI
	// Définissez les nouvelles vitesses de rotation souhaitées en fonction de la GUI
	
	if (gui.rotation_speed1)
		new_rotation_speed = 1.00f;
	else if (gui.rotation_speed2)
		new_rotation_speed = 1.40f;
	else if (gui.rotation_speed3)
		new_rotation_speed = 2.0f;
	else
		new_rotation_speed = 0.0f;

	// Ajoutez une valeur de changement de rotation progressif
	const float rotation_speed_change_rate = 0.01f; // Réglez cela selon la vitesse souhaitée
	float truncated_new_rotation_speed = std::round(new_rotation_speed * 100.0f) / 100.0f;
	float truncated_rotation_speed = std::round(rotation_speed * 100.0f) / 100.0f;

	// Comparer les nombres tronqués
	if (truncated_new_rotation_speed != truncated_rotation_speed)
	{
		float delta = truncated_new_rotation_speed - truncated_rotation_speed;
		if (std::abs(delta) < rotation_speed_change_rate) 
		{
			// Si la différence est inférieure à la valeur de changement, fixez-la à la nouvelle valeur
			rotation_speed = new_rotation_speed;
		} 
		else 
		{
			// Sinon, ajustez la vitesse progressivement
			rotation_speed += (delta > 0.0f ? 1.0f : -1.0f) * rotation_speed_change_rate;
		}
	}
	

	// Fan position
	hierarchy_fan["fan_base"].transform_local.translation = { hierarchy_fan_position.first, hierarchy_fan_position.second, constraintF1.ground_z };
	parameters.fan_position = hierarchy_fan["fan_base"].transform_local.translation + vec3{0, 0, 1};

	// Update the wind source position
	parameters.wind.source = hierarchy_fan["fan_base"].transform_local.translation;
	parameters.wind.source.z = parameters.wind.initial_direction.z;

	// Fan rotation
	hierarchy_fan["fan_propellers"].transform_local.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, speed * timer.t);
	hierarchy_fan["fan_base_head"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, clamp(std::sin(timer.t) * rotation_speed, -1, 1));

	// Update the direction of the wind with de rotation of the fan
	mat3 objectTransform = hierarchy_fan["fan_base_head"].transform_local.rotation.matrix();
	parameters.wind.direction = normalize(objectTransform * parameters.wind.initial_direction);

	// Display the fan
	hierarchy_fan.update_local_to_global_coordinates();
	draw(hierarchy_fan, environment);
	


	// Simulation of the cloth
	// ***************************************** //

	auto simulation = [](cloth_structure &cloth, simulation_parameters &parameters, constraint_structure &constraint) 
	{
		simulation_compute_force(cloth, parameters);
		simulation_numerical_integration(cloth, parameters.dt);
		simulation_apply_constraints(cloth, constraint, parameters);

		bool const simulation_diverged = simulation_detect_divergence(cloth);
		if (simulation_diverged) 
		{
			std::cout << "\n *** Simulation has diverged for ***" << std::endl;
			std::cout << " > The simulation is stoped" << std::endl;
			return false;
		}
		return true;
	};

	int const N_step = 10; // Adapt here the number of intermediate simulation steps (ex. 5 intermediate steps per frame)
	for (int k_step = 0; simulation_running == true && k_step < N_step; ++k_step)
	{
		simulation(clothF1, parameters, constraintF1) ? simulation_running = true :  simulation_running = false;
		simulation(clothF2, parameters, constraintF2) ? simulation_running = true :  simulation_running = false;
		simulation(clothF3, parameters, constraintF3) ? simulation_running = true :  simulation_running = false;

		simulation(clothR1, parameters, constraintR1) ? simulation_running = true :  simulation_running = false;
		simulation(clothR2, parameters, constraintR2) ? simulation_running = true :  simulation_running = false;
		simulation(clothR3, parameters, constraintR3) ? simulation_running = true :  simulation_running = false;

		simulation(clothL1, parameters, constraintL1) ? simulation_running = true :  simulation_running = false;
		simulation(clothL2, parameters, constraintL2) ? simulation_running = true :  simulation_running = false;
		simulation(clothL3, parameters, constraintL3) ? simulation_running = true :  simulation_running = false;
		simulation(clothL4, parameters, constraintL4) ? simulation_running = true :  simulation_running = false;
		simulation(clothL5, parameters, constraintL5) ? simulation_running = true :  simulation_running = false;

		simulation(clothLC1, parameters, constraintLC1) ? simulation_running = true :  simulation_running = false;
	}



	// Cloth display
	// ***************************************** //

	auto cloth_display = [](cloth_structure_drawable &cloth_drawable, cloth_structure &cloth, gui_parameters gui, environment_structure &e)
	{
		// Prepare to display the updated cloth
		cloth.update_normal();        // compute the new normals
		cloth_drawable.update(cloth); // update the positions on the GPU

		// Display the cloth
		draw(cloth_drawable, e);
		if (gui.display_wireframe)
			draw_wireframe(cloth_drawable, e);
	};

	cloth_display(cloth_drawableF1, clothF1, gui, environment);
	cloth_display(cloth_drawableF2, clothF2, gui, environment);
	cloth_display(cloth_drawableF3, clothF3, gui, environment);

	cloth_display(cloth_drawableR1, clothR1, gui, environment);
	cloth_display(cloth_drawableR2, clothR2, gui, environment);
	cloth_display(cloth_drawableR3, clothR3, gui, environment);

	cloth_display(cloth_drawableL1, clothL1, gui, environment);
	cloth_display(cloth_drawableL2, clothL2, gui, environment);
	cloth_display(cloth_drawableL3, clothL3, gui, environment);
	cloth_display(cloth_drawableL4, clothL4, gui, environment);
	cloth_display(cloth_drawableL5, clothL5, gui, environment);

	cloth_display(cloth_drawableLC1, clothLC1, gui, environment);
}

void scene_structure::display_gui()
{
	bool reset = false;

	ImGui::Text("Display");
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("Simulation parameters");
	ImGui::SliderFloat("Time step", &parameters.dt, 0.0001f, 0.02f, "%.4f", 2.0f);

	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("Fan parameters");
	ImGui::SliderFloat("x", &hierarchy_fan_position.first, -9, 9);

	// Check if the fan is in collision with a clothesline
	if (!parameters.fan_max_y && !parameters.fan_min_y)
	{
		vec3 res_col = simulation_fan_clothesline(parameters, 'x');
		if (res_col.x != 0 && res_col.y != 0 && res_col.z != 0)
		{
			if (parameters.fan_max_x)
			{
				if (hierarchy_fan_position.first > res_col.x)
					hierarchy_fan_position.first = res_col.x;
			}
			else if (parameters.fan_min_x)
			{
				if (hierarchy_fan_position.first < res_col.x)
					hierarchy_fan_position.first = res_col.x;
			}
			hierarchy_fan_position.second = res_col.y;
		}
		else
		{
		parameters.fan_max_y = false;
		parameters.fan_min_y = false;
		}
	}
	else
	{
		parameters.fan_max_x = false;
		parameters.fan_min_x = false;
	}

	ImGui::SliderFloat("y", &hierarchy_fan_position.second, -9, 9);

	// Check if the fan is in collision with a clothesline
	if (!parameters.fan_max_x && !parameters.fan_min_x)
	{
		vec3 res_col = simulation_fan_clothesline(parameters, 'y');
		if (res_col.x != 0 && res_col.y != 0 && res_col.z != 0)
		{
			if (parameters.fan_max_y)
			{
				if (hierarchy_fan_position.second > res_col.y)
					hierarchy_fan_position.second = res_col.y;
			}
			else if (parameters.fan_min_y)
			{
				if (hierarchy_fan_position.second < res_col.y)
					hierarchy_fan_position.second = res_col.y;
			}
			hierarchy_fan_position.first = res_col.x;
		}
		else
		{
		parameters.fan_max_y = false;
		parameters.fan_min_y = false;
		}
	}
	else
	{
		parameters.fan_max_y = false;
		parameters.fan_min_y = false;
	}
	ImGui::Text("Wind force");
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

