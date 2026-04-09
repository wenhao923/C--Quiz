#pragma once
#include <coroutine>
#include <webgpu/webgpu_cpp.h>

// ==========================================
// 1. 极其轻量的协程任务类型 (Fire-and-Forget)
// ==========================================
struct AsyncTask {
    struct promise_type {
        AsyncTask get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

// ==========================================
// 2. 显卡请求的等待体 (Adapter Awaiter)
// ==========================================
struct AdapterAwaiter {
    wgpu::Instance instance;
    wgpu::RequestAdapterOptions options;
    wgpu::Adapter result;

    bool await_ready() const noexcept { return false; } // 强制挂起

    void await_suspend(std::coroutine_handle<> handle) {
        instance.RequestAdapter(
            &options, wgpu::CallbackMode::AllowSpontaneous,
            // C++ 协程魔法：把 handle 传进回调里，拿到显卡后执行 handle.resume() 唤醒主线程！
            [this, handle](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView) {
                if (status == wgpu::RequestAdapterStatus::Success) {
                    this->result = std::move(adapter);
                }
                handle.resume(); // 解冻！
            });
    }

    wgpu::Adapter await_resume() noexcept { return std::move(result); }
};

// 暴露给外部的极其干净的接口
inline AdapterAwaiter RequestAdapterAsync(wgpu::Instance instance, wgpu::RequestAdapterOptions options) {
    return AdapterAwaiter{instance, options, nullptr};
}

// ==========================================
// 3. 设备请求的等待体 (Device Awaiter)
// ==========================================
struct DeviceAwaiter {
    wgpu::Adapter adapter;
    wgpu::DeviceDescriptor descriptor;
    wgpu::Device result;

    bool await_ready() const noexcept { return false; } // 强制挂起

    void await_suspend(std::coroutine_handle<> handle) {
        adapter.RequestDevice(
            &descriptor, wgpu::CallbackMode::AllowSpontaneous,
            [this, handle](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView) {
                if (status == wgpu::RequestDeviceStatus::Success) {
                    this->result = std::move(device);
                }
                handle.resume(); // 解冻！
            });
    }

    wgpu::Device await_resume() noexcept { return std::move(result); }
};

// 暴露给外部的极其干净的接口
inline DeviceAwaiter RequestDeviceAsync(wgpu::Adapter adapter, wgpu::DeviceDescriptor descriptor) {
    return DeviceAwaiter{adapter, descriptor, nullptr};
}