#pragma once

class Engine;

class App
{
public:
	App(Engine& engine) : engine(engine) {};
	virtual ~App() = default;
	virtual void onInit();
	virtual void onUpdate(float deltaTime);
	virtual void onRender();
	virtual void onCleanup();
private:
	Engine& engine;
};
