#include <chrono>
#include <cmath>
#include <glad/gl.h>        
#include <GLFW/glfw3.h>    
#include <numbers>
#include <vector>


// define


// made up 
const float gravity = -0.25f;

// floors
const float floorY = -1.0f;

// position
struct pos {
    float x;
    float y;
};

// velocity
struct velocity {
    float x;
    float y;
};

// body 
struct body {
    pos p;
    velocity v;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void circleVertecies(std::vector<float>& points, const pos& center, int split);
float applyGravity(float currentVelocity, float dt);
// make this more serious later
void updateBodies(body& b, float dt);


// triangle split  // change to in place 
void circleVertecies(std::vector<float>& points, const pos& center, int split){
    // three vertecies (x,y) 3 * 2
    const int pointsPerTriangle = 6; 
    // could get a awful error if not
    points.resize(pointsPerTriangle * split); 

    // angle split
    double delta = 2*std::numbers::pi/split;

    // change this later for fun implications on speed/velocity 
    // can we add angular speed?
    float radian = 0.20f; 

    for(int i=0; i < split; ++i){
        // offset
        int base = pointsPerTriangle * i; 

        // 0
        points[base + 0] = center.x;
        points[base + 1] = center.y;

        // 1
        points[base + 2] = (center.x + std::cos(i * delta) * radian);
        points[base + 3] = (center.y + std::sin(i * delta) * radian);

        // 2
        points[base + 4] = (center.x + std::cos((i+1) * delta) * radian);
        points[base + 5] = (center.y + std::sin((i+1) * delta) * radian);

    }
}
// linear function for each step add g
float applyGravity(float currentVelocity, float dt){
    return currentVelocity + gravity * dt;
}

// update positions given velocity in x,y
// only vertical vel for now
void updateBodies(body& b, float dt){
    b.p.y = b.p.y + b.v.y * dt;

    if (b.p.y < floorY){
        b.p.y = floorY;
        b.v.y = 0;
    }
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


    // set clock
    auto startTime = std::chrono::steady_clock::now();
    auto lastFrame = startTime;

    // my circle
    pos initPosition = {0.0f, 0.0f};
    velocity v0 = {0.0f, 0.0f};
    body circle = {
        initPosition,
        v0
    };
    std::vector<float> my_vs;
    circleVertecies(my_vs, circle.p, 20);

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

    while (!glfwWindowShouldClose(window)) {

        // time logic
        auto now = std::chrono::steady_clock::now();
        
        auto elapsedTime = std::chrono::duration<float>(now - startTime).count();
        auto deltaTime = std::chrono::duration<float>(now - lastFrame).count();
        lastFrame = now;

        // calculate gravity on our circle 
        circle.v.y = applyGravity(circle.v.y, deltaTime);
        updateBodies(circle, deltaTime);

        // get new vertecies
        circleVertecies(my_vs, circle.p, 20);

        // expensive update 
        // upload my vertecies to the gpu draw 
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, my_vs.size() * sizeof(float), my_vs.data(), GL_DYNAMIC_DRAW);


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
