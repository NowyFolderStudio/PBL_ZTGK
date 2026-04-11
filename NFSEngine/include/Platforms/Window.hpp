#pragma once
#include <memory>
#include <string>
#include <functional>

#include "Events/Event.hpp"

namespace NFSEngine
{

class Window
{
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() = default;
    virtual void OnUpdate() = 0;
    virtual bool ShouldClose() const = 0;
    virtual void* GetNativeWindow() const = 0;

    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

    static std::unique_ptr<Window> Create(const std::string& title = "NFS Engine", int width = 1280, int height = 720);
};

}