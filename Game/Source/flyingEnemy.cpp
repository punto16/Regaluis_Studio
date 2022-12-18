#include "flyingEnemy.h"
#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "ModuleFadeToBlack.h"
#include "Map.h"

FlyingEnemy::FlyingEnemy(bool startEnabled) : Entity(EntityType::FLYINGENEMY)
{
	name.Create("flyingEnemy");
}

FlyingEnemy::~FlyingEnemy() {

}

bool FlyingEnemy::Awake() {

	initialPosition.x = parameters.attribute("x").as_int();
	initialPosition.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	//idle anim
	if (idleAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("idleAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			idleAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
										nodePlayer.attribute("y").as_int(),
										nodePlayer.attribute("w").as_int(),
										nodePlayer.attribute("h").as_int() });
		}
	}
	//walk left anim
	if (walkLeftAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("walkLeftAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			walkLeftAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		walkLeftAnimation.speed = parameters.child("walkLeftAnim").attribute("speed").as_float();
		walkLeftAnimation.loop = parameters.child("walkLeftAnim").attribute("loop").as_bool();
	}
	//walk right anim
	if (walkRightAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("walkRightAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			walkRightAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		walkRightAnimation.speed = parameters.child("walkRightAnim").attribute("speed").as_float();
		walkRightAnimation.loop = parameters.child("walkRightAnim").attribute("loop").as_bool();
	}
	//attack left anim
	if (attackLeftAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("attackLeftAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			attackLeftAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		attackLeftAnimation.speed = parameters.child("attackLeftAnim").attribute("speed").as_float();
		attackLeftAnimation.loop = parameters.child("attackLeftAnim").attribute("loop").as_bool();
	}
	//attack right anim
	if (attackRightAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("attackRightAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			attackRightAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		attackRightAnimation.speed = parameters.child("attackRightAnim").attribute("speed").as_float();
		attackRightAnimation.loop = parameters.child("attackRightAnim").attribute("loop").as_bool();
	}
	//dead anim
	if (deadAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("deadAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			deadAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
										nodePlayer.attribute("y").as_int(),
										nodePlayer.attribute("w").as_int(),
										nodePlayer.attribute("h").as_int() });
		}
	}

	return true;
}

bool FlyingEnemy::Start() {

	texture = app->tex->Load(texturePath);
	currentAnimation = &idleAnimation;

	// L07 DONE 5: Add physics to the player - initialize physics body

	febody = app->physics->CreateRectangle(initialPosition.x + 6, initialPosition.y - 12, 12, 12, bodyType::DYNAMIC);
	febody->listener = this;
	febody->ctype = ColliderType::FLYINGENEMY;
	febody->body->SetFixedRotation(1);
	app->map->enemies.Add(febody);

	currentAnimation = &idleAnimation;
	collisionWith = NULL;

	alive = true;
	febody->body->SetGravityScale(0);
	collisionWithPosition = { 0,0 };

	state = STATE::NORMALPATH;
	direction = DIRECTION::LEFT;
	attackState = AttackState::CHARGINGATTACK;
	chargingAttackTime = 0;

	position.x = initialPosition.x;
	position.y = initialPosition.y;

	return true;
}

bool FlyingEnemy::Update()
{
	PhysBody* pbody = app->scene->player->getPbody();
	b2Vec2 vel, distanceVector;
	distanceVector = febody->body->GetPosition() - app->scene->player->getPbody()->body->GetPosition();
	float distancePlayerFE = distanceVector.Length();

	vel = febody->body->GetLinearVelocity();// +b2Vec2(0, -GRAVITY_Y * 0.05);
	float32 speed;
	b2Vec2 force;

	if (!app->physics->pause)
	{
		//FIRST, I WILL SET THE STATE OF THE TERRESTRE ENEMY
		if (!alive)
		{
			state = STATE::DYING;
		}
		//condition if player is REALLY close from terrestre enemy
		else if (distancePlayerFE <= 3)
		{
			state = STATE::ATTACKING;
		}
		//condition if player is close from terrestre enemy
		else if (distancePlayerFE <= 7)
		{
			state = STATE::AGRESSIVEPATH;
		}
		//condition if player is far from terrestre enemy
		else if (distancePlayerFE > 7)
		{
			state = STATE::NORMALPATH;
		}




		switch (state)
		{
		case STATE::NORMALPATH:
			vel.y = 0;

			switch (direction)
			{
			case DIRECTION::LEFT:
				speed = 5.0f;
				//move to left
				force = { -speed, 0 };
				febody->body->ApplyForceToCenter(force, true);
				if (vel.x < -1.5f)
				{
					vel.x = -1.5f;
				}
				currentAnimation = &walkLeftAnimation;
				if (app->physics->debug)
				{
					//ray that is the PATH of the terrestre enemy 
					app->render->DrawLine(	METERS_TO_PIXELS(febody->body->GetPosition().x),
											METERS_TO_PIXELS(febody->body->GetPosition().y),
											0,
											METERS_TO_PIXELS(febody->body->GetPosition().y),
											0, 255, 0); //green
				}
				break;
			case DIRECTION::RIGHT:
				speed = 5.0f;
				//move to left
				force = { speed, 0 };
				febody->body->ApplyForceToCenter(force, true);
				if (vel.x > 1.5f)
				{
					vel.x = 1.5f;
				}
				if (collisionWith != NULL && collisionWithPosition.x + collisionWith->width * 2 - 8 < position.x + febody->width * 2)
				{
					direction = DIRECTION::LEFT;
				}
				if (collisionWith != NULL && app->physics->debug)
				{
					//ray that is the PATH of the terrestre enemy 
					app->render->DrawLine(	METERS_TO_PIXELS(febody->body->GetPosition().x),
											METERS_TO_PIXELS(febody->body->GetPosition().y),
											app->map->mapData.width * app->map->mapData.tileWidth,
											METERS_TO_PIXELS(febody->body->GetPosition().y),
											0, 255, 0); //green
				}
				currentAnimation = &walkRightAnimation;
				break;
			default:
				break;
			}
			break;
		case STATE::AGRESSIVEPATH:
			if (abs(objective.x + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) - febody->body->GetPosition().x) <= PIXEL_TO_METERS(1))
			{
				direction = DIRECTION::LEFT;
				vel.x = 0;
			}
			else if (objective.x + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) <= febody->body->GetPosition().x)
			{
				speed = 5.0f;
				//move to left
				force = { -speed, 0 };
				febody->body->ApplyForceToCenter(force, true);
				if (vel.x < -3)
				{
					vel.x = -3;
				}
				direction = DIRECTION::LEFT;
				currentAnimation = &walkLeftAnimation;
			}
			else if (objective.x + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) > febody->body->GetPosition().x)
			{
				speed = 5.0f;
				//move to left
				force = { speed, 0 };
				febody->body->ApplyForceToCenter(force, true);
				if (vel.x > 3)
				{
					vel.x = 3;
				}
				direction = DIRECTION::RIGHT;
				currentAnimation = &walkRightAnimation;
			}

			if (abs(objective.y + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) - febody->body->GetPosition().y) <= PIXEL_TO_METERS(1))
			{
				direction = DIRECTION::LEFT;
				vel.y = 0;
			}
			else if (objective.y + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) <= febody->body->GetPosition().y)
			{
				speed = 5.0f;
				//move to left
				force = { 0, -speed };
				febody->body->ApplyForceToCenter(force, true);
				if (vel.y < -3)
				{
					vel.y = -3;
				}
			}
			else if (objective.y + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) > febody->body->GetPosition().y)
			{
				speed = 5.0f;
				//move to left
				force = { 0, speed };
				febody->body->ApplyForceToCenter(force, true);
				if (vel.y > 3)
				{
					vel.y = 3;
				}
			}



			if (app->physics->debug)//ray that is the PATH of the terrestre enemy
			{
				app->render->DrawLine(	METERS_TO_PIXELS(febody->body->GetPosition().x),
										METERS_TO_PIXELS(febody->body->GetPosition().y),
										METERS_TO_PIXELS(objective.x) + 16,
										METERS_TO_PIXELS(objective.y) + 16,
										0, 255, 0); //green
			}
			break;
		case STATE::ATTACKING:
			//the idea is that the te wont move for some frames (loading the attack), he will take the coords of the player when he starts loading, then he will have a force applied as a vector of the position of the te
			// with respect the first coords taken of the player and do a jump to there.
			
			switch (attackState)
			{
			case AttackState::CHARGINGATTACK:
				vel.x = 0; vel.y = 0;
				chargingAttackTime++;
				if ((float)(chargingAttackTime) >= (float)chargingAttackTimeInSeconds * 10)
				{
					attackState = AttackState::JUMPINGATTACK;
				}
				LOG("CHARGING ATTACK FE STATE");
				switch (direction)
				{
				case DIRECTION::LEFT:
					currentAnimation = &walkLeftAnimation;
					break;
				case DIRECTION::RIGHT:
					currentAnimation = &walkRightAnimation;
					break;
				default:
					break;
				}
				break;
			case AttackState::JUMPINGATTACK:
				vel = b2Vec2(	(pbody->body->GetPosition().x - febody->body->GetPosition().x) * 2,
								(pbody->body->GetPosition().y - febody->body->GetPosition().y) * 2);
				//te jumps to bite the player
				febody->body->ApplyLinearImpulse(vel, febody->body->GetPosition(), true);
				LOG("JUMPING ATTACK FE STATE");
				chargingAttackTime = 0;
				attackState = AttackState::RECOVERINGATTACK;
				break;
			case AttackState::RECOVERINGATTACK:
				recoverAttackTime++;
				if ((float)(recoverAttackTime) >= (float)recoverAttackTimeInSeconds * 60 * 2)//if it has loaded for 2 seconds it changes attack state
				{
					recoverAttackTime = 0;
					attackState = AttackState::CHARGINGATTACK;
				}
				break;
			default:
				break;
			}

			if (pbody->body->GetPosition().x < febody->body->GetPosition().x)
			{
				direction = DIRECTION::LEFT;
			}
			else if (pbody->body->GetPosition().x > febody->body->GetPosition().x)
			{
				direction = DIRECTION::RIGHT;
			}
			if (attackState != AttackState::CHARGINGATTACK)
			{

				switch (direction)
				{
				case DIRECTION::LEFT:
					currentAnimation = &attackLeftAnimation;
					break;
				case DIRECTION::RIGHT:
					currentAnimation = &attackRightAnimation;
					break;
				default:
					break;
				}
			}
			break;
		case STATE::DYING:
			currentAnimation = &deadAnimation;
			vel = b2Vec2(0, -GRAVITY_Y);
			position.y = METERS_TO_PIXELS(febody->body->GetTransform().p.y) - 16;
			febody->body->SetActive(false);
			break;
		default:
			break;
		}
		if (state != STATE::ATTACKING)
		{
			attackState = AttackState::CHARGINGATTACK;
			chargingAttackTime = 0;
			recoverAttackTime = 0;
		}

		febody->body->SetLinearVelocity(vel);
		position.x = METERS_TO_PIXELS(febody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(febody->body->GetTransform().p.y) - 16;

		//blit sprite at the end
		currentAnimation->Update();
	}
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y - 9, &rect);

	return true;
}

