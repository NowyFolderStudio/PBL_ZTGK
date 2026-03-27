#pragma once
#include <string>

class Window {
public:
	virtual ~Window() = default;
	virtual void OnUpdate() = 0;
	virtual bool ShouldClose() const = 0;
	virtual void* GetNativeWindow() const = 0;

	static Window* Create(const std::string& title = "NFS Engine", int width = 1280, int height = 720);
};