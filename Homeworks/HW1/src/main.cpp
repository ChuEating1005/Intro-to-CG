#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <bits/stdc++.h>

#include "./header/Shader.h"
#include "./header/Object.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void drawModel(const string& name, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int r, int g, int b);
void init();

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
float helicopterX = 0.0f, helicopterY = 10.0f; // Initial helicopter position

// Shader
Shader *shader;

// Objects to display
Object *cube, *cylinder, *I, *C, *G, *tree_up, *tree_btn;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ICG_2024_HW1", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);
    
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // TODO: Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

    // Display loop
	glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Initialize Object and Shader
    init();

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // render
        glClearColor(153/255.0, 204/255.0, 255/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader
        shader->use();
        
        
        /*=================== Example of creating model matrix ======================= 
        1. translate
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.0f));
        drawModel("Cube",model,view,projection,255, 0, 0);
        
        2. scale
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 2.0f)); 
        drawModel("Cube",model,view,projection,255, 0, 0);
        
        3. rotate
        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        drawModel("Cube",model,view,projection,255, 0, 0);
        ==============================================================================*/

        // TODO: Create model, view, and perspective matrix
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 50.0f, 90.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);


        // TODO: Helicopter (Body + Connector + Rotor Blades)
        // Body (At least 3 different color cubes)
        // Connector (One cube)
        //     - On the body
        //     - Rotate: 5 degrees/frame about +y axis
        // Rotor Blades (4 different color cubes)
        //     - On the connector
        //     - Each cubes is offset by 90 degrees
        // You can be creative in design !!
        // static float helicopterX = 0.0f, helicopterY = 10.0f; // Initial helicopter position
        
        // * Body
        glm::vec3 block1_pos = glm::vec3(helicopterX, helicopterY, 0.0f);
        glm::vec3 block2_pos = block1_pos + glm::vec3(8.0f, 0.0f, 0.0f);
        glm::vec3 block3_pos = block2_pos + glm::vec3(4.5f, 0.0f, 0.0f);
        glm::mat4 block1 = glm::translate(model, block1_pos);
        block1 = glm::scale(block1, glm::vec3(10.0f, 8.0f, 15.0f));
        drawModel("Cube", block1, view, projection, 255, 241, 0);
        glm::mat4 block2 = glm::translate(model, block2_pos);
        block2 = glm::scale(block2, glm::vec3(6.0f, 7.5f, 14.0f));
        drawModel("Cube", block2, view, projection, 0, 0, 0);
        glm::mat4 block3 = glm::translate(model, block3_pos);
        block3 = glm::scale(block3, glm::vec3(3.0f, 6.0f, 10.0f));
        drawModel("Cube", block3, view, projection, 240, 230, 100);
        
        // * Connector
        static float connectorRotation = 0.0f;
        glm::vec3 connector_pos = block1_pos + glm::vec3(0.0f, 5.5f, 0.0f);
        glm::mat4 connector = glm::translate(model, connector_pos);
        connector = glm::rotate(connector, glm::radians(connectorRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        connector = glm::scale(connector, glm::vec3(3.0f, 3.0f, 3.0f));
        drawModel("Cube", connector, view, projection, 255, 255, 255);

        // * Rotor Blades
        float bladeLength = 4.0f; // Adjust this value to control the length of the blades
        float bladeHeight = 0.1f;
        float bladeWidth = 0.5f;  // Adjust this value to control the width of the blades

        vector<glm::vec3> blade_colors = {glm::vec3(250, 247, 240), glm::vec3(216, 210, 194), glm::vec3(177, 116, 87), glm::vec3(74, 73, 71)};
        for (int i = 0; i < 4; ++i) {
            glm::mat4 blade = glm::rotate(connector, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
            blade = glm::translate(blade, glm::vec3(bladeLength / 2.0f, 0.0f, 0.0f)); // Translate to position
            blade = glm::scale(blade, glm::vec3(bladeLength, bladeHeight, bladeWidth)); // Scale the blade
            drawModel("Cube", blade, view, projection, blade_colors[i].r, blade_colors[i].g, blade_colors[i].b);
        }


        // TODO: Draw Ground (Scale: (100, 1, 120))
        glm::vec3 ground_pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::mat4 ground = glm::translate(model, ground_pos);
        ground = glm::scale(ground, glm::vec3(100.0f, 1.0f, 120.0f));
        drawModel("Cube", ground, view, projection, 34, 139, 34);

        // TODO: Draw Tree1 (Position: (-30, 0, -20) relative to the Ground, Scale: (4, 4, 4))
        glm::mat4 tree1 = glm::translate(model, ground_pos + glm::vec3(-30.0f, 0.0f, -20.0f));
        tree1 = glm::scale(tree1, glm::vec3(4.0f, 4.0f, 4.0f));
        drawModel("Tree_down", tree1, view, projection, 139, 69, 19);
        drawModel("Tree_up", tree1, view, projection, 0, 100, 0);

        // TODO: Draw Tree2 (Position: (30, 0, 20) relative to the Ground, Scale: (3.5, 3.5, 3.5))
        glm::mat4 tree2 = glm::translate(model, ground_pos + glm::vec3(30.0f, 0.0f, 20.0f));
        tree2 = glm::scale(tree2, glm::vec3(3.5f, 3.5f, 3.5f));
        drawModel("Tree_down", tree2, view, projection, 139, 69, 19);
        drawModel("Tree_up", tree2, view, projection, 0, 100, 0);

        // TODO: Draw C
        glm::vec3 C_pos = glm::vec3(-25.0f, 2.0f, 25.0f);
        glm::mat4 C = glm::translate(model, C_pos);
        drawModel("C", C, view, projection, 255, 0, 0);

        // TODO: Draw I (Rotate: 0.8 degrees/ frame about +y axis around the C)
        static float iRotation = 0.0f;
        glm::mat4 I = glm::translate(model, C_pos);
        I = glm::rotate(I, glm::radians(iRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        I = glm::translate(I, glm::vec3(-12.0f, 0.0f, 0.0f));
        drawModel("I", I, view, projection, 0, 255, 0);

        // TODO: Draw G (Rotate: 0.8 degrees/ frame about +y axis around the C)
        static float gRotation = 0.0f;
        glm::mat4 G = glm::translate(model, C_pos);
        G = glm::rotate(G, glm::radians(gRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        G = glm::translate(G, glm::vec3(12.0f, 0.0f, 0.0f));
        drawModel("G", G, view, projection, 0, 0, 255);

        // TODO: Control rotation
        connectorRotation += 5.0f;
        iRotation += 0.8f;
        gRotation += 0.8f;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// TODO:
// Add key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
        
    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE. 
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html

    // 1. Press W to move the helicopter in the Y direction by 1
    // 2. Press S to move the helicopter in the Y direction by -1 
    // 3. Press D to move the helicopter in the X direction by 1
    // 4. Press A to move the helicopter in the X direction by -1
    // 5. The helicopter cannot fly below the ground.
    // ** Both GLFW_PRESS and GLFW_REPEAT are OK! You can try it out and compare the differences. **

   

    if (action == GLFW_REPEAT) {
        if (key == GLFW_KEY_W) helicopterY += 1.0f;
        if (key == GLFW_KEY_S && helicopterY > 1.0f) helicopterY -= 1.0f; // Prevent flying below the ground
        if (key == GLFW_KEY_D) helicopterX += 1.0f;
        if (key == GLFW_KEY_A) helicopterX -= 1.0f;
        cout << action;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void drawModel(const string& name, 
               const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, 
               int r, int g, int b){

    shader->set_uniform("projection",projection);
    shader->set_uniform("view",view);
    shader->set_uniform("model",model);
    shader->set_uniform("objectColor", glm::vec3(r/255.0,g/255.0,b/255.0));
    
    if(name == "Cube"){
        cube->draw();
    }else if(name == "Cylinder"){
        cylinder->draw();
    }else if(name == "I"){
        I->draw();
    }else if(name == "C"){
        C->draw();
    }else if(name == "G"){
        G->draw();
    }else if(name == "Tree_down"){
        tree_btn->draw();
    }else if(name == "Tree_up"){
        tree_up->draw();
    }
}

void init(){
    #if defined(__linux__) || defined(__APPLE__)
        string dirShader = "../../src/shaders/";
        string dirAsset = "../../src/asset/";
    #else
        string dirShader = "..\\..\\src\\shaders\\";
        string dirAsset = "..\\..\\src\\asset\\";
    #endif

    string v = dirShader + "easy.vert";
    string f = dirShader + "easy.frag";
    shader = new Shader(v.c_str(),f.c_str());
    cube = new Object(dirAsset + "cube.obj");
    cylinder = new Object(dirAsset + "cylinder.obj");
    I = new Object(dirAsset + "I.obj");
    C = new Object(dirAsset + "C.obj");
    G = new Object(dirAsset + "G.obj");
    tree_up = new Object(dirAsset + "tree_up.obj");
    tree_btn = new Object(dirAsset + "tree_btn.obj");
}