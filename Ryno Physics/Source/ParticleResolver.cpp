#include "ParticleResolver.h"
#include "ParticleContact.h"
namespace RynoEngine {

	void ParticleResolver::resolve_contacts(const std::vector<ParticleContact*>& contacts, F duration)
	{
		//Iterate the whole algorithm n times
		for (current_iteration = 0; current_iteration < max_iterations; current_iteration++) {

			//Find the strogest contact, which means the one
			//with greater velocity. This is to resolve
			//for collision, the resoution for interpenetration
			//is done after.
			F max_vel = FLT_MAX;
			U max_idx = contacts.size();

			//Find contact with lower separating speed;
			for (I i = 0; i < contacts.size(); i++) {

				F sep_vel = contacts[i]->calculate_separating_velocity();

				//After first iterations, many contacts will not be contacts anymore.
				//So check that the objects are penetrating or colliding
				if (sep_vel < max_vel && (sep_vel < 0 || contacts[i]->penetration > 0)) {
					max_idx = i;
					max_vel = sep_vel;
				}
			}

			//No more contacts needs resolving
			if (max_idx == contacts.size())
				break;

			//Resolve "strongest" contact
			contacts[max_idx]->resolve(duration);

			V3* movements = contacts[max_idx]->particle_movements;

			for (I i = 0; i < contacts.size(); i++) {

				//If the current contact is also in contact with a previously resolved contact, 
				//then remove the movements generated by that contact.
				//It must be dotted with the contact normal, because if it is, for instance, 
				//at 90 degrees, zero movement must be removed
				if (contacts[i]->particles[0] == contacts[max_idx]->particles[0])
				{
					contacts[i]->penetration -= dot(movements[0], contacts[i]->contact_normal);
				}
				else if (contacts[i]->particles[0] == contacts[max_idx]->particles[1])
				{
					contacts[i]->penetration -= dot(movements[1], contacts[i]->contact_normal);
				}
				if (contacts[i]->particles[1])
				{
					if (contacts[i]->particles[1] == contacts[max_idx]->particles[0])
					{
						contacts[i]->penetration += dot(movements[0], contacts[i]->contact_normal);
					}
					else if (contacts[i]->particles[1] == contacts[max_idx]->particles[1])
					{
						contacts[i]->penetration += dot(movements[1], contacts[i]->contact_normal);
					}
				}
			}

		}
	}
}