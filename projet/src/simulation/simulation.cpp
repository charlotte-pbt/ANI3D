#include "simulation.hpp"

using namespace cgp;


// Fill value of force applied on each particle
// - Gravity
// - Drag
// - Spring force
// - Wind force
void simulation_compute_force(cloth_structure& cloth, simulation_parameters const& parameters)
{
    // Direct access to the variables
    //  Note: A grid_2D is a structure you can access using its 2d-local index coordinates as grid_2d(k1,k2)
    //   The index corresponding to grid_2d(k1,k2) is k1 + N1*k2, with N1 the first dimension of the grid.
    //   
    grid_2D<vec3>& force = cloth.force;  // Storage for the forces exerted on each vertex

    grid_2D<vec3> const& position = cloth.position;  // Storage for the positions of the vertices
    grid_2D<vec3> const& velocity = cloth.velocity;  // Storage for the normals of the vertices
    grid_2D<vec3> const& normal = cloth.normal;      // Storage for the velocity of the vertices
    

    size_t const N_total = cloth.position.size();       // total number of vertices
    size_t const N_x = cloth.N_samples_x();                 // number of vertices in one dimension of the grid
    size_t const N_y = cloth.N_samples_y();                 // number of vertices in one dimension of the grid

    // Retrieve simulation parameter
    //  The default value of the simulation parameters are defined in simulation.hpp
    float const K = cloth.K;              // spring stifness
    float const m = cloth.mass_total / N_total; // mass of a particle
    float const mu = cloth.mu;            // damping/friction coefficient
    float const	L0_x = cloth.lenght_x / (N_x - 1.0f);        // rest length between two direct neighboring particle
    float const	L0_y = cloth.lenght_y / (N_y - 1.0f);        // rest length between two direct neighboring particle


    // Gravity
    const vec3 g = { 0,0,-9.81f };
    for (int ku = 0; ku < N_x; ++ku)
        for (int kv = 0; kv < N_y; ++kv)
            force(ku, kv) = m * g;

    // Drag (= friction)
    for (int ku = 0; ku < N_x; ++ku)
        for (int kv = 0; kv < N_y; ++kv)
            force(ku, kv) += -mu * m * velocity(ku, kv);


    // TO DO: Add spring forces ...
    for (int ku = 0; ku < N_x; ++ku) {
        for (int kv = 0; kv < N_y; ++kv) 
        {
            // ...
            // force(ku,kv) = ... fill here the force exerted by all the springs attached to the vertex at coordinates (ku,kv).
            // 
            // Notes:
            //   - The vertex positions can be accessed as position(ku,kv)
            //   - The neighbors are at position(ku+1,kv), position(ku-1,kv), position(ku,kv+1), etc. when ku+offset is still in the grid dimension.
            //   - You may want to loop over all the neighbors of a vertex to add each contributing force to this vertex
            //   - To void repetitions and limit the need of debuging, it may be a good idea to define a generic function that computes the spring force between two positions given the parameters K and L0
            //   - If the simulation is a bit too slow, you can speed it up in adapting the parameter N_step in scene.cpp that loops over several simulation step between two displays.

            auto spring_force = [&](int x, int y, float L) 
            {
                vec3 const& p1 = position(ku, kv);
                vec3 const& p2 = position(x, y);
                force(ku, kv) += K * (norm(p2 - p1) - L) * (p2 - p1) / norm(p2 - p1);
            };

            // voisins a 1 de distance
            if (ku + 1 < N_x) spring_force(ku + 1, kv, L0_x);
            if (ku - 1 >= 0) spring_force(ku - 1, kv, L0_x);
            if (kv + 1 < N_y) spring_force(ku, kv + 1, L0_y);
            if (kv - 1 >= 0) spring_force(ku, kv - 1, L0_y);

            // voisins diagonales
            if (ku + 1 < N_x && kv + 1 < N_y) spring_force(ku + 1, kv + 1, sqrt(L0_x*L0_x + L0_y*L0_y));
            if (ku - 1 >= 0 && kv - 1 >= 0) spring_force(ku - 1, kv - 1, sqrt(L0_x*L0_x + L0_y*L0_y));
            if (ku + 1 < N_x && kv - 1 >= 0) spring_force(ku + 1, kv - 1, sqrt(L0_x*L0_x + L0_y*L0_y));
            if (ku - 1 >= 0 && kv + 1 < N_y) spring_force(ku - 1, kv + 1, sqrt(L0_x*L0_x + L0_y*L0_y));

            // voisins à 2 de distance
            if (ku + 2 < N_x) spring_force(ku + 2, kv, 2 * L0_x);
            if (ku - 2 >= 0) spring_force(ku - 2, kv, 2 * L0_x);
            if (kv + 2 < N_y) spring_force(ku, kv + 2, 2 * L0_y);
            if (kv - 2 >= 0) spring_force(ku, kv - 2, 2 * L0_y);
        }
    }

    // Wind force

    for (int ku = 0; ku < N_x; ++ku) {
        for (int kv = 0; kv < N_y; ++kv) 
        {
            if (parameters.wind.magnitude == 0)
                continue;

            // Calcul vector from wind source to vertex
            vec3 windToVertex = normalize(cloth.position(ku, kv) - parameters.wind.source);
            
            auto length = [](vec3 const& v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); };

            // Calcul scalaire product between windToVertex and wind direction
            float dotProduct = dot(windToVertex, parameters.wind.direction);
            float angleInRadians = acos(dotProduct / (length(windToVertex) * length(parameters.wind.direction)));
            float angleInDegrees = angleInRadians * (180.0f / Pi);

            float v = dot(windToVertex, normal(ku, kv));

            // Calcul distance between wind source and vertex
            float distance = norm(cloth.position(ku, kv) - parameters.wind.source);

            // Calcul new wind mignitude with distance
            float newMagnitude = parameters.wind.magnitude / (distance * distance);
            
            // Verify if it is in the wind direction cone
            if (cgp::abs(angleInDegrees) <= 40.0f) {
                vec3 windForce = v * normal(ku, kv) * newMagnitude;
                force(ku, kv) += windForce;
            }
        }
    }

}

