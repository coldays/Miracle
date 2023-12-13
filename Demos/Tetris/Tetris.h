#pragma once

#include <Miracle/Miracle.hpp>
#include "Row.h"
#include "Shapes.h"
#include "Sound.h"
#include "Text.h"
#include <algorithm>
#include "Menu.h"
#include "Constants.h"
#include <iostream>
#include "ScoreBoard.h"

using namespace Miracle;

bool ShowGhost = false;
bool EnableHold = false;

Vector3 PreviewBlockPos;
Vector3 HoldBlockPos;

class ShapeWrapper {
public:
	EntityContext Shape;
	std::vector<EntityContext> Children;
	Vector3 SpawnPoint;
	bool CanHold = true;

	void Destroy() {
		Shape.destroyEntity();
		for (EntityContext& ec : Children) {
			ec.destroyEntity();
		}
	}

	void Show() {
		for (EntityContext& ec : Children) {
			ec.getAppearance().setVisible(true);
		}
	}

	void Hide() {
		for (EntityContext& ec : Children) {
			ec.getAppearance().setVisible(false);
		}
	}

	void SetActive(bool active) {
		Shape.getAppearance().setVisible(active);
	}

	void Spawn() {
		Move(SpawnPoint);
		Shape.getAppearance().setVisible(true);
	}

	void Hold() {
		Move(HoldBlockPos);
		Shape.getAppearance().setVisible(false);
	}

	void Preview() {
		Move(PreviewBlockPos);
		Shape.getAppearance().setVisible(false);
	}

	void Move(Vector3 pos) {
		auto& parentTransform = Shape.getTransform();
		Vector3 parentTranslate = parentTransform.getTranslation();
		Vector3 diff = pos - parentTranslate;
		parentTransform.translate(Vector3{ .x = diff.x, .y = diff.y }, TransformSpace::scene);
		for (auto& ec : Children) {
			auto& childTransform = ec.getTransform();
			childTransform.translate(Vector3{ .x = diff.x, .y = diff.y }, TransformSpace::scene);
		}
	}
};

class Hud {
	Text m_scoreHeader;
	Text m_scoreText;
	Text m_levelHeader;
	Text m_levelText;
	Text m_linesHeader;
	Text m_linesText;
	Text m_previewHeader;
	Text m_holdHeader;
public:
	Hud() {
		float x = 5.5;
		float y = 8;
		const float xIndent = 0.5;
		const float headerSpacing = 2;
		const float spacing = 1;
		m_holdHeader = Text(Vector2{ .x = -10, .y = y }, 1, ColorRgbs::white, "Hold:");
		m_holdHeader.Hide();
		HoldBlockPos = Vector3{ .x = -8, .y = y - headerSpacing, .z = zIndexGui };
		m_previewHeader = Text(Vector2{ .x = x, .y = y }, 1, ColorRgbs::white, "Preview:");
		m_previewHeader.Hide();
		y -= headerSpacing;
		PreviewBlockPos = { .x = x + 2.5f, .y = y, .z = zIndexGui };
		y -= 3.0f;
		m_scoreHeader = Text(Vector2{ .x = x, .y = y }, 1, ColorRgbs::white, "Score:");
		m_scoreHeader.Hide();
		y -= spacing;
		m_scoreText = Text(Vector2{ .x = x + xIndent, .y = y }, 1, ColorRgbs::white, "0");
		m_scoreText.Hide();
		y -= headerSpacing;
		m_levelHeader = Text(Vector2{ .x = x, .y = y }, 1, ColorRgbs::white, "Level:");
		m_levelHeader.Hide();
		y -= spacing;
		m_levelText = Text(Vector2{ .x = x + xIndent, .y = y }, 1, ColorRgbs::white, "0");
		m_levelText.Hide();
		y -= headerSpacing;
		m_linesHeader = Text(Vector2{ .x = x, .y = y }, 1, ColorRgbs::white, "Lines:");
		m_linesHeader.Hide();
		y -= spacing;
		m_linesText = Text(Vector2{ .x = x + xIndent, .y = y }, 1, ColorRgbs::white, "0");
		m_linesText.Hide();
	}

	void UpdateScore(int value) {
		m_scoreText.ChangeText(std::to_string(value));
	}

	void UpdateLevel(int value) {
		m_levelText.ChangeText(std::to_string(value));
	}

	void UpdateLines(int value) {
		m_linesText.ChangeText(std::to_string(value));
	}

	void Show() {
		m_scoreHeader.Show();
		m_scoreText.Show();
		m_levelHeader.Show();
		m_levelText.Show();
		m_linesHeader.Show();
		m_linesText.Show();
		m_previewHeader.Show();
		if (EnableHold)
			m_holdHeader.Show();
	}

	void Hide() {
		m_scoreHeader.Hide();
		m_scoreText.Hide();
		m_levelHeader.Hide();
		m_levelText.Hide();
		m_linesHeader.Hide();
		m_linesText.Hide();
		m_previewHeader.Hide();
		m_holdHeader.Hide();
	}
};

struct GameBoard {
private:
	std::vector<EntityContext> m_children;

