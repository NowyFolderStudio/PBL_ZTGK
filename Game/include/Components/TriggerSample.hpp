#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include <iostream>

namespace NFSEngine {

    // =========================================================================
    // PRZYKŁAD 1: ZNAJDŹKA / JEDNORAZOWY EVENT (np. Moneta, Checkpoint)
    // =========================================================================
    class CoinSample : public Component {
    public:
        explicit CoinSample(GameObject* owner)
            : Component(owner) { }
        std::string GetName() const override { return "CoinSample"; }

    protected:
        void OnStart() override {
            // 1. Pobieramy jakikolwiek collider podpięty pod ten sam obiekt
            auto* collider = m_Owner->GetComponent<ColliderComponent>();

            if (collider) {
                // 2. Zmieniamy go w strefę widmo (obiekty będą przez niego przenikać)
                collider->IsTrigger = true;

                // 3. Rejestrujemy akcję tylko na WEJŚCIE w strefę
                collider->OnTriggerEnter = [this](GameObject* other) {
                    // Sprawdzamy, czy to na pewno gracz zebrał monetę
                    if (other->GetName() == "Player") {
                        std::cout << "[CoinSample] Gracz zebral monete!\n";

                        // Tutaj np: other->GetComponent<PlayerStats>()->AddScore(100);

                        // Usuwamy monetę ze sceny
                        m_Owner->SetActive(false);
                    }
                };
            }
        }
    };

    // =========================================================================
    // PRZYKŁAD 2: STREFA CIĄGŁA (np. Rzeka Lawy, Ognisko Leczące, Radar Drzwi)
    // =========================================================================
    class HealingZoneSample : public Component {
    public:
        explicit HealingZoneSample(GameObject* owner)
            : Component(owner) { }
        std::string GetName() const override { return "HealingZoneSample"; }

    protected:
        void OnStart() override {
            auto* collider = m_Owner->GetComponent<ColliderComponent>();

            if (collider) {
                collider->IsTrigger = true;

                // A) Akcja wywoływana RAZ, gdy obiekt wchodzi w strefę
                collider->OnTriggerEnter = [](GameObject* other) {
                    if (other->GetName() == "Player") {
                        std::cout << "[HealingZone] Gracz wszedl do strefy leczenia. Odpalam efekty czasteczkowe!\n";
                    }
                };

                // B) Akcja wywoływana CO KLATKĘ FIZYKI (FixedUpdate), dopóki obiekt jest w środku
                collider->OnTriggerStay = [](GameObject* other) {
                    if (other->GetName() == "Player") {
                        // Uwaga: To odpala się kilkadziesiąt razy na sekundę!
                        // Tutaj można dodawać małe wartości HP, np:
                        // other->GetComponent<Health>()->AddHealth(0.1f);
                        std::cout << "[HealingZone] Gracz jest leczony...\n";
                    }
                };

                // C) Akcja wywoływana RAZ, gdy obiekt wychodzi ze strefy (lub strefa/gracz zostanie zniszczona)
                collider->OnTriggerExit = [](GameObject* other) {
                    if (other->GetName() == "Player") {
                        std::cout << "[HealingZone] Gracz opuscil strefe. Wylaczam efekty leczenia.\n";
                    }
                };
            }
        }
    };

} // namespace NFSEngine