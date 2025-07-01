#include <string_view>
#include <glad/glad.h>
#include "shader.h"
#include <sstream>
#include <iostream>
#include <fstream>

Shader::Shader(std::string_view vertPath, std::string_view fragPath) {
    std::ifstream fs{ vertPath.data() };
    fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string vertTemp{};
    std::string fragTemp{};
    const char* vertSource;
    const char* fragSource;

    try {
        // Read vertex shader
        std::stringstream ss{};
        ss << fs.rdbuf();
        vertTemp = ss.str();
        vertSource = vertTemp.c_str();

        // Read fragment shader
        fs.close();
        fs.open(fragPath.data());
        ss.str("");
        ss << fs.rdbuf();
        fragTemp = ss.str();
        fragSource = fragTemp.c_str();
    }
    catch (std::ifstream::failure e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << vertPath << " : " << fragPath << "\n";
    }

    // Compile vertex
    unsigned int vertShader{ glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);

    int success;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertPath << infoLog << "\n";
    }

    // Compile fragment
    unsigned int fragShader{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);

    success = 0;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragPath << infoLog << "\n";
    }

    // Program
    mID = glCreateProgram();
    glAttachShader(mID, vertShader);
    glAttachShader(mID, fragShader);
    glLinkProgram(mID);

    success = 0;
    glGetProgramiv(mID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(mID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}