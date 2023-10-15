#pragma once

#include <Miracle/Miracle.hpp>
#include "Row.h"
#include "Shapes.h"
#include "Sound.h"

//#define DEBUG_TEST 1

using namespace Miracle;

class GameManager;
EntityContext CreateBlock(ShapeType);

float TickIntervalSeconds = 0.5;

class GameManager : public Behavior {
private:
	const int m_height = 20;
	Row m_rows[20];
	float m_tickTime = 0;
	std::optional<EntityContext> m_currentShape;
	const float m_clearPause = 1.0f;
	float m_clearPauseTime = 0;
	Sound m_theme = Sound("Theme.wav");
	Sound m_clickSound = Sound("Click.wav");
	Sound m_rowCompleteSound = Sound("Row.wav");
	Sound m_tetrisSound = Sound("Tetris.wav");
	Sound m_gameOverSound = Sound("GameOver.wav");
	//Sound m_levelUpSound = Sound("LevelUp.wav");

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
			m_theme.Pause();
			if (fullRows < 4) {
				m_rowCompleteSound.Play();
			} else {
				m_tetrisSound.Play();
			}
			return;
		}

		if (!m_currentShape.has_value()) {
			m_currentShape.emplace(CreateBlock((ShapeType)(rand() % 6)));
			//m_currentShape.emplace(CreateBlock(ShapeType::T));
			// Check if spawned block overlaps with any row
			if (WillOverlap(m_currentShape.value(), Vector3{})) {
				GameOver = true;
				Logger::info("**** GAME OVER! ****");
				m_theme.Stop();
				m_gameOverSound.Play();
				return;
			}
		}
	}

	void ClearFullRows() {
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
		Lines += removed;
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
		Score += removed * (Level + 1);
		if (Lines >= (Level + 1) * 10) {
			Level++;
			TickIntervalSeconds *= .8f;
			// m_levelUpSound.Play();
		}
	}

	int GetRowIndex(float yPos) {
		return yPos + 9.5f;
	}

	int GetColumnIndex(float xPos) {
		return xPos + 4.5f;
	}

public:
	inline static GameManager* Instance;
	bool GameOver = false;
	int Score = 0;
	int Level = 0;
	int Lines = 0;

	GameManager(const EntityContext& context) : Behavior(context) {
		Instance = this;
		srand(time(0));

		m_theme.SetLoop(true);
		m_theme.Play();
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
		if (GameOver)
			return;

		if (m_clearPauseTime > 0) {
			const float numberOfFlashes = 4;
			for (int i = 0; i < m_height; i++) {
				m_rows[i].UpdateVisual(m_clearPause/numberOfFlashes);
			}
			m_clearPauseTime -= DeltaTime::get();
			if (m_clearPauseTime <= 0) {
				ClearFullRows();
				m_theme.Play();
			}
			return;
		}

		m_tickTime += DeltaTime::get();
		if (m_tickTime >= TickIntervalSeconds) {
			m_tickTime = 0;
			Tick();
		}

#ifdef DEBUG_TEST
		if (Keyboard::isKeyPressed(KeyboardKey::key1)) {
			CreateBlock(this, ShapeType::Square);
		}

		if (Keyboard::isKeyPressed(KeyboardKey::key2)) {
			CreateBlock(this, ShapeType::Line);
		}

		if (Keyboard::isKeyPressed(KeyboardKey::key3)) {
			CreateBlock(this, ShapeType::T);
		}

		if (Keyboard::isKeyPressed(KeyboardKey::key4)) {
			CreateBlock(this, ShapeType::J);
		}

		if (Keyboard::isKeyPressed(KeyboardKey::key5)) {
			CreateBlock(this, ShapeType::L);
		}

		if (Keyboard::isKeyPressed(KeyboardKey::key6)) {
			CreateBlock(this, ShapeType::S);
		}

		if (Keyboard::isKeyPressed(KeyboardKey::key7)) {
			CreateBlock(this, ShapeType::Z);
		}
#endif
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
	Shape(const EntityContext& context, std::vector<EntityContext> entities) : Behavior(context) {
		m_gameManager = GameManager::Instance;
		m_entities = std::move(entities);
	}

	virtual void act() override {
		if (m_gameManager->GameOver)
			return;
		if (m_destroyed) return;

		if (Keyboard::isKeyPressed(KeyboardKey::keySpace)) {
			FallToEnd();
			return;
		}

		float move = Keyboard::isKeyPressed(KeyboardKey::keyD) - Keyboard::isKeyPressed(KeyboardKey::keyA);

		int rotate = Keyboard::isKeyPressed(KeyboardKey::keyW) - Keyboard::isKeyPressed(KeyboardKey::keyS);

		// Fall faster if arrow down is held
		float tickrate = TickIntervalSeconds;
		if (Keyboard::isKeyHeld(KeyboardKey::keyDown)) {
			tickrate = fminf(tickrate, 0.05);
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
		}
	}
};

EntityContext CreateBlock(ShapeType type) {
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
			std::exit(-1);
	}
	return CurrentScene::createAndGetEntity(EntityConfig{
		.transformConfig = TransformConfig{
			.translation = Vector3::createFromVector2(startPos, 0),
		},
		/*.appearanceConfig = AppearanceConfig{
			.meshIndex = 0,
			.color = ColorRgb::red
		},*/
		.behaviorFactory = BehaviorFactory::createFactoryFor<Shape>(entities)
		});
}