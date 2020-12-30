#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Texture.hpp"
#include "Geometry.hpp"
#include "ShaderProgram.hpp"

#include <string>
#include <vector>

struct Object
{
	struct Property
	{
		std::string name;
		std::string type;
		std::string value;
	};

	std::string           name;
	std::string           type;
	HitBox                bounds;
	std::vector<Property> properties;
};

struct Layer
{
	GLuint VAO;
	GLuint VBO;
	size_t size;
};

class TileMap
{
public:
	TileMap(glm::ivec2* scr_size);
	TileMap(const TileMap&) = delete;
	TileMap& operator = (const TileMap&) = delete;
	~TileMap();

	bool load(const char* tmx_file_path, Texture* texture);
	void setViewport(const glm::vec2& center);
	void render(ShaderProgram* shader);

	Object*              getObject(const std::string& name);
	std::vector<Object>  getObjectsByName(const std::string& name);
	std::vector<Object>  getObjectsByType(const std::string& type);
	std::vector<Object>* getAllObjects();

private:
	Texture*            tileset;
	std::vector<Layer>  layers;	
	std::vector<Object> objects;
	glm::vec2           position;
	glm::vec2           bounds;
	glm::ivec2*         screen_size;
	bool                viewport_need_update;
};