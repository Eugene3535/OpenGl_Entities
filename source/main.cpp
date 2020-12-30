#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.hpp"
#include "Sprite.hpp"
#include "Animation.hpp"
#include "TileMap.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <Windows.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool IsKeyPressed(GLFWwindow* window, const int key);

glm::ivec2 screen_size = glm::ivec2(1200, 800);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screen_size.x, screen_size.y, "OpenGL Entities", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture* tileset = GetTexture("main_tileset.png");
    Texture* characters = GetTexture("Characters_1.png");

    TileMap level(&screen_size);
    level.load("Levels/Map_1.tmx", tileset);

    glm::mat4 projection(1.0f);
    projection = glm::ortho(0.0f, (float)screen_size.x, (float)screen_size.y, 0.0f, 0.0f, 1.0f);

    ShaderProgram tilemap_shader;
    tilemap_shader.compile("Shaders/tilemap_shader.vert", GL_VERTEX_SHADER);
    tilemap_shader.compile("Shaders/tilemap_shader.frag", GL_FRAGMENT_SHADER);
    tilemap_shader.addUniform("view");
    tilemap_shader.addUniform("projection");

    tilemap_shader.use();
    tilemap_shader.setUniform("projection", glm::value_ptr(projection));

    ShaderProgram sprite_shader;
    sprite_shader.compile("Shaders/sprite_shader.vert", GL_VERTEX_SHADER);
    sprite_shader.compile("Shaders/sprite_shader.frag", GL_FRAGMENT_SHADER);
    sprite_shader.addUniform("model");
    sprite_shader.addUniform("projection");

    sprite_shader.use();
    sprite_shader.setUniform("projection", glm::value_ptr(projection));

    AnimationManager sprite;
    sprite.setTexture(characters);
    sprite.add("walk down",  0, 192, 32, 48, 3, 0.5f);
    sprite.add("walk left",  0, 240, 32, 48, 3, 0.5f);
    sprite.add("walk right", 0, 288, 32, 48, 3, 0.5f);
    sprite.add("walk up",    0, 336, 32, 48, 3, 0.5f);
    sprite.set("walk left");
    sprite.play();
    sprite.setPosition(1180, 520);

    float fps = 0;
    float time = 0, last_time = 0;
    float frame_time = 0;
    unsigned counter = 0;

    while (!glfwWindowShouldClose(window))
    {
        time = static_cast<float>(glfwGetTime());
        frame_time = time - last_time;
        last_time = time;
        counter++;

        if(IsKeyPressed(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        sprite.pause();

        if (IsKeyPressed(window, GLFW_KEY_A))
        {
            sprite.move(-2.0f, 0.0f);
            sprite.set("walk left");
            sprite.play();
        }

        if (IsKeyPressed(window, GLFW_KEY_D))
        {
            sprite.move(2.0f, 0.0f);
            sprite.set("walk right");
            sprite.play();
        }

        if (IsKeyPressed(window, GLFW_KEY_W))
        {
            sprite.move(0.0f, -2.0f);
            sprite.set("walk up");
            sprite.play();
        }

        if (IsKeyPressed(window, GLFW_KEY_S))
        {
            sprite.move(0.0f, 2.0f);          
            sprite.set("walk down");
            sprite.play();
        }

        sprite.tick(frame_time * 5);
        level.setViewport(sprite.getPosition());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        level.render(&tilemap_shader);
        sprite.render(&sprite_shader);

        glfwSwapBuffers(window);
        glfwPollEvents();      
    }

    fps = counter / time;

    std::cout << "sprite " << sizeof(Sprite)
    << "\nfps " << fps 
    << "\ncounter " << counter 
    << "\ntime " << time
    << "\nframe time: " << frame_time;

    glfwTerminate();
    return 0;
}

bool IsKeyPressed(GLFWwindow* window, const int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS) ? true : false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    screen_size = glm::ivec2(width, height);
    glViewport(0, 0, width, height);
}