	void RefreshGridLines() {
		for (int i = 1; i < m_children.size() - 1; i++) {
			if (i == 10) continue;
			if (i == 11) continue;
			m_children[i].getAppearance().setVisible(ShowGridLines);
		}
	}

public:
	bool ShowGridLines = true;

	GameBoard() {
		float size = 0.05;
		float horizontalLimit = 5;
		float verticalLimit = 10;
		for (float i = -horizontalLimit; i <= horizontalLimit; i++) {
			m_children.push_back(CurrentScene::createAndGetEntity(EntityConfig{
								.transformConfig = TransformConfig{
									.translation = Vector3{
										.x = i,
										.y = 0,
										.z = zIndexMap
									},
									.scale = Vector3{.x = size, .y = verticalLimit * 2, .z = 1 }
								},
								.appearanceConfig = AppearanceConfig{
									.visible = false,
									.meshIndex = 0,
									.color = ColorRgbs::black,
								}
				}));
		}
		for (float i = -verticalLimit; i <= verticalLimit; i++) {
			m_children.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = 0,
									.y = i,
									.z = zIndexMap
								},
								.scale = Vector3{.x = verticalLimit, .y = size, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.visible = false,
								.meshIndex = 0,
								.color = ColorRgbs::black
							}
				}));
		}
	}

	void ToggleGridLines() {
		ShowGridLines = !ShowGridLines;
		RefreshGridLines();
	}

	void Show() {
		for (EntityContext& ec : m_children) {
			ec.getAppearance().setVisible(true);
		}
		RefreshGridLines();
	}

	void Hide() {
		for (EntityContext& ec : m_children) {
			ec.getAppearance().setVisible(false);
		}
	}
};

class GameManager;
ShapeWrapper CreateBlock(ShapeType);

struct KeyboardConfig {
	KeyboardKey MoveLeftKey = KeyboardKey::keyA;
	KeyboardKey MoveRightKey = KeyboardKey::keyD;
	KeyboardKey RotateLeftKey = KeyboardKey::keyS;
	KeyboardKey RotateRightKey = KeyboardKey::keyW;
	KeyboardKey HardDropKey = KeyboardKey::keySpace;
	KeyboardKey SoftDropKey = KeyboardKey::keyDown;
	KeyboardKey HoldKey = KeyboardKey::keyH;
};

bool isAndreasConfig = true;

KeyboardConfig AndreasConfig = {
	.MoveLeftKey = KeyboardKey::keyA,
	.MoveRightKey = KeyboardKey::keyD,
	.RotateLeftKey = KeyboardKey::keyS,
	.RotateRightKey = KeyboardKey::keyW,
	.HardDropKey = KeyboardKey::keySpace,
	.SoftDropKey = KeyboardKey::keyDown,
};
KeyboardConfig CristinaConfig = {
	.MoveLeftKey = KeyboardKey::keyA,
	.MoveRightKey = KeyboardKey::keyD,
	.RotateLeftKey = KeyboardKey::keyComma,
	.RotateRightKey = KeyboardKey::keyPeriod,
	.HardDropKey = KeyboardKey::keyW,
	.SoftDropKey = KeyboardKey::keyS,
};

KeyboardKey MoveLeftKey = KeyboardKey::keyA;
KeyboardKey MoveRightKey = KeyboardKey::keyD;
KeyboardKey RotateLeftKey = KeyboardKey::keyS;
KeyboardKey RotateRightKey = KeyboardKey::keyW;
KeyboardKey HardDropKey = KeyboardKey::keySpace;
KeyboardKey SoftDropKey = KeyboardKey::keyDown;
KeyboardKey HoldKey = KeyboardKey::keyH;

void SwapKeyConfig(KeyboardConfig config) {
	MoveLeftKey = config.MoveLeftKey;
	MoveRightKey = config.MoveRightKey;
	RotateLeftKey = config.RotateLeftKey;
	RotateRightKey = config.RotateRightKey;
	SoftDropKey = config.SoftDropKey;
	HardDropKey = config.HardDropKey;
	HoldKey = config.HoldKey;
}

std::string GetKeyName(KeyboardKey key) {
	if ((short)key >= 39 && (short)key <= 96) {
		return std::string() + (char)key;
	}
	switch (key) {
		case KeyboardKey::keySpace:
			return "space";
		case KeyboardKey::keyLeft:
			return "left";
		case KeyboardKey::keyRight:
			return "right";
		case KeyboardKey::keyUp:
			return "up";
		case KeyboardKey::keyDown:
			return "down";
		case KeyboardKey::keyEnter:
			return "enter";
		case KeyboardKey::keyEscape:
			return "escape";
		default:
			return "unknown";
	}
}

static float LevelToTickIntervalSec(int level) {
	const float gameboyFps = 59.73;
	const float baseFrames = 53;
	if (level <= 10)
		return 1.0 / (gameboyFps / (baseFrames - 4 * level));
	level -= 10;
	return 1.0 / (gameboyFps / (10 - level));
}

float TickIntervalSeconds = 0.5;

enum class GameState {
	Menu,
	Playing,
	GameOver,
	Paused,
	HighScore
};