void simulation_numerical_integration(cloth_structure& cloth, float dt)
{
    int const N_x = cloth.N_samples_x();
    int const N_y = cloth.N_samples_y();
    int const N_total = cloth.position.size();
    float const m = cloth.mass_total/ static_cast<float>(N_total);

    for (int ku = 0; ku < N_x; ++ku) {
        for (int kv = 0; kv < N_y; ++kv) 
        {
            vec3& v = cloth.velocity(ku, kv);
            vec3& p = cloth.position(ku, kv);
            vec3 const& f = cloth.force(ku, kv);

            // Standard semi-implicit numerical integration
            v = v + dt * f / m;
            p = p + dt * v;
        }
    }
    
}

// Vector length
float length(vec3 v) 
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Calcul distance between a point and a line segment
float DistanceToSegment(vec3 point, vec3 segmentStart, vec3 segmentEnd) 
{
    vec3 segment = segmentEnd - segmentStart;
    vec3 v = point - segmentStart;
    float t = dot(v, segment) / dot(segment, segment);

    if (t < 0.0f) 
        return length(point - segmentStart); // Distance at the beginning of the segment
    else if (t > 1.0f)
        return length(point - segmentEnd); // Distance at the end of the segment
    else 
    {
        vec3 projection = segmentStart + t * segment;
        return length(point - projection); // Distance between the point and the projection on the segment
    }
}

