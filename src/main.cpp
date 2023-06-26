#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <memory>
#include <iostream>

#include "shader.h"
#include "mesh.h"
#include "controls.h"
#include "window.h"
#include "gui.h"
#include "buffers.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const auto imageResolution = glm::vec2(1920, 1080);

static void init()
{
    // Change the current directory to the directory of the executable
    if (chdir("..") != 0)
    {
        std::cerr << "Failed to change directory" << std::endl;
        exit(EXIT_FAILURE);
    }

    // start GLEW extension handler
    glewInit();
    // get version info
    const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte *version = glGetString(GL_VERSION);   // version as a string
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

static void setCommonUniforms(std::unique_ptr<Shader> &shader, int iteration, float time, bool update)
{
    shader->setUniform("time", time);
    shader->setUniform("imageResolution", imageResolution);
    shader->setUniform("cameraPosition", getControls()->cameraPosition);
    shader->setUniform("cameraDirection", getControls()->cameraDirection);
    shader->setUniform("currentFrame", iteration);
    shader->setUniform("update", update);
}

int main()
{
    init();

    // Initalize the quad mesh
    std::vector<Vertex> vertices = {
        {glm::vec3(-1, 1, 0), glm::vec2(0, 0)},
        {glm::vec3(1, 1, 0), glm::vec2(1, 0)},
        {glm::vec3(-1, -1, 0), glm::vec2(0, 1)},
        {glm::vec3(1, -1, 0), glm::vec2(1, 1)},
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        1, 2, 3};
    auto screenQuad = std::make_unique<Mesh>(vertices, indices);

    // Initialize GUI
    auto gui = std::make_unique<PathTracerGUI>();

    auto calculateScene = std::make_unique<Shader>(std::vector<std::string>({"data/shader/pathtracer/calculateScene.comp"}));
    auto renderScene = std::make_unique<Shader>(std::vector<std::string>({"data/shader/pathtracer/renderScene.vert", "data/shader/pathtracer/renderScene.frag"}));
    auto emptyBuffer = std::vector<glm::vec4>(imageResolution.x * imageResolution.y);
    auto imageBuffer = std::make_unique<SSBO<glm::vec4>>(emptyBuffer, 0);

    const int xDispatches = imageResolution.x / 16 + 1;
    const int yDispatches = imageResolution.y / 16 + 1;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto previousTime = startTime;
    int iteration = 0;
    bool update = true;
    while (!glfwWindowShouldClose(Window::getInstance().getGLFWWindow()))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate time between frames
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - previousTime;
        std::chrono::duration<float> runningTime = currentTime - startTime;
        previousTime = currentTime;
        float dt = deltaTime.count();
        float time = runningTime.count();

        // Update controls
        BaseControls *controls = getControls();
        if (!gui->calculate)
        {
            update = controls->update(dt);
            if (controls->reload)
            {
                calculateScene->reload();
                renderScene->reload();
                controls->reload = false;
                iteration = 0;
                imageBuffer->uploadToGPU(emptyBuffer);
                update = true;
            }
        }

        // Build GUI
        gui->build(dt);

        if (gui->reset)
        {
            iteration = 0;
            imageBuffer->uploadToGPU(emptyBuffer);
            update = true;
        }

        // Dispatch scene calculation
        calculateScene->bind();
        setCommonUniforms(calculateScene, iteration, time, update);
        gui->setUniforms(calculateScene);
        calculateScene->dispatch(xDispatches, yDispatches);
        calculateScene->unbind();

        // Render scene
        renderScene->bind();
        setCommonUniforms(renderScene, iteration, time, update);
        gui->setUniforms(renderScene);
        screenQuad->draw();
        gui->render();
        renderScene->unbind();

        glfwSwapBuffers(Window::getInstance().getGLFWWindow());
        iteration++;
        update = false;

        if (gui->save)
        {
            // Save image to file in working directory
            std::cout << "Saving image..." << std::endl;
            std::vector<glm::vec4> imageColMajor;
            imageBuffer->downloadFromGPU(imageColMajor);
            std::vector<glm::vec3> imageRowMajor(imageColMajor.size());
            // Convert from column major to row major
            for (int y = 0; y < imageResolution.y; y++)
            {
                for (int x = 0; x < imageResolution.x; x++)
                {
                    int rowMajorIndex = y * imageResolution.x + x;
                    int columnMajorIndex = x * imageResolution.y + y;
                    imageRowMajor[rowMajorIndex] = imageColMajor[columnMajorIndex];
                }
            }

            auto data = std::vector<char>(3 * imageRowMajor.size());
            for (size_t i = 0; i < imageRowMajor.size(); i++)
            {
                data[3 * i] = (char)(255 * imageRowMajor[i].x);
                data[3 * i + 1] = (char)(255 * imageRowMajor[i].y);
                data[3 * i + 2] = (char)(255 * imageRowMajor[i].z);
            }

            auto now = std::chrono::system_clock::now();
            char timestamp[100];
            std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
            std::strftime(timestamp, 100, "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
            strcat(timestamp, ".png");
            int err = stbi_write_png(timestamp, imageResolution.x, imageResolution.y, 3, data.data(), imageResolution.x * 3 * sizeof(char));
            if (err)
            {
                std::cout << "Saved image to " << timestamp << std::endl;
            }
            else
            {
                std::cerr << "Failed to save image" << std::endl;
            }
            gui->save = false;
        }
    }

    return EXIT_SUCCESS;
}
