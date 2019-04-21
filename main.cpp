#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#include <Shader.hpp>

static bool loadOBJ(
        const char * path,
        std::vector<glm::vec3> & out_vertices,
        std::vector<glm::vec2> & out_uvs,
        std::vector<glm::vec3> & out_normals
){
    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;


    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        getchar();
        return false;
    }

    while( 1 ){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                fclose(file);
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        glm::vec2 uv = temp_uvs[ uvIndex-1 ];
        glm::vec3 normal = temp_normals[ normalIndex-1 ];

        // Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs     .push_back(uv);
        out_normals .push_back(normal);

    }
    fclose(file);
    return true;
}

static GLint getUniform(GLint prog, const char name[])
{
    auto res = glGetUniformLocation(prog, name);
    //printf("%s : %d\n", name, res);
    return res;
}

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Tutorial01", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewInit();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;

    loadOBJ("data/suz.obj", vertices, textureCoords, normals);

    printf("%d\n", vertices.size());
    printf("%d\n", normals.size());

    GLuint vboHandle[2];
    glGenBuffers(2, vboHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

    GLuint vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    auto programHandle = glCreateProgram();

    glAttachShader(programHandle, Shader::loadShaderFromFile("data/basic.vsh", GL_VERTEX_SHADER).getHandle());
    glAttachShader(programHandle, Shader::loadShaderFromFile("data/basic.fsh", GL_FRAGMENT_SHADER).getHandle());

    glLinkProgram(programHandle);
    int status = 0;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
        // Store log and return false
        int length = 0;
        std::string logString;

        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            char *c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(programHandle, length, &written, c_log);
            logString = c_log;
            delete[] c_log;
        }

        printf("%s\n", (std::string("Program link failed:\n") + logString).c_str());
    }

    glUseProgram(programHandle);

    glm::mat4 projection = glm::perspective(
            glm::radians(75.0f), 800.f / 600.f, 1.0f, 1000.0f);

    glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );

    float alpha = 0;
    double lastTimeStamp = 0;

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto timeStamp = glfwGetTime();
        auto dt = timeStamp - lastTimeStamp;
        lastTimeStamp = timeStamp;

        alpha += dt * 50.0f;
        if (alpha >= 360.0f) {
            alpha -= 360.0f;
        }

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(alpha), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 mv = view * model;
        glm::mat4 mvp = projection * mv;

        glUniformMatrix4fv(getUniform(programHandle, "ModelViewProjection"),
                           1, GL_FALSE, glm::value_ptr(mvp));

        glUniformMatrix4fv(getUniform(programHandle, "ModelViewMatrix"),
                           1, GL_FALSE, glm::value_ptr(mv));

        glm::vec4 lp = view * glm::vec4(5.0f, 5.0f, 4.0f, 1.0f);
        glUniform4f(getUniform(programHandle, "LightPosition"), lp.x, lp.y, lp.z, lp.w);
        glUniform3f(getUniform(programHandle, "Kd"), 0.9f, 0.5f, 0.3f);
        glUniform3f(getUniform(programHandle, "Ld"), 1.0f, 1.0f, 1.0f);

        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}