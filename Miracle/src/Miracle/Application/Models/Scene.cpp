#include <Miracle/Application/Models/Scene.hpp>

namespace Miracle::Application {
	Scene::Scene(
		IEcs& ecs,
		const SceneInitProps& initProps
	) :
		m_container(ecs.createContainer()),
		m_backgroundColor(initProps.backgroundColor)
	{
		for (auto& entityConfig : initProps.entityConfigs) {
			m_container->createEntity(entityConfig);
		}
	}

	void Scene::setBackgroundColor(const ColorRgb& color) {
		m_backgroundColor = color;
	}

	void Scene::addEntity(const EntityConfig& config) {
		m_container->createEntity(config);
	}

	void Scene::forEachEntityTransform(const std::function<void(const Matrix4&)>& forEach) const {
		m_container->forEachTransform(forEach);
	}

	void Scene::update() {
		m_container->forEachBehaviour(
			[](Behaviour& behaviour) {
				behaviour.act();
			}
		);
	}
}
