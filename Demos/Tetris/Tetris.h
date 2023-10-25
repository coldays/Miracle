#pragma once

#include <Miracle/Miracle.hpp>
#include "Row.h"
#include "Shapes.h"
#include "Sound.h"
#include "Text.h"
#include <algorithm>
#include "Menu.h"

using namespace Miracle;

struct ShapeWrapper {
	EntityContext Shape;
	std::vector<EntityContext> Children;

	void Destroy() {
		Shape.destroyEntity();
		for (EntityContext& ec : Children) {
			ec.destroyEntity();
		}
	}

	void Show() {
		Shape.getAppearance().setVisible(true);
		for (EntityContext& ec : Children) {
			ec.getAppearance().setVisible(true);
		}
	}

	void Hide() {
		Shape.getAppearance().setVisible(false);
		for (EntityContext& ec : Children) {
			ec.getAppearance().setVisible(false);
		}
	}
};

float zIndexGui = 0;
float zIndexBlocks = 1;
float zIndexMap = 2;

class GameManager;
ShapeWrapper CreateBlock(ShapeType);

std::vector<EntityContext> Grid;

struct KeyboardConfig {
	KeyboardKey MoveLeftKey = KeyboardKey::keyA;
	KeyboardKey MoveRightKey = KeyboardKey::keyD;
	KeyboardKey RotateLeftKey = KeyboardKey::keyS;
	KeyboardKey RotateRightKey = KeyboardKey::keyW;
	KeyboardKey HardDropKey = KeyboardKey::keySpace;
	KeyboardKey SoftDropKey = KeyboardKey::keyDown;
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

void SwapKeyConfig(KeyboardConfig config) {
	MoveLeftKey = config.MoveLeftKey;
	MoveRightKey = config.MoveRightKey;
	RotateLeftKey = config.RotateLeftKey;
	RotateRightKey = config.RotateRightKey;
	SoftDropKey = config.SoftDropKey;
	HardDropKey = config.HardDropKey;
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
	Paused
};

class GameManager : public Behavior {
private:
	const int m_maxLevel = 20;
	const int m_height = 20;
	int m_score = 0;
	int m_lines = 0;
	int m_level = 0;
	Row m_rows[20];
	float m_tickTime = 0;
	std::optional<ShapeWrapper> m_currentShape;
	const float m_clearPause = 0.8f;
	float m_clearPauseTime = 0;
	bool m_showGrid = true;
	bool m_screenShakeEnabled = true;
	float m_screenShakeTime = 0;
	float m_screenShakeStrength = 0;
	Sound m_theme = Sound("Theme.wav");
	Sound m_clickSound = Sound("Click.wav");
	Sound m_rowCompleteSound = Sound("Row.wav");
	Sound m_tetrisSound = Sound("Tetris.wav");
	Sound m_gameOverSound = Sound("GameOver.wav");
	//Sound m_levelUpSound = Sound("LevelUp.wav");
	Text m_scoreHeader;
	Text m_scoreText;
	Text m_levelHeader;
	Text m_levelText;
	Text m_LinesHeader;
	Text m_linesText;
	std::unique_ptr<Menu> m_mainMenu;
	std::unique_ptr<Menu> m_levelMenu;
	std::unique_ptr<Menu> m_optionsMenu;
	std::unique_ptr<Menu> m_keyConfMenu;
	std::unique_ptr<Menu> m_gameOverMenu;
	Menu* m_currentMenu = nullptr;

	void SetCurrentMenu(std::unique_ptr<Menu>& menu) {
		if (m_currentMenu)
			m_currentMenu->Hide();
		m_currentMenu = menu.get();
		m_currentMenu->Show();
	}

