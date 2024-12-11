#ifndef ENTITY_H
#define ENTITY_H

/*
Concept of entity:

Single "thing" in the world. Can be a 3D object, a camera, light...

Common thing is that this class needs to be a transform, needs to provide a way
to represent position, scale and orientation.

From there we have two solutions:

- Inheritance

	To make a light, we can inherit from entity and create the light related stuff.
	Like special settings, debug drawing etc

	Entity: Update() Render()

		Light

		Camera

		Model

- Composition

	We only have entity as a class that is instanced in the world. From there, you can
	start adding components to it.

	So we have entity->AddComponent(new DirectionalLight)

*/

class Entity
{
public:
	Entity();
};

#endif