enum class DropType {
	Gravity,
	SoftDrop,
	HardDrop
};

class GameManager : public BehaviorBase {
private:
	const int m_maxLevel = 20;
	const int m_height = 20;
	unsigned long long m_score = 0;
	int m_lines = 0;
	int m_level = 0;
	Row m_rows[20];
	float m_tickTime = 0;
	std::optional<ShapeWrapper> m_currentShape;
	std::optional<ShapeWrapper> m_previewShape;
	std::optional<ShapeWrapper> m_heldShape;
	const float m_clearPause = 0.8f;
	float m_clearPauseTime = 0;
	bool m_screenShakeEnabled = false;
	float m_screenShakeTime = 0;
	float m_screenShakeStrength = 0;
	const float m_screenShakeDurationSec = 0.2f;
	float m_pauseCoolDown = 0;
	const float m_pauseCoolDownTime = 5.0f;

	Sound m_theme = Sound("Theme.wav");
	Sound m_rowCompleteSound = Sound("Row.wav");
	Sound m_tetrisSound = Sound("Tetris.wav");
	Sound m_gameOverSound = Sound("GameOver.wav");
	Sound m_levelUpSound = Sound("LevelUp.wav");
	Sound m_clickSound = Sound("Click.wav");
	Sound m_hardDropSound = Sound("HardDrop.wav");

	std::unique_ptr<Scoreboard> m_classicScoreboard;
	std::unique_ptr<Scoreboard> m_ghostOnlyScoreboard;
	std::unique_ptr<Scoreboard> m_holdOnlyScoreboard;
	std::unique_ptr<Scoreboard> m_fullModernScoreboard;

	Scoreboard* m_currentScoreBoard;

	std::unique_ptr<Menu> m_mainMenu;
	std::unique_ptr<Menu> m_levelMenu;
	std::unique_ptr<Menu> m_optionsMenu;
	std::unique_ptr<Menu> m_keyConfMenu;
	std::unique_ptr<Menu> m_gameOverMenu;
	std::unique_ptr<Menu> m_pauseMenu;
	Menu* m_currentMenu = nullptr;

	std::unique_ptr<Hud> m_hud;
	std::unique_ptr<GameBoard> m_gameBoard;

	void SetCurrentMenu(std::unique_ptr<Menu>& menu) {
		if (m_currentMenu)
			m_currentMenu->Hide();
		m_currentMenu = menu.get();
		m_currentMenu->Show();
	}

	void InitKeyConfigMenu(std::string desc, KeyboardKey* keyPtr) {
		m_keyConfMenu->AddMenuNode(desc + " " + GetKeyName(*keyPtr), [desc, keyPtr](MenuNode* node) {
			MenuIgnoreKeyboard = true;
			MenuSkipFrame = 2;
			Keyboard::setKeyPressedCallback([node, desc, keyPtr](KeyboardKey key) {
				if (key == KeyboardKey::keyEscape) {
					Keyboard::unsetKeyPressedCallback();
					MenuIgnoreKeyboard = false;
					return;
				}
				*keyPtr = key;
				node->ChangeText(desc + " " + GetKeyName(key));
				Keyboard::unsetKeyPressedCallback();
				MenuIgnoreKeyboard = false;
				});
			});
	}

	void InitMenus() {
		float x = -5;
		float y = 8;

		m_levelMenu = std::make_unique<Menu>("Level", x, y);
		for (int i = 0; i < 10; i++) {
			m_levelMenu->AddMenuNode(std::to_string(i), [this, i](MenuNode*) { SetLevel(i); StartGame(); });
		}

		m_keyConfMenu = std::make_unique<Menu>("Key config", x, y);
		m_keyConfMenu->AddMenuNode("Back", [this](MenuNode*) { SetCurrentMenu(m_optionsMenu); });
		InitKeyConfigMenu("Move left:", &MoveLeftKey);
		InitKeyConfigMenu("Move right:", &MoveRightKey);
		InitKeyConfigMenu("Rotate left:", &RotateLeftKey);
		InitKeyConfigMenu("Rotate right:", &RotateRightKey);
		InitKeyConfigMenu("Soft drop:", &SoftDropKey);
		InitKeyConfigMenu("Hard drop:", &HardDropKey);

		m_optionsMenu = std::make_unique<Menu>("Options", x, y);
		m_optionsMenu->AddMenuNode("Back", [this](MenuNode*) { SetCurrentMenu(m_mainMenu); });
		m_optionsMenu->AddMenuNode("Key config", [this](MenuNode*) { SetCurrentMenu(m_keyConfMenu); });
		m_optionsMenu->AddToggleMenuNode("Grid", "Shown", "Hidden", &m_gameBoard->ShowGridLines);
		m_optionsMenu->AddToggleMenuNode("Screen shake", "On", "Off", &m_screenShakeEnabled);

		m_optionsMenu->AddToggleMenuNode("Hold", "On", "Off", &EnableHold);
		m_optionsMenu->AddToggleMenuNode("Ghost", "On", "Off", &ShowGhost);

		m_mainMenu = std::make_unique<Menu>("Tetris", x, y);
		m_mainMenu->AddMenuNode("Play", [this](MenuNode*) { SetCurrentMenu(m_levelMenu); });
		m_mainMenu->AddMenuNode("High score", [this](MenuNode*) { ShowHighScore(); });
		m_mainMenu->AddMenuNode("Options", [this](MenuNode*) { SetCurrentMenu(m_optionsMenu); });
		m_mainMenu->AddMenuNode("Exit", [](MenuNode*) { CurrentApp::close(); });

		m_gameOverMenu = std::make_unique<Menu>("Game Over!", -12.0f, y);
		m_gameOverMenu->AddMenuNode("Restart", [this](MenuNode*) { ResetGame(); });
		m_gameOverMenu->AddMenuNode("Exit", [](MenuNode*) { CurrentApp::close(); });

		m_pauseMenu = std::make_unique<Menu>("Pause", x, y);
		m_pauseMenu->AddMenuNode("Resume", [this](MenuNode*) { TogglePause(); });
		m_pauseMenu->AddMenuNode("Main Menu", [this](MenuNode*) { ResetGame(); });
		m_pauseMenu->AddMenuNode("Exit", [](MenuNode*) { CurrentApp::close(); });

		SetCurrentMenu(m_mainMenu);
	}

