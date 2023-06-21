#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>

#include "Engine/Mesh/Shader.h"

struct ShaderProgramSource
{
    std::string vertexSource;
    std::string fragmentSource;

    Shader::BlendType srcblendtype{ Shader::BlendType::SrcAlpha };
    Shader::BlendType destblendtype{ Shader::BlendType::OneMinusSrcAlpha };

    std::unordered_map<Shader::Param, bool> params;
};

Shader::Shader() { InitializeParameters(); }

Shader::Shader(const std::string& filepath)
{
    ShaderProgramSource src = ParseShader(filepath);
    id = CreateShader(src.vertexSource, src.fragmentSource);

    InitializeParameters();
    for (auto& pair : src.params)
        parameters[pair.first] = pair.second;
    
    srcblend = src.srcblendtype;
    destblend = src.destblendtype;
}

Shader::Shader(const Shader& obj)
{
    this->id = obj.id;

    InitializeParameters();
    for (auto& pair : obj.parameters)
        parameters[pair.first] = pair.second;

    this->srcblend = obj.srcblend;
    this->destblend = obj.destblend;
}

Shader::Shader(Shader&& obj) noexcept
{
    this->id = obj.id;
    obj.id = -1;

    InitializeParameters();
    for (auto& pair : obj.parameters)
    {
        parameters[pair.first] = pair.second;
        obj.parameters[pair.first] = false;
    }

    this->srcblend = obj.srcblend;
    obj.srcblend = BlendType::SrcAlpha;
    this->destblend = obj.destblend;
    obj.destblend = BlendType::OneMinusSrcAlpha;
}

Shader::~Shader()
{
    if (id != -1)
        glDeleteProgram(id);
}

void Shader::SetPath(const std::string& filepath)
{
	ShaderProgramSource src = ParseShader(filepath);
	id = CreateShader(src.vertexSource, src.fragmentSource);
}

Shader& Shader::operator=(const Shader& other)
{
    this->id = other.id;
    
    InitializeParameters();
    for (auto& pair : other.parameters)
        parameters[pair.first] = pair.second;

    this->srcblend = other.srcblend;
    this->destblend = other.destblend;
    return *this;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    this->id = other.id;
    other.id = -1;
    
    InitializeParameters();
    for (auto& pair : other.parameters)
    {
        parameters[pair.first] = pair.second;
        other.parameters[pair.first] = false;
    }

    this->srcblend = other.srcblend;
    other.srcblend = BlendType::SrcAlpha;
    this->destblend = other.destblend;
    other.destblend = BlendType::OneMinusSrcAlpha;
    return *this;
}

void Shader::Bind() const
{
    // zwrite
    if (parameters.find(Param::ZWrite)->second)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    // cull
    if (parameters.find(Param::Cull)->second)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    // blending
    if (parameters.find(Param::Blend)->second)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    glBlendFunc((int)srcblend, (int)destblend);
    glUseProgram(id);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::Wireframe(bool on)
{
    if (on)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Shader::InitializeParameters()
{
    parameters[Param::Blend] = true;
    parameters[Param::Cull] = false;
    parameters[Param::SetProjMatrix] = false;
    parameters[Param::ZWrite] = false;
    parameters[Param::LightingInfo] = false;
}

/*
shader #subscripts:
#shader [type] - type is vertex or fragment, denotes the shader section
#zwrite [off/on] - whether to write to the zbuffer
#cull [off/on] - whether to cull
#blend [off/on] - whether to use blending

#blendsrc [blendtype]; - must use semicolon to remove ambiguity
#blenddest [blendtype]; - must use semicolon to remove ambiguity
blendtype - Zero, One, SrcColour, SrcAlpha, OneMinusSrcAlpha, ConstantColour, ConstantAlpha

#setProjectionMatrix [off/on] - sets the mat4x4 uniforms "projectionMatrix" and "modelMatrix" automatically
#lightingInformation [off/on] - sets lots of lighting uniforms
*/

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];

    std::unordered_map<Param, bool> params;
    params[Param::Blend] = true;
    params[Param::Cull] = false;
    params[Param::SetProjMatrix] = false;
    params[Param::ZWrite] = false;
    params[Param::LightingInfo] = false;
    
    BlendType srcblend = BlendType::SrcAlpha;
    BlendType destblend = BlendType::OneMinusSrcAlpha;

    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        // for zwrite, cull, etc
        bool option = false;
        if (line.find("on") != std::string::npos)
            option = true;

        // for blend type
        BlendType blendType = BlendType::Zero;
        if (line.find("Zero") != std::string::npos)
            blendType = BlendType::Zero;
        else if (line.find("One") != std::string::npos)
            blendType = BlendType::One;
        else if (line.find("SrcColour") != std::string::npos)
            blendType = BlendType::SrcColour;
        else if (line.find("SrcAlpha") != std::string::npos)
            blendType = BlendType::SrcAlpha;
        if (line.find("OneMinusSrcAlpha") != std::string::npos)
            blendType = BlendType::OneMinusSrcAlpha;
        else if (line.find("ConstantColour") != std::string::npos)
            blendType = BlendType::ConstantColour;
        else if (line.find("ConstantAlpha") != std::string::npos)
            blendType = BlendType::ConstantAlpha;

        // shader type
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }

        // zwrite on
        else if (line.find("#zwrite ") != std::string::npos)
            params[Param::ZWrite] = option;
        // cull on
        else if (line.find("#cull ") != std::string::npos)
            params[Param::Cull] = option;
        // blending
        else if (line.find("#blend ") != std::string::npos)
            params[Param::Blend] = option;
        else if (line.find("#blendsrc ") != std::string::npos)
            srcblend = blendType;
        else if (line.find("#blenddest ") != std::string::npos)
            destblend = blendType;

        // automatically set projection matrix
        else if (line.find("#setProjectionMatrix ") != std::string::npos)
            params[Param::SetProjMatrix] = option;

        // adds lots of lighting information
        else if (line.find("#lightingInformation ") != std::string::npos)
            params[Param::LightingInfo] = option;

        else if (type != ShaderType::NONE)
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str(), srcblend, destblend, params };
}

unsigned int Shader::CompileShader(const std::string& source, unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // handle errors D:
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}