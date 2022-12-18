#ifndef __FLYING_ENEMY_H__
#define __FLYING_ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"

#include "terrestreEnemy.h"

struct SDL_Texture;

class FlyingEnemy : public Entity
{
public:

	FlyingEnemy(bool startEnabled);

	virtual ~FlyingEnemy();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	//oncollisionbox2d..
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:

	//PhysBody* pendingToSetInactive;
	PhysBody* febody;
	PhysBody* collisionWith;
	iPoint collisionWithPosition;

	STATE state;
	DIRECTION direction;
	AttackState attackState;
	int chargingAttackTime;
	int recoverAttackTime;
	iPoint objective;

	bool alive;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;



	//animations
	Animation* currentAnimation = nullptr;
	Animation idleAnimation;
	Animation walkRightAnimation;
	Animation walkLeftAnimation;
	Animation attackRightAnimation;
	Animation attackLeftAnimation;
	Animation deadAnimation;



};

#endif // __FLYING_ENEMY_H__