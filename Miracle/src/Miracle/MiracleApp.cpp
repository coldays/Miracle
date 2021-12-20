﻿#include <Miracle/components/Miracle/MiracleApp.hpp>

#include <Miracle/components/Miracle/Diagnostics/Logger.hpp>
#include <Miracle/View/Implementations/Window.hpp>
#include <Miracle/Input/Devices/Implementations/Keyboard.hpp>

using namespace Miracle::Diagnostics;
using namespace Miracle::View;
using namespace Miracle::View::Implementations;
using namespace Miracle::Input::Devices::Implementations;

namespace Miracle {
	MiracleApp::MiracleApp(
		const WindowProps& windowProps,
		const std::function<void()>& startScript,
		const std::function<void()>& updateScript
	) :
		m_windowProps(windowProps),
		m_startScript(startScript),
		m_updateScript(updateScript)
	{
		Logger::initialize();
	}

	int MiracleApp::run() const {
		Logger::info("Starting Miracle");

		int exitCode = 0;

		try {
			Window window = Window(m_windowProps);
			Keyboard keyboard = Keyboard(window);

			m_startScript();

			while (!window.shouldClose()) {
				m_updateScript();

				window.update();
			}

			Logger::info("Closing Miracle");
		}
		catch (const WindowError& error) {
			switch (error) {
			case WindowError::InitializationError:
				exitCode = 1;
				break;

			case WindowError::WindowCreationError:
				exitCode = 2;
				break;
			default:
				exitCode = -1;
			}
		}

		Logger::info("Shutting down...");

		return exitCode;
	}
}
