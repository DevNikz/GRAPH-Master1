#include "GRAPH-Master1.h"
using namespace std;

float x_mod = 0;
float y_mod = 0;
float z_mod = 0;
float x_rot = 0;
float y_rot = 0;
float z_rot = 0;
float scale = 1.0f;

glm::vec3 cameraPos = glm::vec3(0, 0.f, 10.f);
//glm::vec3 cameraFront = glm::vec3(0.f, 3.f, 0.f);
glm::vec3 cameraFront = glm::vec3(0.f, 0.f,-1.f);
glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

float yaw = -90.0f;
float pitch = 0.f;
float moveSpeed = 25.0f;
float turnSpeed = 180.f;

float deltaTime = 0.f;
float lastFrame = 0.f;

struct Object {
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
};
std::vector<Object> objectList;

void spawnObjectInFront(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) {
    Object obj;
    obj.position = position;
    obj.up = up;
    obj.forward = glm::normalize(front);
    objectList.push_back(obj);
}

//Input
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float velocity = moveSpeed * deltaTime;
    float angular = turnSpeed * deltaTime;
    static bool spawnPressed = false;

    if ((key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) || 
        (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
        (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)))
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += velocity * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos -= velocity * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            cameraPos += velocity * cameraUp;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            cameraPos -= velocity * cameraUp;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            pitch += angular;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            pitch -= angular;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            yaw += angular;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            yaw -= angular;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glm::vec3 spawnPos = cameraPos + cameraFront * 10.0f; //spawn object 10 units forward
        spawnObjectInFront(spawnPos, cameraFront, cameraUp);
        spawnPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spawnPressed = false;
    }
    // Clamp pitch to avoid flip
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    // Update camera direction
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

//MODEL CLASS
class Model {
private:
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;
    tinyobj::attrib_t attributes;
    std::vector<GLuint> mesh_indices;
    GLuint vao, vbo, ebo, vbo_uv, texture;
    int img_width, img_height, colorChannels;

public:
    string modelName;
    glm::vec3 position;

    Model(string _name);
    void InitModel();
    void DrawTex(GLuint _shader);
    void DrawModel();
    void DeleteBuffers();
    void InitTextures();
};

Model::Model(string _name) {
    modelName = "3D/" + _name + ".obj";
}

void Model::InitTextures() {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes =
        stbi_load("3D/ayaya.png",
            &img_width,
            &img_height,
            &colorChannels,
            0);

    if (tex_bytes) {
        GLenum format;
        if (colorChannels == 1) format = GL_RED;
        else if (colorChannels == 3) format = GL_RGB;
        else if (colorChannels == 4) format = GL_RGBA;

        //(TEXTURE) Load / bind after shaders
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
            0, //Texture 0
            GL_RGBA, //Target color format
            img_width,
            img_height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            tex_bytes //loaded texture in bytes
        );
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(tex_bytes);
    }

    else {
        stbi_image_free(tex_bytes);
    }
}

void Model::DrawTex(GLuint _shader) {
    GLuint tex0 = glGetUniformLocation(_shader, "diffuseMap");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(tex0 , 0);
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

void Model::InitModel() {
    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &materials,
        &error,
        //path.c_str()
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
    glGenBuffers(1, &vbo_uv);
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

    //UV
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(float) * (sizeof(UV) / sizeof(UV[0])),
        &UV[0],
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

//MAIN
int main(void)
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float windowWidth = 1280;
    float windowHeight = 720;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    window = glfwCreateWindow(windowWidth, windowHeight, "Niks :>", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //Can be used for split screen games
    //glViewport(0,0,1280,720);

    //Loading vertex shader
    std::fstream vertSrc("Shaders/classShader.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //Load Fragment Shader
    std::fstream fragSrc("Shaders/classShader.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress); // if using CMAKE
    glfwSetKeyCallback(window, processInput);

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
    GLuint indices[]{ 0,1,2 };

    Model newModel = Model("myCube");
    newModel.InitModel();
    newModel.InitTextures();
    
    //Projection Camera
    glm::mat4 proj = glm::perspective(
        glm::radians(60.f),
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

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
        //Object Transforms
        for (const Object& o : objectList) {
            //identity
            glm::mat4 objectMatrix = glm::mat4(1.0f);

            //translate
            objectMatrix = glm::translate(objectMatrix, o.position);

            //scale (1f)
            objectMatrix = glm::scale(objectMatrix, glm::vec3(1.f));

            //rotate
            glm::quat rot = glm::quatLookAt(o.forward, o.up);
            objectMatrix *= glm::mat4_cast(rot);

            unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
            glUniformMatrix4fv(transformLoc, //transform Variable
                1, //matrices to assign
                GL_FALSE, //transpose?
                glm::value_ptr(objectMatrix) //pointer to matrix
            );

            unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");
            glUniformMatrix4fv(projectionLoc,
                1,
                GL_FALSE,
                glm::value_ptr(proj));

            unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
            glUniformMatrix4fv(viewLoc,
                1,
                GL_FALSE,
                glm::value_ptr(view));

            newModel.DrawTex(shaderProg);
            newModel.DrawModel();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    newModel.DeleteBuffers();

    //Terminate gl
    glfwTerminate();
    return 0;
}