	void InitMenus() {
		float x = -12;
		float y = 8;

		m_levelMenu = std::make_unique<Menu>("Level", x, y);
		m_levelMenu->AddMenuNode("Config: Andreas", [this] {
			if (isAndreasConfig) {
				SwapKeyConfig(CristinaConfig);
				m_levelMenu->UpdateNodeText(0, "Config: Cristina");
			}
			else {
				SwapKeyConfig(AndreasConfig);
				m_levelMenu->UpdateNodeText(0, "Config: Andreas");
			}
			isAndreasConfig = !isAndreasConfig;
		});
		for (int i = 0; i < 10; i++) {
			m_levelMenu->AddMenuNode(std::to_string(i), [this, i] { SetLevel(i); StartGame(); });
		}

		m_keyConfMenu = std::make_unique<Menu>("Key config", x, y);
		m_keyConfMenu->AddMenuNode("Back", [this] { SetCurrentMenu(m_optionsMenu); });
		m_keyConfMenu->AddMenuNode("Move Left: " + GetKeyName(MoveLeftKey), [] {});
		m_keyConfMenu->AddMenuNode("Move Right: " + GetKeyName(MoveRightKey), [] {});
		m_keyConfMenu->AddMenuNode("Rotate Left: " + GetKeyName(RotateLeftKey), [] {});
		m_keyConfMenu->AddMenuNode("Rotate Right: " + GetKeyName(RotateRightKey), [] {});
		m_keyConfMenu->AddMenuNode("Soft Drop: " + GetKeyName(SoftDropKey), [] {});
		m_keyConfMenu->AddMenuNode("Hard Drop: " + GetKeyName(HardDropKey), [] {});

		m_optionsMenu = std::make_unique<Menu>("Options", x, y);
		m_optionsMenu->AddMenuNode("Back", [this] { SetCurrentMenu(m_mainMenu); });
		m_optionsMenu->AddMenuNode("Key config", [this] { SetCurrentMenu(m_keyConfMenu); });
		m_optionsMenu->AddToggleMenuNode("Grid", "Shown", "Hidden", m_showGrid, [this] { ToggleGrid(); });
		m_optionsMenu->AddToggleMenuNode("Screen shake", "On", "Off", m_screenShakeEnabled, [this] { m_screenShakeEnabled = !m_screenShakeEnabled; });
		
		m_mainMenu = std::make_unique<Menu>("Tetris", x, y);
		m_mainMenu->AddMenuNode("Play", [this] { SetCurrentMenu(m_levelMenu);  });
		m_mainMenu->AddMenuNode("Options", [this] { SetCurrentMenu(m_optionsMenu); });
		m_mainMenu->AddMenuNode("Exit", [] { CurrentApp::close(); });
		
		m_gameOverMenu = std::make_unique<Menu>("Game Over!", x - 1, y);
		m_gameOverMenu->AddMenuNode("Restart", [this] { ResetGame(); });
		m_gameOverMenu->AddMenuNode("Exit", [] { CurrentApp::close(); });

		SetCurrentMenu(m_mainMenu);
	}

	void ToggleGrid() {
		bool show = !m_showGrid;
		for (int i = 1; i < Grid.size() - 1; i++) {
			if (i == 10) continue;
			if (i == 11) continue;
			Grid[i].getAppearance().setVisible(show);
		}
		m_showGrid = show;
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
			} else {
				m_theme.Pause();
				m_tetrisSound.Play();
			}
			return;
		}

