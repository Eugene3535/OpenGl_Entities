#include "Sprite.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sprite::Sprite():
    VAO(0), VBO(),
    texture(nullptr),
    color(),
    transform_need_update(true),
    position(0.0f),
    scale(1.0f),
    angle(0)
{
    GLfloat vertices[] 
    {
        0.0f, 0.0f, 
        1.0f, 0.0f,  
        1.0f, 1.0f, 
        0.0f, 1.0f
    };

    Color colors[]
    {
        Color::WHITE,
        Color::WHITE,
        Color::WHITE,
        Color::WHITE
    };

    GLfloat tex_coords[] 
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), &colors, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    glEnableVertexAttribArray(2);
}

Sprite::~Sprite()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(3, VBO);
}

void Sprite::setTexture(Texture* tex)
{   
    texture = tex;
    float w = (float)texture->getSize().x;
    float h = (float)texture->getSize().y;

    GLfloat vertices[] =
    {
        0.0f, 0.0f,  
          w,  0.0f,   
          w,   h,     
        0.0f,  h 
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    transform_need_update = true;
}

void Sprite::setTextureRect(const glm::fRect& rect)
{
    GLfloat vertices[] = 
    {
        0.0f,       0.0f,   
        rect.width, 0.0f,   
        rect.width, rect.height,  
        0.0f,       rect.height 
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float tex_width  = (float)texture->getSize().x;
    float tex_height = (float)texture->getSize().y;

    float left   = rect.left / tex_width;
    float top    = rect.top / tex_height;
    float right  = (rect.left + rect.width) / tex_width;
    float bottom = (rect.top + rect.height) / tex_height;

    GLfloat tex_coords[] =
    {
        left,  top,
        right, top,
        right, bottom,
        left,  bottom
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tex_coords), tex_coords);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    transform_need_update = true;
}

void Sprite::move(float offsetX, float offsetY)
{
    position.x += offsetX;
    position.y += offsetY;
    setPosition(position.x, position.y);
    transform_need_update = true;
}

void Sprite::setPosition(float dx, float dy)
{
    position.x = dx;
    position.y = dy;
    transform_need_update = true;
}

void Sprite::setScale(float factorX, float factorY)
{
    scale.x = factorX;
    scale.y = factorY;
    transform_need_update = true;
}

void Sprite::setRotation(float degrees)
{
    angle = glm::radians(degrees);
    transform_need_update = true;
}

void Sprite::setColor(const Color& new_color)
{
    Color colors[] =
    {
        new_color,
        new_color,
        new_color,
        new_color
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), &colors);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    transform_need_update = true;
}

const glm::vec2& Sprite::getPosition() const
{
    return position;
}

const glm::vec2& Sprite::getScale() const
{
    return scale;
}

const float Sprite::getRotation() const
{
    return glm::degrees(angle);
}

const Color& Sprite::getColor() const
{
    return color;
}

void Sprite::render(ShaderProgram* shader)
{
    texture->bind(true);
    glBindVertexArray(VAO);

    shader->use();

    if (transform_need_update)
    {
        glm::mat4 transform_matrix(1.0f); 
        transform_matrix = glm::translate(transform_matrix, glm::vec3(position, 0.0f));

        transform_matrix = glm::translate(transform_matrix, glm::vec3(0.5f * scale.x, 0.5f * scale.x, 0.0f));
        transform_matrix = glm::rotate(transform_matrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        transform_matrix = glm::translate(transform_matrix, glm::vec3(-0.5f * scale.x, -0.5f * scale.y, 0.0f));

        transform_matrix = glm::scale(transform_matrix, glm::vec3(scale, 0.0f));

        shader->setUniform("model", glm::value_ptr(transform_matrix));

        transform_need_update = false;
    }
        
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    texture->bind(false);
    glBindVertexArray(0);
}