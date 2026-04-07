#include "Engine.h"
#include <iostream>
#include <condition_variable>
#include <mutex>
#include "MyVector.h"

#include <webgpu/webgpu_cpp.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <SFML/Window.hpp>

// --- 定义一些全局变量来存放 Dawn 核心状态 (为了演示，实际应用应放在类成员中) ---
static wgpu::Instance     g_instance = nullptr;
static wgpu::Adapter      g_adapter  = nullptr;
static wgpu::Device       g_device   = nullptr;
static wgpu::Surface      g_surface  = nullptr;
static wgpu::RenderPipeline g_pipeline = nullptr;
static wgpu::Queue        g_queue    = nullptr;

// 同步等待工具（用于等待异步请求完成）
std::mutex g_mutex;
std::condition_variable g_cv;
bool g_requestFinished = false;

// 记录当前窗口大小，用于 Surface 配置
static uint32_t g_width  = 800;
static uint32_t g_height = 600;

// --- 现代 WGSL 着色器 (保持不变，标准已稳定) ---
const char* shaderWGSL = R"(
struct VertexOutput {
    @builtin(position) position : vec4<f32>,
    @location(0) color : vec3<f32>,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex : u32) -> VertexOutput {
    var pos = array<vec2<f32>, 3>(
        vec2<f32>( 0.0,  0.5),
        vec2<f32>(-0.5, -0.5),
        vec2<f32>( 0.5, -0.5)
    );
    var colors = array<vec3<f32>, 3>(
        vec3<f32>(1.0, 0.0, 0.0),
        vec3<f32>(0.0, 1.0, 0.0),
        vec3<f32>(0.0, 0.0, 1.0)
    );
    var output : VertexOutput;
    output.position = vec4<f32>(pos[vertexIndex], 0.0, 1.0);
    output.color = colors[vertexIndex];
    return output;
}

@fragment
fn fs_main(@location(0) color : vec3<f32>) -> @location(0) vec4<f32> {
    let srgbColor = pow(color, vec3<f32>(1.0 / 2.2));
    return vec4<f32>(srgbColor, 1.0);
}
)";

// --- 初始化引擎 ---
void Engine::Init(const sf::Window& window) {
    std::cout << "[Dawn] Booting up Next-Gen WGPU Reactor..." << std::endl;
    g_width = window.getSize().x;
    g_height = window.getSize().y;

    // 1. 创建 Instance (包含特性请求配置)
    wgpu::InstanceDescriptor instanceDesc = {};
    g_instance = wgpu::CreateInstance(&instanceDesc);
    if (!g_instance) {
        std::cerr << "Fatal: Failed to create Dawn Instance!" << std::endl;
        return;
    }

    // 2. 创建 Surface (连接到 SFML 窗口)
#ifdef _WIN32
    wgpu::SurfaceSourceWindowsHWND hwndDesc;
    hwndDesc.hinstance = GetModuleHandle(nullptr); // 最新 Dawn 往往需要传入当前程序的实例句柄
    hwndDesc.hwnd = window.getSystemHandle();
    
    wgpu::SurfaceDescriptor surfaceDesc;
    surfaceDesc.nextInChain = &hwndDesc;
    g_surface = g_instance.CreateSurface(&surfaceDesc);
#else
    std::cerr << "Fatal: Only Windows surface is implemented in this snippet!" << std::endl;
    return;
#endif

    //
    // Create an adapter
    //

    {
        // Base RequestAdapterOptions
        wgpu::RequestAdapterOptions adapterOptions = {};
        adapterOptions.compatibleSurface = g_surface;
        adapterOptions.powerPreference = wgpu::PowerPreference::HighPerformance;

        // Synchronously create the adapter
        g_instance.WaitAny(
            g_instance.RequestAdapter(&adapterOptions, wgpu::CallbackMode::WaitAnyOnly,
                                            [](wgpu::RequestAdapterStatus status,
                                               wgpu::Adapter adapter, wgpu::StringView message) {
                                                if (status != wgpu::RequestAdapterStatus::Success) {
                                                    return;
                                                }
                                                g_adapter = std::move(adapter);
                                            }),
            0);
        if (g_adapter == nullptr) {
            return;
        }
    }

    //
    // Create a device
    //

    {
        // Base DeviceDescriptor
        wgpu::DeviceDescriptor deviceDesc = {};
        deviceDesc.SetDeviceLostCallback(
            wgpu::CallbackMode::AllowSpontaneous,
            [](const wgpu::Device&, wgpu::DeviceLostReason reason, wgpu::StringView message) {
                const char* reasonName = "";
                switch (reason) {
                    case wgpu::DeviceLostReason::Unknown:
                        reasonName = "Unknown";
                        break;
                    case wgpu::DeviceLostReason::Destroyed:
                        reasonName = "Destroyed";
                        break;
                    case wgpu::DeviceLostReason::CallbackCancelled:
                        reasonName = "CallbackCancelled";
                        break;
                    case wgpu::DeviceLostReason::FailedCreation:
                        reasonName = "FailedCreation";
                        break;
                    default:
                        break;
                }
                std::string_view msg(message.data, message.length);

                if (reason == wgpu::DeviceLostReason::Destroyed) {
                    std::cout << "[Dawn Info] Device gracefully destroyed." << std::endl;
                } else {
                    std::cerr << "[FATAL] WebGPU Device Lost!" << "\n"
                            << "   -> Reason : " << reasonName << "\n"
                            << "   -> Message: " << msg << "\n"
                            << "----------------------------------------" << std::endl;
                }
            });
        deviceDesc.SetUncapturedErrorCallback(
            [](const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message) {
                std::string_view msg(message.data, message.length);
                const char* errorTypeName = "";
                switch (type) {
                    case wgpu::ErrorType::Validation:
                        errorTypeName = "Validation";
                        break;
                    case wgpu::ErrorType::OutOfMemory:
                        errorTypeName = "Out of memory";
                        break;
                    case wgpu::ErrorType::Internal:
                        errorTypeName = "Internal";
                        break;
                    case wgpu::ErrorType::Unknown:
                        errorTypeName = "Unknown";
                        break;
                    default:
                        break;
                }
                std::cerr << "🚨 [WebGPU ERROR - " << errorTypeName << "] " << msg << std::endl;
                assert(type != wgpu::ErrorType::Validation && "遇到了 WebGPU 验证错误，请检查渲染参数！");
            });

        // Synchronously create the device
        g_instance.WaitAny(
            g_adapter.RequestDevice(
                &deviceDesc, wgpu::CallbackMode::WaitAnyOnly,
                [](wgpu::RequestDeviceStatus status, wgpu::Device device,
                   wgpu::StringView message) {
                    if (status != wgpu::RequestDeviceStatus::Success) {
                        return;
                    }

                    g_device = std::move(device);
                    g_queue = g_device.GetQueue();
                }),
            0);
        if (g_device == nullptr) {
            return;
        }
    }



    // 5. [新架构] 配置 Surface (废弃了旧版 SwapChain)
    wgpu::TextureFormat surfaceFormat = wgpu::TextureFormat::BGRA8Unorm; 
    
    wgpu::SurfaceConfiguration config = {};
    config.device = g_device;
    config.format = surfaceFormat;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.width = g_width;
    config.height = g_height;
    config.presentMode = wgpu::PresentMode::Fifo;
    
    // 现代 API：直接使用 Configure 初始化表面
    g_surface.Configure(&config);

    // 6. 锻造图形管线
    wgpu::ShaderSourceWGSL wgslDesc = {};
    wgslDesc.code = shaderWGSL;
    wgpu::ShaderModuleDescriptor smDesc = {};
    smDesc.nextInChain = &wgslDesc;
    wgpu::ShaderModule shaderModule = g_device.CreateShaderModule(&smDesc);

    wgpu::RenderPipelineDescriptor pipelineDesc = {};
    
    // 顶点状态
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.bufferCount = 0;

    // 片元状态
    wgpu::BlendState blend = {};
    blend.color.srcFactor = wgpu::BlendFactor::One;
    blend.color.dstFactor = wgpu::BlendFactor::Zero;
    blend.color.operation = wgpu::BlendOperation::Add;
    blend.alpha.srcFactor = wgpu::BlendFactor::One;
    blend.alpha.dstFactor = wgpu::BlendFactor::Zero;
    blend.alpha.operation = wgpu::BlendOperation::Add;

    wgpu::ColorTargetState colorTarget = {};
    colorTarget.format = surfaceFormat;
    colorTarget.blend = &blend;
    colorTarget.writeMask = wgpu::ColorWriteMask::All;

    wgpu::FragmentState fragmentState = {};
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;
    pipelineDesc.fragment = &fragmentState;

    // 图元拓扑
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;
    
    // 多重采样 (默认 1)
    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    g_pipeline = g_device.CreateRenderPipeline(&pipelineDesc);

    std::cout << "[Dawn] Reactor is ONLINE. Pipeline configured via modern API." << std::endl;
}

