#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bits/stdc++.h>

#include "./header/Object.h"
#include "./header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int createShader(const string &filename, const string &type);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
unsigned int modelVAO(Object &model);
unsigned int loadTexture(const char *tFileName);
void drawModel(const string &modelName, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection, int r, int g, int b);
void init();

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// Shader
unsigned int vertexShader, fragmentShader, shaderProgram;

// Texture
unsigned int airplaneTexture, earthTexture;

// VAO, VBO
unsigned int airplaneVAO, earthVAO, cubeVAO;

// Objects to display
Object *airplaneObject, *earthObject, *cubeObject;

// Constants you may need
const float airplaneHeight = 27.0f;
const int rotateEarthSpeed = 30;
const int rotateAirplaneSpeed = 90;
const int squeezeSpeed = 90;
const int rainbowSpeed = 72;

// You can use these parameters
float rotateEarthDegree = 0;
float rotateAirplaneDegree = 0;
float rotateAxisDegree = 180;
bool useSqueeze = false;
float squeezeFactor = 0;
bool useRainbowColor = false;
glm::vec3 rainbowColor = glm::vec3(1.0f, 1.0f, 1.0f);

// Bonus
bool useHelicopter = false;
float rainbowDegree = 0;
float rotateConnectorDegree = 0.0f;
const int rotateConnectorSpeed = 5;

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 - 111550093", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize Object, Shader, Texture, VAO, VBO
    init();

    // Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // render loop variables
    double dt;
    double lastTime = glfwGetTime();
    double currentTime;

    // TODO#5: Data connection - Retrieve uniform variable locations
    // move to drawModel()

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // render
        glClearColor(0 / 255.0, 0 / 255.0, 0 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 50.0f, 90.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        glm::mat4 base(1.0f), airplaneModel(1.0f), earthModel(1.0f), helicopterModel(1.0f);

        // TODO#6-1: Render Airplane
        // First rotate around Y axis for orbit path (controlled by A/D keys)
        airplaneModel = glm::rotate(airplaneModel, glm::radians(rotateAxisDegree), glm::vec3(0.0f, 1.0f, 0.0f));

        // Translate to orbit position (radius 27)        
        float orbitY = 27.0f * sin(glm::radians(rotateAirplaneDegree));
        float orbitZ = 27.0f * cos(glm::radians(rotateAirplaneDegree));
        airplaneModel = glm::translate(airplaneModel, glm::vec3(0.0f, orbitY, orbitZ));

        // Rotate around X axis for airplane rotation (controlled by Q/E keys)
        airplaneModel = glm::rotate(airplaneModel, glm::radians(rotateAirplaneDegree - 90), glm::vec3(-1.0f, 0.0f, 0.0f));


        // Update rainbow color if in rainbow mode
        if (useRainbowColor) {
            float hue = fmod(glfwGetTime() * 72.0f, 360.0f);  // 72 degrees per second
            float saturation = 1.0f;
            float value = 1.0f;
            
            // Convert HSV to RGB
            float c = value * saturation;
            float x = c * (1 - abs(fmod(hue / 60.0f, 2.0f) - 1));
            float m = value - c;
            
            glm::vec3 rgb;
            if (hue < 60) rgb = glm::vec3(c, x, 0);
            else if (hue < 120) rgb = glm::vec3(x, c, 0);
            else if (hue < 180) rgb = glm::vec3(0, c, x);
            else if (hue < 240) rgb = glm::vec3(0, x, c);
            else if (hue < 300) rgb = glm::vec3(x, 0, c);
            else rgb = glm::vec3(c, 0, x);
            
            rainbowColor = rgb + glm::vec3(m);
        }

        // TODO#6-2: Render Earth
        // Set up earth model matrix
        earthModel = glm::scale(earthModel, glm::vec3(10.0f));  // Scale 10x
        earthModel = glm::rotate(earthModel, glm::radians(rotateEarthDegree), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around Y axis

        // Draw the earth
        drawModel("earth", earthModel, view, projection, 1.0f, 1.0f, 1.0f);

        // TODO#Bonus: Render Helicopter
        //* Body
        helicopterModel = glm::rotate(helicopterModel, glm::radians(rotateAxisDegree), glm::vec3(0.0f, 1.0f, 0.0f));
        helicopterModel = glm::translate(helicopterModel, glm::vec3(0.0f, orbitY, orbitZ));
        helicopterModel = glm::rotate(helicopterModel, glm::radians(rotateAirplaneDegree - 90), glm::vec3(-1.0f, 0.0f, 0.0f));
        helicopterModel = glm::rotate(helicopterModel, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        glm::vec3 block1_pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 block2_pos = block1_pos + glm::vec3(8.0f, 0.0f, 0.0f);
        glm::vec3 block3_pos = block2_pos + glm::vec3(4.5f, 0.0f, 0.0f);
        glm::mat4 block1 = glm::translate(helicopterModel, block1_pos);
        block1 = glm::scale(block1, glm::vec3(10.0f, 8.0f, 10.0f));
        glm::mat4 block2 = glm::translate(helicopterModel, block2_pos);
        block2 = glm::scale(block2, glm::vec3(6.0f, 7.5f, 8.0f));
        glm::mat4 block3 = glm::translate(helicopterModel, block3_pos);
        block3 = glm::scale(block3, glm::vec3(3.0f, 6.0f, 5.0f));
        
        // * Connector
        glm::vec3 connector_pos = block1_pos + glm::vec3(0.0f, 5.5f, 0.0f);
        glm::mat4 connector = glm::translate(helicopterModel, connector_pos);
        connector = glm::rotate(connector, glm::radians(rotateConnectorDegree), glm::vec3(0.0f, 1.0f, 0.0f));
        connector = glm::scale(connector, glm::vec3(3.0f, 3.0f, 3.0f));

        // * Rotor Blades
        float bladeLength = 4.0f; // Adjust this value to control the length of the blades
        float bladeHeight = 0.1f;
        float bladeWidth = 0.5f;  // Adjust this value to control the width of the blades
        vector<glm::vec3> blade_colors = {glm::vec3(250, 247, 240), glm::vec3(216, 210, 194), glm::vec3(177, 116, 87), glm::vec3(74, 73, 71)};
        vector<glm::mat4> blades;
        for (int i = 0; i < 4; ++i) {
            glm::mat4 blade = glm::rotate(connector, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
            blade = glm::translate(blade, glm::vec3(bladeLength / 2.0f, 0.0f, 0.0f)); // Translate to position
            blade = glm::scale(blade, glm::vec3(bladeLength, bladeHeight, bladeWidth)); // Scale the blade
            blades.push_back(blade);
        }

        if (useHelicopter) {
            drawModel("cube", block1, view, projection, 255, 241, 0);
            drawModel("cube", block2, view, projection, 0, 0, 0);
            drawModel("cube", block3, view, projection, 177, 116, 87);
            drawModel("cube", connector, view, projection, 255, 255, 255);
            drawModel("cube", connector, view, projection, 255, 255, 255);
            for (int i = 0; i < 4; ++i) {
                drawModel("cube", blades[i], view, projection, blade_colors[i][0], blade_colors[i][1], blade_colors[i][2]);
            }
        }
        else {
            // Draw the airplane
            drawModel("airplane", airplaneModel, view, projection, 1.0f, 1.0f, 1.0f);
        }


        // Status update
        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        /* TODO#7: Update "rotateEarthDegree", "rotateAirplaneDegree", "rotateAxisDegree",
         *          "squeezeFactor", "rainbowColor"
         */
        
        // Update rotation angles based on elapsed time (dt is in seconds)
        rotateEarthDegree = fmod(rotateEarthDegree + rotateEarthSpeed * dt, 360.0f);  // 30 degrees/sec
        rotateAirplaneDegree = fmod(rotateAirplaneDegree + rotateAirplaneSpeed * dt, 360.0f);  // 90 degrees/sec
        rotateConnectorDegree = fmod(rotateConnectorDegree + rotateConnectorSpeed, 360.0f);  // 90 degrees/sec
        
        // Update squeeze factor if squeezing is enabled
        if (useSqueeze) {
            squeezeFactor = fmod(squeezeFactor + 90 * dt, 360.0f);  // 90 degrees/sec
        }

        // Update rainbow color if rainbow mode is enabled
        if (useRainbowColor) {
            rainbowDegree = fmod(rainbowDegree + rainbowSpeed * dt, 360.0f);  // 72 degrees/sec
            
            // Convert HSV to RGB
            float hue = rainbowDegree;
            float saturation = 1.0f;
            float value = 1.0f;
            
            float c = value * saturation;
            float x = c * (1 - abs(fmod(hue / 60.0f, 2.0f) - 1));
            float m = value - c;
            
            if (hue < 60) rainbowColor = glm::vec3(c, x, 0);
            else if (hue < 120) rainbowColor = glm::vec3(x, c, 0);
            else if (hue < 180) rainbowColor = glm::vec3(0, c, x);
            else if (hue < 240) rainbowColor = glm::vec3(0, x, c);
            else if (hue < 300) rainbowColor = glm::vec3(x, 0, c);
            else rainbowColor = glm::vec3(c, 0, x);
            
            rainbowColor += glm::vec3(m);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// TODO#1: createShader
unsigned int createShader(const string &filename, const string &type) {
    // Create shader object based on type
    unsigned int shader;
    if (type == "vert")
        shader = glCreateShader(GL_VERTEX_SHADER);
    else if (type == "frag") 
        shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shader source from file
    string shaderStr;
    ifstream shaderFile(filename);
    stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    shaderStr = shaderStream.str();
    const char* shaderSrc = shaderStr.c_str();

    // Compile shader
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    // Check for compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << endl;
    }

    return shader;
}

// TODO#2: createProgram
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    // Create program object
    GLuint program = glCreateProgram();

    // Attach shaders
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link program
    glLinkProgram(program);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    // Detach shaders after linking
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    return program;
}

// TODO#3: modelVAO
unsigned int modelVAO(Object &model) {
    // Generate and bind VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBOs
    GLuint VBO[3]; // One VBO for positions, normals, and texcoords
    glGenBuffers(3, VBO);

    // Bind and set VBO for positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(float), model.positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Position attribute
    glEnableVertexAttribArray(0);

    // Bind and set VBO for normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(float), model.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Normal attribute
    glEnableVertexAttribArray(1);

    // Bind and set VBO for texcoords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, model.texcoords.size() * sizeof(float), model.texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Texture coordinate attribute
    glEnableVertexAttribArray(2);

    return VAO;
}

// TODO#4: loadTexture
unsigned int loadTexture(const string &filename) {
    // Enable texture 2D
    glEnable(GL_TEXTURE_2D);

    // Generate texture ID
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // Load image data using stb_image
    GLint width, height, nrChannels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    
    if (data) {
        // Flip the image vertically
        for (int i = 0; i < height / 2; i++) {
            for (int j = 0; j < width * nrChannels; j++) {
                std::swap(data[i * width * nrChannels + j], data[(height - i - 1) * width * nrChannels + j]);
            }
        }

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Generate texture and mipmap
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Free image data
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << filename << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

// TODO#8: Key callback
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Only handle GLFW_PRESS and GLFW_REPEAT actions
    if (action != GLFW_PRESS && action != GLFW_REPEAT) 
        return;

    switch (key) {
        case GLFW_KEY_D:
            // Increase rotation axis by 1 degree
            rotateAxisDegree += 1.0f;
            break;

        case GLFW_KEY_A:
            // Decrease rotation axis by 1 degree
            rotateAxisDegree -= 1.0f;
            break;

        case GLFW_KEY_S:
            // Toggle squeeze effect
            useSqueeze = !useSqueeze;
            break;

        case GLFW_KEY_H:
            // Toggle helicopter effect
            useHelicopter = !useHelicopter;
            break;

        case GLFW_KEY_R:
            // Toggle rainbow color mode
            useRainbowColor = !useRainbowColor;
            if (!useRainbowColor) {
                // Reset rainbow color when turning off
                rainbowColor = glm::vec3(1.0f);
                rainbowDegree = 0.0f;
            }
            break;

        case GLFW_KEY_ESCAPE:
            // Close the window when ESC is pressed
            glfwSetWindowShouldClose(window, true);
            break;
    }
}

// Bonus: Helicopter Helper Function: drawModel
void drawModel(const string &modelName, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection, int r, int g, int b) {
    // Vertex Shader: model, view, projection, squeezeFactor
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint squeezeFactorLoc = glGetUniformLocation(shaderProgram, "squeezeFactor");

    // Fragment Shader: ourTexture, rainbowColor, useRainbowColor
    GLuint ourTextureLoc = glGetUniformLocation(shaderProgram, "ourTexture");
    GLuint rainbowColorLoc = glGetUniformLocation(shaderProgram, "rainbowColor");
    GLuint useRainbowColorLoc = glGetUniformLocation(shaderProgram, "useRainbowColor");
    GLuint cubeColorLoc = glGetUniformLocation(shaderProgram, "cubeColor");
    GLuint useHelicopterLoc = glGetUniformLocation(shaderProgram, "useHelicopter");

    // Send transformation matrices to vertex shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    if (modelName == "earth") {
        glUniform1f(squeezeFactorLoc, glm::radians(squeezeFactor));
    }
    else {
        glUniform1f(squeezeFactorLoc, 0.0f);
    }

    // Send uniforms to fragment shader
    if (modelName == "airplane") {
        glUniform1i(ourTextureLoc, 0);
        glUniform3fv(rainbowColorLoc, 1, glm::value_ptr(rainbowColor));
        glUniform1i(useRainbowColorLoc, useRainbowColor);
        glUniform3fv(cubeColorLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // No color for airplane
        glUniform1i(useHelicopterLoc, useHelicopter);
    }
    else if (modelName == "earth") {
        glUniform1i(ourTextureLoc, 0);
        glUniform3fv(rainbowColorLoc, 1, glm::value_ptr(glm::vec3(1.0f)));  // No rainbow effect for earth
        glUniform1i(useRainbowColorLoc, false);  // Disable rainbow effect
        glUniform3fv(cubeColorLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glUniform1i(useHelicopterLoc, false); // Disable helicopter effect
    }
    else if (modelName == "cube") {
        glUniform1i(ourTextureLoc, 0);
        glUniform3fv(rainbowColorLoc, 1, glm::value_ptr(glm::vec3(1.0f)));  // No rainbow effect for cube
        glUniform1i(useRainbowColorLoc, false);  // Disable rainbow effect
        glUniform3fv(cubeColorLoc, 1, glm::value_ptr(glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f)));
        glUniform1i(useHelicopterLoc, useHelicopter);
    }

    // Bind texture and VAO and draw
    if (modelName == "airplane") {
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glBindTexture(GL_TEXTURE_2D, airplaneTexture); // Bind the texture to unit 0
        glBindVertexArray(airplaneVAO);
        glDrawArrays(GL_TRIANGLES, 0, airplaneObject->positions.size() / 3);
    }
    else if (modelName == "earth") {
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glBindTexture(GL_TEXTURE_2D, earthTexture); // Bind the texture to unit 0
        glBindVertexArray(earthVAO);
        glDrawArrays(GL_TRIANGLES, 0, earthObject->positions.size() / 3);
    }
    else if (modelName == "cube") {
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, cubeObject->positions.size() / 3);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    string dirShader = "../../src/shaders/";
    string dirAsset = "../../src/asset/obj/";
    string dirTexture = "../../src/asset/texture/";
#else
    string dirShader = "..\\..\\src\\shaders\\";
    string dirAsset = "..\\..\\src\\asset\\obj\\";
    string dirTexture = "..\\..\\src\\asset\\texture\\";
#endif

    // Object
    airplaneObject = new Object(dirAsset + "airplane.obj");
    earthObject = new Object(dirAsset + "earth.obj");
    cubeObject = new Object(dirAsset + "cube.obj");

    // Shader
    vertexShader = createShader(dirShader + "vertexShader.vert", "vert");
    fragmentShader = createShader(dirShader + "fragmentShader.frag", "frag");
    shaderProgram = createProgram(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    // Texture
    airplaneTexture = loadTexture(dirTexture + "airplane.jpg");
    earthTexture = loadTexture(dirTexture + "earth.jpg");

    // VAO, VBO
    airplaneVAO = modelVAO(*airplaneObject);
    earthVAO = modelVAO(*earthObject);
    cubeVAO = modelVAO(*cubeObject);
}
