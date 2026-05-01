#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "json/json.hpp"

using json = nlohmann::json;

namespace NFSEngine {
	struct NoteEvent {
		int bar; 
		int beat;
		int sixteenth;

		int trackID;
		float pitchOffset;
	};

	struct Pattern {
		std::string name;
		std::string audioFile;
		int totalBars;
		std::vector<NoteEvent> notes;
	};

	class PatternParser {
	public:
		static int GetNoteValue(const std::string& noteString) {
			static std::unordered_map<std::string, int> noteMap = {
				{"C", 0}, {"C#", 1}, {"D", 2}, {"D#", 3}, {"E", 4}, {"F", 5},
				{"F#", 6}, {"G", 7}, {"G#", 8}, {"A", 9}, {"A#", 10}, {"B", 11}
			};

			int octave = noteString.back() - '0';
			std::string noteName = noteString.substr(0, noteString.size() - 1);

			return (octave - 4) * 12 + noteMap[noteName];
		}

		static Pattern LoadFromFile(const std::string& filepath) {
			Pattern result;
			std::ifstream file(filepath);

			if (!file.is_open()) {
				std::cout << "Blad: Nie mozna otworzyc pliku " << filepath << std::endl;
				return result;
			}

			json data = json::parse(file);

			result.name = data["patternName"];
			result.totalBars = data["lengthInBars"];

			result.audioFile = data["audioFile"];

			int rootNoteValue = GetNoteValue(data["rootNote"]);

			for (const auto& noteNode : data["notes"]) {
				NoteEvent note;
				note.bar = noteNode["bar"];
				note.beat = noteNode["beat"];
				note.sixteenth = noteNode["16th"];

				int targetNoteValue = GetNoteValue(noteNode["note"]);
				note.pitchOffset = static_cast<float>(targetNoteValue - rootNoteValue);

				result.notes.push_back(note);
			}

			return result;
		}
	};
}
