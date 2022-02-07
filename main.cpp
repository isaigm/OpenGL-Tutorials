#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>

struct VertexInfo
{
    sf::Vector3f coords;
    sf::Vector3f color;
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
    }
}
void genCircle(std::vector<VertexInfo> &points)
{
    float r = 0.5f;
    for (float angle = 0; angle <= 2 * M_PI; angle += 0.5f)
    {

        float x = r * cos(angle);
        float y = r * sin(angle);
        VertexInfo vert;
        vert.coords = {x, y, 0.0f};
        vert.color = {rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f};

        points.push_back(vert);
    }
}

int main()
{
    srand(time(nullptr));
    sf::ContextSettings ctx;
    ctx.depthBits = 0;
    ctx.stencilBits = 0;
    ctx.antialiasingLevel = 0;
    ctx.majorVersion = 4;
    ctx.minorVersion = 2;

    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, ctx);
    window.setVerticalSyncEnabled(true);
    glewInit();

    auto vertexShaderSource = loadShader("vertex_shader.glsl");
    const char *vShaderSrc = vertexShaderSource.c_str();
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderSrc, NULL);
    glCompileShader(vertexShader);
    getCompilationErrors(vertexShader);

    auto fragmentShaderSource = loadShader("fragment_shader.glsl");
    const char *fShaderSrc = fragmentShaderSource.c_str();
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderSrc, NULL);
    glCompileShader(fragmentShader);
    getCompilationErrors(fragmentShader);

    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f, // izquierda
        0.5f, -0.5f, 0.0f,  // derecha
        0.0f, 0.5f, 0.0f    // arriba
    };
    std::vector<VertexInfo> points;

    genCircle(points);
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)offsetof(VertexInfo, coords)); //offsetof obtiene a partir de que byte dentro del struct VertexInfo la variable que se le pasa como parametro
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)offsetof(VertexInfo, color)); //debido al struct padding que es confuso como se hace, mejor no calculamos este valor a mano xd
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, points.size());

        window.display();
    }

    return 0;
}