#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"

#define MAXJUMPS 2

struct SDL_Texture;

class Player : public Entity
{
public:

	Player(bool startEnabled);
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	//oncollisionbox2d..
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	//outcollisionbox2d..
	void EndCollision(PhysBody* bodyA, PhysBody* bodyB);

public:

	PhysBody* getPbody()
	{
		return pbody;
	}


	bool jumping;
	int jumpsRemaining;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;

	bool alive;
	

	//animations
	Animation* currentAnimation = nullptr;
	Animation idleAnimation;
	Animation walkRightAnimation;
	Animation walkLeftAnimation;
	Animation jumpRightAnimation;
	Animation jumpLeftAnimation;
	Animation deadAnimation;

	// L07 DONE 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	//PhysBody* pbodyAura;
	//int playerAuraX, playerAuraY;

	int pickCoinFxId;

};

#endif // __PLAYER_H__