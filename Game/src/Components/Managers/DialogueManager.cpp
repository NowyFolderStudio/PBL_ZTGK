#include "Components/Managers/DialogueManager.hpp"
#include <sstream>

namespace NFSEngine {

    DialogueManager& DialogueManager::Get() {
        static DialogueManager instance;
        return instance;
    }

    void DialogueManager::Init() { m_ActiveDialogue.IsVisible = false; }

    NFSEngine::Text* DialogueManager::GetFont() {
        if (!m_Font) {
            m_Font = std::make_shared<NFSEngine::Text>("assets/fonts/Pulang.ttf", 36);
        }
        return m_Font.get();
    }

    void DialogueManager::Update(float deltaTime) {
        if (!m_ActiveDialogue.IsVisible) return;

        m_ActiveDialogue.TimeRemaining -= deltaTime;
        if (m_ActiveDialogue.TimeRemaining <= 0.0f) {
            HideMessage();
            return;
        }

        if (m_ActiveDialogue.VisibleCharacters < m_ActiveDialogue.FullMessage.length()) {
            m_ActiveDialogue.CharacterTimer += deltaTime;

            if (m_ActiveDialogue.CharacterTimer >= m_ActiveDialogue.TextSpeed) {
                m_ActiveDialogue.CharacterTimer = 0.0f;
                m_ActiveDialogue.VisibleCharacters++;

                m_ActiveDialogue.DisplayedMessage = m_ActiveDialogue.FullMessage.substr(0, m_ActiveDialogue.VisibleCharacters);
            }
        }
    }

    void DialogueManager::ShowMessage(const std::string& speaker, const std::string& msg, const std::string& portraitPath,
                                      float duration) {
        std::istringstream linesStream(msg);
        std::string line;
        std::string wrappedText;
        size_t maxLineLength = 40;

        while (std::getline(linesStream, line)) {
            std::istringstream words(line);
            std::string word;
            size_t currentLineLength = 0;

            while (words >> word) {
                if (currentLineLength + word.length() > maxLineLength) {
                    if (!wrappedText.empty() && wrappedText.back() == ' ') wrappedText.pop_back();
                    wrappedText += "\n" + word + " ";
                    currentLineLength = word.length() + 1;
                } else {
                    wrappedText += word + " ";
                    currentLineLength += word.length() + 1;
                }
            }

            if (!wrappedText.empty() && wrappedText.back() == ' ') wrappedText.pop_back();
            wrappedText += "\n";
        }

        if (!wrappedText.empty() && wrappedText.back() == '\n') wrappedText.pop_back();

        m_ActiveDialogue.SpeakerName = speaker;
        m_ActiveDialogue.FullMessage = wrappedText;
        m_ActiveDialogue.DisplayedMessage = "";
        m_ActiveDialogue.PortraitPath = portraitPath;
        m_ActiveDialogue.TimeRemaining = duration;

        m_ActiveDialogue.VisibleCharacters = 0;
        m_ActiveDialogue.CharacterTimer = 0.0f;
        m_ActiveDialogue.IsVisible = true;
    }

    void DialogueManager::HideMessage() { m_ActiveDialogue.IsVisible = false; }

} // namespace NFSEngine