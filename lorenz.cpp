#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
enum class Type
{
    VERTEXSHADER,
    FRAGMENTSHADER
};
std::string loadShader(std::string path)
{
    std::string shaderSource;
    std::ifstream inputFile(path);

    if (!inputFile.is_open())
    {
        std::cout << "no se encontro el archivo\n";
        exit(1);
    }
    std::string line;
    while (std::getline(inputFile, line))
    {
        shaderSource += line + "\n";
    }
    inputFile.close();
    return shaderSource;
}

void getCompilationErrors(GLuint shader)
{
    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cout << log << std::endl;
        exit(1);
    }
}
GLuint createShader(std::string path, Type type)
{
    auto shaderSrc = loadShader(path);
    const char* vShaderSrc = shaderSrc.c_str();
    GLuint shader = 0;
    if (type == Type::VERTEXSHADER)
    {
        shader = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (type == Type::FRAGMENTSHADER)
    {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    }
    glShaderSource(shader, 1, &vShaderSrc, NULL);
    glCompileShader(shader);
    getCompilationErrors(shader);
    return shader;
}
int main()
{
    std::vector<float> points;
    float a = 10;
    float b = 28;
    float c = 8.0f / 3;
    float x = 10;
    float y = 10;
    float z = 10;
    float dt = 0.01f;
    for (int i = 0; i < 5000; i++)
    {
        points.push_back(x);
        points.push_back(y);
        points.push_back(z);
        x += (a * (y - x)) * dt;
        y += (x * (b - z) - y) * dt;
        z += (x * y - c *  z) * dt;
    }
    sf::ContextSettings ctx;
    ctx.depthBits = 0;
    ctx.stencilBits = 0;
    ctx.antialiasingLevel = 0;
    ctx.majorVersion = 4;
    ctx.minorVersion = 0;
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, ctx);
    window.setFramerateLimit(60);
    glewInit();

    auto vertexShader = createShader("C:\\Users\\isaig\\Desktop\\shaders\\vertex_shader.glsl", Type::VERTEXSHADER);
   
    /*
    vertex_shader.glsl
        #version 400 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    */
    auto fragmentShader = createShader("C:\\Users\\isaig\\Desktop\\shaders\\fragment_shader.glsl", Type::FRAGMENTSHADER);
    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GL_FLOAT), points.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -20.0f, -80.0f));
    
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        char title[256];
        snprintf(title, 256, "FPS: %f", 1 / dt);
        window.setTitle(title);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
            else if (event.type == sf::Event::Resized)
            {

                glViewport(0, 0, event.size.width, event.size.height);
            }
            
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glBindVertexArray(VAO);
        model = glm::rotate(model, glm::radians(50.0f * dt), glm::vec3(0.0f, 0.0f, 1.0f));

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUseProgram(shaderProgram);
        glDrawArrays(GL_LINES, 0, points.size());
        window.display();
    }
    return 0;
}
