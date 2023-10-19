#pragma once

#include <Miracle/Miracle.hpp>
#include "Text.h"

using namespace Miracle;

class MenuNode {

public:
	Text NodeText;
	std::function<void()> Action;
	float X;
	float Y;
};

class Menu {
private:
	std::vector<MenuNode> m_menuNodes;
	int m_selectedIndex;
	EntityContext m_selectorIndicator;
	const float m_indicatorOffsetY = 0.25;
	const float m_headerSpacing = 1.5;
	const float m_entrySpacing = 1;
	const float m_entryIndent = 1;
	float m_initialX;
	float m_initialY;
	float m_currentX;
	float m_currentY;
	Text m_header;

	EntityContext InitSelector(float x, float y) {
		return CurrentScene::createAndGetEntity(EntityConfig{
			.transformConfig = TransformConfig{
				.translation = Vector3{
					.x = x + 0.5f,
					.y = y + 0.25f - 1.5f,
					.z = 0
				},
				.scale = Vector3 {.x = 0.25, .y = 0.25 }
			},
			.appearanceConfig = AppearanceConfig{
				.meshIndex = 1,
				.color = ColorRgb::red
			}
		});
	}

	void MoveIndicator() {
		auto& transform = m_selectorIndicator.getTransform();
		auto& translation = transform.getTranslation();
		transform.setTranslation(Vector3{
			.x = translation.x,
			.y = m_menuNodes[m_selectedIndex].Y + m_indicatorOffsetY });
	}

public:
	Menu(std::string header, float x, float y) :
		m_selectorIndicator(InitSelector(x, y)),
		m_header(Vector2{ .x = x, .y = y}, 1.2, ColorRgb::white, header)
	{
		m_initialX = x;
		m_initialY = y;
		m_currentY = y - m_headerSpacing;
		m_currentX = x + m_entryIndent;
	}

	void AddMenuNode(std::string text, std::function<void()> action) {
		float x = m_currentX;
		float y = m_currentY;
		m_menuNodes.push_back(MenuNode{
			.NodeText = Text(Vector2{.x = x, .y = y }, 1, ColorRgb::white, text),
			.Action = action,
			.X = x,
			.Y = y,
			});
		m_currentY -= m_entrySpacing;
	}

	void Act() {
		if (Keyboard::isKeyPressed(KeyboardKey::keyEnter)) {
			m_menuNodes[m_selectedIndex].Action();
			return;
		}

		int dir = Keyboard::isKeyPressedOrRepeated(KeyboardKey::keyDown) - Keyboard::isKeyPressedOrRepeated(KeyboardKey::keyUp);
		if (dir != 0) {
			m_selectedIndex = (m_selectedIndex + m_menuNodes.size() + dir) % m_menuNodes.size();
			MoveIndicator();
		}
	}

	void Show() {
		for (MenuNode& node : m_menuNodes) {
			node.NodeText.Show();
		}
		m_header.Show();
		m_selectorIndicator.getAppearance().setVisible(true);
	}

	void Hide() {
		for (MenuNode& node : m_menuNodes) {
			node.NodeText.Hide();
		}
		m_header.Hide();
		m_selectorIndicator.getAppearance().setVisible(false);
	}
};