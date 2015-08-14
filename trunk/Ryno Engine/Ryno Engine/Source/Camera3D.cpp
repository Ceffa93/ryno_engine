#include "Camera3D.h"
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/transform.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <iostream>
#include <math.h>
//Mathematical constants
#define M_HALF_PI 1.57079632679489661923
#define M_PI 3.14159265358979323846
#define M_PI_2 6.28318530717958647692


namespace Ryno{

	Camera3D::Camera3D(U32 w, U32 h) :width(w), height(h), yaw(0), pitch(0)
	{
		projection_matrix = generate_perspective_matrix(45, w, h, .1f, 10000.0f);

		ortho_matrix = glm::ortho<float>(-500, 500, -500, 500, -500,500);

	
	}
	Camera3D::~Camera3D(){

	}

	glm::mat4 Camera3D::generate_perspective_matrix(F32 angle, U32 width, U32 height, F32 near, F32 far){
		return glm::perspective(angle,width/(F32) height, near, far);
	}


	void Camera3D::generate_camera_matrix(){
		
		//I ignore the scale.
		//I get the rotation from the pitch and yaw, and i make it faster with quaternions.
		//Finally i translate by the position 
		view_matrix = glm::translate(
			glm::toMat4(glm::quat(glm::vec3(pitch, 0, 0)) * glm::quat(glm::vec3(0, yaw, 0))),
			glm::vec3(-position.x, -position.y, position.z)
			); 
		camera_matrix = projection_matrix * view_matrix;
						
	
	}
	

	void Camera3D::move_forward(F32 speed){
		position += speed* glm::vec4(sin(yaw), -sin(pitch), cos(yaw),0);
		
		

	}
	void Camera3D::move_right(F32 speed){
		position += speed* glm::vec4(cos(yaw),0,-sin(yaw),0);

	}
	void Camera3D::move_back(F32 speed){
		move_forward(-speed);
	}
	void Camera3D::move_left(F32 speed){
		move_right(-speed);
	}


	void Camera3D::rotate(F32 y, F32 p){
		yaw += y;
		if (yaw > M_PI_2 || yaw <0) yaw -= (F32)((I32)(yaw / M_PI_2))*M_PI_2;
		pitch += p;
		if (pitch > M_HALF_PI)
			pitch = M_HALF_PI;
		else if (pitch < -M_HALF_PI)
			pitch = -M_HALF_PI;


	
	}
	

}