#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace NFSEngine {
	struct NoteEvent {
		int bar; 
		int beat;
		int sixteenth;

		int trackID;
		float pitchOffset;
		std::string noteName;
	};

	struct Pattern {
		std::string name;
		std::string audioFile;
		int totalBars;
		std::vector<NoteEvent> notes;
	};

	class PatternParser {
	public:
		static int GetNoteValue(const std::string& noteString);

		static Pattern LoadFromFile(const std::string& filepath);
	};
}
