#pragma once
#include "Lights/DirectionalLight.h"
#include "DeferredRenderer.h"
#include <Sprite.h>
#include "Scene.h"
#include "Game.h"


namespace Ryno {

	class LightsScene : public Scene
	{

	public:
		LightsScene() : Scene() {}
	private:
		void start() override;
		void input() override;
		void update() override;

		I32 mesh,sphere;
		Shader shader,dir_light_shader, point_light_shader;
		Texture white, white_normal, brick, brick_normal;
		I32 lato = 100;
		std::vector<GameObject> lights;
		GameObject base,parent;
	};
}

