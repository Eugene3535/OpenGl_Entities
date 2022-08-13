#include "TileMap.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tinyxml2.h"

#include <cstdlib>
#include <cctype>
#include <iostream>
#include <algorithm>

TileMap::TileMap(glm::ivec2* scr_size):
	tileset(nullptr), position(), bounds(), screen_size(scr_size), viewport_need_update(true)
{	
}

TileMap::~TileMap()
{
	for (auto& layer : layers)
	{
		glDeleteBuffers(1, &layer.VBO);
		glDeleteVertexArrays(1, &layer.VAO);
	}	
}

bool TileMap::load(const char* tmx_file_path, Texture* texture)
{
	tileset = texture;

	tinyxml2::XMLDocument document;

	if (document.LoadFile(tmx_file_path) != tinyxml2::XML_SUCCESS)
	{
		std::cout << "Loading file " << tmx_file_path << " failed...\n";
		return false;
	}

	tinyxml2::XMLElement* root_element = document.FirstChildElement("map");

	const GLuint map_width    = std::atoi(root_element->Attribute("width"));
	const GLuint map_height   = std::atoi(root_element->Attribute("height"));
	const GLuint tile_count   = map_width * map_height;

	const GLuint tile_width   = std::atoi(root_element->Attribute("tilewidth"));
	const GLuint tile_height  = std::atoi(root_element->Attribute("tileheight"));

	const GLuint row_count    = tileset->getSize().y / tile_height;
	const GLuint column_count = tileset->getSize().x / tile_width;
	
	const glm::uvec2 tex_size   = tileset->getSize();

	bounds = glm::vec2(map_width * tile_width, map_height * tile_height);
	
	std::vector<glm::vec2> texture_grid; // Contains left - top coords of each tile in the tileset
	texture_grid.reserve(std::size_t(row_count * column_count));

	for (unsigned y = 0u; y < row_count; ++y)
		for (unsigned x = 0u; x < column_count; ++x)
			texture_grid.emplace_back(glm::vec2(x * tile_width, y * tile_height));

	// Tiles
	for (auto layer = root_element->FirstChildElement("layer");
		      layer != nullptr;
		      layer = layer->NextSiblingElement("layer"))
	{
		auto data = layer->FirstChildElement("data");
		std::string dirty_string = data->GetText(), buffer;

		std::vector<unsigned> current_layer;
		current_layer.reserve(dirty_string.size());

		for (auto& character : dirty_string)
		{
			if (isdigit(character))
				buffer += character;
			else
				if (!buffer.empty())
				{
					current_layer.push_back(std::stoi(buffer));
					buffer.clear();
				}
		}
		current_layer.shrink_to_fit();

		std::vector<glm::vec4> vertices;  // vec4 = vec2(vertices) + vec2(tex_coords)
		vertices.reserve(tile_count * 4); 

		std::vector<GLuint> indices;  
		indices.reserve(tile_count * 4); // two triangles composed by 3 vertices

		GLuint tile_num = 0;

		for (GLuint y = 0u; y < map_height; ++y)
		{
			for (GLuint x = 0u; x < map_width; ++x)
			{
				GLuint index = x + y * map_width;
				GLuint tile_id = current_layer[index];
		
				if (tile_id) // Zero value means that current tile is empty
				{
					const glm::vec2& tex_coords = texture_grid[tile_id - 1]; 

					float left   = tex_coords.x / tex_size.x;
					float top    = tex_coords.y / tex_size.y;
					float right  = (tex_coords.x + tile_width)  / tex_size.x;
					float bottom = (tex_coords.y + tile_height) / tex_size.y;

					// Quad
					vertices.push_back({ x * tile_width,              y * tile_height + tile_height, left,  bottom });
					vertices.push_back({ x * tile_width + tile_width, y * tile_height + tile_height, right, bottom });
					vertices.push_back({ x * tile_width + tile_width, y * tile_height,               right, top    });
					vertices.push_back({ x * tile_width,              y * tile_height,               left,  top    });

					// 1-st triangle
					indices.push_back(tile_num);
					indices.push_back(tile_num + 1);
					indices.push_back(tile_num + 2);

					// 2-nd triangle
					indices.push_back(tile_num);
					indices.push_back(tile_num + 2);
					indices.push_back(tile_num + 3);

					tile_num += 4;
				}
			}
		}

		GLuint VAO = 0, VBO = 0, EBO = 0;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* indices.size(), indices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);		

		layers.push_back({ VAO, VBO, EBO, static_cast<GLuint>(indices.size()) });
	}

    // Objects
	for (auto object_group = root_element->FirstChildElement("objectgroup");
		      object_group != nullptr;
		      object_group = object_group->NextSiblingElement("objectgroup"))
	{
		for (auto object = object_group->FirstChildElement("object");
			      object != nullptr;
			      object = object->NextSiblingElement("object"))
		{
			std::string object_name;
			if (object->Attribute("name"))
				object_name = object->Attribute("name");

			std::string object_type;
			if (object->Attribute("type"))
				object_type = object->Attribute("type");
		
			float x = std::atof(object->Attribute("x"));
			float y = std::atof(object->Attribute("y"));

			float width{}, height{};

			if (object->Attribute("width") && object->Attribute("height"))
			{
				width  = std::atof(object->Attribute("width"));
				height = std::atof(object->Attribute("height"));
			}

			Object obj;
			obj.name = object_name;
			obj.type = object_type;
			obj.bounds = glm::fRect(x, y, width, height);

			auto properties = object->FirstChildElement("properties");

			if (properties)
			{
				for (auto property = properties->FirstChildElement("property");
					      property != nullptr;
					      property = property->NextSiblingElement("property"))
				{
					std::string name, type, value;

					if (property->Attribute("name"))
						name = property->Attribute("name");

					if (property->Attribute("type"))
						type = property->Attribute("type");

					if (property->Attribute("value"))
						value = property->Attribute("value");

					obj.properties.push_back(Object::Property({ name, type, value }));
				}
			}
			objects.emplace_back(std::move(obj));
		}
	}
	return true;
}

