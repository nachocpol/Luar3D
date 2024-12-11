#ifndef SCENE_H
#define SCENE_H

class World;

class Scene final
{
private:
	friend class World;
	Scene();
	Scene(const Scene& other) = delete;
	~Scene();

public:
	void Update();
};

#endif