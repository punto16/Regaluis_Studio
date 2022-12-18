#include "terrestreEnemy.h"
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

TerrestreEnemy::TerrestreEnemy(bool startEnabled) : Entity(EntityType::TERRESTREENEMY)
{
	name.Create("terrestreEnemy");
}

TerrestreEnemy::~TerrestreEnemy() {

}

bool TerrestreEnemy::Awake() {

	initialPosition.x = parameters.attribute("x").as_int();
	initialPosition.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	//idle anim right
	if (idleAnimationRight.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("idleAnimRight").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			idleAnimationRight.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
	}
	//idle anim left
	if (idleAnimationLeft.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("idleAnimLeft").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			idleAnimationLeft.PushBack({	nodePlayer.attribute("x").as_int(),
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

bool TerrestreEnemy::Start() {

	texture = app->tex->Load(texturePath);
	currentAnimation = &idleAnimationLeft;

	// L07 DONE 5: Add physics to the player - initialize physics body

	tebody = app->physics->CreateRectangle(initialPosition.x + 14, initialPosition.y + 6, 28,12, bodyType::DYNAMIC);
	tebody->listener = this;
	tebody->ctype = ColliderType::TERRESTREENEMY;
	tebody->body->SetFixedRotation(1);
	app->map->enemies.Add(tebody);

	alive = true;
	
	state = STATE::NORMALPATH;
	direction = DIRECTION::LEFT;
	attackState = AttackState::CHARGINGATTACK;
	chargingAttackTime = 0;
	recoverAttackTime = 0;

	position.x = initialPosition.x;
	position.y = initialPosition.y;

	return true;
}

bool TerrestreEnemy::Update()
{
	PhysBody* pbody = app->scene->player->getPbody();
	b2Vec2 vel, distanceVector;
	distanceVector = tebody->body->GetPosition() - app->scene->player->getPbody()->body->GetPosition();
	float distancePlayerTE = distanceVector.Length();

	vel = tebody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.05);
	float32 speed;
	b2Vec2 force;

	iPoint collisionWithPosition;
	if (collisionWith != NULL)
	{
		collisionWith->GetPosition(collisionWithPosition.x, collisionWithPosition.y);
	}

	if (!app->physics->pause)
	{
		//FIRST, I WILL SET THE STATE OF THE TERRESTRE ENEMY
		if (!alive)
		{
			state = STATE::DYING;
		}
		//condition if player is REALLY close from terrestre enemy
		else if (distancePlayerTE <= 3)
		{
			state = STATE::ATTACKING;
		}
		//condition if player is close from terrestre enemy
		else if (distancePlayerTE <= 7)
		{
			state = STATE::AGRESSIVEPATH;
		}
		//condition if player is far from terrestre enemy
		else if (distancePlayerTE > 7)
		{
			state = STATE::NORMALPATH;
		}




		switch (state)
		{
		case STATE::NORMALPATH:
			//TO IMPLEMENT NORMAL PATH THE IDEA IS -> TAKE THE CLOSER WALL OR PLATFORM COLLIDER -> COMPARE THIS COLLIDER X WITH TE X, AND THEN, MOVE LEFT OR RIGHT.
			//ANOTHER IDEA IS TO IMPLEMENT MODULE PATHS FROM LAST YEAR PROJECT 1



			switch (direction)
			{
			case DIRECTION::LEFT:
				speed = 5.0f;
				//move to left
				force = { -speed, 0 };
				tebody->body->ApplyForceToCenter(force, true);
				if (vel.x < -1.5f)
				{
					vel.x = -1.5f;
				}
				if (collisionWith != NULL && collisionWithPosition.x + 5 > position.x)
				{
					direction = DIRECTION::RIGHT;
				}
				currentAnimation = &walkLeftAnimation;
				if (app->physics->debug)
				{
					//ray that is the PATH of the terrestre enemy 
					app->render->DrawLine(	METERS_TO_PIXELS(tebody->body->GetPosition().x),
											METERS_TO_PIXELS(tebody->body->GetPosition().y),
											collisionWithPosition.x,
											METERS_TO_PIXELS(tebody->body->GetPosition().y),
											0, 255, 0); //green
				}
				break;
			case DIRECTION::RIGHT:
				speed = 5.0f;
				//move to left
				force = { speed, 0 };
				tebody->body->ApplyForceToCenter(force, true);
				if (vel.x > 1.5f)
				{
					vel.x = 1.5f;
				}
				if (collisionWith != NULL && collisionWithPosition.x + collisionWith->width * 2 - 8 < position.x + tebody->width * 2)
				{
					direction = DIRECTION::LEFT;
				}
				if (collisionWith != NULL && app->physics->debug)
				{
					//ray that is the PATH of the terrestre enemy 
					app->render->DrawLine(	METERS_TO_PIXELS(tebody->body->GetPosition().x),
											METERS_TO_PIXELS(tebody->body->GetPosition().y),
											collisionWithPosition.x + collisionWith->width * 2,
											METERS_TO_PIXELS(tebody->body->GetPosition().y),
											0, 255, 0); //green
				}
				currentAnimation = &walkRightAnimation;
				break;
			default:
				break;
			}
			break;
		case STATE::AGRESSIVEPATH:
			if (abs(objective.x + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) - tebody->body->GetPosition().x) <= PIXEL_TO_METERS(1))
			{
				direction = DIRECTION::LEFT;
				vel.x = 0;
				currentAnimation = &idleAnimationLeft;
			}
			else if (objective.x + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) <= tebody->body->GetPosition().x)
			{
				speed = 5.0f;
				//move to left
				force = { -speed, 0 };
				tebody->body->ApplyForceToCenter(force, true);
				if (vel.x < -3)
				{
					vel.x = -3;
				}
				direction = DIRECTION::LEFT;
				currentAnimation = &walkLeftAnimation;
			}
			else if (objective.x + PIXEL_TO_METERS(app->map->mapData.tileWidth / 2) > tebody->body->GetPosition().x)
			{
				speed = 5.0f;
				//move to left
				force = { speed, 0 };
				tebody->body->ApplyForceToCenter(force, true);
				if (vel.x > 3)
				{
					vel.x = 3;
				}
				direction = DIRECTION::RIGHT;
				currentAnimation = &walkRightAnimation;
			}
			if (app->physics->debug)//ray that is the PATH of the terrestre enemy
			{
				app->render->DrawLine(	METERS_TO_PIXELS(tebody->body->GetPosition().x),
										METERS_TO_PIXELS(tebody->body->GetPosition().y),
										METERS_TO_PIXELS(objective.x) + 16,
										METERS_TO_PIXELS(tebody->body->GetPosition().y),
										0, 255, 0); //green
			}
			break;
		case STATE::ATTACKING:

			switch (attackState)
			{
			case AttackState::CHARGINGATTACK:
				vel.x = 0;
				chargingAttackTime++;
				if ((float)(chargingAttackTime) >= (float)chargingAttackTimeInSeconds * 20)//if it has loaded for 2 seconds it changes attack state
				{
					attackState = AttackState::JUMPINGATTACK;
				}
				LOG("CHARGING ATTACK TE STATE");
				break;
				break;
			case AttackState::JUMPINGATTACK:
				vel = b2Vec2(	(pbody->body->GetPosition().x - tebody->body->GetPosition().x) * 3,
								(pbody->body->GetPosition().y - tebody->body->GetPosition().y) * 1.5f - 10);
				//te jumps to bite the player
				tebody->body->ApplyLinearImpulse(vel, tebody->body->GetPosition(), true);
				LOG("JUMPING ATTACK TE STATE");
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

			if (pbody->body->GetPosition().x < tebody->body->GetPosition().x)
			{
				direction = DIRECTION::LEFT;
			}
			else if (pbody->body->GetPosition().x > tebody->body->GetPosition().x)
			{
				direction = DIRECTION::RIGHT;
			}

			switch (direction)
			{
			case DIRECTION::LEFT:
				if (attackState != AttackState::CHARGINGATTACK)
				{
					currentAnimation = &attackLeftAnimation;
				}
				else
				{
					currentAnimation = &idleAnimationLeft;
					attackLeftAnimation.Reset();
				}
				break;
			case DIRECTION::RIGHT:
				if (attackState != AttackState::CHARGINGATTACK)
				{
					currentAnimation = &attackRightAnimation;
				}
				else
				{
					currentAnimation = &idleAnimationRight;
					attackRightAnimation.Reset();
				}
				break;
			default:
				break;
			}
			break;
		case STATE::DYING:
			currentAnimation = &deadAnimation;
			vel = b2Vec2(0, -GRAVITY_Y);
			position.y = METERS_TO_PIXELS(tebody->body->GetTransform().p.y) - 16;
			tebody->body->SetActive(false);
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

		tebody->body->SetLinearVelocity(vel);
		position.x = METERS_TO_PIXELS(tebody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(tebody->body->GetTransform().p.y) - 16;

		//blit sprite at the end
		currentAnimation->Update();
	}
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y - 9, &rect);

	return true;
}

bool TerrestreEnemy::CleanUp()
{
	LOG("Cleanup of the terrestre enemy");
	app->tex->UnLoad(texture);

	return true;
}


void TerrestreEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA->ctype == ColliderType::TERRESTREENEMY)
	{
		switch (physB->ctype)
		{
		case ColliderType::ITEM:
			LOG("TERRESTRE ENEMY Collision ITEM");
			break;
		case ColliderType::PLATFORM:
			LOG("TERRESTRE ENEMY Collision PLATFORM");
			break;
		case ColliderType::WALL:
			LOG("TERRESTRE ENEMY Collision Wall");
			break;
		case ColliderType::WATER:
			LOG("TERRESTRE ENEMY Collision Water");
			alive = false;
			break;
		case ColliderType::VICTORY:
			LOG("TERRESTRE ENEMY Collision Victory");
			break;
		case ColliderType::PLAYER:
			LOG("TERRESTRE ENEMY Collision Player");
			int py, tey;
			py = METERS_TO_PIXELS(physB->body->GetPosition().y);
			tey = METERS_TO_PIXELS(physA->body->GetPosition().y);

			//terrestre enemy dies
			if (!app->scene->godMode &&
				alive &&
				app->scene->player->alive &&
				py + physB->height + 3 <= tey)
			{
				alive = false;
				physB->body->SetLinearVelocity(b2Vec2(0, -20.0f));
			}
			//player dies
			else if (	!app->scene->godMode && 
						app->scene->player->alive && 
						alive &&
						tey < physA->height + 4 + py)
			{
				app->scene->player->alive = false;
				app->sceneIntro->Win = false;
				app->sceneIntro->beforePlay = false;
				app->fade->FadeToBlack(app->scene, (Module*)app->sceneIntro, 20);
			}

			break;
		case ColliderType::TERRESTREENEMY:
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
			LOG("TERRESTRE ENEMY Collision UNKNOWN");
			break;
		}
	}
}