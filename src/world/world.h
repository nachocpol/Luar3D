#ifndef WORLD_H
#define WORLD_H

class Scene;

class World final
{
private:
	World();
	World(const World& other) = delete;
	~World();

public:
	static World& Get();

	void Update();

	Scene* CreateScene();

private:
	Scene* m_ActiveScene;
};

#endif