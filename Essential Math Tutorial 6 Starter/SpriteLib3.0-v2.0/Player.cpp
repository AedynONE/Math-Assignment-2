#include "Player.h"


Player::Player()
{
}

Player::Player(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite,
	AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	InitPlayer(fileName, animationJSON, width, height, sprite, controller, transform, hasPhys, body);
}

void Player::InitPlayer(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite,
	AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	//Store references to the components
	m_sprite = sprite;
	m_animController = controller;
	m_transform = transform;
	m_hasPhysics = hasPhys;
	if (hasPhys)
	{
		m_physBody = body;
	}

	//Initialize UVs
	m_animController->InitUVs(fileName);

	//Loads the texture and sets width and height
	m_sprite->LoadSprite(fileName, width, height, true, m_animController);
	m_animController->SetVAO(m_sprite->GetVAO());
	m_animController->SetTextureSize(m_sprite->GetTextureSize());

	//Loads in the animations json file
	nlohmann::json animations = File::LoadJSON(animationJSON);

	//IDLE ANIMATIONS\\
	
	//Idle Left
	m_animController->AddAnimation(animations["IdleLeft"].get<Animation>());
	//Idle Right
	m_animController->AddAnimation(animations["IdleRight"].get<Animation>());
	//Idle Up
	m_animController->AddAnimation(animations["IdleDown"].get<Animation>());
	//Idle Down
	m_animController->AddAnimation(animations["IdleUp"].get<Animation>());

	//Move Animations\\

	//WalkLeft
	m_animController->AddAnimation(animations["MoveLeft"].get<Animation>());
	//WalkRight
	m_animController->AddAnimation(animations["MoveRight"].get<Animation>());
	//WalkUP
	m_animController->AddAnimation(animations["MoveDown"].get<Animation>());
	//WalkDown
	m_animController->AddAnimation(animations["MoveUp"].get<Animation>());

	//Set Default Animation
	m_animController->SetActiveAnim(IDLERIGHT);
	std::cout << "test\n";
}

void Player::Update()
{
	if (!m_locked)
	{
		MovementUpdate();
	}

	AnimationUpdate();
}

void Player::MovementUpdate()
{
	m_moving = false;
	//if (m_hasPhysics) {
		if (Input::GetKey(Key::W))
		{
			std::cout << "\tW";
			m_facing = UP;
			m_moving = true;
		}
		if (Input::GetKey(Key::S))
		{
			std::cout << "\tS";
			m_facing = DOWN;
			m_moving = true;
		}
		if (Input::GetKey(Key::A))
		{
			std::cout << "\tA";
			m_facing = LEFT;
			m_moving = true;
		}
		if (Input::GetKey(Key::D))
		{
			std::cout << "\tD";
			m_facing = RIGHT;
			m_moving = true;
		}
	//}
}

void Player::AnimationUpdate()
{
	int activeAnimation = 0;

	if (m_moving)
	{
		//Puts it into the MOVE category
		activeAnimation = MOVE;
	}
	else
	{
		activeAnimation = IDLE;
	}

	SetActiveAnimation(activeAnimation + (int)m_facing);
}

void Player::SetActiveAnimation(int anim)
{
	m_animController->SetActiveAnim(anim);
}
