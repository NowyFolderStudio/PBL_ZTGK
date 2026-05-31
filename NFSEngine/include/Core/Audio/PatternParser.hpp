#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace NFSEngine {
	struct NoteEvent {
		int bar = 1; 
		int beat = 1;
		int sixteenth = 1;
		int lengthIn16ths = 4;

		int trackID = 0;
		float pitchOffset = 0.0f;
		std::string noteName;
	};

	struct Pattern {
		std::string name;
		std::string audioFile;
		int totalBars = 4;
		std::vector<NoteEvent> notes;
	};

	class PatternParser {
	public:
		static int GetNoteValue(const std::string& noteString);

		static Pattern LoadFromFile(const std::string& filepath);
	};
}
