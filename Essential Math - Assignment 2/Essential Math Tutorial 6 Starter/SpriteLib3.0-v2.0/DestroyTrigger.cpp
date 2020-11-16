#include "DestroyTrigger.h"
#include "ECS.h"

void DestroyTrigger::OnTrigger()
{
	Trigger::OnTrigger(); // Try not to use it directly

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			PhysicsBody::m_bodiesToDelete.push_back(m_targetEntities[i]);
		}

		triggered = true;
	}
}

void DestroyTrigger::OnEnter()
{
	Trigger::OnEnter();

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			PhysicsBody::m_bodiesToDelete.push_back(m_targetEntities[i]);
		}

		triggered = true;
	}
}

void DestroyTrigger::OnExit()
{
	Trigger::OnExit();
}