// Project a point on a capsule
vec3 projectPointOntoCapsule(vec3 point, vec3 capsuleStart, vec3 capsuleEnd, float capsuleRadius) 
{
    vec3 capsuleDirection = normalize(capsuleEnd - capsuleStart);
    vec3 pointToStart = point - capsuleStart;
    float t = dot(pointToStart, capsuleDirection);

    // Calculate the projection of the point on the capsule segment
    if (t <= 0.0f) 
    {
        // The point is before the beginning of the capsule, use the beginning of the capsule
        return capsuleStart + normalize(pointToStart) * capsuleRadius;
    }
    else if (t >= length(capsuleEnd - capsuleStart)) 
    {
        // The point is after the end of the capsule, use the end of the capsule
        return capsuleEnd + normalize(point - capsuleEnd) * capsuleRadius;
    }
    else 
    {
        // The point is on the segment of the capsule
        vec3 closestPointOnSegment = capsuleStart + t * capsuleDirection;

        // Calculate the direction of the point from the center of the capsule
        vec3 pointToCenter = point - closestPointOnSegment;

        // If point is below the capsule, use the projection on the segment
        if (dot(pointToCenter, pointToCenter) <= capsuleRadius * capsuleRadius) 
        {
            vec3 correctionVector = normalize(point - closestPointOnSegment) * capsuleRadius;
            return closestPointOnSegment + correctionVector;
        }
        else 
        {
            // The point is above the capsule, project it on the upper half-sphere
            vec3 topOfCapsule = capsuleStart + t * capsuleDirection + vec3(0.0f, capsuleRadius, 0.0f);
            vec3 directionToTop = normalize(point - topOfCapsule);

            // Ensure that the corrected point lies within the capsule
            vec3 correctedPoint = topOfCapsule + directionToTop * capsuleRadius;
            
            // Make sure the corrected point is within the capsule segment
            float distanceToSegment = DistanceToSegment(correctedPoint, capsuleStart, capsuleEnd);
            if (distanceToSegment > capsuleRadius) 
            {
                // If the corrected point is outside the segment, use the closest endpoint
                return (distanceToSegment < 0.5f) ? capsuleStart : capsuleEnd;
            }

            return correctedPoint;
        }
    }
}


void simulation_apply_constraints(cloth_structure& cloth, constraint_structure const& constraint, simulation_parameters const& parameters)
{
    // Fixed positions of the cloth
    for (auto const& it : constraint.fixed_sample) 
    {
        position_contraint c = it.second;
        cloth.position(c.ku, c.kv) = c.position; // set the position to the fixed one
    }

    // Floor
    for (int ku = 0; ku < cloth.N_samples_x(); ++ku) 
    {
        for (int kv = 0; kv < cloth.N_samples_y(); ++kv) 
        {
            vec3& p = cloth.position(ku, kv);
            if (p.z < constraint.ground_z) 
                p.z = constraint.ground_z + 0.001f;
        }
    }

    // Fan collision
    vec3 capsuleStart = parameters.fan_position +  vec3({ 0.0f, 0.0f, -1.5 });
    vec3 capsuleEnd = parameters.fan_position +  vec3({ 0.0f, 0.0f, 1.2f });
    float capsuleRadius = 1.55f;

    for (int ku = 0; ku < cloth.N_samples_x(); ++ku) 
    {
        for (int kv = 0; kv < cloth.N_samples_y(); ++kv) 
        {
            vec3& p = cloth.position(ku, kv);
            // Calcul distance between p and capsule
            float distanceToCapsule = DistanceToSegment(p, capsuleStart, capsuleEnd);

            // If the p is in collision with the capsule
            if (distanceToCapsule < capsuleRadius) 
            {
                // Adjust the position of the point
                vec3 correctedPosition = projectPointOntoCapsule(p, capsuleStart, capsuleEnd, capsuleRadius);
                p = correctedPosition;
            }
        }
    }

    
    // Clothesline poles collision
    for (int i = 0; i < parameters.clothesline_poles.size(); i++)
    {
        vec3 cylinderStart = parameters.clothesline_poles[i].first;
        vec3 cylinderEnd = parameters.clothesline_poles[i].second;
        float cylinderRadius = 0.3f;

        for (int ku = 0; ku < cloth.N_samples_x(); ++ku) 
        {
            for (int kv = 0; kv < cloth.N_samples_y(); ++kv) 
            {
                vec3& p = cloth.position(ku, kv);
                // Calcul distance between p and capsule
                float distanceToCylinder = DistanceToSegment(p, cylinderStart, cylinderEnd);

                // If the p is in collision with the capsule
                if (distanceToCylinder < cylinderRadius) 
                {
                    // Adjust the position of the point
                    vec3 correctedPosition = projectPointOntoCapsule(p, cylinderStart, cylinderEnd, cylinderRadius);
                    p = correctedPosition;
                }
            }
        }
    }
    
    /*
    // Clothesline collision

    for (int i = 0; i < parameters.clothesline_poles.size(); i++)
    {
        vec3 lineStart = parameters.clothesline[i].first;
        vec3 lineEnd = parameters.clothesline[i].second;
        float threshold = 0.1f;

        for (int ku = 0; ku < cloth.N_samples_x(); ++ku) 
        {
            for (int kv = 0; kv < cloth.N_samples_y(); ++kv) 
            {
                vec3& p = cloth.position(ku, kv);
                // Calculate the distance between p and the line
                float distanceToLine = DistanceToSegment(p, lineStart, lineEnd);

                distanceToLine = std::round(distanceToLine * 100.0f) / 100.0f;
                // If p is in collision with the line
                if (distanceToLine < threshold && p.z >= 6.05f) 
                {
                    // Adjust the position of the point
                    vec3 correctedPosition = projectPointOntoCapsule(p, lineStart, lineEnd, threshold);
                    p = correctedPosition;
                    std::cout << correctedPosition << std::endl;
                }
            }
        }
    }*/   
}  


