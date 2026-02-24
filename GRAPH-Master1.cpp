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
glm::vec3 cameraFront = glm::vec3(0.f, 0.f,-1.f);
glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

float yaw = -90.0f;
float pitch = 0.f;
float moveSpeed = 100.0f;
float turnSpeed = 180.f;

float deltaTime = 0.f;
float lastFrame = 0.f;

struct Object {
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
};
std::vector<Object> objectList;

#ifndef SHADER_H
#define SHADER_H
class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec3(const std::string& name, int i, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), i, glm::value_ptr(value));
    }
    void setMat4(const std::string& name, int i, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), i, GL_FALSE, glm::value_ptr(value));
    }
    void LoadTexture(GLuint texture) {
        GLuint tex0 = glGetUniformLocation(ID, "diffuseMap");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0, 0);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif

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
        glm::vec3 spawnPos = cameraPos + cameraFront * 100.0f; //spawn object 10 units forward
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
    std::vector<GLfloat> fullVertexData;
    GLenum format;

public:
    string modelName;
    string texName;
    glm::vec3 position;

    Model(string _name, string _tex, string _file);
    void InitModel();
    void InitCube();
    void DrawTex(GLuint _shader);
    void DrawCube();
    void DrawModel();
    void DeleteBuffers();
    void InitTextures();
    GLuint GetTexture();
};

GLuint Model::GetTexture() {
    return texture;
}

Model::Model(string _name, string _tex, string _file) {
    modelName = "3D/" + _name + ".obj";
    texName = "3D/" + _tex + _file;
}

void Model::InitTextures() {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes = stbi_load(texName.c_str(), &img_width, &img_height, &colorChannels, 0);
    /*
    unsigned char* tex_bytes =
        stbi_load("3D/partenza.jpg",
            &img_width,
            &img_height,
            &colorChannels,
            0);
            */

    if (tex_bytes) {
        if (colorChannels == 1) format = GL_RED;
        else if (colorChannels == 3) format = GL_RGB;
        else if (colorChannels == 4) format = GL_RGBA;

        //(TEXTURE) Load / bind after shaders
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
            0, //Texture 0
            format, //Target color format
            img_width,
            img_height,
            0,
            format,
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
void Model::DrawTex(GLuint value) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(value, 0);
}

void Model::DrawCube() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
}

void Model::DrawModel() {
    glBindVertexArray(vao);
    //Vertex Data Method
    glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);
}

void Model::DeleteBuffers() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Model::InitCube() {
    //Load Object. If success, it loads
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

void Model::InitModel() {
    //Load Object. If success, it loads
    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &materials,
        &error,
        modelName.c_str()
    );
    
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]); //X
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]); //Y
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]); //Z
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);
        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]); //U
        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]); //V
    }

    GLintptr normalPtr = 3 * sizeof(float);
    GLintptr uvPtr = 6 * sizeof(float);

    //(SHADERS) Generate vertices and buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    //(POSITIONS) VBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //Using fullvertexdata
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * fullVertexData.size(),
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(
        //0 = Position
        0, // Index / buffer index
        3, // x y z
        GL_FLOAT, // array of floats
        GL_FALSE, // if its normalized
        8 * sizeof(GLfloat), // size of data per vertex
        (void*)0
    );
    glEnableVertexAttribArray(0);

    //Normal
    glVertexAttribPointer(
        //0 = Position
        1, // Index / buffer index
        3, // x y z
        GL_FLOAT, // array of floats
        GL_FALSE, // if its normalized
        8 * sizeof(GLfloat), // size of data per vertex
        (void*)normalPtr
    );
    glEnableVertexAttribArray(1);

    //UV
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)uvPtr
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

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress); // if using CMAKE
    glfwSetKeyCallback(window, processInput);

    Shader defaultShader("Shaders/classShader.vert", "Shaders/classShader.frag");
    Shader lightShader("Shaders/lightCube.vert", "Shaders/lightCube.frag");

    /*
    //Loading vertex shader
    std::fstream vertSrc("Shaders/classShader.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //Load Fragment Shader for default meshes
    std::fstream fragSrc("Shaders/classShader.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    //Load Frag Shader for Debug Lights

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
    */

    //Load Vertices
    GLfloat vertices[]{
        0.f, 0.5f, 0.f,     //0
        -1.f, -0.f, 0.f,    //1
        0.5f, -0.5f, 0.f    //2
    };
    GLuint indices[]{ 0,1,2 };

    Model newModel = Model("djSword", "partenza", ".jpg");
    Model lightModel = Model("myCube", "white", ".jpg");

    newModel.InitModel();
    newModel.InitTextures();
    lightModel.InitCube();
    lightModel.InitTextures();
    
    //Projection Camera
    glm::mat4 proj = glm::perspective(
        glm::radians(60.f),
        windowWidth / windowHeight,
        0.1f,
        1000.f
    );

    //For LightStruct
    //glm::vec3 lPosition = glm::vec3(-5, 3, 0);
    //glm::vec3 lAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    //glm::vec3 lDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    //glm::vec3 lSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

    //In Class
    glm::vec3 lightPosition = glm::vec3(-25, 10, -35);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);
    float lightStrength = 5.0f;

    //ambient
    float ambientStrength = 0.1f;
    glm::vec3 ambientColor = lightColor;

    //specular
    float specularStrength = 0.5f;
    float specularPhong = 16.0f;

    //light att
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        //Use Light Shader
        lightShader.use();

        //Camera
        lightShader.setMat4("projection", 1, proj);
        lightShader.setMat4("view", 1, view);

        //Light Object Properties
        glm::mat4 lightMatrix = glm::mat4(1.0f);
        lightMatrix = glm::translate(lightMatrix, lightPosition);
        lightMatrix = glm::scale(lightMatrix, glm::vec3(1.f));
        lightShader.setMat4("model", 1, lightMatrix);
        lightModel.DrawCube();

        //Use defaultShader
        defaultShader.use();
        defaultShader.setMat4("projection", 1, proj);
        defaultShader.setMat4("view", 1, view);
        defaultShader.setVec3("cameraPos", 1, cameraPos);

        glm::mat4 objectMatrix = glm::mat4(1.0f);
        objectMatrix = glm::translate(objectMatrix, glm::vec3(0, 0, -50));
        objectMatrix = glm::scale(objectMatrix, glm::vec3(0.5f));
        defaultShader.setMat4("transform", 1, objectMatrix);
        defaultShader.setVec3("lightPosition", 1, lightPosition);
        defaultShader.setVec3("lightColor", 1, lightColor);
        defaultShader.setFloat("lightStrength", lightStrength);
        defaultShader.setVec3("ambientColor", 1, ambientColor);
        defaultShader.setFloat("ambientStrength", ambientStrength);
        defaultShader.setFloat("constant", constant);
        defaultShader.setFloat("linear", linear);
        defaultShader.setFloat("specularStrength", specularStrength);
        defaultShader.setFloat("specularPhong", specularPhong);
        defaultShader.LoadTexture(newModel.GetTexture());
        newModel.DrawModel();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    //lightModel.DeleteBuffers();
    newModel.DeleteBuffers();
    lightModel.DeleteBuffers();

    //Terminate gl
    glfwTerminate();
    return 0;
}