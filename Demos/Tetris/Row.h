#pragma once

#include <Miracle/Miracle.hpp>

using namespace Miracle;

class Row {
private:
	std::optional<EntityContext> m_entities[10];
	std::unordered_map<int, ColorRgb> m_entityColorMap;
	const ColorRgb m_clearColor = ColorRgbs::white;
	// Set to max so that we trigger animation on first frame
	float m_tickTime = FLT_MAX;

	void SwapColor() {
		for (int i = 0; i < 10; i++) {
			if (m_entities[i].has_value()) {
				Appearance& a = m_entities[i].value().getAppearance();
				ColorRgb swapColor = m_entityColorMap[i];
				m_entityColorMap[i] = a.getColor();
				a.setColor(swapColor);
			}
		}
	}

public:
	bool MarkedForDeletion = false;

	Row() {
		for (int i = 0; i < 10; i++) {
			m_entityColorMap[i] = m_clearColor;
		}
	}

	void Show() {
		for (int i = 0; i < 10; i++) {
			if (m_entities[i].has_value()) {
				m_entities[i].value().getAppearance().setVisible(true);
			}
		}
	}

	void Hide() {
		for (int i = 0; i < 10; i++) {
			if (m_entities[i].has_value()) {
				m_entities[i].value().getAppearance().setVisible(false);
			}
		}
	}

	void AddEntity(int index, EntityContext& entity) {
		if (m_entities[index].has_value()) {
			Logger::error("Entity placed in row twice");
		}
		m_entities[index].emplace(entity);
	}

	void Destroy() {
		for (int i = 0; i < 10; i++) {
			m_entityColorMap[i] = m_clearColor;
			if (m_entities[i].has_value()) {
				m_entities[i].value().destroyEntity();
				m_entities[i].reset();
			}
		}
		MarkedForDeletion = false;
		m_tickTime = FLT_MAX;
	}

	void TransferTo(Row& other) {
		for (int i = 0; i < 10; i++) {
			if (m_entities[i].has_value()) {
				EntityContext& ec = m_entities[i].value();
				m_entities[i].reset();
				other.AddEntity(i, ec);
				// Correct visual position
				Transform& transform = ec.getTransform();
				transform.translate(Vector3{ .y = -1 }, TransformSpace::scene);
			}
		}
	}

	bool IsFull() {
		for (int i = 0; i < 10; i++) {
			if (!m_entities[i].has_value()) {
				return false;
			}
		}
		return true;
	}

	bool HasBlockAt(int index) {
		return m_entities[index].has_value();
	}

	void UpdateVisual(float tickInterval) {
		if (!MarkedForDeletion) return;
		m_tickTime += DeltaTime::get();
		if (m_tickTime >= tickInterval) {
			m_tickTime = 0;
			SwapColor();
		}
	}
};