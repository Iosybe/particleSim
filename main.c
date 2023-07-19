#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "loadShader.hpp"

// void get_resolution() {
//     const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

//     window_width = mode->width;
//     window_height = mode->height;
// }

typedef struct scaleStruct {
    int previousWidth;
    int previousHeight;

    float scaleX;
    float scaleY;
} ScaleStruct;

ScaleStruct* scaleStruct = (ScaleStruct*) malloc(sizeof(ScaleStruct));

void buildCircle(GLfloat* circle, float radius, int segments) {
    float angleInterval = M_PI * 2 / segments;

    for (int i = 0; i < segments; i++) {
        float currentAngle = angleInterval * i;

        int j = i * 9;

        circle[j] = 0.0f;
        circle[j + 1] = 0.0f;
        circle[j + 2] = 0.0f;

        circle[j + 3] = radius * cos(currentAngle - angleInterval);
        circle[j + 4] = radius * sin(currentAngle - angleInterval);
        circle[j + 5] = 0.0f;

        // kan sneller. remember prev vertex.
        circle[j + 6] = radius * cos(currentAngle);
        circle[j + 7] = radius * sin(currentAngle);
        circle[j + 8] = 0.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// void setAspectRatio(int width, int height) {
//     currentAspectRatio = width / height;
// }

void window_size_callback(GLFWwindow* window, int width, int height) {
    scaleStruct->scaleX = width / scaleStruct->previousWidth;
    scaleStruct->scaleY = height / scaleStruct->previousHeight;

    // scaleStruct->scaleX = scaleStruct->previousWidth / width;
    // scaleStruct->scaleY = scaleStruct->previousHeight / height;

    // setAspectRatio(width, height);
}

void transformCoordinates(GLfloat* circle, int segments) {
    // for (int i = 0; i < segments; i++) {
    //     printf("%i - %f - %f - %f\n", i, circle[i * 3], circle[i * 3 + 1], circle[i * 3 + 2]);
    // }

    for (int i = 0; i < segments * 3; i++) {
        circle[i * 3] = circle[i * 3] / scaleStruct->scaleX;
        circle[i * 3 + 1] = circle[i * 3 + 1] / scaleStruct->scaleY; 

        // printf("%f - %f\n", scaleStruct->scaleX, scaleStruct->scaleY);
    }
}

void drawCircle(GLfloat* templateCircle, int segments, float centerX, float centerY) {
    GLfloat circle[128 * 9];
    float angleInterval = M_PI * 2 / segments;

    for (int i = 0; i < segments; i++) {
        float currentAngle = angleInterval * i;

        int j = i * 9;

        circle[j + 0] = templateCircle[j + 0] + centerX;
        circle[j + 1] = templateCircle[j + 1] + centerY;
        circle[j + 2] = 0.0f;

        circle[j + 3] = templateCircle[j + 3] + centerX;
        circle[j + 4] = templateCircle[j + 4] + centerY;
        circle[j + 5] = 0.0f;

        circle[j + 6] = templateCircle[j + 6] + centerX;
        circle[j + 7] = templateCircle[j + 7] + centerY;
        circle[j + 8] = 0.0f;
    }

    transformCoordinates(circle, segments);

    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, 128 * 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
}


int main() {
    glewExperimental = 1;

    if (!glfwInit()) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    GLFWwindow* window;
    window = glfwCreateWindow( 100, 100, "Tutorial 01", NULL, NULL);
    // setAspectRatio(1400, 1400);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    scaleStruct->previousWidth = width;
    scaleStruct->previousHeight = height;
    scaleStruct->scaleX = 1.0f;
    scaleStruct->scaleY = 1.0f;

    if ( window == NULL ) {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental = 1; // Needed in core profile

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    GLfloat circle[128 * 9];
    buildCircle(circle, 0.2, 128);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // static const GLfloat g_vertex_buffer_data[] = {
    //     -1.0f, -1.0f, 0.0f,
    //     -1.0f, 1.0f, 0.0f,
    //     1.0f,  -1.0f, 0.0f,
    //     -1.0f, 1.0f, 0.0f,
    //     1.0f, 1.0f, 0.0f,
    //     1.0f, -1.0f, 0.0f
    // };

    // GLuint vertexbuffer;
    // // Generate 1 buffer, put the resulting identifier in vertexbuffer
    // glGenBuffers(1, &vertexbuffer);
    // // The following commands will talk about our 'vertexbuffer' buffer
    // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // // Give our vertices to OpenGL.
    // // glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


    GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    float i = 0;

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);

        drawCircle(circle, 128, 0, 0);
        // i += 0.001;

        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // // glVertexAttribPointer(
        // //     0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        // //     3,             // size
        // //     GL_FLOAT,           // type
        // //     GL_FALSE,           // normalized?
        // //     0,                  // stride
        // //     (void*)0            // array buffer offset
        // // );
        // // // Draw the triangle !
        // // glDrawArrays(GL_TRIANGLES, 0, 128 * 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        // glVertexAttribPointer(
        //     0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        //     3,             // size
        //     GL_FLOAT,           // type
        //     GL_FALSE,           // normalized?
        //     0,                  // stride
        //     (void*)0            // array buffer offset
        // );
        // // Draw the triangle !
        // glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
        // glDisableVertexAttribArray(0);

        // GLFWmonitor* monitor = glfwGetPrimaryMonitor();

        // const GLFWvidmode* return_struct = glfwGetVideoMode(monitor);


        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}
