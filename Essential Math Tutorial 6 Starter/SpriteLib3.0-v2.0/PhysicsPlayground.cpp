#include "PhysicsPlayground.h"
#include "Utilities.h"
//using namespace std;
#include <random>

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(0.f, -80.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;
	//float aspectRatio = 16.f / 9.f;

	//		MainCamera
	{
		/*Scene::CreateCamera(m_sceneReg, vec4(-75.f, 75.f, -75.f, 75.f), -100.f, 100.f, windowWidth, windowHeight, true, true);*/

		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//Attaches the camera to vert and horiz scrolls
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//		Background
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Set up the components
		std::string fileName = "black.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 6000, 6000);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));
	}

	//		Player
	{
		/*Scene::CreatePhysicsSprite(m_sceneReg, "LinkStandby", 80, 60, 1.f, vec3(0.f, 30.f, 2.f), b2_dynamicBody, 0.f, 0.f, true, true)*/

		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Player>(entity);
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<AnimationController>(entity);
		ECS::AttachComponent<CanJump>(entity);

		//Sets up the components
		std::string fileName = "spritesheets/Drone.png";
		std::string animations = "Drone.json";
		ECS::GetComponent<Player>(entity).InitPlayer(fileName, animations, 32, 32, &ECS::GetComponent<Sprite>(entity),
			&ECS::GetComponent<AnimationController>(entity), &ECS::GetComponent<Transform>(entity));
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 11.f;
		float shrinkY = 11.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(2000), float32(60.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//Square body
		//tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.15f, 20.f);

		float Height = tempSpr.GetHeight() - shrinkY;
		float Width = tempSpr.GetWidth() - shrinkX;
		std::vector<b2Vec2> points = {
			b2Vec2(Width / 3.f, -Height / 2.f),
			b2Vec2(Width / 2.f, -Height / 3.f),
			b2Vec2(Width / 2.f, Height / 3.f),
			b2Vec2(Width / 3.f, Height / 2.f),
			b2Vec2(-Width / 3.f, Height / 2.f),
			b2Vec2(-Width / 2.f, Height / 3.f),
			b2Vec2(-Width / 2.f, -Height / 3.f),
			b2Vec2(-Width / 3.f, -Height / 2.f)
		};
		tempPhsBody = PhysicsBody(entity, BodyType::OCTAGON, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.1f, 20.f);

		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(0.f);
	}

	//		Drop Ailens 
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		wall = entity;

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 150, 20);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1295, 150, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(1295), float32(150));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
			float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS, 1.f, 1.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
		tempPhsBody.SetRotationAngleDeg(0);
	}

	//		second gate
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		secondWall = entity;

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100, 100);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1800, 95, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(1800), float32(95));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
			float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS, 1.f, 1.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
		tempPhsBody.SetRotationAngleDeg(0);
	}

	//		Expandable Ramp
	{
		auto entity = ECS::CreateEntity();
		expanableRamp = entity;

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up the components
		std::string fileName = "RightTriangle.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100, 100);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1990, 100.f, 3.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(1990), float32(100));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		// The program detects these points clockwise, so top of the triangle, bottom right, bottom left. box 2d is counter clockwise
		std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f,-tempSpr.GetHeight() / 2), b2Vec2(tempSpr.GetWidth() / 2.f, tempSpr.GetHeight() / 2.f) };
		tempPhsBody = PhysicsBody(entity, BodyType::RIGHTTRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
	}

	//		end gate
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		endGate = entity;

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 40, 100);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(2230, 190, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(2230), float32(190));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
			float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS, 1.f, 1.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
		tempPhsBody.SetRotationAngleDeg(0);
	}

	//		Drop Ailens trigger
	{
		//Creates entity
		auto entity = ECS::CreateEntity();;

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Trigger*>(entity);

		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 100);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1200, 0.f, 80.f));
		ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
		ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(wall);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(1200.f), float32(0.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	}

	//		SECOND wall trigger
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Trigger*>(entity);

		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 30);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1675, 75, 80.f));
		ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
		ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(secondWall);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(1675.f), float32(75));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, OBJECTS);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	}

	//		Expandable Ramp trigger
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Trigger*>(entity);

		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 40);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1860, 155, 80.f));
		ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(expanableRamp);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(1860), float32(155));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	}

	//		Ball pocket
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Trigger*>(entity);

		//Sets up components
			std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 30);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1860, 155, 80.f));
		ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
		ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(endGate);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(2170), float32(120));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, OBJECTS);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	}

	//		Ceiling
	PhysicsPlayground::makeBox(750, 10, 300, 50, false, 0);
	PhysicsPlayground::makeBox(250, 10, 800, 150, false, 0);
	PhysicsPlayground::makeBox(300, 10, 1070, 50, false, 0);
	PhysicsPlayground::makeBox(500, 10, 1620, 140, false, 0);
	PhysicsPlayground::makeBox(400, 10, 2035, 240, false, 0);

	//		Floor
	PhysicsPlayground::makeBox(350, 10, 100, -50, false, 0);
	PhysicsPlayground::makeBox(350, 10, 520, -50, false, 0);
	PhysicsPlayground::makeBox(100, 10, 780, 100, false, 0);
	PhysicsPlayground::makeBox(250, 10, 800, -80, false, 0);
	PhysicsPlayground::makeBox(500, 10, 1085, -40, false, 0);
	PhysicsPlayground::makeBox(500, 10, 1200, -40, false, 0);
	PhysicsPlayground::makeBox(300, 10, 1500, -40, false, 0);
	PhysicsPlayground::makeBox(100, 10, 1600, 45, false, 0);
	PhysicsPlayground::makeBox(50, 10, 1730, 45, false, 0);
	PhysicsPlayground::makeBox(70, 10, 1675, 45, false, 0);
	PhysicsPlayground::makeBox(300, 10, 1900, 45, false, 0);
	PhysicsPlayground::makeBox(100, 10, 2100, 140, false, 0);
	PhysicsPlayground::makeBox(50, 10, 2175, 100, false, 0);
	PhysicsPlayground::makeBox(50, 10, 2225, 140, false, 0);

	//		Puzzle 2 barrier blocks
	PhysicsPlayground::makeBox(45, 10, 790, -40, false, 0);

	//		Walls
	PhysicsPlayground::makeBox(10, 100, -50, 0, false, 0);
	PhysicsPlayground::makeBox(10, 100, 680, 95, false, 0);
	PhysicsPlayground::makeBox(10, 60, 730, 75, false, 0);

	PhysicsPlayground::makeBox(10, 60, 830, 75, false, 0);
	PhysicsPlayground::makeBox(10, 100, 920, 95, false, 0);
	PhysicsPlayground::makeBox(10, 25, 700, -45, false, 0);
	PhysicsPlayground::makeBox(10, 200, 1215, 150, false, 0);
	PhysicsPlayground::makeBox(10, 200, 1365, 150, false, 0);
	PhysicsPlayground::makeBox(10, 100, 1550, 0, false, 0);
	PhysicsPlayground::makeBox(10, 100, 2045, 100, false, 0);
	PhysicsPlayground::makeBox(10, 100, 1845, 190, false, 0);
	PhysicsPlayground::makeBox(10, 50, 2145, 120, false, 0);
	PhysicsPlayground::makeBox(10, 50, 2195, 120, false, 0);

	//		Second layer
	PhysicsPlayground::makeBox(200, 10, 350, -110, false, 0);

	//		Bottom wall
	PhysicsPlayground::makeBox(10, 80, 350, -80, false, 0);

	//		Making a ball
	PhysicsPlayground::makeBall(32, 32, 20, -8);
	PhysicsPlayground::makeBall(32, 32, 2150, 150);

	//		Making Triangles
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 215);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 230);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1270, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);
	PhysicsPlayground::makeTriangle(30, 30, 1240, 200);

	//		Moveable box
	//PhysicsPlayground::makeBox(60, 60, 80, -8, true, 0);
	PhysicsPlayground::makeBox(20, 39, 800, 130, true, 0);
	PhysicsPlayground::makeBox(59, 39, 850, 130, true, 0);

	//PhysicsPlayground::makeRightTriangle(20, 20, 2030, 50, false, 0);
	PhysicsPlayground::makePoly(60, 60, 80, -8, true, 0);

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));

	//ECS::GetComponent<Player>(MainEntities::MainPlayer()).SetSprite(&ECS::GetComponent<Sprite>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::makeBox(int xSize, int ySize, float xPos, float yPos, bool moveable, float rotation)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	std::string fileName;
	
	//Sets up components
	b2Body* tempBody;
	b2BodyDef tempDef;
	if (moveable == false) {
		fileName = "boxSprite.jpg";
		tempDef.type = b2_staticBody;
	}
	else {
		fileName = "Masks/SquareMask.png";
		tempDef.type = b2_dynamicBody;
	}
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, xSize, ySize);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(xPos, yPos, 2.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	tempDef.position.Set(float32(xPos), float32(yPos));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
		float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS, 1.f, 1.f);
	tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
	tempPhsBody.SetRotationAngleDeg(rotation);
}