		if (!m_currentShape.has_value()) {
			m_currentShape.emplace(CreateBlock((ShapeType)(rand() % 7)));
			//m_currentShape.emplace(CreateBlock(ShapeType::Z));
			// Check if spawned block overlaps with any row
			if (WillOverlap(m_currentShape.value().Children, Vector3{})) {
				GameState = GameState::GameOver;
				Logger::info("**** GAME OVER! ****");
				Logger::info(std::string("\tScore: ") + std::to_string(m_score));
				Logger::info(std::string("\tLevel: ") + std::to_string(m_level));
				Logger::info(std::string("\tLines: ") + std::to_string(m_lines));
				m_theme.Stop();
				m_gameOverSound.Play();
				SetCurrentMenu(m_gameOverMenu);
				return;
			}
		}
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
			// m_levelUpSound.Play();
		}
		return removed;
	}

	int GetRowIndex(float yPos) {
		return yPos + 9.5f;
	}

	int GetColumnIndex(float xPos) {
		return xPos + 4.5f;
	}

	void InitGameHud() {
		float x = 5.5;
		float xIndent = 0.5;
		float y = 8;
		float headerSpacing = 2;
		float spacing = 1;
		//Text(Vector2{ .x = -7.5 }, 1, ColorRgb::white, "0123456789 amglevcorints");
		m_scoreHeader = Text(Vector2{ .x = x, .y = y }, 1, ColorRgb::white, "Score:");
		y -= spacing;
		m_scoreText = Text(Vector2{ .x = x + xIndent, .y = y }, 1, ColorRgb::white, "0");
		y -= headerSpacing;
		m_levelHeader = Text(Vector2{ .x = x, .y = y }, 1, ColorRgb::white, "Level:");
		y -= spacing;
		m_levelText = Text(Vector2{ .x = x + xIndent, .y = y }, 1, ColorRgb::white, "0");
		y -= headerSpacing;
		m_LinesHeader = Text(Vector2{ .x = x, .y = y }, 1, ColorRgb::white, "Lines:");
		y -= spacing;
		m_linesText = Text(Vector2{ .x = x + xIndent, .y = y }, 1, ColorRgb::white, "0");
	}

	void TogglePause() {
		switch (GameState) {
			case GameState::Playing:
				// Hide all blocks
				for (Row& row : m_rows) {
					row.Hide();
				}
				if (m_currentShape.has_value()) {
					m_currentShape.value().Hide();
				}
				GameState = GameState::Paused;
				break;
			case GameState::Paused:
				// Show all blocks
				for (Row& row : m_rows) {
					row.Show();
				}
				if (m_currentShape.has_value()) {
					m_currentShape.value().Show();
				}
				GameState = GameState::Playing;
				break;
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
		if (Keyboard::isKeyPressed(KeyboardKey::keyEscape)) {
			TogglePause();
			return;
		}

		CameraAct();

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

public:
	inline static GameManager* Instance;
	GameState GameState = GameState::Menu;

	GameManager(const EntityContext& context) : Behavior(context) {
		Instance = this;
		srand(time(0));

		m_theme.SetLoop(true);
	}

	void ResetGame() {
		SetLevel(0);
		m_lines = 0;
		m_score = 0;
		m_scoreText.ChangeText(std::to_string(m_score));
		m_linesText.ChangeText(std::to_string(m_lines));
		for (Row& row : m_rows) {
			row.Destroy();
		}
		if (m_currentShape.has_value()) {
			m_currentShape.value().Destroy();
			m_currentShape.reset();
		}
		GameState = GameState::Menu;
		SetCurrentMenu(m_levelMenu);
	}

	void StartGame() {
		if (m_currentMenu)
			m_currentMenu->Hide();
		m_theme.Play();
		GameState = GameState::Playing;
	}

	int GetLevel() { return m_level; }

	int GetScore() { return m_score; }

	int GetLines() { return m_lines; }

	void SetLevel(int level) {
		m_level = level;
		m_levelText.ChangeText(std::to_string(m_level));
		TickIntervalSeconds = LevelToTickIntervalSec(m_level);
	}

	void IncrementLevel() {
		m_level = std::min(m_maxLevel, m_level + 1);
		m_levelText.ChangeText(std::to_string(m_level));
		TickIntervalSeconds = LevelToTickIntervalSec(m_level);
	}

	void IncrementScore(int score) {
		m_score += score;
		m_scoreText.ChangeText(std::to_string(m_score));
	}

	void IncrementLines(int lines) {
		m_lines += lines;
		m_linesText.ChangeText(std::to_string(m_lines));
	}

	/// <param name="strength">float between 0 and 1</param>
	void ScreenShake(float strength) {
		if (!m_screenShakeEnabled) return;
		m_screenShakeTime = 0.2f;
		m_screenShakeStrength = std::clamp(strength, 0.0f, 1.0f);
	}

	void FinalizeShape(std::vector<EntityContext>& entities) {
		for (EntityContext& ec : entities) {
			auto& pos = ec.getTransform().getTranslation();
			int rowIndex = GetRowIndex(pos.y);
			int colIndex = GetColumnIndex(pos.x);
			Row& row = m_rows[rowIndex];
			row.AddEntity(colIndex, ec);
			m_clickSound.Play();
		}

		// Clear current shape
		m_currentShape.reset();
	}

	bool WillOverlap(EntityContext& entity, const Vector3& moveVector) {
		auto& pos = entity.getTransform().getTranslation();
		int rowIndex = GetRowIndex(pos.y + moveVector.y);
		if (rowIndex < 0 || rowIndex >= 20)
			return false;
		int colIndex = GetColumnIndex(pos.x + moveVector.x);
		Row& row = m_rows[rowIndex];
		return row.HasBlockAt(colIndex);
	}

	bool WillOverlap(std::vector<EntityContext>& entities, const Vector3& moveVector) {
		for (EntityContext& ec : entities) {
			if (WillOverlap(ec, moveVector))
				return true;
		}
		return false;
	}

	virtual void act() override {
		/*if (true) {
			auto& transform = m_context.getTransform();
			transform.translate(Vector3{ .x = 1, .z = 1 } * DeltaTime::get(), TransformSpace::scene);
			transform.rotate(Quaternion::createRotation(Vector3::up, -5.0_deg * DeltaTime::get()));
		}*/
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
				if (Keyboard::isKeyPressed(KeyboardKey::keyEscape)) {
					TogglePause();
				}
				break;
			default:
				break;
		}
		
	}

	void InitText() {
		InitGameHud();
		InitMenus();
	}
};

