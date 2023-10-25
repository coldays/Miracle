#pragma once

#include <Miracle/Miracle.hpp>
#include "Text.h"
#include "Constants.h"

using namespace Miracle;

class MenuNode {
private:
	std::function<void()> Action;
	Text NodeText;

public:
	MenuNode(std::string text, float x, float y, std::function<void()> action) {
		NodeText = Text(Vector2{ .x = x, .y = y }, 1, ColorRgb::white, text);
		Action = action;
		X = x;
		Y = y;
	}

	void Execute() {
		Action();
	}

	void Show() {
		NodeText.Show();
	}

	void Hide() {
		NodeText.Hide();
	}

	void ChangeText(std::string text) {
		NodeText.ChangeText(text);
	}

	float X;
	float Y;
};

class ToggleMenuNode : public MenuNode {
private:
	std::string EnabledVerb;
	std::string DisabledVerb;
	std::string BaseText;

	void RefreshText() {
		ChangeText(BaseText + " [" + (Enabled ? EnabledVerb : DisabledVerb) + "]");
	}

public:
	bool Enabled = false;

	ToggleMenuNode(std::string text, std::string enabledVerb, std::string disabledVerb,
		bool defaultState, float x, float y, std::function<void()> action) :
		MenuNode(text, x, y, [this, action] { action(); Toggle(); }) {
		BaseText = text;
		EnabledVerb = enabledVerb;
		DisabledVerb = disabledVerb;
		Enabled = defaultState;
		
		RefreshText();
	}

	void Toggle() {
		Enabled = !Enabled;
		RefreshText();
	}
};

class Menu {
private:
	std::vector<std::unique_ptr<MenuNode>> m_menuNodes;
	int m_selectedIndex = 0;
	EntityContext m_selectorIndicator;
	const float m_indicatorOffsetY = 0.25;
	const float m_headerSpacing = 1.5;
	const float m_entrySpacing = 1;
	const float m_entryIndent = 1;
	const float m_animationTime = 0.5;
	float m_time = 0;
	bool m_isHidden = true;
	float m_initialX;
	float m_initialY;
	float m_currentX;
	float m_currentY;
	float m_cursorX;
	float m_cursorDir = 1;
	Text m_header;

	EntityContext InitSelector(float x, float y) {
		m_cursorX = x + 0.5f;
		return CurrentScene::createAndGetEntity(EntityConfig{
			.transformConfig = TransformConfig{
				.translation = Vector3{
					.x = m_cursorX,
					.y = y + 0.25f - 1.5f,
					.z = zIndexGui
				},
				.scale = Vector3 {.x = 0.25, .y = 0.25 }
			},
			.appearanceConfig = AppearanceConfig{
				.visible = false,
				.meshIndex = 1,
				.color = ColorRgb::red,
			}
		});
	}

	void MoveIndicator() {
		auto& transform = m_selectorIndicator.getTransform();
		auto& translation = transform.getTranslation();
		transform.setTranslation(Vector3{
			.x = m_cursorX,
			.y = m_menuNodes[m_selectedIndex]->Y + m_indicatorOffsetY,
			.z = zIndexGui
		});
	}

public:
	Menu(std::string header, float x, float y) :
		m_selectorIndicator(InitSelector(x, y)),
		m_header(Vector2{ .x = x, .y = y}, 1.2, ColorRgb::white, header)
	{
		m_header.Hide();
		m_initialX = x;
		m_initialY = y;
		m_currentY = y - m_headerSpacing;
		m_currentX = x + m_entryIndent;
	}

	void AddMenuNode(std::string text, std::function<void()> action) {
		float x = m_currentX;
		float y = m_currentY;
		m_menuNodes.emplace_back(new MenuNode(text, x, y, action));
		if (m_isHidden) {
			m_menuNodes[m_menuNodes.size() - 1]->Hide();
		}
		m_currentY -= m_entrySpacing;
	}

	void AddToggleMenuNode(std::string text, std::string enabledVerb,
		std::string disabledVerb, bool defaultState, std::function<void()> action) {
		float x = m_currentX;
		float y = m_currentY;
		m_menuNodes.emplace_back(new ToggleMenuNode(text, enabledVerb, disabledVerb,
			defaultState, x, y, action));
		if (m_isHidden) {
			m_menuNodes[m_menuNodes.size() - 1]->Hide();
		}
		m_currentY -= m_entrySpacing;
	}

	void UpdateNodeText(int index, std::string newText) {
		m_menuNodes[index]->ChangeText(newText);
	}

	void Act() {
		if (m_isHidden)
			return;

		m_time += DeltaTime::get();
		if (m_time > m_animationTime) {
			m_time = 0;
			auto& transform = m_selectorIndicator.getTransform();
			auto& translation = transform.getTranslation();
			transform.setTranslation(Vector3{
				.x = m_cursorX + 0.25f * m_cursorDir,
				.y = m_menuNodes[m_selectedIndex]->Y + m_indicatorOffsetY,
				.z = zIndexGui });
			m_cursorDir = -m_cursorDir;
		}

		if (Keyboard::isKeyPressed(KeyboardKey::keyEnter)) {
			m_menuNodes[m_selectedIndex]->Execute();
			return;
		}

		int dir = Keyboard::isKeyPressedOrRepeated(KeyboardKey::keyDown) - Keyboard::isKeyPressedOrRepeated(KeyboardKey::keyUp);
		if (dir != 0) {
			m_selectedIndex = (m_selectedIndex + m_menuNodes.size() + dir) % m_menuNodes.size();
			MoveIndicator();
			m_time = 0;
			m_cursorDir = 1;
		}
	}

	void Show() {
		m_isHidden = false;
		for (auto& node : m_menuNodes) {
			node->Show();
		}
		m_header.Show();
		m_selectorIndicator.getAppearance().setVisible(true);
	}

	void Hide() {
		m_isHidden = true;
		for (auto& node : m_menuNodes) {
			node->Hide();
		}
		m_header.Hide();
		m_selectorIndicator.getAppearance().setVisible(false);
	}
};

