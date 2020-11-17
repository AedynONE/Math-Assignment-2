#pragma once

#include "Scene.h"
#include "PhysicsPlaygroundListener.h"

class PhysicsPlayground : public Scene
{
public:
	PhysicsPlayground(std::string name);

	void InitScene(float windowWidth = 1920, float windowHeight = 1080) override;

	void Update() override;

	void makeBox(int xSize, int ySize, float xPos, float yPos);

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:
	PhysicsPlaygroundListener listener;

	int ball = 0;
};
