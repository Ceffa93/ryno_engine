#pragma once
#include "FireworkScene.h"
#include "ValleyScene.h"
#include "HouseScene.h"
#include "Batches/Batch3DGeometry.h"
#include "Shader.h"

namespace Ryno{

	F32 power_lerper(F32 i, F32 factor){
		return (std::pow(factor,i) - 1) / (factor -1);
	}

	void FireworkScene::start(){

		camera->movement_speed = 2;
		camera->position = glm::vec4(0, 150, -5000, 1);

		Texture white = game->texture_manager->load_png("white_pixel", GAME);
		Texture white_normal = game->texture_manager->load_png("normal_pixel", GAME);
		I32 star_mesh = game->mesh_manager->load_mesh("star", 1, GAME);
		I32 cube_mesh = game->mesh_manager->load_mesh("cube", 1, GAME);


		////loading audio
		//sound = game->audio_manager->load_sound("stomp.wav", GAME);
		//music = game->audio_manager->load_music("journey.ogg", GAME);
		//sound.set_volume(1.0f);
		//music.set_volume(1.0f);
		//music.play();

		go[0].create(game->stack_allocator);
		go[0]->transform.create(game->stack_allocator);
		go[0]->transform->set_scale(100, 100, 100);
		go[0]->transform->set_position(0, 55, 50);
	/*	go[0]->model.create(game->stack_allocator);
		go[0]->model->mesh = cube_mesh;
		go[0]->model->set_texture_normal(white, white_normal);*/
		go[0]->dir_light.create(game->stack_allocator);
		go[0]->dir_light->set_direction(-65, 150);
		go[0]->dir_light->diffuse_intensity = 0.2;
		go[0]->dir_light->set_diffuse_color(255, 255, 200);
		go[0]->dir_light->specular_intensity = .05;
		go[0]->dir_light->set_specular_color(255, 255, 200);
		go[0]->dir_light->ambient_intensity = .05;
		go[0]->dir_light->set_ambient_color(255, 255, 200);



		Emitter* emitter = go[0]->emitter.create(game->stack_allocator, *go[0]);
		emitter->save_map.add("texture", white);
		emitter->save_map.add("normal", white_normal);
		emitter->save_map.add("mesh", star_mesh);



		




		emitter->lambda_creation = [](Emitter* e, Particle3D* p){
			Texture white, normal;
			//Emitter* emitter;
			I32 mesh;
			e->save_map.get("texture", &white);
			e->save_map.get("normal", &normal);
			e->save_map.get("mesh", &mesh);

			p->decay_rate = .0005f;
			p->speed = .5f;
			p->model.create(StackAllocator::get_instance());
			p->model->material = new Material();
			p->model->material->set_shader(Batch3DGeometry::s);
			p->model->set_texture_normal(white, normal);
			p->model->mesh = mesh;
			p->model->material->set_attribute("in_Color",ColorRGBA::yellow);
			p->model->material->set_attribute("in_Tiling", glm::vec2(1,1));

			//p->model->color = ColorRGBA::yellow;
			p->transform->add_rotation(glm::vec3(ryno_math::rand_int_range(0, 360), ryno_math::rand_int_range(0, 360),0));
			p->model->cast_shadows = false;
			p->model->set_flatness(255);
			
		};
		emitter->lambda_spawn = [](Emitter* e){
			for (U8 i = 0; i < 15; i++){
				Particle3D* p = e->new_particle();
				p->transform->position = e->game_object->transform->position;
				p->direction = ryno_math::get_rand_dir(0, 360, 0, 360);
			}
		};

		emitter->lambda_particle_update = [](Emitter* e, Particle3D* p, float _delta_time)
		{
			p->transform->set_position(p->direction * p->speed * _delta_time + p->transform->position);
			p->transform->set_scale(ryno_math::lerp(glm::vec3(30), glm::vec3(50), p->lifetime));

			ColorRGBA from = ColorRGBA::yellow;
			ColorRGBA to = ColorRGBA::red;

			p->model->material->set_attribute("in_Color",ryno_math::lerp(from, to, power_lerper(p->lifetime,20)));
		};

		emitter->init(2200);
		go[1].create(game->stack_allocator, *go[0]);
		go[1]->dir_light.~New<DirectionalLight>();
		go[2].create(game->stack_allocator, *go[1]);
		go[3].create(game->stack_allocator, *go[1]);
		


		go[1]->emitter->lambda_particle_update = [](Emitter* e, Particle3D* p, float _delta_time)
		{
			p->transform->set_position(p->direction * p->speed * _delta_time + p->transform->position);
			p->transform->set_scale(ryno_math::lerp(glm::vec3(30), glm::vec3(50), p->lifetime));

			ColorRGBA from = ColorRGBA::green;
			ColorRGBA to = ColorRGBA::blue;

			p->model->material->set_attribute("in_Color", ryno_math::lerp(from, to, power_lerper(p->lifetime, 20)));
		};
		go[2]->emitter->lambda_particle_update = [](Emitter* e, Particle3D* p, float _delta_time)
		{
			p->transform->set_position(p->direction * p->speed * _delta_time + p->transform->position);
			p->transform->set_scale(ryno_math::lerp(glm::vec3(30), glm::vec3(50), p->lifetime));
			ColorRGBA from = ColorRGBA::white;
			ColorRGBA to = ColorRGBA::magenta;

			p->model->material->set_attribute("in_Color", ryno_math::lerp(from, to, power_lerper(p->lifetime, 20)));
		};

		go[3]->emitter->lambda_particle_update = [](Emitter* e, Particle3D* p, float _delta_time)
		{
			p->transform->set_position(p->direction * p->speed * _delta_time + p->transform->position);
			p->transform->set_scale(ryno_math::lerp(glm::vec3(30), glm::vec3(50), p->lifetime));

			
		};
		
		go[3]->emitter->lambda_spawn = [](Emitter* e){
			for (U8 i = 0; i < 15; i++){
				Particle3D* p = e->new_particle();
				p->transform->position = e->game_object->transform->position;
				p->direction = ryno_math::get_rand_dir(0, 360, 0, 360);
				p->model->material->set_attribute("in_Color", ryno_math::rand_color_range(ColorRGBA(100, 100, 100, 255), ColorRGBA::white));
			}
		};

		go[0]->transform->position += glm::vec3(1200, 1200, 0);
		go[1]->transform->position += glm::vec3(-1200, 1200, 0);
		go[2]->transform->position += glm::vec3(1200, -1200, 0);
		go[3]->transform->position += glm::vec3(-1200, -1200, 0);

	}


	void FireworkScene::update(){
		
	}

	void FireworkScene::input(){
		if (!game->shell->active){
			if (game->input_manager->is_key_pressed(SDLK_c, KEYBOARD)){
				game->set_scene("house");

			}
			if (game->input_manager->is_key_pressed(SDLK_n, KEYBOARD)){
				i = ++i % 4;
			}
			if (game->input_manager->is_key_down(SDLK_RIGHT, KEYBOARD)){
				go[i]->transform->position += game->delta_time * 1.0f * glm::vec3(1, 0, 0);
			}
			if (game->input_manager->is_key_down(SDLK_LEFT, KEYBOARD)){
				go[i]->transform->position += game->delta_time * 1.0f * glm::vec3(-1, 0, 0);
			}
			if (game->input_manager->is_key_down(SDLK_UP, KEYBOARD)){
				go[i]->transform->position += game->delta_time * 1.0f * glm::vec3(0,1, 0);
			}
			if (game->input_manager->is_key_down(SDLK_DOWN, KEYBOARD)){
				go[i]->transform->position += game->delta_time * 1.0f * glm::vec3(0,-1, 0);
			}
		}
	}



}