void TileMap::setViewport(const glm::vec2& center)
{	// Compute ratio tilemap size to screen size
	float ratioX = bounds.x / screen_size->x;
	float ratioY = bounds.y / screen_size->y;
	// Shift the map relatively window
	position = { -center.x * ratioX + screen_size->x, -center.y * ratioY + screen_size->y * 0.5f };

	viewport_need_update = true;	
}

void TileMap::render(ShaderProgram* shader)
{
	tileset->bind(true);
	
	shader->use();

	if (viewport_need_update)
	{
		glm::mat4 viewport_matrix(1.0f);
		viewport_matrix = glm::translate(viewport_matrix, glm::vec3(position, 0.0f));
		shader->setUniform("view", glm::value_ptr(viewport_matrix));

		viewport_need_update = false;
	}

	for (auto& layer : layers)
	{
		glBindVertexArray(layer.VAO);
		glDrawElements(GL_TRIANGLES, layer.size, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
			
	tileset->bind(false);	
}

Object* TileMap::getObject(const std::string& name)
{
	if (auto object = std::find_if(objects.begin(), objects.end(), [&name](const Object& obj)
		{
			return obj.name == name;
		}); 
		object != objects.end()) 
			return object._Ptr;

	return nullptr;
}

std::vector<Object> TileMap::getObjectsByName(const std::string& name)
{
	std::vector<Object> found_objects;

	std::copy_if(objects.begin(), objects.end(), std::back_inserter(found_objects), [&name](const Object& obj)
		{
			return obj.name == name;
		});

	return found_objects;
}

std::vector<Object> TileMap::getObjectsByType(const std::string& type)
{
	std::vector<Object> found_objects;

	std::copy_if(objects.begin(), objects.end(), std::back_inserter(found_objects), [&type](const Object& obj)
		{
			return obj.type == type;
		});

	return found_objects;
}

std::vector<Object>* TileMap::getAllObjects()
{
	return &objects;
}