bool FlyingEnemy::CleanUp()
{
	LOG("Cleanup of the flying enemy");
	app->tex->UnLoad(texture);
	collisionWith = NULL;
	return true;
}


void FlyingEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA->ctype == ColliderType::FLYINGENEMY)
	{
		//if (physB->ctype == ColliderType::FLOATINGTERRAIN ||
		//	physB->ctype == ColliderType::WALL)
		//{
		//	collisionWith = physB;
		//	collisionWith->GetPosition(collisionWithPosition.x, collisionWithPosition.y);
		//}
		switch (physB->ctype)
		{
		case ColliderType::ITEM:
			LOG("FLYING ENEMY Collision ITEM");
			break;
		case ColliderType::PLATFORM:
			LOG("FLYING ENEMY Collision PLATFORM");
			break;
		case ColliderType::WALL:
			if (direction == DIRECTION::RIGHT)
			{
				direction = DIRECTION::LEFT;
			}
			else
			{
				direction = DIRECTION::RIGHT;
			}
			LOG("FLYING ENEMY Collision Wall");
			break;
		case ColliderType::WATER:
			LOG("FLYING ENEMY Collision Water");
			alive = false;
			break;
		case ColliderType::VICTORY:
			LOG("FLYING ENEMY Collision Victory");
			break;
		case ColliderType::PLAYER:
			LOG("FLYING ENEMY Collision Player");
			int py, fey;
			py = METERS_TO_PIXELS(physB->body->GetPosition().y);
			fey = METERS_TO_PIXELS(physA->body->GetPosition().y);

			//terrestre enemy dies
			if (!app->scene->godMode &&
				alive &&
				app->scene->player->alive &&
				py + physB->height + 3 <= fey)
			{
				alive = false;
				physB->body->SetLinearVelocity(b2Vec2(0, -20.0f));
			}
			//player dies
			else if (	!app->scene->godMode && 
						app->scene->player->alive && 
						alive &&
						fey < physA->height + 4 + py)
			{
				app->scene->player->alive = false;
				app->sceneIntro->Win = false;
				app->sceneIntro->beforePlay = false;
				app->fade->FadeToBlack(app->scene, (Module*)app->sceneIntro, 20);
			}

			break;
		case ColliderType::FLYINGENEMY:
			if (direction == DIRECTION::RIGHT && state != STATE::ATTACKING)
			{
				direction = DIRECTION::LEFT;
			}
			else if (direction == DIRECTION::LEFT && state != STATE::ATTACKING)
			{
				direction = DIRECTION::RIGHT;
			}
			break;
		case ColliderType::UNKNOWN:
			LOG("FLYING ENEMY Collision UNKNOWN");
			break;
		}
	}
}