	void ShowHighScore() {
		m_currentMenu->Hide();
		m_gameBoard->Hide();
		m_hud->Hide();
		m_currentScoreBoard = GetDefaultScoreboard();
		m_currentScoreBoard->Show();
		GameState = GameState::HighScore;
	}

	Scoreboard* GetDefaultScoreboard() {
		if (!ShowGhost && !EnableHold)
			return m_classicScoreboard.get();
		if (!ShowGhost && EnableHold)
			return m_holdOnlyScoreboard.get();
		if (ShowGhost && !EnableHold)
			return m_ghostOnlyScoreboard.get();
		return m_fullModernScoreboard.get();
	}

	Scoreboard* GetPrevScoreboard() {
		if (m_currentScoreBoard == m_classicScoreboard.get())
			return m_fullModernScoreboard.get();
		if (m_currentScoreBoard == m_fullModernScoreboard.get())
			return m_ghostOnlyScoreboard.get();
		if (m_currentScoreBoard == m_ghostOnlyScoreboard.get())
			return m_holdOnlyScoreboard.get();
		if (m_currentScoreBoard == m_holdOnlyScoreboard.get())
			return m_classicScoreboard.get();
	}

	Scoreboard* GetNextScoreboard() {
		if (m_currentScoreBoard == m_classicScoreboard.get())
			return m_holdOnlyScoreboard.get();
		if (m_currentScoreBoard == m_holdOnlyScoreboard.get())
			return m_ghostOnlyScoreboard.get();
		if (m_currentScoreBoard == m_ghostOnlyScoreboard.get())
			return m_fullModernScoreboard.get();
		if (m_currentScoreBoard == m_fullModernScoreboard.get())
			return m_classicScoreboard.get();
	}

	void SpawnNext() {
		// Will only happen on first tick
		if (!m_previewShape.has_value())
			m_previewShape.emplace(CreateBlock((ShapeType)(CurrentApp::getRandom().next<int>(6))));
		// Swap currentshape with preview shape
		m_currentShape.emplace(m_previewShape.value());
		// Create new preview shape
		m_previewShape.emplace(CreateBlock((ShapeType)(CurrentApp::getRandom().next<int>(6))));
		m_previewShape.value().Preview();
		// Move and activate the current shape
		m_currentShape.value().Spawn();
	}

	void Tick() {
		int fullRows = 0;
		// Check if we have a complete row
		for (int i = m_height - 1; i >= 0; i--) {
			if (m_rows[i].IsFull()) {
				fullRows++;
				m_rows[i].MarkedForDeletion = true;
			}
		}

		if (fullRows > 0) {
			m_clearPauseTime = m_clearPause;
			if (fullRows < 4) {
				m_rowCompleteSound.Play();
			}
			else {
				m_theme.Pause();
				m_tetrisSound.Play();
			}
			return;
		}

		if (!m_currentShape.has_value()) {
			SpawnNext();
			// Check if spawned block overlaps with any row
			if (WillOverlap(m_currentShape.value().Children, Vector3{})) {
				Logger::info("**** GAME OVER! ****");
				Logger::info(std::string("\tScore: ") + std::to_string(m_score));
				Logger::info(std::string("\tLevel: ") + std::to_string(m_level));
				Logger::info(std::string("\tLines: ") + std::to_string(m_lines));
				Scoreboard* currentScoreboard = GetDefaultScoreboard();
				if (!currentScoreboard->MadeScoreboard(m_score)) {
					GameState = GameState::GameOver;
					SetCurrentMenu(m_gameOverMenu);
				}
				else {
					Logger::info("New highscore!");
					GameState = GameState::HighScore;
					int placement = currentScoreboard->Add(m_score, m_lines, m_level);
					ShowEnterHighscore(placement);
				}
				m_theme.Stop();
				m_gameOverSound.Play();
				return;
			}
		}
	}

