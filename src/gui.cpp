#include "gui.h"
#include "window.h"
#include "controls.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool BaseGUI::initialized = false;

BaseGUI::BaseGUI()
{
    if (!initialized)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        ImGui::StyleColorsDark();
        // Setup Platform/Renderer backends
        auto window = Window::getInstance().getGLFWWindow();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        initialized = true;
    }
}

BaseGUI::~BaseGUI()
{
    if (initialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        initialized = false;
    }
}

void BaseGUI::render() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void PathTracerGUI::build(float dt)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Pathtracer Parameters", &show);
    ImGui::PushItemWidth(200);
    ImGui::Text("FPS: %.1f", 1 / dt);
    reset = ImGui::Button("Reset");
    bool old = calculate;
    ImGui::Checkbox("Calculate", &calculate);
    // Reset buffer if calculate is started
    if (calculate && !old)
    {
        reset = true;
    }
    if (ImGui::CollapsingHeader("Ray Marching Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        BaseControls *controls = getControls();
        ImGui::InputFloat3("Camera Position", &controls->cameraPosition[0]);
        ImGui::InputFloat3("Camera Direction", &controls->cameraDirection[0]);
        ImGui::InputInt("Maximal Ray Marching Steps", &maximalRayMarchingSteps);
        ImGui::InputFloat("Maximal Ray Marching Distance", &maximalRayMarchingDistance);
        ImGui::InputFloat("Epsilon", &epsilon, 0.00001f, 0.0001f, "%.6f");
    }
    if (ImGui::CollapsingHeader("Path-Tracing Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("Number of Bounces", &numberOfBounces);
        ImGui::Checkbox("Automatic Focal Length", &automaticFocalLength);
        if (!automaticFocalLength)
        {
            ImGui::InputFloat("Focal Length", &focalLength);
        }
        ImGui::InputFloat("Aperture", &aperture);
    }
    ImGui::PopItemWidth();
    ImGui::End();
}

void PathTracerGUI::setUniforms(const std::unique_ptr<Shader> &shader)
{
    shader->setUniform("maximalRayMarchingSteps", maximalRayMarchingSteps);
    shader->setUniform("maximalRayMarchingDistance", maximalRayMarchingDistance);
    shader->setUniform("epsilon", epsilon);
    shader->setUniform("numberOfBounces", numberOfBounces);
    shader->setUniform("calculate", calculate);
    shader->setUniform("automaticFocalLength", automaticFocalLength);
    shader->setUniform("focalLength", focalLength);
    shader->setUniform("aperture", aperture);
    reset = false;
}