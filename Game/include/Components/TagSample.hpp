#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Core/Tags.hpp"
#include <iostream>

namespace NFSEngine {

    class TagSample : public Component {
    public:
        explicit TagSample(GameObject* owner)
            : Component(owner) {}

        std::string GetName() const override { return "TagSample"; }

    protected:
        void OnStart() override {
            // === USTAWIENIE TAGÓW ===

            // Nadpisuje wszystkie tagi — obiekt ma teraz tylko tag Player
            m_Owner->SetTag(Tags::Player);

            // Dokłada tag — obiekt ma teraz Player | Coin
            m_Owner->AddTag(Tags::Coin);

            // Usuwa tag — obiekt ma znowu tylko Player
            m_Owner->RemoveTag(Tags::Coin);

            // === SPRAWDZANIE TAGÓW ===

            auto* collider = m_Owner->GetComponent<ColliderComponent>();
            if (!collider) return;
            collider->IsTrigger = true;

            collider->OnTriggerEnter = [this](GameObject* other) {

                if (other->CompareTag(Tags::Player)) {
                    std::cout << "[TagSample] Gracz dotknal obiektu\n";
                }

                if (other->CompareTag(Tags::Enemy)) {
                    std::cout << "[TagSample] Przeciwnik dotknal obiektu\n";
                }

                if (other->CompareTag(Tags::DeadlyHazard)) {
                    std::cout << "[TagSample] Gracz wszedl w kolce!\n";
                }

                if (other->CompareTag(Tags::Coin)) {
                    std::cout << "[TagSample] Zebrano monete!\n";
                }

                if (!other->CompareTag(Tags::Untagged)) {
                    std::cout << "[TagSample] Obiekt ma jakis tag\n";
                }

                // === SZUKANIE PO TAGU W SCENIE ===

                auto* scene = m_Owner->GetScene();

                GameObject* gameManager = scene->FindWithTag(Tags::GameManager);
                if (gameManager) {
                    auto* scoreComp = gameManager->GetComponent<ScoreManagerComponent>();
                    if (scoreComp) scoreComp->AddScore(100);
                }

                auto enemies = scene->FindGameObjectsWithTag(Tags::Enemy);
                std::cout << "[TagSample] Znaleziono " << enemies.size() << " przeciwnikow na scenie\n";
            };
        }
    };

} // namespace NFSEngine
