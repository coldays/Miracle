#pragma once

#include <optional>
#include <string>

namespace Miracle {
	struct WindowConfig {
		std::optional<std::string> title = std::nullopt;
		int width = 640;
		int height = 480;
	};
}
