#include "simulation.hpp"

using namespace cgp;



void simulate(std::vector<particle_structure>& particles, float dt)
{
	vec3 const g = { 0,0,-9.81f };
	size_t const N = particles.size();
	for (size_t k = 0; k < N; ++k)
	{
		particle_structure& particle = particles[k];

		vec3 const f = particle.m * g;

		particle.v = (1 - 0.9f * dt) * particle.v + dt * f;
		particle.p = particle.p + dt * particle.v;
	}

	// To do :
	//  Handle collision ...


	for(int i = 0; i < N; i++)
	{
		//collison with other spheres
		for(int y = 0; y < N; y++)
		{
			if (i != y)
			{
				vec3 dist = particles[i].p - particles[y].p;
				float n = norm(dist);

				vec3 u = dist / n;
				vec3 v1 = particles[i].v;
				vec3 v2 = particles[y].v;
				float m1 = particles[i].m;
				float m2 = particles[y].m;

				if (n <= particles[i].r + particles[y].r)
				{
					float eps = 0.3f;
					if (norm(v2-v1) <= eps)
					{
						particles[i].v = 0.5f * v1;
						particles[y].v = 0.5f * v2;
					}
					else
					{					
						particles[i].v = v1 + 2*m2/(m1+m2) * (dot(v2-v1, u) * u);
						particles[y].v = v2 - 2*m1/(m1+m2) * (dot(v2-v1, u) * u);
					}

					float d = particles[i].r + particles[y].r - n;
					particles[i].p = particles[i].p + (d/2) * u;
					particles[y].p = particles[y].p - (d/2) * u;
				}
			}
		}

		//collision with the box
		vec3 normal1 = {0, 0, 1};
		vec3 a1 = {0, 0, -1};
		float detection = dot(particles[i].p - a1, normal1);

		if (detection <= particles[i].r)
		{
			vec3 tmp = dot(particles[i].v, normal1) * normal1;
			particles[i].v = (particles[i].v - tmp) - tmp;
			particles[i].p = particles[i].p + (particles[i].r - detection) * normal1;
		}

		vec3 normal2 = {1, 0, 0};
		vec3 a2 = {-1, 0, 0};
		detection = dot(particles[i].p - a2, normal2);

		if (detection <= particles[i].r)
		{
			vec3 tmp = dot(particles[i].v, normal2) * normal2;
			particles[i].v = (particles[i].v - tmp) - tmp;
			particles[i].p = particles[i].p + (particles[i].r - detection) * normal2;
		}

		vec3 normal3 = {-1, 0, 0};
		vec3 a3 = {1, 0, 0};
		detection = dot(particles[i].p - a3, normal3);

		if (detection <= particles[i].r)
		{
			vec3 tmp = dot(particles[i].v, normal3) * normal3;
			particles[i].v = (particles[i].v - tmp) - tmp;
			particles[i].p = particles[i].p + (particles[i].r - detection) * normal3;
		}

		vec3 normal4 = {0, 1, 0};
		vec3 a4 = {0, -1, 0};
		detection = dot(particles[i].p - a4, normal4);

		if (detection <= particles[i].r)
		{
			vec3 tmp = dot(particles[i].v, normal4) * normal4;
			particles[i].v = (particles[i].v - tmp) - tmp;
			particles[i].p = particles[i].p + (particles[i].r - detection) * normal4;
		}

		vec3 normal5 = {0, -1, 0};
		vec3 a5 = {0, 1, 0};
		detection = dot(particles[i].p - a5, normal5);

		if (detection <= particles[i].r)
		{
			vec3 tmp = dot(particles[i].v, normal5) * normal5;
			particles[i].v = (particles[i].v - tmp) - tmp;
			particles[i].p = particles[i].p + (particles[i].r - detection) * normal5;
		}

		vec3 normal6 = {0, 0, -1};
		vec3 a6 = {0, 0, 1};
		detection = dot(particles[i].p - a6, normal6);

		if (detection <= particles[i].r)
		{
			vec3 tmp = dot(particles[i].v, normal6) * normal6;
			particles[i].v = (particles[i].v - tmp) - tmp;
			particles[i].p = particles[i].p + (particles[i].r - detection) * normal6;
		}
	}
}
