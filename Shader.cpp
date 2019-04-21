#include <fstream>
#include <vector>
#include <cstring>
#include "Shader.hpp"

static void checkShader(GLuint shader)
{
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        printf("Shader compilation failed!\n");
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *log = new char[logLen];
            GLsizei written;
            glGetShaderInfoLog(shader, logLen, &written, log);
            printf("Shader log:\n%s\n", log);
            delete[] log;
        }
    }
}

Shader::Shader(const std::vector<char> &source, GLenum type)
{
    _handle = glCreateShader(type);

    const GLchar *sourceArray[] = {source.data()};
    glShaderSource(_handle, 1, sourceArray, nullptr);
    glCompileShader(_handle);
    checkShader(_handle);
}

GLuint Shader::getHandle()
{
    return _handle;
}

Shader Shader::loadShaderFromFile(const std::string &path, GLenum type)
{
    FILE *fileStream = fopen(path.c_str(), "r");

    fseek(fileStream, 0, SEEK_END);
    auto size = ftell(fileStream);
    rewind(fileStream);

    std::vector<char> source(size + 1, 0);

    fread(source.data(), size, 1, fileStream);
    fclose(fileStream);

    return Shader(source, type);
}
