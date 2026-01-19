#include "GRAPH-Master1.h"
using namespace std;

float x_mod = 0;
float y_mod = 0;
float z_mod = 0;
float x_rot = 0;
float y_rot = 1.0f;
float z_rot = 0;
float theta = 0;
float scale = 1.0f;


void Key_Callback(GLFWwindow* window,
                int key,
                int scancode,
                int action,
                int mods)
{
    if ((key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)))
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            y_rot -= 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            y_rot += 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            x_rot += 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            x_rot -= 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            z_rot += 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            z_rot -= 0.1f;
        }
    }

    ////Move rabbit to the right with D Key by 0.1f
    //if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    //    //x_mod += 0.001f;
    //}

    //if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    //    //x_mod += 0.001f;
    //}

    //if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    //    //x_mod += 0.001f;
    //}

    //if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    //    //x_mod += 0.001f;
    //}
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Niks :>", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //Loading vertex shader
    std::fstream vertSrc("Shaders/Sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //Load Fragment Shader
    std::fstream fragSrc("Shaders/Sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress); // if using CMAKE
    glfwSetKeyCallback(window, Key_Callback);

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

    //Load Vertices
    GLfloat vertices[]{
        0.f, 0.5f, 0.f,     //0
        -1.f, -0.f, 0.f,    //1
        0.5f, -0.5f, 0.f    //2
    };

    //Load Indices
    GLuint indices[]{
        0,1,2
    };

    //Load Meshes
    std::string path = "3D/bunny.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &materials,
        &error,
        path.c_str()
    );

    //EBO array
    std::vector<GLuint> mesh_indices;

    //for loop all indices
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    GLuint VAO, VBO, EBO;

    //Generate vertices and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * attributes.vertices.size(),
        &attributes.vertices[0],
        GL_STATIC_DRAW
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

    glEnableVertexAttribArray(0); //Enable Index 0

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
            
        //Translate -> scale -> rotate
        /*
        glm::mat4 transform = glm::mat4(1.0f); //identity matrix4
        transform = glm::translate(transform, glm::vec3(x_mod, y_mod, z_mod)); // 0 0 0
        transform = glm::scale(transform, glm::vec3(scale, scale, scale)); // 1 1 1
        transform = glm::rotate(transform, glm::radians(theta), glm::normalize(glm::vec3(x_rot, y_rot, z_rot)));

        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc, //transform Variable
            1, //matrices to assign
            GL_FALSE, //transpose?
            glm::value_ptr(transform) //pointer to matrix
        );
        */

        //Use Shader
        glUseProgram(shaderProg);

        //Using Glad
        glBindVertexArray(VAO);

        //Draw Mesh
        glDrawElements(GL_TRIANGLES,
            mesh_indices.size(),
            GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    //Delete Vertex and buffers after.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    //Terminate gl
    glfwTerminate();
    return 0;
}