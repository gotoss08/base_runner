
#include <iostream>
#include <memory>

#include "App.h"
#include "Engine.h"
#include "version.h"

int main(int, char**) {
	const auto engine = std::make_unique<Engine>(1280, 720, "BaseRunner " PROJECT_VERSION_STRING);
	auto app = std::make_unique<App>(*engine);
	engine->run(std::move(app));
	return 0;
}
