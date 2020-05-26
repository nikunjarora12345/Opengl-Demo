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
#include "ObjLoader.hpp"

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
        Loader::ObjLoader obj1("res/models/suzanne.obj");
        std::vector<float> vertex1 = obj1.GetVertexData();
        float* vertices1 = vertex1.data();

        Loader::ObjLoader obj2("res/models/cube.obj");
        std::vector<float> vertex2 = obj2.GetVertexData();
        float* vertices2 = vertex2.data();

        Loader::ObjLoader obj3("res/models/cube.obj");
        std::vector<float> vertex3 = obj3.GetVertexData();
        float* vertices3 = vertex3.data();

        Renderer::Renderer renderer;

        Renderer::VertexBuffer vb1(vertices1, vertex1.size() * sizeof(float));
        Renderer::VertexBuffer vb2(vertices2, vertex2.size() * sizeof(float));
        Renderer::VertexBuffer vb3(vertices3, vertex3.size() * sizeof(float));

        Renderer::VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(2); // texture coords
        layout.Push<float>(3); // normals
        
        Renderer::VertexArray va1;
        Renderer::VertexArray va2;
        Renderer::VertexArray va3;

        Shader::Shader shader("res/Basic.glsl");
        shader.Bind();

        Renderer::Texture texture("res/models/mask.png");
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

        std::vector<glm::vec3> translation = { 
            glm::vec3(0.0f, 0.0f, -1.0f), 
            glm::vec3(0.5f, 0.0f, 0.0f), 
            glm::vec3(-1.0f, 0.0f, 0.0f) 
        };
        std::vector<glm::vec3> rotation = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
        };
        std::vector<glm::vec3> scale = {
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        };

        std::vector<glm::mat4> nodes = { glm::mat4(1), glm::mat4(1), glm::mat4(1) };

        int currentNode = 0;
        const char* items[] = { "Suzanne", "  Cube1", "    Cube2" };

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
                ImGui::ListBox("Node", &currentNode, items, 3);
                ImGui::Text("Transform");
                ImGui::InputFloat3("Translation", glm::value_ptr(translation[currentNode]), 1);
                ImGui::InputFloat3("Rotation", glm::value_ptr(rotation[currentNode]), 1);
                ImGui::InputFloat3("Scale", glm::value_ptr(scale[currentNode]), 1);
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
                rotation[currentNode].x += 20 * deltaTime;
                if (rotation[currentNode].x >= 360) {
                    rotation[currentNode].x -= 360;
                }
            }

            if (autoRotateY) {
                rotation[currentNode].y += 20 * deltaTime;
                if (rotation[currentNode].y >= 360) {
                    rotation[currentNode].y -= 360;
                }
            }

            if (autoRotateZ) {
                rotation[currentNode].z += 20 * deltaTime;
                if (rotation[currentNode].z >= 360) {
                    rotation[currentNode].z -= 360;
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

            nodes[0] = glm::mat4(1);
            nodes[0] = glm::translate(nodes[0], translation[0]);

            nodes[1] = glm::mat4(1);
            nodes[1] = glm::translate(nodes[1], translation[1]);

            nodes[2] = glm::mat4(1);
            nodes[2] = glm::translate(nodes[2], translation[2]);

            for (int i = 0; i < nodes.size(); i++) {
                nodes[i] = glm::translate(nodes[i], translation[i]); // translation
                nodes[i] =
                    glm::rotate(nodes[i], glm::radians(rotation[i].x), glm::vec3(1, 0, 0))
                    * glm::rotate(nodes[i], glm::radians(rotation[i].y), glm::vec3(0, 1, 0))
                    * glm::rotate(nodes[i], glm::radians(rotation[i].z), glm::vec3(0, 0, 1)); // rotation
                nodes[i] = glm::scale(nodes[i], scale[i]); // scale
            }

            // simulate heirarchy Suzanne -> Cube1 -> Cube2
            nodes[1] = nodes[0] * nodes[1];
            nodes[2] = nodes[1] * nodes[2];

            shader.SetUniformMat4f("u_View", view);
            shader.SetUniformMat4f("u_Projection", proj);
            shader.SetUniform3fv("u_CameraPos", camera.GetPosition());

            shader.SetUniform3fv("u_AlbedoColor", albedoColor);

            shader.SetUniform3fv("u_AmbientColor", ambientColor);
            shader.SetUniform3fv("u_LightColor", lightColor);
            shader.SetUniform3fv("u_SpecularColor", specularColor);
            shader.SetUniform3fv("u_LightPos", lightPos);
            shader.SetUniform1f("u_AmbientIntensity", ambientIntensity);
            shader.SetUniform1f("u_LightIntensity", lightIntensity);
            shader.SetUniform1f("u_SpecularIntensity", specularIntensity);
            
            {
                shader.SetUniformMat4f("u_Model", nodes[0]);

                va1.AddBuffer(vb1, layout);
                renderer.Draw(va1, shader, obj1.vertices.size());

                va1.Unbind();
                vb1.Unbind();
            }

            {
                shader.SetUniformMat4f("u_Model", nodes[1]);

                va2.AddBuffer(vb2, layout);
                renderer.Draw(va2, shader, obj2.vertices.size());

                va2.Unbind();
                vb2.Unbind();
            }

            {
                shader.SetUniformMat4f("u_Model", nodes[2]);

                va3.AddBuffer(vb3, layout);
                renderer.Draw(va3, shader, obj3.vertices.size());

                va3.Unbind();
                vb3.Unbind();
            }

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
