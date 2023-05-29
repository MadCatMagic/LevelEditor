#pragma once
#include <unordered_map>
#include "GL/glew.h"

struct ShaderProgramSource;
// self contained shader class
// give it a directory and it will parse the shader, compile it, 
// and create a program id for use with mesh
class Shader
{
public:
    enum class BlendType {
        Zero = GL_ZERO, One = GL_ONE, SrcColour = GL_SRC_COLOR,
        SrcAlpha = GL_SRC_ALPHA, OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        ConstantColour = GL_CONSTANT_COLOR, ConstantAlpha = GL_CONSTANT_ALPHA
    };

    enum class Param {
        ZWrite, Cull, Blend, SetProjMatrix, LightingInfo
    };

    unsigned int id = -1;                // program id
    Shader();
    Shader(const std::string& filepath); // constructor
    Shader(const Shader& obj);
    Shader(Shader&& obj) noexcept;
    ~Shader();

    void SetPath(const std::string& filepath);

    Shader& operator=(const Shader& other);
    Shader& operator=(Shader&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    // lets you draw as a wireframe
    static void Wireframe(bool on);

    // uses weird m.find(p)->second to remain const
    inline bool GetParam(Param param) const { return parameters.find(param)->second; }

private:
    std::unordered_map<Param, bool> parameters;

    BlendType srcblend{ BlendType::SrcAlpha };
    BlendType destblend{ BlendType::OneMinusSrcAlpha };

    void InitializeParameters();

    ShaderProgramSource ParseShader(const std::string& filepath);                                  // splits shader into vert + frag
    unsigned int CompileShader(const std::string& source, unsigned int type);                      // compiles one shader
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader); // compiles vert + frag into program
};

