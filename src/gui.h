#ifndef GUI_H
#define GUI_H

#include "glm/glm.hpp"
#include "shader.h"
#include <memory>

class BaseGUI
{
public:
    BaseGUI();

    ~BaseGUI();

    /**
     * @brief Render the GUI
     * 
     * Should be called in the rendering loop after 
     * calling build()
    */
    virtual void render() const;

    /**
     * @brief Build the GUI
     * 
     * Build the ImGUI Frames in this function
    */
    virtual void build(float dt) = 0;

    /**
     * @brief Set uniforms
     * 
     * Update uniform values in the shader based
     * on the GUI values
    */
    virtual void setUniforms(const std::unique_ptr<Shader> &shader) = 0;

protected:
    bool show = false;

private:
    static bool initialized;
};

class PathTracerGUI : public BaseGUI
{
public:
    bool reset = false;
    bool calculate = false;

    void build(float dt);

    void setUniforms(const std::unique_ptr<Shader> &shader);

private:
    int maximalRayMarchingSteps = 300;
    float maximalRayMarchingDistance = 1000.f;
    float epsilon = 0.00001f;
    int numberOfBounces = 10;
    bool automaticFocalLength = true;
    float focalLength = 1.f;
    float aperture = 30;
};

#endif // GUI_H