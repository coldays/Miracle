#pragma once
#include <string>
#include <Miracle/Miracle.hpp>

using namespace Miracle;

class Text {
private:
	std::vector<EntityContext> m_entites;

	void CreateQuestionMark(Vector2 pos, float scale, ColorRgb color) {
		CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = pos.x,
									.y = pos.y,
									.z = 0},
								.scale = scale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = color
							}
			});
	}

	void AddChar(Vector2 pos, float scale, ColorRgb color, char c) {
		switch (c) {
			case '?':
			default:
				CreateQuestionMark(pos, scale, color);
				break;
		}
	}

public:
	Text(Vector2 pos, float scale, ColorRgb color, std::string text) {

		Vector2 currentPos = pos;
		for (char c : text) {
			AddChar(currentPos, scale, color, c);
			currentPos.x += 10 * scale;
		}
	}

	void ChangeText(std::string text) {

	}

	void Move(Vector2 dest) {

	}


};