#pragma once

#include <Miracle/Miracle.hpp>
#include "Text.h"
#include "Constants.h"
#include <fstream>
#include <algorithm>

using namespace Miracle;

struct ScoreEntry {
	std::string Name;
	unsigned long long Score;
	int Lines;
	int Level;
	bool Unsaved;
};

class Scoreboard {
private:
	std::string m_filePath;
	std::optional<ScoreEntry> m_scoreList[10];
	Text m_header;
	std::unique_ptr<Text> m_scoreText[10];
	const std::string delimiter = ",";
	bool m_hasEntered = false;

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
						entry.Score = std::stoull(token);
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

	std::string FormatThousandsNumber(unsigned long long num, char thousandSep = ' ') {
		std::string value = std::to_string(num);
		int len = value.length();
		int dlen = 3;

		while (len > dlen) {
			value.insert(len - dlen, 1, thousandSep);
			dlen += 4;
			len += 1;
		}
		return value;
	}

	std::string FormatScore(int i) {
		std::string str = std::to_string(i + 1) + ". ";
		if (i < 9)
			str += " ";
		if (!m_scoreList[i].has_value()) return str;
		ScoreEntry& entry = m_scoreList[i].value();
		const size_t maxNameLength = 10;
		size_t sublen = std::min(maxNameLength, entry.Name.size());
		str += entry.Name.substr(0, sublen);
		const size_t maxScoreLength = 14;
		std::string scoreStr = FormatThousandsNumber(entry.Score);
		if (scoreStr.size() > maxScoreLength) {
			scoreStr = scoreStr.substr(0, maxScoreLength - 3) + "...";
		}
		size_t endLength = 25 - scoreStr.size();
		for (size_t index = sublen; index < endLength; index++)
			str += " ";
		str += scoreStr;
		return str;
	}

	void UpdateText() {
		for (int i = 0; i < 10; i++) {
			if (m_scoreList[i].has_value() && m_scoreList[i]->Unsaved) {
				m_scoreText[i]->ChangeColor(ColorRgb::createFromColorCode(0x555555));
			} else {
				m_scoreText[i]->ChangeColor(ColorRgbs::white);
			}
			m_scoreText[i]->ChangeText(FormatScore(i));
		}
	}

public:
	Scoreboard(std::string name, std::string fileName) :
		m_header(Vector2{ .x = -12, .y = 8 }, 1.0f, ColorRgbs::white, name)
	{
		m_header.Hide();
		std::string path = BaseFolderPath + "Score\\";
		m_filePath = path + fileName;
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
		ReadFromFile();

		float y = 6;
		for (int i = 0; i < 10; i++) {
			m_scoreText[i] = std::make_unique<Text>(
				Vector2{ .x = -9, .y = y-- },
				1.0f,
				ColorRgbs::white,
				FormatScore(i)
				);
			m_scoreText[i]->Hide();
		}
	}

	bool IsAddingEntry = false;

	void Show() {
		m_header.Show();
		for (int i = 0; i < 10; i++)
			m_scoreText[i]->Show();
	}

	void Hide() {
		m_header.Hide();
		for (int i = 0; i < 10; i++)
			m_scoreText[i]->Hide();
	}

	bool MadeScoreboard(unsigned long long score) {
		for (int i = 0; i < 10; i++) {
			if (!m_scoreList[i].has_value() || m_scoreList[i].value().Score < score)
				return true;
		}
		return false;
	}

	int Add(unsigned long long score, int lines, int level) {
		for (int i = 0; i < 10; i++) {
			if (!m_scoreList[i].has_value() || m_scoreList[i].value().Score < score) {
				IsAddingEntry = true;
				Place(ScoreEntry{
					.Name = "Enter name...",
					.Score = score,
					.Lines = lines,
					.Level = level,
					.Unsaved = true
					}, i);
				UpdateText();
				m_hasEntered = false;
				ScoreEntry* entry = &m_scoreList[i].value();
				Keyboard::setKeyPressedCallback([this, entry](KeyboardKey key) {
					if (!m_hasEntered) {
						m_hasEntered = true;
						entry->Name = "";
					}
					if (key == KeyboardKey::keyEnter && entry->Name.size() > 0) {
						entry->Unsaved = false;
						this->FinalizeEntry();
						Keyboard::unsetKeyPressedCallback();
						return;
					}
					if (key == KeyboardKey::keyBackspace) {
						if (entry->Name.size() > 0) {
							entry->Name = entry->Name.substr(0, entry->Name.size() - 1);
						}
					}
					else if (key == KeyboardKey::keyDelete) {
						entry->Name = "";
					}
					else if (key == KeyboardKey::keyLeftShift || key == KeyboardKey::keyRightShift) {
						return;
					}
					else if (entry->Name.size() >= 10) {
						return;
					}
					else {
						char c = (char)key;
						if (c >= 65 && c <= 90) {
							if (!Keyboard::isKeyHeld(KeyboardKey::keyRightShift) && !Keyboard::isKeyHeld(KeyboardKey::keyLeftShift))
								c += 32;
						}
						entry->Name = entry->Name + c;
					}
					this->UpdateEntry();
					});
				return i;
			}
		}
		return -1;
	}

	ScoreEntry& GetEntry(int i) {
		return m_scoreList[i].value();
	}

	void UpdateEntry() {
		UpdateText();
	}

	void FinalizeEntry() {
		WriteToFile();
		UpdateText();
		IsAddingEntry = false;
	}
};