	void ShowEnterHighscore(int placement) {
		m_gameBoard->Hide();
		m_hud->Hide();
		ResetGame();
		ShowHighScore();
	}

	int ClearFullRows() {
		int removed = 0;
		for (int i = m_height - 1; i >= 0; i--) {
			if (m_rows[i].IsFull()) {
				m_rows[i].Destroy();
				for (int j = i + 1; j < m_height; j++) {
					m_rows[j].TransferTo(m_rows[j - 1]);
				}
				removed++;
			}
		}
		IncrementLines(removed);
		// Gameboy scoring system:
		int scoreBase = 0;
		switch (removed) {
			case 1:
				scoreBase = 40;
				break;
			case 2:
				scoreBase = 100;
				break;
			case 3:
				scoreBase = 300;
				break;
			case 4:
				scoreBase = 1200;
				break;
		}
		IncrementScore(scoreBase * (m_level + 1));
		if (m_lines >= (m_level + 1) * 10) {
			IncrementLevel();
			m_levelUpSound.Play();
		}
		return removed;
	}

	int GetRowIndex(float yPos) {
		return yPos + 9.5f;
	}

	int GetColumnIndex(float xPos) {
		return xPos + 4.5f;
	}

	void TogglePause() {
		switch (GameState) {
			case GameState::Playing:
				if (m_pauseCoolDown > 0) return;
				// Hide all blocks
				for (Row& row : m_rows) {
					row.Hide();
				}
				if (m_currentShape.has_value()) {
					m_currentShape.value().Hide();
				}
				m_gameBoard->Hide();
				GameState = GameState::Paused;
				SetCurrentMenu(m_pauseMenu);
				return;
			case GameState::Paused:
				// Show all blocks
				for (Row& row : m_rows) {
					row.Show();
				}
				if (m_currentShape.has_value()) {
					m_currentShape.value().Show();
				}
				m_gameBoard->Show();
				m_pauseMenu->Hide();
				GameState = GameState::Playing;
				m_pauseCoolDown = m_pauseCoolDownTime;
				return;
			default:
				return;
		}
	}

	void CameraAct() {
		// Shake screen
		if (m_screenShakeTime > 0) {
			auto& transform = m_context.getTransform();
			auto& translate = transform.getTranslation();
			m_screenShakeTime -= DeltaTime::get();

			float offset = 50 + m_screenShakeStrength * 50;
			float scale = m_screenShakeStrength / 5.0f;

			Vector3 move = {
				.x = (float)std::cos(CurrentApp::getRuntimeDuration().count() * offset) * scale,
				.y = (float)std::sin(CurrentApp::getRuntimeDuration().count() * offset) * scale
			};

			transform.setTranslation(Vector3{} + move);

			// Reset shake since we are out of time
			if (m_screenShakeTime <= 0) {
				transform.setTranslation(Vector3{});
			}
		}
	}

	void InPlayAct() {
		if (m_pauseCoolDown > 0) {
			m_pauseCoolDown -= DeltaTime::get();
		}
		if (Keyboard::isKeyPressed(KeyboardKey::keyEscape)) {
			TogglePause();
			return;
		}

		CameraAct();

		if (EnableHold && Keyboard::isKeyPressed(HoldKey) &&
			m_currentShape.has_value() && m_currentShape.value().CanHold) {
			if (!m_heldShape.has_value()) {
				// Put current shape in held
				m_heldShape.emplace(m_currentShape.value());
				m_currentShape.reset();
				m_heldShape.value().Hold();
				SpawnNext();
			}
			else {
				// Swap current held and current shape
				ShapeWrapper held = m_heldShape.value();
				ShapeWrapper current = m_currentShape.value();
				m_heldShape.emplace(current);
				m_currentShape.emplace(held);
				m_currentShape.value().Spawn();
				m_heldShape.value().Hold();
			}
			m_currentShape.value().CanHold = false;
		}

		if (m_clearPauseTime > 0) {
			const float numberOfFlashes = 4;
			for (int i = 0; i < m_height; i++) {
				m_rows[i].UpdateVisual(m_clearPause / numberOfFlashes);
			}
			m_clearPauseTime -= DeltaTime::get();
			if (m_clearPauseTime <= 0) {
				int removed = ClearFullRows();
				// Resume theme if we got tetris
				if (removed == 4) {
					m_theme.Play();
				}
			}
			return;
		}

		m_tickTime += DeltaTime::get();
		if (m_tickTime >= TickIntervalSeconds) {
			m_tickTime = 0;
			Tick();
		}
	}

	void ResetBoard() {
		for (Row& row : m_rows) {
			row.Destroy();
		}
		if (m_currentShape.has_value()) {
			m_currentShape.value().Destroy();
			m_currentShape.reset();
		}
		if (m_previewShape.has_value()) {
			m_previewShape.value().Destroy();
			m_previewShape.reset();
		}
		if (m_heldShape.has_value()) {
			m_heldShape.value().Destroy();
			m_heldShape.reset();
		}
	}

public:
	inline static GameManager* Instance;
	GameState GameState = GameState::Menu;

	GameManager(const EntityContext& context) : BehaviorBase(context) {
		Instance = this;
		srand(time(0));

		m_theme.SetLoop(true);
	}

