#pragma once
#include "Components/ActivatebleComponent.hpp"
#include "Components/Component.hpp"
#include "Components/Doors.hpp"
#include "ConsolePuzzleButton.hpp"
#include "Core/Audio/AudioClip.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Core/GameObject.hpp"
#include "Core/Scene.hpp"
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>

class ConsolePuzzle : public ActivatebleComponent {
public:
    ConsolePuzzle(NFSEngine::GameObject* owner)
        : ActivatebleComponent(owner) { }

    std::string GetName() const override { return "Console Puzzle"; }

    void OnActivationEnter() override {
        if (buttonOrder.empty() || m_Solved) return;

        m_IsPlayingSequence = true;
        m_SequenceIndex = 0;
        m_SequenceTimer = 0.0f;
    }

    void PressButton(int buttonId) {
        if (m_Solved) {
            NFSEngine::AudioEngine::PlayClip(correctSound.get());
            m_ConsoleButtons[buttonId]->SetMaterialEmission(correctColor, 1);
            return;
        }

        if (m_IsPlayingSequence) {
            m_IsPlayingSequence = false;

            if (m_SequenceIndex > 0) {
                int litButtonId = buttonOrder[m_SequenceIndex - 1];
                m_ConsoleButtons[litButtonId]->ResetMaterialEmission();
            }

            m_PressedButtons = 0;
        }

        if (buttonId == buttonOrder[m_PressedButtons]) {
            m_PressedButtons++;
            m_ConsoleButtons[buttonId]->SetMaterialEmission(correctColor, 1);
            NFSEngine::AudioEngine::PlayClip(correctSound.get());

            if (m_PressedButtons >= buttonOrder.size()) {
                m_Solved = true;
                PuzzleSolved();
            }
        } else {
            m_PressedButtons = 0;
            m_ConsoleButtons[buttonId]->SetMaterialEmission(wrongColor, 1);
            NFSEngine::AudioEngine::PlayClip(wrongSound.get());
        }
    }

    std::vector<int> buttonOrder;
    glm::vec3 wrongColor;
    glm::vec3 correctColor;
    glm::vec3 solutionColor;
    std::shared_ptr<NFSEngine::AudioClip> correctSound;
    std::shared_ptr<NFSEngine::AudioClip> wrongSound;
    std::shared_ptr<NFSEngine::AudioClip> solutionSound;

    NFSEngine::GameObject* buttonsContainer = nullptr;
    float solutionSpeed;

protected:
    void OnStart() override {
        if (buttonsContainer == nullptr) {
            return;
        }

        auto* containerTransform = buttonsContainer->GetTransform();

        if (containerTransform->GetChildCount() < 9) {
            return;
        }

        for (int i = 0; i < 9; i++) {
            auto* childObj = containerTransform->GetChild(i)->GetOwner();
            if (childObj == nullptr) continue;

            childObj->AddComponent<ConsolePuzzleButton>();
            auto* btn = childObj->GetComponent<ConsolePuzzleButton>();

            btn->puzzleManager = this;
            btn->buttonID = i;

            m_ConsoleButtons.push_back(btn);
        }

        correctSound = std::make_shared<NFSEngine::AudioClip>("assets/audio/sounds/note.mp3");
        solutionSound = std::make_shared<NFSEngine::AudioClip>("assets/audio/sounds/snaph01.ogg");
        wrongSound = std::make_shared<NFSEngine::AudioClip>("assets/audio/sounds/whoosh.wav");
        m_Doors = GetOwner()->GetScene()->FindGameObject("drzwi")->GetComponent<Doors>();
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_IsPlayingSequence) return;

        m_SequenceTimer -= deltaTime.GetSeconds();
        if (m_SequenceTimer <= 0.0f) {

            if (m_SequenceIndex > 0) {
                int prevButtonId = buttonOrder[m_SequenceIndex - 1];
                m_ConsoleButtons[prevButtonId]->ResetMaterialEmission();
            }

            if (m_SequenceIndex >= buttonOrder.size()) {
                if (m_Solved) {
                    m_SequenceIndex = 0;

                    m_SequenceTimer = solutionSpeed;
                    return;
                } else {
                    m_IsPlayingSequence = false;
                    return;
                }
            }
            int currentButtonId = buttonOrder[m_SequenceIndex];
            if (m_Solved) {
                m_ConsoleButtons[currentButtonId]->SetMaterialEmission(correctColor, 1.0f);
                NFSEngine::AudioEngine::PlayClip(correctSound.get());
            } else {
                m_ConsoleButtons[currentButtonId]->SetMaterialEmission(solutionColor, 1.0f);
                NFSEngine::AudioEngine::PlayClip(solutionSound.get());
            }

            m_SequenceIndex++;
            m_SequenceTimer = solutionSpeed;
        }
    }

private:
    Doors* m_Doors;
    int m_PressedButtons = 0;
    bool m_Solved = false;
    std::vector<ConsolePuzzleButton*> m_ConsoleButtons;

    bool m_IsPlayingSequence = false;
    float m_SequenceTimer = 0.0f;
    int m_SequenceIndex = 0;

    void PuzzleSolved() {
        m_IsPlayingSequence = true;
        m_SequenceIndex = 0;

        m_SequenceTimer = 1.0f;

        m_Doors->Open();
    }
};