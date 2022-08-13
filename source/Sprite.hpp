#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Texture.hpp"
#include "Color.hpp"
#include "ShaderProgram.hpp"
#include "Rectangle.hpp"

class Sprite
{
public:
    Sprite();
    ~Sprite();

    void setTexture(Texture* tex);
    void setTextureRect(const glm::fRect& rect);

    void move(float offsetX, float offsetY);
    void setPosition(float dx, float dy);
    void setScale(float factorX, float factorY);
    void setRotation(float degrees);
    void setColor(const Color& new_color);

    const glm::vec2& getPosition() const;
    const glm::vec2& getScale()    const;
    const float      getRotation() const;
    const Color&     getColor()    const;

    void render(ShaderProgram* shader);

private:  
    GLuint VAO, VBO[3];
    Texture* texture; 
    Color color;

    bool transform_need_update;
    glm::vec2 position;
    glm::vec2 scale;
    float     angle;
};
