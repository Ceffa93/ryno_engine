E(Includes/global)
E(Includes/spot_include)


uniform sampler3D jitter;
uniform sampler2DShadow shadow_tex;
uniform int index;


layout(std430, binding = 1) buffer spot_ssbo
{
	SpotLight lights[];
};

//Inverse matrix to rebuild position from depth
uniform mat4 light_VP_matrix;

uniform float shadow_strength;

out vec3 fracolor;

//This function generate a depth value from the direction vector, so that it can be compared 
//with the depth value in the shadow cube

float vector_to_depth(vec3 light_vec, float n, float f)
{
	vec3 AbsVec = abs(light_vec);
	float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));



	float NormZComp = (f + n) / (f - n) - (2 * f*n) / (f - n) / LocalZcomp;
	return (NormZComp + 1.0) * 0.5;
}

float get_shadow(vec3 position_world_space, float dotNL) {

	SpotLight spot_light = lights[index];
	vec4 position_light_MVP_matrix = light_VP_matrix * vec4(position_world_space, 1);
	vec3 position_light_MVP_matrix_norm = position_light_MVP_matrix.xyz / position_light_MVP_matrix.w;

	//SHADOWS
	float blur_width = .01;
	float shadow = 1;

	//If no blur, just calculate shadow
	if (spot_light.blur == 0) {
		float bias = 0.0005;

		shadow = texture(shadow_tex, vec3(position_light_MVP_matrix_norm.xy, position_light_MVP_matrix_norm.z - bias));
	}
	else
	{

		//Shadows blur info
		uint SIZE = spot_light.blur * 2 + 2;
		uint HALF_SIZE = SIZE / 2;
		uint SAMPLES_COUNT = SIZE * SIZE;
		uint SAMPLES_COUNT_DIV_2 = SAMPLES_COUNT / 2;
		float INV_SAMPLES_COUNT = (1.0f / SAMPLES_COUNT);

		shadow = 0;
		vec4 shadowMapPos = position_light_MVP_matrix;
		vec4 smCoord = shadowMapPos;
		float fsize = smCoord.w * blur_width;
		vec2 jxyscale = vec2(1, 1);
		vec3 jcoord = vec3(gl_FragCoord.xy * jxyscale, 0);


		//Sample the outher eight shadows
		for (int i = 0; i<HALF_SIZE; i++) {

			vec4 offset = texture(jitter, jcoord);

			jcoord.z += 1.0f / SAMPLES_COUNT_DIV_2;

			smCoord.xy = offset.xy * fsize + shadowMapPos.xy;

			shadow += textureProj(shadow_tex, smCoord) / SIZE;

			smCoord.xy = offset.zw * fsize + shadowMapPos.xy;

			shadow += textureProj(shadow_tex, smCoord) / SIZE;

		}

		//If there is a medium value of shadow, continue with the remaining ones
		if ((shadow - 1) * shadow * dotNL != 0) {

			// most likely, we are in the penumbra
			shadow *= 1.0f / SIZE; // adjust running total


								   // refine our shadow estimate

			for (int i = 0; i<SAMPLES_COUNT_DIV_2 - 4; i++) {

				vec4 offset = texture(jitter, jcoord);

				jcoord.z += 1.0f / SAMPLES_COUNT_DIV_2;


				//The vec 4 holds two informations, doit for both
				smCoord.xy = offset.xy * fsize + shadowMapPos.xy;
				shadow += textureProj(shadow_tex, smCoord)* INV_SAMPLES_COUNT;

				smCoord.xy = offset.zw * fsize + shadowMapPos.xy;
				shadow += textureProj(shadow_tex, smCoord)* INV_SAMPLES_COUNT;

			}

		}
	}

	shadow = lerp(1, shadow, shadow_strength);



	//CONE CUTOFF (with smoothing to the edges)

	float actual_cutoff = dot(normalize(position_world_space - spot_light.position.xyz), spot_light.direction.rgb);




	return shadow * clamp(mix(0, 1, (actual_cutoff - spot_light.cutoff) / (spot_light.cutoff * 0.1)), 0.0, 1.1);


}


void main(){
	//Get uvs of the current fragment
	fracolor = get_shaded_fragment(lights[index], ivec2(gl_FragCoord.xy));
}