	void ResetGame() {
		SetLevel(0);
		m_lines = 0;
		m_score = 0;
		m_hud->UpdateScore(m_score);
		m_hud->UpdateLines(m_lines);
		ResetBoard();
		m_gameBoard->Hide();
		m_hud->Hide();
		m_theme.Stop();
		GameState = GameState::Menu;
		SetCurrentMenu(m_mainMenu);
	}

	void StartGame() {
		if (m_currentMenu)
			m_currentMenu->Hide();
		m_hud->Show();
		m_gameBoard->Show();
		m_theme.Play();
		GameState = GameState::Playing;
	}

	int GetLevel() { return m_level; }

	int GetScore() { return m_score; }

	int GetLines() { return m_lines; }

	void SetLevel(int level) {
		m_level = level;
		m_hud->UpdateLevel(m_level);
		TickIntervalSeconds = LevelToTickIntervalSec(m_level);
	}

	void IncrementLevel() {
		m_level = std::min(m_maxLevel, m_level + 1);
		m_hud->UpdateLevel(m_level);
		TickIntervalSeconds = LevelToTickIntervalSec(m_level);
	}

	void IncrementScore(int score) {
		m_score += score;
		m_hud->UpdateScore(m_score);
	}

	void IncrementLines(int lines) {
		m_lines += lines;
		m_hud->UpdateLines(m_lines);
	}

	/// <param name="strength">float between 0 and 1</param>
	void ScreenShake(float strength) {
		if (!m_screenShakeEnabled) return;
		m_screenShakeTime = m_screenShakeDurationSec;
		m_screenShakeStrength = std::clamp(strength, 0.0f, 1.0f);
	}

	void FinalizeShape(std::vector<EntityContext>& entities, DropType drop) {
		for (EntityContext& ec : entities) {
			auto& pos = ec.getTransform().getTranslation();
			int rowIndex = GetRowIndex(pos.y);
			int colIndex = GetColumnIndex(pos.x);
			Row& row = m_rows[rowIndex];
			row.AddEntity(colIndex, ec);
		}

		switch (drop)
		{
			case DropType::HardDrop:
				m_hardDropSound.Play();
				break;
			case DropType::Gravity:
			case DropType::SoftDrop:
			default:
				m_clickSound.Play();
				break;
		}

		// Clear current shape
		m_currentShape.reset();
	}

	bool WillOverlap(float x, float y, const Vector3& moveVector) {
		int rowIndex = GetRowIndex(y + moveVector.y);
		if (rowIndex < 0 || rowIndex >= 20)
			return false;
		int colIndex = GetColumnIndex(x + moveVector.x);
		Row& row = m_rows[rowIndex];
		return row.HasBlockAt(colIndex);
	}

	bool WillOverlap(EntityContext& entity, const Vector3& moveVector) {
		auto& pos = entity.getTransform().getTranslation();
		return WillOverlap(pos.x, pos.y, moveVector);
	}

	bool WillOverlap(std::vector<EntityContext>& entities, const Vector3& moveVector) {
		for (EntityContext& ec : entities) {
			if (WillOverlap(ec, moveVector))
				return true;
		}
		return false;
	}

	virtual void act() override {
		switch (GameState)
		{
			case GameState::Menu:
				if (m_currentMenu) m_currentMenu->Act();
				break;
			case GameState::Playing:
				InPlayAct();
				break;
			case GameState::GameOver:
				m_gameOverMenu->Act();
				break;
			case GameState::Paused:
				m_pauseMenu->Act();
				break;
			case GameState::HighScore:
				if (m_currentScoreBoard->IsAddingEntry)
					break;
				if (Keyboard::isKeyPressed(KeyboardKey::keyLeft)) {
					m_currentScoreBoard->Hide();
					m_currentScoreBoard = GetPrevScoreboard();
					m_currentScoreBoard->Show();
				}
				if (Keyboard::isKeyPressed(KeyboardKey::keyRight)) {
					m_currentScoreBoard->Hide();
					m_currentScoreBoard = GetNextScoreboard();
					m_currentScoreBoard->Show();
				}
				if (Keyboard::isKeyPressed(KeyboardKey::keyEscape)) {
					m_currentScoreBoard->Hide();
					ResetGame();
				}
				break;
			default:
				break;
		}
	}

	void InitText() {
		m_gameBoard = std::make_unique<GameBoard>();
		m_hud = std::make_unique<Hud>();
		InitMenus();

		m_classicScoreboard = std::make_unique<Scoreboard>("Classic", "classic.csv");
		m_ghostOnlyScoreboard = std::make_unique<Scoreboard>("Ghost Only", "ghost_only.csv");
		m_holdOnlyScoreboard = std::make_unique<Scoreboard>("Hold Only", "hold_only.csv");
		m_fullModernScoreboard = std::make_unique<Scoreboard>("Full Modern", "full_modern.csv");
	}
};

class QuadBlock {
private:
	EntityContext m_blocks[4];


public:
	QuadBlock(EntityContext& e1, EntityContext& e2, EntityContext& e3, EntityContext& e4) :
		m_blocks{ e1, e2, e3, e4 } {}

