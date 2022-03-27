#pragma once

#include <variant>
#include <vector>

#include <Miracle/MiracleError.hpp>
#include <Miracle/Io/ResourceLoader.hpp>
#include "Vulkan.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"

namespace Miracle::Graphics::Implementations::Vulkan {
	class GraphicsPipeline {
	private:
		const Device& m_device;

		vk::raii::PipelineLayout m_layout = nullptr;
		vk::raii::Pipeline m_pipeline = nullptr;

	public:
		GraphicsPipeline(
			const Device& device,
			const Swapchain& swapchain,
			const Io::ResourceLoader& resourceLoader
		);

		void bind(const vk::raii::CommandBuffer& commandBuffer) const;

	private:
		std::variant<MiracleError, vk::raii::ShaderModule> createShaderModule(
			const std::vector<char>& shaderByteCode
		) const;

		std::variant<MiracleError, vk::raii::PipelineLayout> createPipelineLayout() const;
	};
}