vec3 simulation_fan_clothesline(simulation_parameters &parameters, char axis)
{   
    for (auto clothesline : parameters.clothesline_poles)
    {
        // Collision between 2 capsules
       
        vec3 fanStart = parameters.fan_position;
        float fanRadius = 1.2f;

        vec3 cylinderStart = clothesline.first;
        vec3 cylinderEnd = clothesline.second;
        float cylinderRadius = 0.3f;

        // Calcul distance between 2 capsules
        float distanceToCapsule = DistanceToSegment(fanStart, cylinderStart, cylinderEnd);

        // If is in collision
        if (distanceToCapsule < fanRadius + cylinderRadius) 
        {
            // Adjust the position of the fan
            vec3 correctedPosition = projectPointOntoCapsule(fanStart, cylinderStart, cylinderEnd, cylinderRadius + fanRadius);

            if (axis == 'x')
            {
                if (fanStart.x < cylinderStart.x)
                {
                    parameters.fan_max_x = true;
                    parameters.fan_min_x = false;
                }
                else
                {
                    parameters.fan_min_x = true;
                    parameters.fan_max_x = false;
                }
                parameters.fan_min_y = false;
                parameters.fan_max_y = false;
                return correctedPosition;
            }
            else if (axis == 'y')
            {
                if (fanStart.y < cylinderStart.y)
                {
                    parameters.fan_max_y = true;
                    parameters.fan_min_y = false;
                }
                else
                {
                    parameters.fan_min_y = true;
                    parameters.fan_max_y = false;
                }
                parameters.fan_min_x = false;
                parameters.fan_max_x = false;
                return correctedPosition;
            }
        }
    }
    parameters.fan_min_x = false;
    parameters.fan_max_x = false;
    parameters.fan_min_y = false;
    parameters.fan_max_y = false;
    return {0,0,0};
}



bool simulation_detect_divergence(cloth_structure const& cloth)
{
    bool simulation_diverged = false;
    const size_t N = cloth.position.size();
    for (size_t k = 0; simulation_diverged == false && k < N; ++k)
    {
        const float f = norm(cloth.force.data.at_unsafe(k));
        const vec3& p = cloth.position.data.at_unsafe(k);

        if (std::isnan(f)) // detect NaN in force
        {
            std::cout << "\n **** NaN detected in forces" << std::endl;
            simulation_diverged = true;
        }

        if (f > 600.0f) // detect strong force magnitude
        {
            std::cout << "\n **** Warning : Strong force magnitude detected " << f << " at vertex " << k << " ****" << std::endl;
            simulation_diverged = true; 
        }

        if (std::isnan(p.x) || std::isnan(p.y) || std::isnan(p.z)) // detect NaN in position
        {
            std::cout << "\n **** NaN detected in positions" << std::endl;
            simulation_diverged = true;
        }
    }

    return simulation_diverged;
}