void PhysicsPlayground::makeBall(int xSize, int ySize, float xPos, float yPos)
{
	auto entity = ECS::CreateEntity();
	// Ball is the name of the entity

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up the components
	std::string fileName = "Ball.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, xSize, ySize);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -8.f, 3.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(xPos), float32(yPos));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	//tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
	tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER, 0.5f, 10.f);

	//Custom body
	// The program detects these points clockwise, so top of the triangle, bottom right, bottom left. box 2d is counter clockwise
	// std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f,-tempSpr.GetHeight() / 2), b2Vec2(0.f,tempSpr.GetHeight() / 2.f) }; 
	// tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

	tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
}

void PhysicsPlayground::makeTriangle(int xSize, int ySize, float xPos, float yPos) {
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up the components
	std::string fileName = "triangle-alien.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, xSize, ySize);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -8.f, 3.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(xPos), float32(yPos));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	// The program detects these points clockwise, so top of the triangle, bottom right, bottom left. box 2d is counter clockwise
	std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f,-tempSpr.GetHeight() / 2), b2Vec2(0.f,tempSpr.GetHeight() / 2.f) }; 
	tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS, 0.5f, 3.f);

	tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
}

//void PhysicsPlayground::makeRightTriangle(int xSize, int ySize, float xPos, float yPos, bool moveable, float rotation)
//{
//	auto 
//	entity = ECS::CreateEntity();
//
//
//	//Add components
//	ECS::AttachComponent<Sprite>(entity);
//	ECS::AttachComponent<Transform>(entity);
//	ECS::AttachComponent<PhysicsBody>(entity);
//
//	std::string fileName;
//
//	//Sets up components
//	b2Body* tempBody;												
//	b2BodyDef tempDef;
//
//	//if (moveable) {
//		//fileName = "Masks/RightTriangleMask2.png";
//		//tempDef.type = b2_dynamicBody;
//	//}
//	//else {
//		fileName = "RightTriangle.png";
//		tempDef.type = b2_staticBody;
//	//}
//	//ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, xSize, ySize);
//	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 20);
//	//ECS::GetComponent<Transform>(entity).SetPosition(vec3(xPos, yPos, 3.f));
//	ECS::GetComponent<Transform>(entity).SetPosition(vec3(2030, 50, 3.f));
//
//	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//	float shrinkX = 0.f;
//	float shrinkY = 0.f;
//	//tempDef.position.Set(float32(xPos), float32(yPos));
//	tempDef.position.Set(float32(2030), float32(50));
//
//	tempBody = m_physicsWorld->CreateBody(&tempDef);
//
//	//Custom body
//	// The program detects these points clockwise, so top of the triangle, bottom right, bottom left. box 2d is counter clockwise
//	std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f,-tempSpr.GetHeight() / 2), b2Vec2(tempSpr.GetWidth() / 2.f, tempSpr.GetHeight() / 2.f) };
//	tempPhsBody = PhysicsBody(entity, BodyType::RIGHTTRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
//
//	tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
//	//tempPhsBody.SetRotationAngleDeg(rotation);
//}

