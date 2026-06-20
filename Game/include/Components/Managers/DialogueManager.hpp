#pragma once
#include <string>

namespace NFSEngine {

    struct ActiveDialogue {
        bool IsVisible = false;
        std::string SpeakerName;
        std::string FullMessage;
        std::string DisplayedMessage;
        std::string PortraitPath;
        float TimeRemaining = 0.0f;

        int VisibleCharacters = 0;
        float CharacterTimer = 0.0f;
        float TextSpeed = 0.03f;
    };

    class DialogueManager {
    public:
        static DialogueManager& Get();

        void Init();
        void Update(float deltaTime);

        void ShowMessage(const std::string& speaker, const std::string& msg, const std::string& portraitPath, float duration);
        void HideMessage();

        const ActiveDialogue& GetActiveDialogue() const { return m_ActiveDialogue; }

    private:
        DialogueManager() = default;
        DialogueManager(const DialogueManager&) = delete;
        DialogueManager& operator=(const DialogueManager&) = delete;

        ActiveDialogue m_ActiveDialogue;
    };

} // namespace NFSEngine