#include <Miracle/Miracle.hpp>
#include <thread>
#include <span>
#include "Row.h"
#include <cstdlib>

#include "Tetris.h"

using namespace Miracle;

static void updateTitle() {
	if (!GameManager::Instance->GameOver) {
		Window::setTitle(
			UnicodeConverter::toUtf8(
				CurrentApp::getName()
				+ " - FPS: " + std::to_string(PerformanceCounters::getFps())
				+ " - UPS: " + std::to_string(PerformanceCounters::getUps())
				+ " - Entity count: " + std::to_string(CurrentScene::getEntityCount())
				+ " - Level: " + std::to_string(GameManager::Instance->Level)
				+ " - Lines: " + std::to_string(GameManager::Instance->Lines)
				+ " - Score: " + std::to_string(GameManager::Instance->Score))
		);
	}
	else {
		Window::setTitle(
			UnicodeConverter::toUtf8(
				CurrentApp::getName() + " - " + "GAME OVER!")
		);
	}
}

void CreateGrid() {
	float size = 0.05;
	int limit = 10;
	for (float i = -limit; i <= limit; i++) {
		CurrentScene::createEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = i,
									.y = i,
									.z = 0},
								.scale = Vector3{.x = 100, .y = size, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = ColorRgb::black
							}
			});
		CurrentScene::createEntity(EntityConfig{
						.transformConfig = TransformConfig{
							.translation = Vector3{
								.x = i,
								.y = i,
								.z = 0},
							.scale = Vector3{.x = size, .y = 100, .z = 1 }
						},
						.appearanceConfig = AppearanceConfig{
							.meshIndex = 0,
							.color = ColorRgb::black
						}
			});
	}
}

void CreateBorders() {
	float x = 20.5;
	float xScale = 31;
	CurrentScene::createEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = -x,
									.y = 0,
									.z = 0},
								.scale = Vector3{.x = xScale, .y = 20, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = ColorRgb::black
							}
		});
	CurrentScene::createEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = x,
									.y = 0,
									.z = 0},
								.scale = Vector3{.x = xScale, .y = 20, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = ColorRgb::black
							}
		});
}

void CreateCrosshair() {
	CurrentScene::createEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = 0,
									.y = 0,
									.z = 0},
								.scale = Vector3{.x = 0.5, .y = 0.5, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = ColorRgb::red
							}
		});
}

int main() {
	auto app = App(
		"Tetris",
		AppConfig{
			.windowConfig = WindowConfig{
				.size = WindowSize{
					.width = 800,
					.height = 600
				},
				.resizable = true
			},
			.rendererConfig = RendererConfig{
				.meshes = std::vector<Mesh>{
					{
						.vertices = std::vector{
							Vertex{.position = Vector3{.x = -0.5f, .y = -0.5f, .z = 0.0f } },
							Vertex{.position = Vector3{.x = 0.5f, .y = -0.5f, .z = 0.0f } },
							Vertex{.position = Vector3{.x = 0.5f, .y = 0.5f, .z = 0.0f } },
							Vertex{.position = Vector3{.x = -0.5f, .y = 0.5f, .z = 0.0f } }
						},
						.faces = std::vector{
							Face{.indices = { 0, 1, 2 } },
							Face{.indices = { 0, 2, 3 } }
						}
					}
				}
			},
			.sceneConfig = SceneConfig{
				.entityConfigs = std::vector<EntityConfig>{
					{
						.cameraConfig = OrthographicCameraConfig{
							.zoomFactor = 0.5,
						},
						.behaviorFactory = BehaviorFactory::createFactoryFor<GameManager>(),
					},
				},
				.entityCreatedCallback = [](EntityId) { updateTitle(); },
				.entityDestroyedCallback = [](EntityId) { updateTitle(); }
			},
			.startScript = []() {
				InitSound();
				CreateGrid();
				CreateBorders();
				//CreateCrosshair();
				PerformanceCounters::setCountersUpdatedCallback(updateTitle);
				updateTitle();
			},
			.updateScript = []() {
				if (Keyboard::isKeyPressed(KeyboardKey::keyEscape)) {
					CurrentApp::close();
				}
			}
		}
	);

	int exitCode = app.run();

	DeinitSound();

	return exitCode;
}
