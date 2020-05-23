#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>

#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

constexpr unsigned screenWidth = 1366;
constexpr unsigned screenHeight = 768;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, int button, int action, int mode);
void CursorCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

Renderer::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;

bool keys[1024];
bool firstMouse = true;
bool mouseHidden = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseCallback);
    glfwSetCursorPosCallback(window, CursorCallback);

    /* Turn on V-Sync */
    glfwSwapInterval(1);

    /* Initialize GLEW */
    if (glewInit() != GLEW_OK) {
        std::cout << "Error Initializing GLEW" << std::endl;
        std::cin.get();
        return -1;
    }

    std::cout << glGetString(GL_VENDOR) << " OpenGL version: "
        << glGetString(GL_VERSION) << std::endl;

    // OpenGL scope. Made for cleaning up the buffers before glfw terminates.
    {
        // Cube vertices with Position, Texture coords and Normal vectors
        float vertices[] = {
            // Front face (+z)
            -1.0f, -1.0f, 1.0f, 0.0f, 2.0f / 3.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 1.0f / 3.0f, 2.0f / 3.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f / 3.0f, 3.0f / 3.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 3.0f / 3.0f, 0.0f, 0.0f, 1.0f,
            // Right face (+x)
            1.0f, -1.0f, 1.0f, 1.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 2.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 2.0f / 3.0f, 3.0f / 3.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f / 3.0f, 3.0f / 3.0f, 1.0f, 0.0f, 0.0f,
            // Bottom face (-y)
            1.0f, -1.0f, 1.0f, 2.0f / 3.0f, 2.0f / 3.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 3.0f / 3.0f, 2.0f / 3.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 3.0f / 3.0f, 3.0f / 3.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 2.0f / 3.0f, 3.0f / 3.0f, 0.0f, -1.0f, 0.0f,
            // Top face (+y)
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f / 3.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f / 3.0f, 1.0f / 3.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f / 3.0f, 2.0f / 3.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 2.0f / 3.0f, 0.0f, 1.0f, 0.0f,
            // Left face (-x)
            -1.0f, -1.0f, -1.0f, 1.0f / 3.0f, 1.0f / 3.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 2.0f / 3.0f, 1.0f / 3.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 2.0f / 3.0f, 2.0f / 3.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 1.0f / 3.0f, 2.0f / 3.0f, -1.0f, 0.0f, 0.0f,
            // Back face (-z)
            1.0f, -1.0f, -1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, 3.0f / 3.0f, 1.0f / 3.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, 1.0f, -1.0f, 3.0f / 3.0f, 2.0f / 3.0f, 0.0f, 0.0f, -1.0f,
            1.0f, 1.0f, -1.0f, 2.0f / 3.0f, 2.0f / 3.0f, 0.0f, 0.0f, -1.0f
        };

        unsigned indices[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
            8, 9, 10,
            10, 11, 8,
            12, 13, 14,
            14, 15, 12,
            16, 17, 18,
            18, 19, 16,
            20, 21, 22,
            22, 23, 20
        };

        Renderer::Renderer renderer;

        Renderer::VertexBuffer vb(vertices, sizeof(vertices));
        Renderer::IndexBuffer ib(indices, sizeof(indices) / sizeof(indices[0]));
        
        Renderer::VertexArray va;
        Renderer::VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(2); // texture coords
        layout.Push<float>(3); // normals
        va.AddBuffer(vb, layout);

        Shader::Shader shader("res/Basic.glsl");
        shader.Bind();

        Renderer::Texture texture("res/textures/texture.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //Setup IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        glm::vec3 albedoColor(1.0f, 1.0f, 1.0f);

        glm::vec3 lightPos(0.0f, 3.0f, 0.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 ambientColor(0.1f, 0.1f, 0.1f);
        glm::vec3 specularColor(1.0f, 1.0f, 1.0f);

        float ambientIntensity = 1.0f;
        float lightIntensity = 1.0f;
        float specularIntensity = 1.0f;

        glm::vec3 translation(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation(0.0f, 0.0f, 0.0f);
        glm::vec3 scale(1.0f, 1.0f, 1.0f);

        bool autoRotateX = false;
        bool autoRotateY = false;
        bool autoRotateZ = false;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            /* Render here */
            renderer.Clear();

            float fov = camera.GetFov();

            // Start the ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                ImGui::Text("Cube Transform");
                ImGui::InputFloat3("Translation", glm::value_ptr(translation), 1);
                ImGui::InputFloat3("Rotation", glm::value_ptr(rotation), 1);
                ImGui::InputFloat3("Scale", glm::value_ptr(scale), 1);
                ImGui::Text("Auto Rotate Cube");
                ImGui::Checkbox("X", &autoRotateX);
                ImGui::SameLine(50);
                ImGui::Checkbox("Y", &autoRotateY);
                ImGui::SameLine(100);
                ImGui::Checkbox("Z", &autoRotateZ);

                ImGui::Spacing();

                ImGui::Text("Camera");
                ImGui::SliderFloat("FOV", &fov, 1.0f, 120.0f);

                ImGui::Spacing();

                ImGui::Text("Albedo");
                ImGui::ColorEdit3("Albedo Color", glm::value_ptr(albedoColor));

                ImGui::Spacing();

                ImGui::Text("Lighting");
                ImGui::ColorEdit3("Ambient Color", glm::value_ptr(ambientColor));
                ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0, 1);
                ImGui::InputFloat3("Light Position", glm::value_ptr(lightPos), 1);
                ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
                ImGui::SliderFloat("Light Intensity", &lightIntensity, 0, 10);
                ImGui::ColorEdit3("Specular Color", glm::value_ptr(specularColor));
                ImGui::SliderFloat("Specular Intensity", &specularIntensity, 0, 10);

                ImGui::Spacing();

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            if (autoRotateX) {
                rotation.x += 20 * deltaTime;
                if (rotation.x >= 360) {
                    rotation.x -= 360;
                }
            }

            if (autoRotateY) {
                rotation.y += 20 * deltaTime;
                if (rotation.y >= 360) {
                    rotation.y -= 360;
                }
            }

            if (autoRotateZ) {
                rotation.z += 20 * deltaTime;
                if (rotation.z >= 360) {
                    rotation.z -= 360;
                }
            }

            camera.SetFov(fov);

            // Model View Projection (MVP) Matrix
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 proj = glm::perspective(
                glm::radians(camera.GetFov()), 
                (float)screenWidth / (float)screenHeight, 
                0.1f, 
                1000.0f
            );
            glm::mat4 model(1);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));

            model = glm::translate(model, translation); // translation
            model = 
                glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0))
                * glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0))
                * glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // rotation
            model = glm::scale(model, scale); // scale

            shader.SetUniformMat4f("u_Model", model);
            shader.SetUniformMat4f("u_View", view);
            shader.SetUniformMat4f("u_Projection", proj);
            shader.SetUniform3fv("u_CameraPos", camera.GetPosition());

            shader.SetUniform3fv("u_albedoColor", albedoColor);

            shader.SetUniform3fv("u_AmbientColor", ambientColor);
            shader.SetUniform3fv("u_LightColor", lightColor);
            shader.SetUniform3fv("u_SpecularColor", specularColor);
            shader.SetUniform3fv("u_LightPos", lightPos);
            shader.SetUniform1f("u_AmbientIntensity", ambientIntensity);
            shader.SetUniform1f("u_LightIntensity", lightIntensity);
            shader.SetUniform1f("u_SpecularIntensity", specularIntensity);

            renderer.Draw(va, ib, shader);

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
            DoMovement();
        }

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key > 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, int button, int action, int mode) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseHidden = true;
        } else if (action == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseHidden = false;
        }
    }
}

void CursorCallback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    if (mouseHidden) {
        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}

void DoMovement() {
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.ProcessKeyboard(Renderer::Camera_Movemet::FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.ProcessKeyboard(Renderer::Camera_Movemet::BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.ProcessKeyboard(Renderer::Camera_Movemet::LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.ProcessKeyboard(Renderer::Camera_Movemet::RIGHT, deltaTime);
    }

    if (keys[GLFW_KEY_E] || keys[GLFW_KEY_PAGE_UP]) {
        camera.ProcessKeyboard(Renderer::Camera_Movemet::UP, deltaTime);
    }

    if (keys[GLFW_KEY_Q] || keys[GLFW_KEY_PAGE_DOWN]) {
        camera.ProcessKeyboard(Renderer::Camera_Movemet::DOWN, deltaTime);
    }
}
