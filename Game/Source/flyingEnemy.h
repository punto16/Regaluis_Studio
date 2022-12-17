#ifndef __FLYING_ENEMY_H__
#define __FLYING_ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

//enum class STATE
//{
//	NORMALPATH,
//	AGRESSIVEPATH,
//	ATTACKING,
//	DYING
//};
//enum class DIRECTION
//{
//	LEFT,
//	RIGHT
//};
//
////ATTACKING STUFF
//
//#define chargingAttack true
//#define jumpAttack false
//#define chargingAttackTimeInSeconds 0.2f

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

	STATE state;
	DIRECTION direction;
	bool attackState;
	int chargingAttackTime;
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