#pragma once

#include "NFSEngine.h"

class LayerExample : public NFSEngine::Layer {
public:
    LayerExample();
    virtual ~LayerExample() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate() override;
    
    void Init();
    void Update();
    void Render();
	void OnEvent(NFSEngine::Event& e) override;
    
private:
    NFSEngine::Shader* myShader = nullptr;
    NFSEngine::Texture* myTexture = nullptr;
    NFSEngine::Text* myText = nullptr;
    NFSEngine::Shader* textShader = nullptr;
    NFSEngine::Cube* myCube = nullptr;
	NFSEngine::Canvas* canvas = nullptr;
};