	Vector3 GetPos() {
		Vector3 pos;
		for (int i = 0; i < 4; i++) {
			pos += m_blocks[i].getTransform().getTranslation();
		}
		pos /= 4.0f;
		return pos;
	}

	void Translate(Vector3 diff) {
		for (int i = 0; i < 4; i++) {
			m_blocks[i].getTransform().translate(diff, TransformSpace::scene);
		}
	}

	void Destroy() {
		for (EntityContext& ec : m_blocks)
			ec.destroyEntity();
	}
};

class GhostShape {
private:
	std::vector<QuadBlock> m_blocks;

public:
	GhostShape(std::vector<QuadBlock> blocks) {
		m_blocks = std::move(blocks);
	}

	void Destroy() {
		for (QuadBlock& ec : m_blocks)
			ec.Destroy();
	}
};

class Shape : public BehaviorBase {
private:
	std::vector<EntityContext> m_entities;
	float m_tickTime = 0;
	GameManager* m_gameManager;
	bool m_destroyed = false;
	float m_sideBarrier = 5;
	float m_bottomBarrier = -9.5;
	int m_softDroppedBlocks = 0;
	int m_hardDroppedBlocks = 0;
	ShapeType m_type;
	std::optional<GhostShape> m_ghostShape;

	void Fall() {
		if (m_destroyed) return;

		auto& transform = m_context.getTransform();
		bool shouldFinalize = false;
		// Are we hitting lower boundary?
		float lowestY = 20;
		for (EntityContext& ec : m_entities) {
			auto& pos = ec.getTransform().getTranslation();
			if (lowestY > pos.y) {
				lowestY = pos.y;
			}
		}
		shouldFinalize |= lowestY <= m_bottomBarrier;

		// Are we touching some other block?
		shouldFinalize |= m_gameManager->WillOverlap(m_entities, Vector3{ .y = -1 });

		if (shouldFinalize) {
			DropType drop = DropType::Gravity;
			// Fastfall bonus score
			if (m_hardDroppedBlocks > 0) {
				m_gameManager->IncrementScore(m_hardDroppedBlocks * 2);
				m_gameManager->ScreenShake(m_hardDroppedBlocks / 20.0f);
				drop = DropType::HardDrop;
			}
			else if (m_softDroppedBlocks > 0) {
				m_gameManager->IncrementScore(m_softDroppedBlocks);
				drop = DropType::SoftDrop;
			}
			m_gameManager->FinalizeShape(m_entities, drop);
			m_entities.clear();
			// Self-destruct
			m_context.destroyEntity();
			m_destroyed = true;
			ResetGhost();
			return;
		}
		transform.translate(Vector3{ .y = -1 }, TransformSpace::scene);
		for (EntityContext& ec : m_entities) {
			auto& childTransform = ec.getTransform();
			childTransform.translate(Vector3{ .y = -1 }, TransformSpace::scene);
		}
	}

	void FallToEnd() {
		while (!m_destroyed) {
			Fall();
			m_hardDroppedBlocks++;
		}
	}

	bool IsOutsideOfBoundsHorizontal(float x) {
		return x <= -m_sideBarrier || x >= m_sideBarrier;
	}

	bool IsOutsideOfBoundsVertical(float y) {
		return y <= m_bottomBarrier;
	}

	bool CanMove(const Vector3& move) {
		for (EntityContext& ec : m_entities) {
			auto& pos = ec.getTransform().getTranslation();
			if (IsOutsideOfBoundsHorizontal(pos.x + move.x))
				return false;
			if (m_gameManager->WillOverlap(ec, move)) {
				return false;
			}
		}
		return true;
	}

	bool CanRotate(int rotate) {
		if (rotate == 0) return true;
		auto& transform = m_context.getTransform();
		auto& parentTranslate = transform.getTranslation();
		for (EntityContext& ec : m_entities) {
			auto& childTransform = ec.getTransform();
			auto pos = childTransform.getTranslation();
			auto relativeTranslate = pos - parentTranslate;
			Vector3 rotatedTranslation = MathUtilities::rotateVector(relativeTranslate,
				Quaternion::createRotation(Vector3s::forward, 90.0_deg * rotate));
			Vector3 newPos = parentTranslate + rotatedTranslation;
			if (IsOutsideOfBoundsHorizontal(newPos.x)) {
				Logger::info("Cannot rotate: Horizontal Bounds");
				return false;
			}
			if (IsOutsideOfBoundsVertical(newPos.y)) {
				Logger::info("Cannot rotate: Vertical Bounds");
				return false;
			}
			if (m_gameManager->WillOverlap(ec, newPos - pos)) {
				Logger::info("Cannot rotate: Overlap");
				return false;
			}
		}
		return true;
	}

	inline void ResetGhost() {
		if (m_ghostShape.has_value()) {
			m_ghostShape.value().Destroy();
			m_ghostShape.reset();
		}
	}

