#include <iostream>
#include <vector>
#include <memory>

class Renderer{
public:
    virtual ~Renderer() = default;
    virtual void Init() = 0;
    virtual void Draw() = 0;
};

class OpenGLRenderer: public Renderer{
public:
    ~OpenGLRenderer() override = default;
    void Init() override {
        std::cout << "Init OpenGLRenderer" << std::endl;
    }
    void Draw() override {
        std::cout << "Draw OpenGLRenderer" << std::endl;
    }
};

class VulkanRenderer: public Renderer{
public:
    ~VulkanRenderer() override = default;
    void Init() override {
        std::cout << "Init VulkanRenderer" << std::endl;
    }
    void Draw() override {
        std::cout << "Draw VulkanRenderer" << std::endl;
    }
};

class RendererFactory {
public:
    static RendererFactory& getFactorySingleton() {
        static RendererFactory mSingleton;
        return mSingleton;
    }

    RendererFactory(const RendererFactory&) = delete;
    RendererFactory(RendererFactory&&) = delete;

    RendererFactory& operator=(const RendererFactory&) = delete;
    RendererFactory& operator=(RendererFactory&&) = delete;

    void CreateRenderer(const std::string &type) {
        if (type == "OpenGL")
        {
            mRenderers.push_back(std::make_unique<OpenGLRenderer>());
        } else if (type == "Vulkan")
        {
            mRenderers.push_back(std::make_unique<VulkanRenderer>());
        }
    }

private:
    std::vector<std::unique_ptr<Renderer>> mRenderers;

    RendererFactory() = default;
    ~RendererFactory() = default;
};