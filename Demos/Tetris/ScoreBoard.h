#pragma once

#include <Miracle/Miracle.hpp>
#include "Text.h"
#include "Constants.h"
#include <fstream>

struct ScoreEntry {
	std::string Name;
	int Score;
	int Lines;
	int Level;
};

class Scoreboard {
private:
	std::string m_filePath;
	std::optional<ScoreEntry> m_scoreList[10];
	Text m_header;
	const std::string delimiter = ",";

	void Clear() {
		for (int i = 0; i < 10; i++)
			m_scoreList[i].reset();
	}

	void ReadFromFile() {
		Clear();
		if (!std::filesystem::exists(m_filePath))
			return;

		std::ifstream file(m_filePath);
		try {
			std::string str;
			int index = 0;
			while (std::getline(file, str)) {
				if (index >= 10) {
					Logger::warning("More than 10 highscores found in \"" + m_filePath + "\"");
					break;
				}
				size_t pos = 0;
				std::string token;
				int part = 0;
				ScoreEntry entry{};
				while ((pos = str.find(delimiter)) != std::string::npos) {
					token = str.substr(0, pos);
					if (part == 0) {
						entry.Score = std::stoi(token);
					}
					else if (part == 1) {
						entry.Lines = std::stoi(token);
					}
					else if (part == 2) {
						entry.Level = std::stoi(token);
					}
					else {
						break;
					}
					part++;
					str.erase(0, pos + delimiter.length());
				}
				if (part < 3) {
					Logger::warning("Found invalid highscore in \"" + m_filePath + "\"");
					break;
				}
				entry.Name = str;
				m_scoreList[index].emplace(entry);
				index++;
			}
		}
		catch (std::exception e) {
			Logger::error("Error reading highscore from \"" + m_filePath + "\": " + e.what());
		}
		file.close();
	}

	void WriteToFile() {
		std::ofstream file;
		file.open(m_filePath, std::ofstream::out | std::ofstream::trunc);
		for (int i = 0; i < 10; i++) {
			if (!m_scoreList[i].has_value())
				break;
			ScoreEntry& entry = m_scoreList[i].value();
			std::string line = std::to_string(entry.Score) + delimiter +
				std::to_string(entry.Lines) + delimiter +
				std::to_string(entry.Level) + delimiter + entry.Name + "\n";
			file.write(line.c_str(), line.size());
		}
		file.close();
	}

	void Place(ScoreEntry entry, int index) {
		ScoreEntry last = entry;
		for (int i = index; i < 10; i++) {
			if (!m_scoreList[i].has_value()) {
				m_scoreList[i].emplace(last);
				break;
			}
			ScoreEntry current = m_scoreList[i].value();
			m_scoreList[i].emplace(last);
			last = current;
		}
	}

public:
	Scoreboard(std::string name, std::string fileName) :
		m_header(Vector2{ .x = -5, .y = 8 }, 1, ColorRgbs::white, name)
	{
		m_header.Hide();
		std::string path = BaseFolderPath + "Score\\";
		m_filePath = path + fileName;
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
		ReadFromFile();
	}

	void Show() {
		m_header.Show();
	}

	void Hide() {
		m_header.Hide();
	}

	bool MadeScoreboard(int score) {
		for (int i = 0; i < 10; i++) {
			if (!m_scoreList[i].has_value())
				break;
			if (m_scoreList[i].value().Score < score)
				return true;
		}
		return false;
	}

	bool TryAdd(std::string name, int score, int lines, int level) {
		bool wasAdded = false;

		for (int i = 0; i < 10; i++) {
			if (!m_scoreList[i].has_value() || m_scoreList[i].value().Score < score) {
				Place(ScoreEntry{
					.Name = name,
					.Score = score,
					.Lines = lines,
					.Level = level,
					}, i);
				wasAdded = true;
				break;
			}
		}

		if (wasAdded)
			WriteToFile();
		return wasAdded;
	}
};