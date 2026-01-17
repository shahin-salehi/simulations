#include <cmath>
#include <glad/gl.h>        
#include <GLFW/glfw3.h>    
#include <numbers>
#include <vector>

// define
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

std::vector<float> circle_vertecies(int split);


// triangle split
std::vector<float> circle_vertecies(int split){
    //array of points 
    std::vector<float> points;

    // angle split
    double delta = 2*std::numbers::pi/split;

    // fill thetas first
    std::vector<double> thetas(split + 1);
    for(int i=0; i <= split ; i++){
        thetas[i] = i * delta;
    }
    

    // unit circle think
    float radian = 0.20f; 

    for(int i=0; i <= split - 1; i++){

        // 0
        points.push_back(0.0f);
        points.push_back(0.0f);

        // 1
        points.push_back(std::cos(thetas[i]) * radian);
        points.push_back(std::sin(thetas[i]) * radian);

        // 2
        points.push_back(std::cos(thetas[i+1]) * radian);
        points.push_back(std::sin(thetas[i+1]) * radian);
    }
    return points;
}



int main() {

    int width = 800;
    int height = 600;


    glfwInit();

    // set modern opengl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(width, height, "my simulation", nullptr, nullptr);


    
    // when resize happens call my function to keep my guy centered
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);


    // not sure
    gladLoadGL(glfwGetProcAddress);

    // background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glViewport.xhtml
    // window spec for gl i guess to it can do its loading w/e that means
    glViewport(0, 0, width ,height);


    // essentially, all my vertecies should follow this spec
    const char* vertexShaderSrc =
        "#version 330 core\n"
        "layout(location=0) in vec2 pos;\n"
        "void main() {\n"
        "    gl_Position = vec4(pos, 0.0, 1.0);\n"
        "}";

    // color them this way
    const char* fragmentShaderSrc =
        "#version 330 core\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}";

    // compile
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fs);

    // link
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Shaders are no longer needed once linked
    glDeleteShader(vs);
    glDeleteShader(fs);

    // my actual pretty vertecies that will follow the spec above
    float vertices[] = {
        -0.25f, -0.25f,   // bottom-left
         0.25f, -0.25f,   // bottom-right
         0.25f,  0.25f,   // top-right

        -0.25f, -0.25f,   // bottom-left
         0.25f,  0.25f,   // top-right
        -0.25f,  0.25f    // top-left
    };

    // my circle
    std::vector<float> my_vs = circle_vertecies(20);

    // no idea some storage
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);  // Describes vertex layout
    glGenBuffers(1, &vbo);       // Stores vertex data

    // upload my vertecies to the gpu draw statically (immediate)
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, my_vs.size() * sizeof(float), my_vs.data(), GL_STATIC_DRAW);

    // not sure
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // --- 15. Main loop (time-stepped system)
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Use our shader program
        glUseProgram(program);

        // Bind geometry description
        glBindVertexArray(vao);

        // draw triangles with my defined six vertecies above
        glDrawArrays(GL_TRIANGLES, 0, my_vs.size()/2);

        // Present the frame and process OS events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- 16. Shutdown
    glfwTerminate();
}

// keep my little guy centered
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