// --- 现代渲染主循环 ---
void Engine::Render() {
    if (!g_device) return;

    // 1. [新架构] 从 Surface 获取当前的纹理
    wgpu::SurfaceTexture surfaceTexture;
    g_surface.GetCurrentTexture(&surfaceTexture);
    if (!surfaceTexture.texture) {
        std::cerr << "Failed to acquire next surface texture!" << std::endl;
        return;
    }

    // 2. 为当前获取的纹理创建一个视图 (View)
    wgpu::TextureViewDescriptor viewDesc = {};
    viewDesc.format = surfaceTexture.texture.GetFormat();
    viewDesc.dimension = wgpu::TextureViewDimension::e2D;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = 1;
    wgpu::TextureView backBufferView = surfaceTexture.texture.CreateView(&viewDesc);

    // 3. 创建命令编码器
    wgpu::CommandEncoderDescriptor encoderDesc = {};
    wgpu::CommandEncoder encoder = g_device.CreateCommandEncoder(&encoderDesc);

    // 4. 配置并启动 Render Pass
    wgpu::RenderPassColorAttachment colorAttachment = {};
    colorAttachment.view = backBufferView;
    colorAttachment.resolveTarget = nullptr;
    colorAttachment.loadOp = wgpu::LoadOp::Clear;
    colorAttachment.storeOp = wgpu::StoreOp::Store;
    colorAttachment.clearValue = wgpu::Color{0.05, 0.05, 0.05, 1.0}; // 极其高级的深灰色背景
#ifndef WEBGPU_CPP_NO_DEPTH_SLICE
    colorAttachment.depthSlice = wgpu::kDepthSliceUndefined; // 最新 W3C 规范要求的显式初始化
#endif

    wgpu::RenderPassDescriptor renderPassDesc = {};
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &colorAttachment;

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
    pass.SetPipeline(g_pipeline);
    pass.Draw(3, 1, 0, 0);
    pass.End();

    // 5. 提交命令到队列
    wgpu::CommandBufferDescriptor cmdBufDesc = {};
    wgpu::CommandBuffer commands = encoder.Finish(&cmdBufDesc);
    g_queue.Submit(1, &commands);

    // 6. [新架构] 直接让 Surface 将画面呈现到屏幕上
    g_surface.Present();

    // 7. [关键] 在某些 Dawn 版本中，必须 tick(刷新) 设备才能处理回调和任务
    g_device.Tick(); 
}