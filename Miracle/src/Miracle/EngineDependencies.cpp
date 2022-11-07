#include <Miracle/EngineDependencies.hpp>

#include <Miracle/Common/Mappings.hpp>
#include <Miracle/Common/UnicodeConverter.hpp>
#include "Infrastructure/Persistance/FileSystem/FileAccess.hpp"
#include "Infrastructure/Framework/Glfw/MultimediaFramework.hpp"
#include "Infrastructure/View/Glfw/Window.hpp"
#include "Infrastructure/Input/Glfw/Keyboard.hpp"
#include "Infrastructure/Graphics/Vulkan/GraphicsApi.hpp"
#include "Infrastructure/Graphics/Vulkan/GraphicsContext.hpp"

namespace Miracle {
	using FileSystemFileAccess = Infrastructure::Persistance::FileSystem::FileAccess;
	using GlfwMultimediaFramework = Infrastructure::Framework::Glfw::MultimediaFramework;
	using GlfwWindow = Infrastructure::View::Glfw::Window;
	using GlfwKeyboard = Infrastructure::Input::Glfw::Keyboard;
	using VulkanGraphicsApi = Infrastructure::Graphics::Vulkan::GraphicsApi;
	using VulkanGraphicsContext = Infrastructure::Graphics::Vulkan::GraphicsContext;

	EngineDependencies::EngineDependencies(
		const std::string_view& appName,
		const WindowConfig& windowConfig,
		const RendererConfig& rendererConfig,
		Application::ILogger& logger,
		Application::EventDispatcher& eventDispatcher
	) :
		m_fileAccess(
			std::make_unique<FileSystemFileAccess>(logger)
		),
		m_multimediaFramework(
			std::make_unique<GlfwMultimediaFramework>(logger)
		),
		m_window(
			std::make_unique<GlfwWindow>(
				logger,
				eventDispatcher,
				*reinterpret_cast<GlfwMultimediaFramework*>(m_multimediaFramework.get()),
				Mappings::toWindowInitProps(windowConfig, UnicodeConverter::toUtf8(appName))
				)
		),
		m_keyboard(
			std::make_unique<GlfwKeyboard>(
				eventDispatcher,
				*m_multimediaFramework.get(),
				*reinterpret_cast<GlfwWindow*>(m_window.get())
				)
		),
		m_graphicsApi(
			std::make_unique<VulkanGraphicsApi>(logger)
		),
		m_graphicsContext(
			m_graphicsApi->createGraphicsContext(
				appName,
				*reinterpret_cast<GlfwWindow*>(m_window.get())
			)
		),
		m_renderer(
			logger,
			*m_fileAccess.get(),
			*m_graphicsApi.get(),
			*m_graphicsContext.get(),
			Mappings::toRendererInitProps(rendererConfig)
		),
		m_textInputService(eventDispatcher),
		m_deltaTimeService(*m_multimediaFramework.get()),
		m_performanceCountingService(*m_multimediaFramework.get())
	{}
}
