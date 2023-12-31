#include "Sprite.h"
#include "Global.h"
#include "TextureManager.h"

namespace Ryno{


    Sprite::Sprite(){

		set_scale(1, 1);
    }
    Sprite::Sprite(const Sprite *s)
    {
        *this = *s;
    }
    Sprite::~Sprite(){
    }

    void Sprite::generate_model_matrix()
    {
        F32 deg_angle = -angle * DEG_TO_RAD;
        F32 sn = sin(deg_angle);
        F32 cs = cos(deg_angle);


        F32 new_sc_x = ratio * scale.x / WindowSize::w;
        F32 new_sc_y =  scale.y / WindowSize::h;


        F32 new_x = (position.x - 0.5) * 2;
        F32 new_y = (position.y - 0.5) * 2;

        //Adapt position depending on anchoring point
        switch (anchor_point)
        {
        case TOP_LEFT:
		case MIDDLE_LEFT:
		case BOTTOM_LEFT:
            new_x += new_sc_x;
            break;
		case TOP_RIGHT:
		case MIDDLE_RIGHT:
		case BOTTOM_RIGHT:
            new_x -= new_sc_x;
            break;
 
        }

		//Adapt position depending on anchoring point
		switch (anchor_point)
		{
		case TOP_LEFT:
		case TOP_MIDDLE:
		case TOP_RIGHT:
			new_y -= new_sc_y;
			break;
		case BOTTOM_LEFT:
		case BOTTOM_MIDDLE:
		case BOTTOM_RIGHT:
			new_y += new_sc_y;
			break;

		}



        model_matrix = glm::mat3(
            glm::vec3(new_sc_x * cs, new_sc_x * sn, 0),
            glm::vec3(-new_sc_y * sn, new_sc_y * cs, 0),
            glm::vec3(new_x, new_y, 1)
            );
    }

    void Sprite::set_texture(Texture t)
    {
        texture = t;
		ratio = t.width / (F32)t.height;

    }


	


}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                