#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define SIZE 500

struct VertexInfo
{
    sf::Vector2f pos;
    sf::Vector2f realPos;
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
        exit(1);
    }
}
void genMandelbrot(std::vector<VertexInfo> &points, int npoints, float xmin, float xmax, float ymin, float ymax)
{
    float deltay = (ymax - ymin) / npoints;
    float deltax = (xmax - xmin) / npoints;
    float delta = 2.0f / npoints;
    float realY = -1;
    for (float y = ymin; y <= ymax; y += deltay)
    {
        float realX = -1;
        for (float x = xmin; x <= xmax; x += deltax)
        {
            VertexInfo vert;
            vert.pos = {x, y};
            vert.realPos = {realX, realY};
            vert.color = {0.0f, 0.0f, 0.0f};
            points.push_back(vert);

            realX += delta;
        }
        realY += delta;
    }
}
int main()
{
    float xmin = -2;
    float xmax = 2.0f;
    float ymin = -2.0f;
    float ymax = 2;
    sf::ContextSettings ctx;
    ctx.depthBits = 0;
    ctx.stencilBits = 0;
    ctx.antialiasingLevel = 0;
    ctx.majorVersion = 4;
    ctx.minorVersion = 2;
    sf::Window window(sf::VideoMode(SIZE, SIZE), "OpenGL", sf::Style::Default, ctx);
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

    std::vector<VertexInfo> points;
    genMandelbrot(points, SIZE, xmin, xmax, ymin, ymax);
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 7 * points.size() * sizeof(GL_FLOAT), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void *)offsetof(VertexInfo, pos)); // offsetof obtiene a partir de que byte dentro del struct VertexInfo la variable que se le pasa como parametro
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void *)offsetof(VertexInfo, realPos)); // debido al struct padding que es confuso como se hace, mejor no calculamos este valor a mano xd
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void *)offsetof(VertexInfo, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float zoomfactor = 4.0f;
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
            else if (event.type == sf::Event::KeyPressed)
            {

                float xlen = xmax - xmin;
                float ylen = ymax - ymin;
                bool makeZoom = false;
                if (event.key.code == sf::Keyboard::W)
                {
                    xmin += xlen / zoomfactor;
                    xmax -= xlen / zoomfactor;
                    ymin += ylen / zoomfactor;
                    ymax -= ylen / zoomfactor;
                    zoomfactor += 0.1f;
                    makeZoom = true;
                }
                else if (event.key.code == sf::Keyboard::S)
                {
                    xmin -= xlen / zoomfactor;
                    xmax += xlen / zoomfactor;
                    ymin -= ylen / zoomfactor;
                    ymax += ylen / zoomfactor;
                    zoomfactor -= 0.1f;
                    makeZoom = true;
                }
                else if (event.key.code == sf::Keyboard::D)
                {

                    xmin += xlen / 10;
                    xmax += xlen / 10;
                    makeZoom = true;
                }
                else if (event.key.code == sf::Keyboard::A)
                {
                    xmin -= xlen / 10;
                    xmax -= xlen / 10;
                    makeZoom = true;
                }
                if (makeZoom)

                {
                    printf("X -> [%f %f] Y -> [%f %f] \n", xmin, xmax, ymin, ymax);
                    points.erase(points.begin(), points.end());
                    genMandelbrot(points, SIZE, xmin, xmax, ymin, ymax);
                    glBindVertexArray(VAO);
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferData(GL_ARRAY_BUFFER, 7 * points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glDrawArrays(GL_POINTS, 0, points.size());

        window.display();
    }

    return 0;
}