	void SpawnGhost() {
		std::vector<EntityContext> entities = CreateGhostEntities(m_entities);
		std::vector<QuadBlock> blocks;
		for (int i = 0; i < entities.size(); i += 4) {
			blocks.push_back(QuadBlock(entities[i], entities[i + 1], entities[i + 2], entities[i + 3]));
		}
		float lowestY = 20;
		bool isPlaced = false;
		Vector3 fall = Vector3{ .y = -1 };
		while (!isPlaced) {
			for (int i = 0; i < blocks.size(); i++) {
				Vector3 pos = blocks[i].GetPos();
				if (lowestY > pos.y) {
					lowestY = pos.y;
				}
				isPlaced |= lowestY <= m_bottomBarrier;

				// Are we touching some other block?
				isPlaced |= m_gameManager->WillOverlap(pos.x, pos.y, fall);

				if (isPlaced)
					break;
			}
			if (isPlaced)
				break;
			for (QuadBlock& qb : blocks) {
				qb.Translate(fall);
			}
		}
		m_ghostShape.emplace(GhostShape(blocks));
	}

public:
	Shape(const EntityContext& context, ShapeType type,
		std::vector<EntityContext> entities) : BehaviorBase(context) {
		m_gameManager = GameManager::Instance;
		m_type = type;
		m_entities = std::move(entities);
	}

	virtual void act() override {
		if (m_gameManager->GameState != GameState::Playing) {
			ResetGhost();
			return;
		}
		if (m_destroyed) return;
		if (!m_context.getAppearance().isVisible()) {
			ResetGhost();
			return;
		}
		if (ShowGhost && !m_ghostShape.has_value()) SpawnGhost();

		if (Keyboard::isKeyPressed(HardDropKey)) {
			FallToEnd();
			return;
		}

		float move = Keyboard::isKeyPressedOrRepeated(MoveRightKey) - Keyboard::isKeyPressedOrRepeated(MoveLeftKey);

		int rotate = Keyboard::isKeyPressed(RotateRightKey) - Keyboard::isKeyPressed(RotateLeftKey);

		// Softdrop: Fall faster if arrow down is held
		float tickrate = TickIntervalSeconds;
		if (Keyboard::isKeyHeld(SoftDropKey)) {
			tickrate = fminf(tickrate, 0.05);
		}
		else {
			m_softDroppedBlocks = 0;
		}

		auto& transform = m_context.getTransform();

		if (rotate == 0)
		{
			// If we are not allowed to move, reset move
			if (!CanMove(Vector3{ .x = move }))
				move = 0;
		}
		else
		{
			// If we cannot rotate, reset rotate
			if (!CanRotate(rotate))
				rotate = 0;
		}

		bool performAction = rotate != 0 || move != 0;

		if (performAction) {
			if (ShowGhost) {
				ResetGhost();
			}

			// Move parent shape
			transform.translate(Vector3{ .x = move }, TransformSpace::scene);

			auto& parentTranslate = transform.getTranslation();
			for (EntityContext& ec : m_entities) {
				auto& childTransform = ec.getTransform();
				if (rotate != 0) {
					auto relativeTranslate = childTransform.getTranslation() - parentTranslate;
					Vector3 rotatedTranslation = parentTranslate + MathUtilities::rotateVector(relativeTranslate,
						Quaternion::createRotation(Vector3s::forward, 90.0_deg * rotate));
					rotatedTranslation.z = 0;
					childTransform.setTranslation(rotatedTranslation);
				}
				else {
					childTransform.translate(Vector3{ .x = move }, TransformSpace::scene);
				}
			}
		}

		m_tickTime += DeltaTime::get();
		if (m_tickTime > tickrate) {
			Fall();
			m_tickTime = 0;
			if (Keyboard::isKeyHeld(KeyboardKey::keyDown)) {
				m_softDroppedBlocks++;
			}
		}
	}
};

ShapeWrapper CreateBlock(ShapeType type) {
	Vector2 startPos = { .x = 0, .y = 9 };
	std::vector<EntityContext> entities;
	switch (type) {
		case ShapeType::Line:
			entities = CreateLine(startPos);
			break;
		case ShapeType::Square:
			entities = CreateSquare(startPos);
			break;
		case ShapeType::T:
			entities = CreateT(startPos);
			startPos.x += 0.5;
			startPos.y -= 0.5;
			break;
		case ShapeType::J:
			entities = CreateJ(startPos);
			break;
		case ShapeType::L:
			entities = CreateL(startPos);
			break;
		case ShapeType::S:
			entities = CreateS(startPos);
			break;
		case ShapeType::Z:
			entities = CreateZ(startPos);
			break;
		default:
		{
			Logger::error("Requested unknown shape type");
			exit(-1);
		}
	}
	return ShapeWrapper{
		.Shape = CurrentScene::createAndGetEntity(EntityConfig{
			.transformConfig = TransformConfig{
				.translation = Vector3::createFromVector2(startPos, zIndexBlocks),
				.scale = 0,
			},
			.appearanceConfig = AppearanceConfig{
				.visible = false,
			},
			.behaviorFactory = BehaviorFactory::createFactoryFor<Shape>(type, entities)
		}),
		.Children = entities,
		.SpawnPoint = Vector3::createFromVector2(startPos, zIndexBlocks),
	};
}