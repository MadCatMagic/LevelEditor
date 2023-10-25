
#include "Engine/Renderer.h"
#include "Engine/Texture.h"

#include "Engine/Mesh/Material.h"
#include "Engine/Mesh/Shader.h"
#include "Engine/Mesh/VertexArray.h"
#include "Engine/Mesh/VertexBuffer.h"

#include "Engine/SpriteRenderer.h"
#include "Editor/EditorGizmos.h"

#include <iostream>

#include "tracy/Tracy.hpp"

namespace Renderer
{
    static void APIENTRY GLErrorCall(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        std::string s = "-- OpenGL Error --\n";

        if (source == GL_DEBUG_SOURCE_API)             s += "Source : API\n";
        if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)   s += "Source : WINDOW_SYSTEM\n";
        if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) s += "Source : SHADER_COMPILER\n";
        if (source == GL_DEBUG_SOURCE_THIRD_PARTY)     s += "Source : THIRD_PARTY\n";
        if (source == GL_DEBUG_SOURCE_APPLICATION)     s += "Source : APPLICATION\n";
        if (source == GL_DEBUG_SOURCE_OTHER)           s += "Source : OTHER\n";
        
        if (type == GL_DEBUG_TYPE_ERROR)               s += "Type : ERROR\n";
        if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) s += "Type : DEPRECATED_BEHAVIOUR\n";
        if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)  s += "Type : UNDEFINED_BEHAVIOUR\n";
        if (type == GL_DEBUG_TYPE_PORTABILITY)         s += "Type : PORTABILITY\n";
        if (type == GL_DEBUG_TYPE_PERFORMANCE)         s += "Type : PERFORMANCE\n";
        if (type == GL_DEBUG_TYPE_OTHER)               s += "Type : OTHER\n";
        if (type == GL_DEBUG_TYPE_MARKER)              s += "Type : MARKER\n";
        if (type == GL_DEBUG_TYPE_PUSH_GROUP)          s += "Type : PUSH_GROUP\n";
        if (type == GL_DEBUG_TYPE_POP_GROUP)           s += "Type : POP_GROUP\n";

        if (severity == GL_DEBUG_SEVERITY_HIGH)         s += "Severity : HIGH\n";
        if (severity == GL_DEBUG_SEVERITY_MEDIUM)       s += "Severity : MEDIUM\n";
        if (severity == GL_DEBUG_SEVERITY_LOW)          s += "Severity : LOW\n";
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) s += "Severity : NOTIFICATION\n";

        s += "Id: " + std::to_string(id) + "\n";
        s += "Message: ";
        s += message;
        s += "\n";
        

        std::cout << s << std::endl;
        if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
            exit(-1);
    }

    void DebugEnable()
    {
        // auto debugging
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLErrorCall, NULL);
    }

    static GLFWwindow* glfwWindow = nullptr;
    v2i GetWinSize()
    {
        int width, height;
        glfwGetWindowSize(glfwWindow, &width, &height);
        return v2i(width, height);
    }

    // The fullscreen quad's FBO
    static const float blitQuadData[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };
    
    static void BlitSetup();
    static void BlitRaw(int src, int depth, int dest, const v2i& winSize, Material* mat);

    void Init(bool debugEnable, GLFWwindow* window)
    {
        glfwWindow = window;
        if (debugEnable)
            DebugEnable();
        BlitSetup();
        SpriteRenderer::Initialise();
        SpriteRenderer::pixelScreenSize = GetWinSize();
    }

    void ClearScreen(GLbitfield mask)
    {
        glClear(mask);
    }

    void Viewport(const v2i& size)
    {
        glViewport(0, 0, size.x, size.y);
    }

    void Viewport(const v2i& size, const v2i& offset)
    {
        glViewport(offset.x, offset.y, size.x, size.y);
    }
    
    void BindScreenBuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Render()
    {
        ZoneScoped;
        SpriteRenderer::pixelScreenSize = GetWinSize();
        SpriteRenderer::RenderAll(0);

        EditorGizmos::DrawAllGizmos();
    }

    // blit stuff

    static VertexBuffer* blitVB;
    static VertexArray* blitVA;
    static Shader* blitShader;
    static Material* blitMat;

    void BlitSetup()
    {
        // setup blit stuff
        blitVB = new VertexBuffer(blitQuadData, sizeof(float) * 18);
        blitVA = new VertexArray();
        blitVA->Construct();
        blitVA->EnableAttribute(0);
        blitVA->FormatAttribute(0, 3, GL_FLOAT, false, 0, 0);
        blitVA->DisableAttribute(0);
        // Create and compile our GLSL program from the shaders
        blitShader = new Shader("res/shaders/Blit.shader");
        blitMat = new Material(blitShader);
    }

    // Blit variations
    // Texture2D
    void Blit(const Texture2D* src, RenderTexture* dest)
    {
        if (dest == nullptr)
            BlitRaw(src->GetID(), 0, 0, GetWinSize(), blitMat);
        else
            BlitRaw(src->GetID(), 0, dest->GetID(), v2i(dest->width, dest->height), blitMat);
    }

    void Blit(const Texture2D* src, RenderTexture* dest, Material* mat)
    {
        if (dest == nullptr)
            BlitRaw(src->GetID(), 0, 0, GetWinSize(), mat);
        else
            BlitRaw(src->GetID(), 0, dest->GetID(), v2i(dest->width, dest->height), mat);
    }

    void Blit(const Texture2D* src, const Texture2D* depth, RenderTexture* dest)
    {
        if (dest == nullptr)
            BlitRaw(src->GetID(), 0, dest->GetID(), GetWinSize(), blitMat);
        else
            BlitRaw(src->GetID(), depth->GetID(), dest->GetID(), v2i(dest->width, dest->height), blitMat);
    }

    void Blit(const Texture2D* src, const Texture2D* depth, RenderTexture* dest, Material* mat)
    {
        if (dest == nullptr)
            BlitRaw(src->GetID(), depth->GetID(), 0, GetWinSize(), mat);
        else
            BlitRaw(src->GetID(), depth->GetID(), dest->GetID(), v2i(dest->width, dest->height), mat);
    }

    // RenderTexture
    void Blit(const RenderTexture* src, RenderTexture* dest, bool drawDepth)
    {
        if (dest == nullptr)
            BlitRaw(src->colourBuffer->GetID(), drawDepth ? src->depthBuffer->GetID() : 0, 0, GetWinSize(), blitMat);
        else
            BlitRaw(src->colourBuffer->GetID(), drawDepth ? src->depthBuffer->GetID() : 0, dest->GetID(), v2i(dest->width, dest->height), blitMat);
    }

    void Blit(const RenderTexture* src, RenderTexture* dest, Material* mat, bool drawDepth)
    {
        if (dest == nullptr)
            BlitRaw(src->colourBuffer->GetID(), drawDepth ? src->depthBuffer->GetID() : 0, 0, GetWinSize(), mat);
        else
            BlitRaw(src->colourBuffer->GetID(), drawDepth ? src->depthBuffer->GetID() : 0, dest->GetID(), v2i(dest->width, dest->height), mat);
    }

    // Raw
    static void BlitRaw(int src, int depth, int dest, const v2i& winSize, Material* mat)
    {
        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, dest);
        Viewport(winSize);

        // Use the shader
        mat->Bind();

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, src);
        // Set our "renderedTexture" sampler to use Texture Unit 0
        mat->SetTexture("blitTexture", 0);

        // depth texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth);
        mat->SetTexture("depthTexture", 1);

        // 1rst attribute buffer : vertices
        blitVB->Bind();
        blitVA->Bind();
        blitVA->EnableAttribute(0);

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, 6);
        blitVA->DisableAttribute(0);
    }

    void Release()
    {
        delete blitVB;
        delete blitVA;
        delete blitShader;
        delete blitMat;

        SpriteRenderer::Release();
    }
}