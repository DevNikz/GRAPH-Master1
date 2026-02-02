#pragma once

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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

GLfloat UV[]{
    0.f, 1.f,
    0.f, 0.f,
    1.f, 1.f,
    1.f, 0.f,
    1.f, 1.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f
};

/*s
    //(VIEWS)
    //Orthographic camera
    //glm::mat4 proj = glm::ortho(
    //    -2.f, //L
    //    2.f, //R
    //    -2.f, //Bottom
    //    2.f, //Top
    //    -1.f, //Near
    //    1.f //Far
    //);
    
    
    */


//Camera
/*
glm::vec3 cameraPos = glm::vec3(0.f, 10.f, 50.f);
glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.f), cameraPos * -1.f);
glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
glm::vec3 cameraCenter = glm::vec3(0.f, 3.f, 0.f);
//Forward (z)
glm::vec3 F = cameraFront - cameraPos;
F = glm::normalize(F);
//Right (x)
glm::vec3 R = glm::cross(F, cameraUp);
//Up (y)
glm::vec3 U = glm::cross(R, F);
glm::mat4 cameraRotMatrix = glm::mat4(1.f);
glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
*/

/*
glm::mat4 objectMatrix = glm::mat4(1.0f);
objectMatrix = glm::translate(objectMatrix, glm::vec3(0, 0, -1));
objectMatrix = glm::translate(objectMatrix, glm::vec3(x_mod, y_mod, z_mod)); // 0 0 0
objectMatrix = glm::scale(objectMatrix, glm::vec3(scale, scale, scale)); // 1 1 1
//objectMatrix = glm::rotate(objectMatrix, glm::radians(y_rot), glm::normalize(glm::vec3(1, 0, 0)));
//objectMatrix = glm::rotate(objectMatrix, glm::radians(x_rot), glm::normalize(glm::vec3(0, 1, 0)));


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

//newModel.DrawTex(shaderProg);

//Use Shader
glUseProgram(shaderProg);

//newModel.DrawModel();

*/
