#include "GLSLProgram.h"
#include "Log.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>


namespace Ryno{
	void GLSLProgram::create(const std::string& name, bool vert, bool geom, bool frag){
		is_shader_present[VERT] = vert;
		is_shader_present[GEOM] = geom;
		is_shader_present[FRAG] = frag;
		init();
		load_shaders(name,ENGINE);
		compile_shaders();
		link_shaders();
	}
	void GLSLProgram::init(){
		m_program_id = 0;
		m_attr_no = 0;
	
		m_shader_ids[VERT] = 0;
		m_shader_ids[GEOM] = 0;
		m_shader_ids[FRAG] = 0;

		m_program_id = glCreateProgram();
	}

	void GLSLProgram::destroy(){

		for (U8 i = VERT; i < VERT + 3; i++){
			if (is_shader_present[i]){
				glDetachShader(m_program_id, m_shader_ids[i]);
				glDeleteShader(m_shader_ids[i]);
			}
		}
		glDeleteProgram(m_program_id);

	}
	void GLSLProgram::load_shaders(const std::string& name, Owner loc){
		shader_name = name;
		static std::string extensions[3]{".vert", ".geom", ".frag"};
		static const std::string middle_path = "Resources/Shaders/";


		for (U8 i = VERT; i < VERT + 3; i++){
			if (is_shader_present[i])
				load_shader((ShadersIndex)i , BASE_PATHS[loc] + middle_path + name + extensions[i-VERT]);
		}
		
	}

	void GLSLProgram::load_shader(ShadersIndex index, const std::string& path){

		switch (index){
		case VERT:	m_shader_ids[VERT] = glCreateShader(GL_VERTEX_SHADER); break;
		case GEOM:	m_shader_ids[GEOM] = glCreateShader(GL_GEOMETRY_SHADER); break;
		case FRAG:	m_shader_ids[FRAG] = glCreateShader(GL_FRAGMENT_SHADER); break;
		}

		if (check_create_errors(index) < 0) {
			destroy();
			return;
		}

		const C* content;
		read_file_inside_string(path, &content);

		//send to GL the shader and keep track of id
		glShaderSource(m_shader_ids[index], 1, &content, NULL);

	}



	void GLSLProgram::compile_shaders(){
		for (U8 i = VERT; i < VERT + 3; i++){
			if (is_shader_present[i])
				compile_shader((ShadersIndex) i);
		}
	}

	void GLSLProgram::compile_shader(ShadersIndex index){

		glCompileShader(m_shader_ids[index]);
		if (check_compile_errors(index) < 0) {
			destroy();
			return;
		}
	}



	
	void GLSLProgram::link_shaders(){

		for (U8 i = VERT; i < VERT + 3; i++)
			if (is_shader_present[i])
				glAttachShader(m_program_id, m_shader_ids[i]);

		glLinkProgram(m_program_id);
		if (check_link_errors() < 0) {
			destroy();
			return;
		}

	}



	GLint GLSLProgram::getUniformLocation(const std::string& uniformName){
		GLint location = glGetUniformLocation(m_program_id, uniformName.c_str());
		if (location == GL_INVALID_INDEX)
			std::cout << "Uniform " + uniformName + " not found in shader." << std::endl;
		return location;
	}

	void GLSLProgram::use(){
		glUseProgram(m_program_id);
		
	}
	void GLSLProgram::unuse(){
		glUseProgram(0);
		
	}

	void GLSLProgram::read_file_inside_string(const std::string&  path, const char** c){

		std::ifstream file(path, std::ios::in);
		file.seekg(0, std::ios::end);
		I32 size = (I32)file.tellg();
		
		file.seekg(0, std::ios::beg);
		C* buffer = (C*)calloc(size, 1);
		file.read(buffer, size + 1);
		buffer[size] = '\0';
		if (!buffer)
			std::cout << "Failed to load from file: " + path << std::endl;
		*c = buffer;


	}

	U8 GLSLProgram::check_create_errors(ShadersIndex index){

		//simply check if glShaderSource returns a shader id
		if (!m_shader_ids[index]){
		
			std::cout << "Shader Failed to be created." << std::endl;
			return -1;
		}
		return 0;
	}

	U8 GLSLProgram::check_compile_errors(ShadersIndex index){

		I32 is_compiled, max_length;
		C* log;

		glGetShaderiv(m_shader_ids[index], GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE)
		{
			glGetShaderiv(m_shader_ids[index], GL_INFO_LOG_LENGTH, &max_length);

			/* The maxLength includes the NULL character */
			log = (C*)malloc(max_length);

			glGetShaderInfoLog(m_shader_ids[index], max_length, &max_length, log);

			std::cout << "Problem compiling shader.\n" + std::string(log) << std::endl;
			free(log);
			return -1;
		}
		return 0;
	}



	U8 GLSLProgram::check_link_errors(){
		I32 is_linked, max_length;
		C* log;
		glGetProgramiv(m_program_id, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			/* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
			glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &max_length);

			/* The maxLength includes the NULL character */
			log = (C *)malloc(max_length);

			/* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
			glGetProgramInfoLog(m_program_id, max_length, &max_length, log);

			std::cout << "Problem linking program.\n" + std::string(log) << std::endl;
			free(log);
			return -1;
		}
		return 0;
	}
}