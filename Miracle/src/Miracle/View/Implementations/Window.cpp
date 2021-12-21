#include "Window.hpp"

#include <fmt/format.h>

#include <Miracle/components/Miracle/Diagnostics/Logger.hpp>

using namespace Miracle::Diagnostics;

namespace Miracle::View::Implementations {
	Window::Window(const WindowProps& props) {
		auto error = initializeGlfw();

		if (error.has_value()) {
			throw error.value();
		}

		glfwSetErrorCallback(
			[](int errorCode, const char* description) {
				Logger::error(description);
			}
		);

		auto primaryMonitor = glfwGetPrimaryMonitor();
		auto monitorVideoMode = glfwGetVideoMode(primaryMonitor);

		glfwWindowHint(GLFW_RESIZABLE, !props.fullscreen && props.resizable);
		glfwWindowHint(GLFW_VISIBLE, false);

		int windowWidth = !props.fullscreen ? props.width : monitorVideoMode->width;
		int windowHeight = !props.fullscreen ? props.height : monitorVideoMode->height;

		m_window = glfwCreateWindow(
			windowWidth,
			windowHeight,
			reinterpret_cast<const char*>(props.title.c_str()),
			props.fullscreen ? primaryMonitor : nullptr,
			nullptr
		);

		if (m_window == nullptr) {
			Logger::error("Failed to create application window!");

			throw WindowError::WindowCreationError;
		}

		Logger::info("Application window created");

		glfwSetWindowRefreshCallback(
			m_window,
			[](GLFWwindow* window) {
				glfwSwapBuffers(window);
			}
		);

		glfwSetWindowPos(
			m_window,
			(monitorVideoMode->width - windowWidth) / 2,
			(monitorVideoMode->height - windowHeight) / 2
		);

		glfwShowWindow(m_window);

		Logger::info("Showing application window");
	}

	Window::~Window() {
		glfwDestroyWindow(m_window);
		glfwTerminate();

		Logger::info("Terminated GLFW");
	}

	void Window::update() const {
		glfwPollEvents();
	}

	bool Window::shouldClose() const {
		return glfwWindowShouldClose(m_window);
	}

	void Window::close() const {
		glfwSetWindowShouldClose(m_window, true);
	}

	std::optional<WindowError> Window::initializeGlfw() const {
		Logger::info(fmt::format("Initializing GLFW version: {}", glfwGetVersionString()));

		bool initialized = glfwInit();

		if (!initialized) {
			Logger::error("Failed to initialize GLFW!");

			return WindowError::InitializationError;
		}

		Logger::info("GLFW initialized");

		return std::nullopt;
	}
}
