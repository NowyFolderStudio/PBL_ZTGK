#pragma once

#include <NFSEngine.h>

class BounceComponent : public NFSEngine::Component {
public:
    BounceComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "BounceComponent"; }

    float BounceHeight = 10.0f;
};