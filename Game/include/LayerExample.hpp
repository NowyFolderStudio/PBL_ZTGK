#pragma once

#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "NFSEngine.h"
#include <memory>

class LayerExample : public NFSEngine::Layer {
public:
    LayerExample();
    virtual ~LayerExample() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    
    void Init();
    void Update();
    void Render();
    
private:
    NFSEngine::Shader* myShader = nullptr;
    std::shared_ptr<NFSEngine::Texture> myTexture;
    NFSEngine::Text* myText = nullptr;
    NFSEngine::Shader* textShader = nullptr;
    NFSEngine::Cube* myCube = nullptr;
	NFSEngine::Canvas* canvas = nullptr;

    NFSEngine::GameObject* movingCube = nullptr;
    std::unique_ptr<NFSEngine::Scene> scene = nullptr;
};