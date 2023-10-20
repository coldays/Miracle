#pragma once

#include <Miracle/Miracle.hpp>

using namespace Miracle;

enum class ShapeType {
	Line, // Green
	Square, // Blue
	T, // Yellow
	J, // Purple
	L, // Red
	S, // Orange
	Z, // Brown
};

Vector3 shapeScale = Vector3{ .x = 0.9, .y = 0.9, .z = 0.9 };

ColorRgb purple = ColorRgb::createFromColorCode(0xaa00ff);
ColorRgb orange = ColorRgb::createFromColorCode(0xFF7F27);
ColorRgb brown = ColorRgb::createFromColorCode(0x00ffdd);

std::vector<EntityContext> CreateLine(Vector2& startPos) {
	float startX = -2.5 + startPos.x;
	float startY = -0.5 + startPos.y;
	ColorRgb line_color = ColorRgb::green;
	std::vector<EntityContext> entities;
	for (int i = 0; i < 4; i++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
						.transformConfig = TransformConfig {
							.translation = Vector3{.x = ++startX, .y = startY, .z = 0 },
							.scale = shapeScale,
						},
						.appearanceConfig = AppearanceConfig{
							.meshIndex = 0,
							.color = line_color
						}
			}));
	}
	return entities;
}

std::vector<EntityContext> CreateSquare(Vector2& startPos) {
	float startX = -.5 + startPos.x;
	float startY = -.5 + startPos.y;
	ColorRgb line_color = ColorRgb::blue;
	std::vector<EntityContext> entities;
	for (float x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY + y,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
				}));
		}
	}
	return entities;
}

std::vector<EntityContext> CreateT(Vector2& startPos) {
	float startX = -0.5 + startPos.x;
	float startY = -0.5 + startPos.y;
	ColorRgb line_color = ColorRgb::yellow;
	std::vector<EntityContext> entities;
	entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + 1,
									.y = startY + 1,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
		}));
	for (float x = 0; x < 3; x++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
			}));
	}
	return entities;
}

std::vector<EntityContext> CreateJ(Vector2& startPos) {
	float startX = -0.5 + startPos.x;
	float startY = -0.5 + startPos.y;
	ColorRgb line_color = purple;
	std::vector<EntityContext> entities;
	entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX,
									.y = startY + 1,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
		}));
	for (float x = 0; x < 3; x++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
			}));
	}
	return entities;
}

std::vector<EntityContext> CreateL(Vector2& startPos) {
	float startX = -1.5 + startPos.x;
	float startY = -0.5 + startPos.y;
	ColorRgb line_color = ColorRgb::red;
	std::vector<EntityContext> entities;
	entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + 2,
									.y = startY + 1,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
		}));
	for (float x = 0; x < 3; x++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
			}));
	}
	return entities;
}

std::vector<EntityContext> CreateS(Vector2& startPos) {
	float startX = -0.5 + startPos.x;
	float startY = -0.5 + startPos.y;
	ColorRgb line_color = orange;
	std::vector<EntityContext> entities;
	for (float x = 1; x < 3; x++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY + 1,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
			}));
	}
	for (float x = 0; x < 2; x++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
			}));
	}
	return entities;
}

std::vector<EntityContext> CreateZ(Vector2& startPos) {
	float startX = -0.5 + startPos.x;
	float startY = -0.5 + startPos.y;
	ColorRgb line_color = brown;
	std::vector<EntityContext> entities;
	for (float x = 0; x < 2; x++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY + 1,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
			}));
	}
	for (float x = 1; x < 3; x++) {
		entities.push_back(CurrentScene::createAndGetEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = startX + x,
									.y = startY,
									.z = 0},
								.scale = shapeScale
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = line_color
							}
			}));
	}
	return entities;
}