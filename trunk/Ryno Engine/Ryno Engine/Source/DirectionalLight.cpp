#include "DirectionalLight.h"

namespace Ryno{


	DirectionalLight::DirectionalLight(F32 p, F32 y, F32 r){
		direction.set_direction(p, y, r);
	}
	

	glm::vec3 DirectionalLight::move_to_view_space(Camera3D* camera){
		
		return glm::vec3(glm::transpose(glm::inverse(camera->get_view_matrix()))*
			glm::vec4(direction.pitch,direction.yaw,-direction.roll,0));
	}

	void DirectionalLight::send_uniforms(Camera3D* camera){

		glm::vec3 r = move_to_view_space(camera);
		glUniform3f(program->getUniformLocation("dir_light.direction"), r.x, r.y, r.z);
		glUniform4f(program->getUniformLocation("dir_light.diffuse"), diffuse_color.r /256.0f, diffuse_color.g /256.0f, diffuse_color.b /256.0f,diffuse_intensity);
		glUniform4f(program->getUniformLocation("dir_light.specular"), specular_color.r /256.0f, specular_color.g /256.0f, specular_color.b /256.0f,specular_intensity);
		glUniform4f(program->getUniformLocation("dir_light.ambient"), ambient_color.r /256.0f, ambient_color.g /256.0f, ambient_color.b /256.0f, ambient_intensity);
		
	}

	void DirectionalLight::set_direction(F32 p, F32 y, F32 r){
		direction.set_direction(p, y, r);
	}

	void DirectionalLight::set_ambient_color(U8 r, U8 g, U8 b){
		ambient_color.set_color(r, g, b);
	}

	glm::vec3 DirectionalLight::get_ambient_color(){
		return ambient_color.to_vec3();
	}
}