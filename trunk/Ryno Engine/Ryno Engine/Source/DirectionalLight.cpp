#include "DirectionalLight.h"

namespace Ryno{


	DirectionalLight::DirectionalLight(F32 p, F32 y, F32 r){
		direction.set_direction(p, y, r);
	}
	

	glm::vec3 DirectionalLight::move_to_view_space(Camera3D* camera){
		
		return glm::vec3(glm::transpose(glm::inverse(camera->get_view_matrix()))*
			glm::vec4(direction.pitch,direction.yaw,-direction.roll,0));
	}

	void DirectionalLight::set_direction(F32 p, F32 y, F32 r){
		direction.set_direction(p, y, r);
	}

	void DirectionalLight::set_ambient_color(U8 r, U8 g, U8 b, U8 a){
		ambient_color.set_color(r, g, b, a);
	}

	glm::vec4 DirectionalLight::get_ambient_color(){
		return ambient_color.to_vec4();
	}
}