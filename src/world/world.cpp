#include "world.h"
#include "scene.h"

static World* s_WorldInstance = nullptr;

World::World()
{
}

World::~World()
{
}

World& World::Get()
{
	if (!s_WorldInstance)
	{
		s_WorldInstance = new World;
	}
	return *s_WorldInstance;
}

void World::Update()
{
	if (m_ActiveScene)
	{
		m_ActiveScene->Update();
	}
}

Scene* World::CreateScene()
{
	Scene* pScene = new Scene;
	m_ActiveScene = pScene; // TODO: make this an option
	return pScene;
}