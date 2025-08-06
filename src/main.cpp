
#include <memory>

#include "App.h"
#include "Engine.h"
#include "version.h"

int main(int, char**) {
	const auto engine = std::make_unique<Engine>(1280, 720, "BaseRunner " BASE_RUNNER_VERSION);
	auto app = std::make_unique<App>(*engine);
	engine->run(std::move(app));
	return 0;
}
