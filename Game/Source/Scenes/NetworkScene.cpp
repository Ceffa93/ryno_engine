#pragma once

#include "NetworkScene.h"
#include <thread>
namespace Ryno {

	void NetworkScene::start() {

		client = Network::get_instance()->client;

		camera->position = glm::vec4(0,0,-10,1);
		camera->yaw = 0;
		camera->pitch = 0;
		camera->movement_speed = 100;
		camera->have_skybox = true;
		camera->skybox = game->texture_manager->load_cube_map("day", ".png", GAME);

		white = game->texture_manager->load_png("white_pixel.png", GAME);
		white_normal = game->texture_manager->load_png("normal_pixel.png", GAME);
		mesh = game->mesh_manager->load_mesh("cube", GAME);

		shader.create("Geometry/geometry", GAME);
		dir_light_shader.create("LightPass/directional", ENGINE);

		

		//dir light
		auto* l = cube.add_script<DirectionalLight>();
		l->model.material.set_shader(&dir_light_shader);
		l->set_rotation(-50, 10, 0);
		l->diffuse_intensity = 1.5f;
		l->set_diffuse_color(255, 235, 200);
		l->specular_intensity = 20;
		l->set_specular_color(0, 0, 0);
		l->ambient_intensity = 0;
		l->set_ambient_color(255, 255, 255);
		l->shadows = false;

		

		

	}

	void NetworkScene::input() {
		if (!controlled) {
			controlled = create_net_obj(NetId(client->local_address));
			controlled->game_object->transform.set_position(ryno_math::rand_vec3_range(glm::vec3(-4, -2, -1), glm::vec3(4, 2, 1)));
			controlled->game_object->get_script<Model>()->sub_models[0].material.set_attribute("in_DiffuseColor", ryno_math::rand_color_range(ColorRGBA::black, ColorRGBA::white));
			controlled->moved = true;
		}
		else {
			controlled->moved = false;
		}
		if (!game->shell->active) {
			if(controlled) {
				float speed = 10.0f;
				if (game->input_manager->is_key_down(SDLK_RIGHT, KEYBOARD)) {
					controlled->game_object->transform.add_position(game->delta_time * speed * glm::vec3(1, 0, 0));
					controlled->moved = true;
				}
				if (game->input_manager->is_key_down(SDLK_LEFT, KEYBOARD)) {
					controlled->game_object->transform.add_position(game->delta_time * speed * glm::vec3(-1, 0, 0));
					controlled->moved = true;
				}
				if (game->input_manager->is_key_down(SDLK_UP, KEYBOARD)) {
					controlled->game_object->transform.add_position(game->delta_time * speed * glm::vec3(0, 1, 0));
					controlled->moved = true;
				}
				if (game->input_manager->is_key_down(SDLK_DOWN, KEYBOARD)) {
					controlled->game_object->transform.add_position(game->delta_time * speed * glm::vec3(0, -1, 0));
					controlled->moved = true;
				}
			}
		}
	}
	void NetworkScene::network_recv(const Message* message) {
		NetObject* received = NetObject::find(message->id);

		const PosAndColor* pos_and_col = (const PosAndColor*)message;

		if (received == nullptr) {
			received = create_net_obj(message->id);
			ColorRGBA color = Message::convert<ColorRGBA>(pos_and_col->color);
			received->game_object->get_script<Model>()->sub_models[0].material.set_attribute("in_DiffuseColor", color);
		}

		
		F32 x = Message::convert<F32>(pos_and_col->x);
		F32 y = Message::convert<F32>(pos_and_col->y);
		F32 z = Message::convert<F32>(pos_and_col->z);


		glm::vec3 p = glm::vec3(x, y, z);

		received->game_object->transform.set_position(glm::vec3(x,y,z));
	}

	void NetworkScene::network_send(NetObject* sender, Message* message) {
		if (!sender->moved)
			return;
		PosAndColor& m = *(PosAndColor*)message;
		m.id = sender->id;
		glm::vec3 p = sender->game_object->transform.get_position();
		ColorRGBA col = *(ColorRGBA*)sender->game_object->get_script<Model>()->sub_models[0].material.get_attribute("in_DiffuseColor");

		m.x = Message::convert<U32>(p.x);
		m.y = Message::convert<U32>(p.y);
		m.z = Message::convert<U32>(p.z);
		m.color = Message::convert<U32>(col);
	}

	NetObject* NetworkScene::create_net_obj(const NetId& id) {
		net_cubes.emplace_back();

		GameObject* c = &(net_cubes.back());
		NetObject* net_obj = c->add_script(new NetObject(id));
		c->transform.set_scale(glm::vec3(1,1,1));
		auto& m = c->add_script<Model>()->add_sub_model();

		m.material.set_shader(&shader);
		m.material.set_attribute("in_DiffuseColor", ColorRGBA(255, 255, 255, 255));
		m.material.set_attribute("in_SpecularColor", ColorRGBA(255, 255, 255, 255));

		m.material.set_attribute("in_Tiling", glm::vec2(1, 1));
		m.material.set_uniform("texture_sampler", white.id);
		m.material.set_uniform("normal_map_sampler", white_normal.id);
		m.mesh = mesh;
		m.cast_shadows = false;

		return net_obj;
	}
}