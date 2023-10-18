#pragma once
#include <string>
#include <Miracle/Miracle.hpp>

using namespace Miracle;

void AddTextMeshes(std::vector<Mesh>& meshes);

class Text {
private:
	std::vector<EntityContext> m_entites;
	Vector2 m_pos;
	float m_scale;
	ColorRgb m_color;

	void CreateGlyphEntity(Vector2 pos, float scale, ColorRgb color, char c);

	void CreateText(Vector2 pos, float scale, ColorRgb color, std::string text);

	void Clear();

public:
	Text();
	Text(Vector2 pos, float scale, ColorRgb color, std::string text);
	void ChangeText(std::string text);
	void Move(Vector2 dest);
	void Show();
	void Hide();
};