#pragma once
#include <GL\glew.h>
#include <string> 
#include <fstream>
#include <vector>
#include <map>
#include "Global.h"
#include "Structures.h"


namespace Ryno{
	struct GLTexture{
		U64 width;
		U64 height;
	};
	class TextureManager{
	public:
		static TextureManager* get_instance();
		Texture loadPNG(const std::string& name, LocationOfResource loc);
		Texture loadCubeMap(const std::string& name, LocationOfResource loc);
	private:
		TextureManager::TextureManager();
		U8 read_file_to_buffer(const std::string& f_path, std::vector<U8>& buffer);
	};
}