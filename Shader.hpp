#ifndef _Shader_hpp_
#define _Shader_hpp_

#include <GL/glew.h>
#include <string>

class Shader
{
public:
    Shader(const std::vector<char> &source, GLenum type);

    Shader(const Shader &) = delete;
    void operator=(const Shader &) = delete;

    /* Get shader handle */
    GLuint getHandle();

    /* Read and compile shader from a file */
    static Shader loadShaderFromFile(const std::string &path, GLenum type);

private:
    GLuint _handle;
};


#endif /* _Shader_hpp_ */