void PhysicsPlayground::makePoly(int xSize, int ySize, float xPos, float yPos, bool moveable, float rotation)
{
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	std::string fileName;

	//Sets up components
	b2Body* tempBody;
	b2BodyDef tempDef;

		fileName = "Masks/OctagonMask.png";
	//if (moveable) {
	//	tempDef.type = b2_staticBody;
	//}
	//if (moveable) {
		tempDef.type = b2_dynamicBody;
	//}
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, xSize, ySize);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(xPos, yPos, 2.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	tempDef.position.Set(float32(xPos), float32(yPos));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	//tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
	//tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER, 0.5f, 10.f);

	//Custom body
	// The program detects these points clockwise, so top of the triangle, bottom right, bottom left. box 2d is counter clockwise
	float Height = tempSpr.GetHeight();
	float Width = tempSpr.GetWidth();
	std::vector<b2Vec2> points = { 
		b2Vec2(Width / 5.f, -Height / 2.f), 
		b2Vec2(Width / 2.f, -Height / 5.f), 
		b2Vec2(Width / 2.f, Height / 5.f), 
		b2Vec2(Width / 5.f, Height / 2.f), 
		b2Vec2(-Width / 5.f, Height / 2.f), 
		b2Vec2(-Width / 2.f, Height / 5.f), 
		b2Vec2(-Width / 2.f, -Height / 5.f), 
		b2Vec2(-Width / 5.f, -Height / 2.f) 
	};
	tempPhsBody = PhysicsBody(entity, BodyType::OCTAGON, tempBody, points, vec2(0.f, 0.f), false, OBJECTS, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

	tempPhsBody.SetColor(vec4(0.5f, 0.f, 1.f, 0.3f));
	tempPhsBody.SetRotationAngleDeg(rotation);
}

void PhysicsPlayground::Update()
{
	auto& player = ECS::GetComponent<Player>(MainEntities::MainPlayer());
	Scene::AdjustScrollOffset();
	player.Update();
}

void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	float speed = 75.f;
	b2Vec2 vel = b2Vec2(0.f, 0.f);
	//std::cout << "\n" << player.GetBody()->GetLinearVelocity().x << ",\t" << player.GetBody()->GetLinearVelocity().y << "\t";
	if (Input::GetKey(Key::Shift))
	{
		speed *= 2.f;
	}

	if (Input::GetKey(Key::W))
	{
		vel.y += Timer::deltaTime;
		//vel += b2Vec2(0.f, 8.f * Timer::deltaTime);
	}
	if (Input::GetKey(Key::S))
	{
		vel.y += -Timer::deltaTime;
		//vel += b2Vec2(0.f, -8.f * Timer::deltaTime);
	}

	if (Input::GetKey(Key::A))
	{
		vel.x += -Timer::deltaTime;
		//vel += b2Vec2(-8.f * Timer::deltaTime, 0.f);
	}
	if (Input::GetKey(Key::D))
	{
		vel.x += Timer::deltaTime;
		//vel += b2Vec2(8.f * Timer::deltaTime, 0.f);
	}
	
	player.GetBody()->SetLinearVelocity(speed * vel + b2Vec2(player.GetBody()->GetLinearVelocity().x * 0.98f, player.GetBody()->GetLinearVelocity().y * 0.98f));
	//player.GetBody()->SetLinearVelocity(speed * vel);
	
	//Change physics body size for circle
	if (Input::GetKey(Key::O))
	{
		player.ScaleBody(1.3f * Timer::deltaTime, 0);
	}
	else if (Input::GetKey(Key::I))
	{
		player.ScaleBody(-1.3f * Timer::deltaTime, 0);
	}
}

void PhysicsPlayground::KeyboardDown()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());

	if (Input::GetKeyDown(Key::T))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}
}

void PhysicsPlayground::KeyboardUp()
{
	

}
