#pragma once

#include <glad/glad.h>
#include "stb_image.h"
#include <glm/glm.hpp>

#include <string>
#include <string_view>

class Texture
{
public:

    Texture();
    ~Texture();

    bool loadFromFile(const std::string& file_path);
    void bind(bool to_bind);
    void setRepeated(bool repeat);
    void setSmooth(bool smooth);
    const glm::uvec2 getSize() const;

private:
    GLuint id;
    glm::uvec2 size;
};

Texture* GetTexture(const std::string_view file_name);