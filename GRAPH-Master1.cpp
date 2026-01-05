#include "GRAPH-Master1.h"
using namespace std;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Nikko Ragudo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Render an nth sided polygon */
        glColor3f(1.0f, 1.0f, 1.0f); //Set Color to white | RGB

        float x = 0.0f;
        float y = 0.0f;
        float radius = 0.5f;
        int num_segments = 9;

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);

        for (int i = 0; i <= num_segments; ++i) {
            float angle = i * 2.0f * PI / num_segments;
            glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
        }

        glEnd();
        glFlush();

        /* Render Triangle*/
        /*
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();
        */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}