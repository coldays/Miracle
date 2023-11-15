#include <Miracle/Miracle.hpp>
#include <thread>
#include <span>
#include "Row.h"
#include <cstdlib>

#include "Tetris.h"

using namespace Miracle;

static void updateTitle() {
	if (GameManager::Instance->GameState != GameState::GameOver) {
		Window::setTitle(
			UnicodeConverter::toUtf8(
				CurrentApp::getName()
				+ " - FPS: " + std::to_string(PerformanceCounters::getFps())
				+ " - UPS: " + std::to_string(PerformanceCounters::getUps())
				+ " - Entity count: " + std::to_string(CurrentScene::getEntityCount())
				+ " - Level: " + std::to_string(GameManager::Instance->GetLevel())
				+ " - Lines: " + std::to_string(GameManager::Instance->GetLines())
				+ " - Score: " + std::to_string(GameManager::Instance->GetScore()))
		);
	}
	else {
		Window::setTitle(
			UnicodeConverter::toUtf8(
				CurrentApp::getName() + " - " + "GAME OVER!")
		);
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
									.z = zIndexMap
								},
								.scale = Vector3{.x = xScale, .y = 20, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = ColorRgbs::black
							}
		});
	CurrentScene::createEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = x,
									.y = 0,
									.z = zIndexMap
								},
								.scale = Vector3{.x = xScale, .y = 20, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = ColorRgbs::black
							}
		});
}

void CreateCrosshair() {
	CurrentScene::createEntity(EntityConfig{
							.transformConfig = TransformConfig{
								.translation = Vector3{
									.x = 0,
									.y = 0,
									.z = zIndexMap
								},
								.scale = Vector3{.x = 0.5, .y = 0.5, .z = 1 }
							},
							.appearanceConfig = AppearanceConfig{
								.meshIndex = 0,
								.color = ColorRgbs::red
							}
		});
}

int main() {

	// Add normal rectangles
	std::vector<Mesh> meshes = std::vector<Mesh>{
		{ // Rectangle
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
		},
		{ // Triangle
			.vertices = std::vector{
				Vertex{.position = Vector3{.x = -0.5f, .y = -0.5f, .z = 0.0f } },
				Vertex{.position = Vector3{.x = 0.5f, .y = 0.0f, .z = 0.0f } },
				Vertex{.position = Vector3{.x = -0.5f, .y = 0.5f, .z = 0.0f } },
			},
				.faces = std::vector{
					Face{.indices = { 0, 1, 2 } }
			}
		}
	};
	// Add text mesh for text rendering later
	AddTextMeshes(meshes);

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
				.meshes = meshes
			},
			.sceneConfig = SceneConfig{
				.entityConfigs = std::vector<EntityConfig>{
					{
						.cameraConfig = OrthographicCameraConfig{
							.zoomFactor = 0.5,
						},
						/*.transformConfig = TransformConfig {
							.translation = Vector3{.x = 0, .y = 0, .z = -20 },
							.scale = shapeScale,
						},
						.cameraConfig = PerspectiveCameraConfig {
							.fieldOfView = 60.0_deg,
						},*/
						.behaviorFactory = BehaviorFactory::createFactoryFor<GameManager>(),
					},
				},
				.entityCreatedCallback = [](EntityId) { updateTitle(); },
				.entityDestroyedCallback = [](EntityId) { updateTitle(); }
			},
			.startScript = []() {
				InitSound();
				//CreateBorders();
				//CreateCrosshair();
				GameManager::Instance->InitText();
				PerformanceCounters::setCountersUpdatedCallback(updateTitle);
				updateTitle();
			},
			.updateScript = []() {
			}
		}
	);

	int exitCode = app.run();

	DeinitSound();

	return exitCode;
}
