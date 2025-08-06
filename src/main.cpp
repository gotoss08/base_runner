
#include <memory>

#include "App.h"
#include "Engine.h"

int main(int, char**) {
	auto engine = std::make_unique<Engine>(1280, 720, "Base Runner");
	auto app = std::make_unique<App>(*engine);
	engine->run(std::move(app));
	return 0;
}
