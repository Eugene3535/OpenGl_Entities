#include "Texture.hpp"

#include <iostream>
#include <map>

Texture::Texture() : id(0), size(0)
{
}

Texture::~Texture()
{
    if(id) glDeleteTextures(1, &id);
}

bool Texture::loadFromFile(const std::string& file_path)
{
    if (id) glDeleteTextures(1, &id);

    glGenTextures(1, &id);

    setRepeated(false);
    setSmooth(false);

    int width, height, channels;
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);

    size = { width, height };

    GLenum mode = channels == 4 ? GL_RGBA : GL_RGB;

    if (data)
    {
        bind(true);
        glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        bind(false);

        stbi_image_free(data);      
        return true;
    }
    std::cout << "Failed to load texture " + file_path + '\n';
    return false;
}

void Texture::bind(bool to_bind)
{
    to_bind ? glBindTexture(GL_TEXTURE_2D, id) : glBindTexture(GL_TEXTURE_2D, 0);;
}

void Texture::setRepeated(bool repeat)
{
    bind(true);

    if (repeat)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float border_color[] { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    }
    bind(false);
}

void Texture::setSmooth(bool smooth)
{
    bind(true);

    if (smooth)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    bind(false);
}

const glm::uvec2 Texture::getSize() const
{
    return size;
}

Texture* GetTexture(const std::string_view file_name)
{
    static std::map<std::string_view, Texture> textures;

    if (auto found = textures.find(file_name); found != textures.end())
        return &found->second;
    else  
        textures[file_name].loadFromFile(std::string(file_name));  

    return &textures[file_name];
}