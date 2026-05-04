#pragma once

#include "Events/Event.hpp"
#include <string>

namespace NFSEngine {
	class NotePlayedEvent : public Event {
	public:
		NotePlayedEvent(const std::string& patternName, const std::string& noteName, int bar, int beat, int sixteenth)
			: m_PatternName(patternName), m_NoteName(noteName), m_Bar(bar), m_Beat(beat), m_Sixteenth(sixteenth) {}

		std::string GetPatternName() const { return m_PatternName; }
		std::string GetNoteName() const { return m_NoteName; }
		int GetBar() const { return m_Bar; }
		int GetBeat() const { return m_Beat; }
		int GetSixteenth() const { return m_Sixteenth; }

		std::string ToString() const override {
			std::stringstream ss;

			ss << "NotePlayedEvent: Pattern: " << m_PatternName << " note: " << m_NoteName << " bar: " << m_Bar
				<< " beat: " << m_Beat << " sixteenth: " << m_Sixteenth;

			return ss.str();
		}

		EVENT_CLASS_TYPE(NotePlayed)
		EVENT_CLASS_CATEGORY(EventCategoryAudio | EventCategoryApplication)
	private:
		std::string m_PatternName;
		std::string m_NoteName;
		int m_Bar;
		int m_Beat;
		int m_Sixteenth;
	};
}