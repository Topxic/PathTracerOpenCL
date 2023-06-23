#ifndef BUFFERS_H
#define BUFFERS_H

#include <vector>
#include "GL/glew.h"

template <typename T> class SSBO {
public:
    
    SSBO(std::vector<T> data, unsigned int binding)
    {
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
        std::cout << "Binded buffer of size " << data.size() * sizeof(T) << " to binding point " << binding << std::endl;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, bufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ~SSBO()
    {
        glDeleteBuffers(1, &bufferID);
    }

    void uploadToGPU(std::vector<T> data)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    std::vector<T> downloadFromGPU()
    {   
        std::vector<T> data;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(T), data.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        return data;
    }


private:
    GLuint bufferID;
};

#endif // BUFFERS_H