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

TerrestreEnemy::TerrestreEnemy(bool startEnabled) : Entity(EntityType::PLAYER)
{

}

TerrestreEnemy::~TerrestreEnemy() {

}

bool TerrestreEnemy::Awake() {



	return true;
}

bool TerrestreEnemy::Start() {



	return true;
}

bool TerrestreEnemy::Update()
{
	
	return true;
}

bool TerrestreEnemy::CleanUp()
{
	LOG("Cleanup of the player");

	return true;
}


void TerrestreEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{

}