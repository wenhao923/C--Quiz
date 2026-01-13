#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void Init() = 0;
    virtual void Draw() = 0;
};

class OpenGLRender : public Renderer {
public:
    ~OpenGLRender() override = default;
    void Init() override {
        std::cout << "Init OpenGL" << std::endl;        
    }
    void Draw() override {
        std::cout << "OpenGL Draw" << std::endl;
    }
};

class VulkanRenderer : public Renderer {
public:
    ~VulkanRenderer() override = default;
    void Init() override {
        std::cout << "Init Vulkan" << std::endl;        
    }
    void Draw() override {
        std::cout << "Vulkan Draw" << std::endl;
    }
};

class RendererFactory {
public:
    static RendererFactory& getSingleton() {
        static RendererFactory instance;
        return instance;
    }

    std::unique_ptr<Renderer> Create(const std::string &type) {
        if (type == "OpenGL")
        {
            return std::make_unique<OpenGLRender>();
        } else if (type == "Vulkan")
        {
            return std::make_unique<VulkanRenderer>();
        } else 
            return nullptr;
    }

    RendererFactory(const RendererFactory&) = delete;
    RendererFactory(RendererFactory&&) = delete;

    RendererFactory& operator=(const RendererFactory&) = delete;
    RendererFactory& operator=(RendererFactory&&) = delete;
private:
    RendererFactory() = default;
    ~RendererFactory() = default;
};