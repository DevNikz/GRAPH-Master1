#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

//General Vars
float x_mod = 0;
float y_mod = 0;
float z_mod = 0;
float x_rot = 0;
float y_rot = 0;
float z_rot = 0;
float scale = 5.0f;

float radius = 5.0f;
float speed = 0.5f;

float windowWidth = 600;
float windowHeight = 600;

//Load Vertices
GLfloat vertices[]{
    0.f, 0.5f, 0.f,     //0
    -1.f, -0.f, 0.f,    //1
    0.5f, -0.5f, 0.f    //2
};
GLuint indices[]{ 0,1,2 };

//MODEL CLASS
class Model {
private:
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;
    tinyobj::attrib_t attributes;
    std::vector<GLuint> mesh_indices;
    GLuint vao, vbo, ebo;

public:
    string modelName;
    glm::vec3 position;

    Model(string _name);
    void InitModel();
    void DrawModel();
    void DeleteBuffers();
};

Model::Model(string _name) {
    modelName = "3D/" + _name + ".obj";
}

void Model::InitModel() {
    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &materials,
        &error,
        modelName.c_str()
    );

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    //(SHADERS) Generate vertices and buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    //(POSITIONS) VBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * attributes.vertices.size(),
        &attributes.vertices[0],
        GL_DYNAMIC_DRAW
    );

    //EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(
        //0 = Position
        0, // Index / buffer index
        3, // x y z
        GL_FLOAT, // array of floats
        GL_FALSE, // if its normalized
        3 * sizeof(float), // size of data per vertex
        (void*)0
    );
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Model::DrawModel() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES,
        mesh_indices.size(),
        GL_UNSIGNED_INT, 0);
}

void Model::DeleteBuffers() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

int main(void) {
	GLFWwindow* window;

	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_SAMPLES, 4); //msaax4

    window = glfwCreateWindow(windowWidth, windowHeight, "Niks Ragudo", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
    //Loading vertex shader
    std::fstream vertSrc("Shaders/quizShader.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //Load Fragment Shader
    std::fstream fragSrc("Shaders/quizShader.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress); // if using CMAKE
    //glfwSetKeyCallback(window, processInput);

    //Compile Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    //Compile Fragment Shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    //Pair Both
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);
    glLinkProgram(shaderProg);

    Model model = Model("bunny");
    model.InitModel();

    glm::mat4 proj = glm::perspective(
        glm::radians(45.f),
        windowWidth / windowHeight,
        0.1f,
        100.f
    );

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProg);

        float t = glfwGetTime();
        float angle1 = t * speed;
        float angle2 = angle1 + glm::radians(120.0f);
        float angle3 = angle1 + glm::radians(240.0f);

        glm::vec3 cameraPos = glm::vec3(0.f, 15.f, 0.f);
        glm::vec3 cameraUp = glm::vec3(0.f, 0.f, -1.f);
        glm::vec3 cameraFront = glm::vec3(0.f, 0.f, 0.f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
        //glm::mat4 view = glm::lookAt(glm::vec3(0, 15, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "view"), 1, GL_FALSE, glm::value_ptr(view));

        //Object 1
        glm::mat4 obj1 = glm::mat4(1.0f);
        //obj1 = glm::rotate(obj1, t * 0.5f, glm::vec3(0, 1, 0));
        obj1 = glm::rotate(obj1, angle1, glm::vec3(0, 1, 0));
        obj1 = glm::translate(obj1, glm::vec3(4, 0, 0));
        obj1 = glm::scale(obj1, glm::vec3(scale, scale, scale)); // 1 1 1
        //obj1 = glm::rotate(obj1, t * 4.f, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "model"), 1, GL_FALSE, glm::value_ptr(obj1));
        model.DrawModel();

        //Object 2
        glm::mat4 obj2 = glm::mat4(1.0f);
        obj2 = glm::rotate(obj2, angle2, glm::vec3(0, 1, 0));
        obj2 = glm::translate(obj2, glm::vec3(4, 0, 0));
        obj2 = glm::scale(obj2, glm::vec3(scale, scale, scale)); // 1 1 1
        //obj2 = glm::rotate(obj2, t * 4.f, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "model"), 1, GL_FALSE, glm::value_ptr(obj2));
        model.DrawModel();

        //Object 3
        glm::mat4 obj3 = glm::mat4(1.0f);
        obj3 = glm::rotate(obj3, angle3, glm::vec3(0, 1, 0));
        obj3 = glm::translate(obj3, glm::vec3(4, 0, 0));
        obj3 = glm::scale(obj3, glm::vec3(scale, scale, scale)); // 1 1 1
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "model"), 1, GL_FALSE, glm::value_ptr(obj3));
        model.DrawModel();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    model.DeleteBuffers();

    //Terminate gl
    glfwTerminate();
    return 0;
}