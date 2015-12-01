#pragma once

#include <memory>
#include <vector>

#include <gecom/Window.hpp>
#include <gecom/Log.hpp>
#include <gecom/Initial3D.hpp>

namespace pxljm {

	class RenderTask {
	public:
		virtual ~RenderTask() {};
		virtual void render() = 0;
	};

	class FrameTask {
	private:

	public:
		gecom::Window *window;
		std::vector<std::unique_ptr<RenderTask>> tasks;
		FrameTask(gecom::Window *w) : window(w) { }

		void execute() {
			for (std::unique_ptr<RenderTask> &task : tasks) {
				task->render();
			}
		}
	};
}