class Shape : public Behavior {
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
			m_gameManager->FinalizeShape(m_entities);
			m_entities.clear();
			// Fastfall bonus score
			if (m_hardDroppedBlocks > 0) {
				m_gameManager->IncrementScore(m_hardDroppedBlocks * 2);
				m_gameManager->ScreenShake(m_hardDroppedBlocks / 20.0f);
				
			} else if (m_softDroppedBlocks > 0) {
				m_gameManager->IncrementScore(m_softDroppedBlocks);
			}
			// Self-destruct
			m_context.destroyEntity();
			m_destroyed = true;
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
				Quaternion::createRotation(Vector3::forward, 90.0_deg * rotate));
			Vector3 newPos = parentTranslate + rotatedTranslation;
			if (IsOutsideOfBoundsHorizontal(newPos.x)) {
				Logger::info("Cannot rotate: Horizontal Bounds");
				return false;
			}
			if (IsOutsideOfBoundsVertical(newPos.y)) {
				Logger::info("Cannot rotate: Vertical Bounds");
				return false;
			}
			if (m_gameManager->WillOverlap(ec, newPos - pos )) {
				Logger::info("Cannot rotate: Overlap");
				return false;
			}
		}
		return true;
	}

public:
	Shape(const EntityContext& context, ShapeType type, std::vector<EntityContext> entities) : Behavior(context) {
		m_gameManager = GameManager::Instance;
		m_type = type;
		m_entities = std::move(entities);
	}

	virtual void act() override {
		if (m_gameManager->GameState != GameState::Playing)
			return;
		if (m_destroyed) return;

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
		} else {
			m_softDroppedBlocks = 0;
		}

		auto& transform = m_context.getTransform();

		if (rotate == 0)
		{
			// If we are not allowed to move, reset move
			if (!CanMove(Vector3{ .x = move }))
				move = 0;

			transform.translate(Vector3{ .x = move }, TransformSpace::scene);
		}
		else
		{
			// If we cannot rotate, reset rotate
			if (!CanRotate(rotate))
				rotate = 0;
		}

		auto& parentTranslate = transform.getTranslation();
		for (EntityContext& ec : m_entities) {
			auto& childTransform = ec.getTransform();
			if (rotate != 0) {
				auto relativeTranslate = childTransform.getTranslation() - parentTranslate;
				Vector3 rotatedTranslation = MathUtilities::rotateVector(relativeTranslate,
					Quaternion::createRotation(Vector3::forward, 90.0_deg * rotate));
				childTransform.setTranslation(parentTranslate + rotatedTranslation);
			}
			else {
				childTransform.translate(Vector3{ .x = move }, TransformSpace::scene);
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
	Vector2 startPos = Vector2{ .x = 0, .y = 9 };
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
	EntityContext shape = CurrentScene::createAndGetEntity(EntityConfig{
		.transformConfig = TransformConfig{
			.translation = Vector3::createFromVector2(startPos, 0),
		},
		/*.appearanceConfig = AppearanceConfig{
			.meshIndex = 0,
			.color = ColorRgb::red
		},*/
		.behaviorFactory = BehaviorFactory::createFactoryFor<Shape>(type, entities)
		});
	return ShapeWrapper{ .Shape = shape, .Children = entities };
}