#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "ModuleFadeToBlack.h"
#include "SceneIntro.h"
#include "ModuleFonts.h"
#include "Pathfinding.h"

#include <math.h>

#include "Defs.h"
#include "Log.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;


	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	app->physics->pause = false;

	//AWAKE ACHICOPLAO

	pugi::xml_node configNode = app->LoadConfigFileToVar();
	pugi::xml_node config = configNode.child(name.GetString());

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");


	//create terrestre enemy
	configNode = app->LoadConfigFileToVar();
	config = configNode.child(name.GetString());

	for (pugi::xml_node itemNode = config.child("terrestreEnemy"); itemNode; itemNode = itemNode.next_sibling("terrestreEnemy"))
	{
		TerrestreEnemy* newTerrestreEnemy = (TerrestreEnemy*)app->entityManager->CreateEntity(EntityType::TERRESTREENEMY);
		newTerrestreEnemy->parameters = itemNode;
		terrestreEnemies.Add(newTerrestreEnemy);
	}


	//pathfinding stuff
	// Texture to highligh mouse position 
	mouseTileTex = app->tex->Load("Assets/Maps/path.png");
	// Texture to show path origin 
	originTex = app->tex->Load("Assets/Maps/x.png");


	char lookupTable[] = { "abcdefghijklmnopqrstuvwxyz 0123456789.,;:$#'! /?%&()@ " };
	blackFont = app->fonts->Load(config.child("blackFont").attribute("texturepath").as_string(), lookupTable, 6);
	whiteFont = app->fonts->Load(config.child("whiteFont").attribute("texturepath").as_string(), lookupTable, 6);
	
	app->audio->PlayMusic(config.child("music").attribute("path").as_string());
	
	// L03: DONE: Load map
	//app->map->Load(name.GetString());


	//create walkability map
	bool retLoad = app->map->Load(name.GetString());
	 
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	if (!app->entityManager->isEnabled)
	{
		app->entityManager->Enable();
	}


	//enable all entities
	app->entityManager->EnableEntities();

	//window data
	
	scale = app->win->GetScale();
	app->win->GetWindowSize(width, height);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	////FLOATING TERRAIN WILL INACTIVE WHILE PLAYER IS BELOW IT
	//for (b2Body* b = app->physics->world->GetBodyList(); b; b = b->GetNext())
	//{
	//	PhysBody* pB = (PhysBody*)b->GetUserData();
	//	if (pB->ctype == ColliderType::FLOATINGTERRAIN) {
	//		int posX = 0;
	//		int posY = 0;
	//		pB->GetPosition(posX, posY);
	//		if (posY < player->position.y + 32) {
	//			pB->body->SetActive(false);
	//		}
	//		else {
	//			pB->body->SetActive(true);
	//		}
	//	}
	//}

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		app->render->camera.y += CAMERASPEED;
		fixedCamera = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		app->render->camera.y -= CAMERASPEED;
		fixedCamera = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		app->render->camera.x += CAMERASPEED;
		fixedCamera = false;
	}
	
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		app->render->camera.x -= CAMERASPEED;
		fixedCamera = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
	{
		app->win->scale++;
		if (app->win->scale >= 8)
		{
			app->win->scale = 8;
		}
		fixedCamera = true;
	}
	else if (app->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		app->win->scale--;
		if (app->win->scale <= 0)
		{
			app->win->scale = 1;
		}
		fixedCamera = true;
	}
	scale = app->win->scale;
	

	//camera fix to player in x axis
	if (fixedCamera && player->position.x > 300 / scale && player->position.x < (app->map->mapData.tileWidth * app->map->mapData.width) - 724 / scale)
	{
 		app->render->camera.x = -1 * scale * (player->position.x - 300 / scale);
	}
	else if (fixedCamera && player->position.x <= 300 / scale)
	{
		app->render->camera.x = 0;
	}
	else if (fixedCamera && player->position.x >= (app->map->mapData.tileWidth * app->map->mapData.width) - 724 / scale)
	{
		app->render->camera.x = -1 * scale * ((app->map->mapData.tileWidth * app->map->mapData.width) - 1024 / scale);
	}

	//camera fix to player in y axis
	if (fixedCamera && player->position.y > 300 / scale && player->position.y < (app->map->mapData.tileHeight * app->map->mapData.height) - 468 / scale)
	{
		app->render->camera.y = -1 * scale * (player->position.y - 300 / scale);
	}
	else if (fixedCamera && player->position.y <= 300 / scale || player->position.y < 0)
	{
		app->render->camera.y = 0;
	}
	else if (fixedCamera && player->position.y >= (app->map->mapData.tileHeight * app->map->mapData.height) - 468 / scale)
	{
		app->render->camera.y = (1 - scale) * height;
	}




	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		app->SaveGameRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		app->LoadGameRequest();
	}



	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)//godmode to player
	{
		if (godMode)
		{
			godMode = false;
		}
		else
		{
			godMode = true;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)//caps fps to 30 or 60
	{
		if (app->FPS == 60)
		{
			app->FPS = 30;
		}
		else 
		{
			app->FPS = 60;
		}
	}


	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)//restart current level
	{
		PhysBody* pbody = player->getPbody();
		pbody->SetPosition(player->initialPosition.x, player->initialPosition.y);
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)//stsrt from 1st level
	{
		PhysBody* pbody = player->getPbody();
		app->fade->FadeToBlack(this,this,0);
		app->sceneIntro->currentLevel = 0;
		pbody->SetPosition(player->initialPosition.x, player->initialPosition.y);
	}
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)//stsrt from 2nd level
	{
		PhysBody* pbody = player->getPbody();
		app->fade->FadeToBlack(this, this, 0);
		app->sceneIntro->currentLevel = 1;
		pbody->SetPosition(player->initialPosition.x, player->initialPosition.y);
	}

	// Draw map
	app->map->Draw();

	//does pathfinding using coord of enemy and player
	ListItem<PhysBody*>* enemyItem;
	enemyItem = app->map->enemies.start;

	ListItem<TerrestreEnemy*>* terrestreEnemyItem = terrestreEnemies.start;

	while (enemyItem != NULL)
	{
		if (terrestreEnemyItem != NULL && terrestreEnemyItem->data->state == STATE::AGRESSIVEPATH)
		{
			//origin = { enemyItem->data->body->GetPosition().x, enemyItem->data->body->GetPosition().x };//app->map->WorldToMap(enemyItem->data->body->GetPosition().x - app->render->camera.x * (float)1 / scale, enemyItem->data->body->GetPosition().y - app->render->camera.y * (float)1 / scale);
			origin.x = enemyItem->data->body->GetPosition().x;
			origin.y = enemyItem->data->body->GetPosition().y;
			iPoint destination;// = { player->getPbody()->body->GetPosition().x, player->getPbody()->body->GetPosition().x };//app->map->WorldToMap(player->getPbody()->body->GetPosition().x - app->render->camera.x * (float)1 / scale, player->getPbody()->body->GetPosition().y - app->render->camera.y * (float)1 / scale);
			destination.x = player->getPbody()->body->GetPosition().x;
			destination.y = player->getPbody()->body->GetPosition().y;
			app->pathfinding->ClearLastPath();
			app->pathfinding->CreatePath(origin, destination);
			// L12: Get the latest calculated path and draw
			const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				if (i == 1)
				{
					terrestreEnemyItem->data->objective.x = PIXEL_TO_METERS(pos.x);
					terrestreEnemyItem->data->objective.y = PIXEL_TO_METERS(pos.y);
				}
				if (app->physics->debug) app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
			}
			
			// L12: Debug pathfinding
			iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
			if (app->physics->debug) app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
		}
		enemyItem = enemyItem->next;
		terrestreEnemyItem = terrestreEnemyItem->next;
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		app->sceneIntro->beforePlay = true;
		app->fade->FadeToBlack(this, (Module*)app->sceneIntro, 20);
	}

	if (app->FPS == 60)
	{
		app->fonts->BlitText((10 - app->render->camera.x) * (float)1 /scale, (10 - app->render->camera.y) * (float)1/scale, blackFont,"60 fps");
	}
	else if (app->FPS == 30)
	{
		app->fonts->BlitText((10 - app->render->camera.x) * (float)1/scale, (10 - app->render->camera.y) * (float)1/scale, blackFont, "30 fps");
	}
	
	//here should add path of terrestre enemy

	if (app->physics->debug)
	{
		ListItem<PhysBody*>* ItemListTE = app->map->enemies.start;
		ListItem<TerrestreEnemy*>* terrestreEnemyItem = terrestreEnemies.start;
		PhysBody* tebody;
		PhysBody* pbody = player->getPbody();
		

		while (ItemListTE != NULL)
		{
			tebody = ItemListTE->data;

			if (ItemListTE->data->body->IsActive())
			{


				//ray between terrestre enemy and player
				app->render->DrawLine(	METERS_TO_PIXELS(tebody->body->GetPosition().x),
										METERS_TO_PIXELS(tebody->body->GetPosition().y),
										METERS_TO_PIXELS(pbody->body->GetPosition().x),
										METERS_TO_PIXELS(pbody->body->GetPosition().y),
										255, 0, 0);//red

				////ray that is the PATH of the terrestre enemy 
				//app->render->DrawLine(	METERS_TO_PIXELS(tebody->body->GetPosition().x),
				//						METERS_TO_PIXELS(tebody->body->GetPosition().y),
				//						METERS_TO_PIXELS(terrestreEnemyItem->data->objective.x) + 16,
				//						METERS_TO_PIXELS(tebody->body->GetPosition().y),
				//						0, 255, 0); //green
			}
			
			terrestreEnemyItem = terrestreEnemyItem->next;
			ItemListTE = ItemListTE->next;
		}
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	if (app->entityManager->IsEnabled())
	{
		app->entityManager->Disable();
	}

	app->fonts->UnLoad(blackFont);
	app->fonts->UnLoad(whiteFont);

	//pathfinding stuff
	app->tex->UnLoad(mouseTileTex);
	app->tex->UnLoad(originTex);


	ListItem<TerrestreEnemy*>* terrestreEnemyItem = terrestreEnemies.start;

	while (terrestreEnemyItem != NULL)
	{
		terrestreEnemyItem->data->CleanUp();
		terrestreEnemyItem = terrestreEnemyItem->next;
	}

	terrestreEnemies.Clear();
	
	app->map->CleanUp();

	return true;
}


bool Scene::LoadState(pugi::xml_node& data)
{
	int currentLevelBefore = app->sceneIntro->currentLevel;
	//current level data
	app->sceneIntro->currentLevel = data.child("CurrentLevel").attribute("CurrentLevel").as_int() - 1;

	if (currentLevelBefore != app->sceneIntro->currentLevel)
	{
		app->map->Disable();
		app->map->Enable();
	}
	//PLAYER DATA
	PhysBody* pbody = player->getPbody();
	pbody->SetPosition(data.child("player").attribute("x").as_int(), data.child("player").attribute("y").as_int());
	pbody->body->SetLinearVelocity(b2Vec2(data.child("player").attribute("velx").as_int(), data.child("player").attribute("vely").as_int()));
	player->jumpsRemaining = data.child("player").attribute("jumpsRemaining").as_int();


	//te data

	ListItem<TerrestreEnemy*>* terrestreEnemyItem = terrestreEnemies.start;
	pugi::xml_node nodeTE = data.child("TerrestreEnemy");
	
	while (terrestreEnemyItem != NULL)
	{
		terrestreEnemyItem->data->tebody->SetPosition(nodeTE.attribute("x").as_int(), nodeTE.attribute("y").as_int());
		terrestreEnemyItem->data->tebody->body->SetLinearVelocity(b2Vec2(nodeTE.attribute("velx").as_int(), nodeTE.attribute("vely").as_int()));
		terrestreEnemyItem->data->state = (STATE)nodeTE.attribute("STATE").as_int();
		terrestreEnemyItem->data->direction = (DIRECTION)nodeTE.attribute("DIRECTION").as_int();
		nodeTE = nodeTE.next_sibling("TerrestreEnemy");
		terrestreEnemyItem = terrestreEnemyItem->next;
	}

	return true;
}

// L03: DONE 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Scene::SaveState(pugi::xml_node& data)
{
	//current level data
	pugi::xml_node currentLevelNode = data.append_child("CurrentLevel");
	
	currentLevelNode.append_attribute("CurrentLevel") = app->sceneIntro->currentLevel + 1;

	//PLAYER DATA
	pugi::xml_node playerNode = data.append_child("player");

	playerNode.append_attribute("x") = (player->position.x + 16);
	playerNode.append_attribute("y") = (player->position.y + 16);
	if (abs(player->getPbody()->body->GetLinearVelocity().x) < 0.5)
	{
		playerNode.append_attribute("velx") = (0);
	}
	else
	{
		playerNode.append_attribute("velx") = (player->getPbody()->body->GetLinearVelocity().x);
	}

	playerNode.append_attribute("vely") = (player->getPbody()->body->GetLinearVelocity().y);
	playerNode.append_attribute("jumpsRemaining") = (player->jumpsRemaining);

	//terrestre enemies data

	ListItem<TerrestreEnemy*>* terrestreEnemyItem = terrestreEnemies.start;

	while (terrestreEnemyItem != NULL)
	{
		pugi::xml_node teNode = data.append_child("TerrestreEnemy");
		teNode.append_attribute("x") = (terrestreEnemyItem->data->position.x + 16);
		teNode.append_attribute("y") = (terrestreEnemyItem->data->position.y + 16);
		if (abs(terrestreEnemyItem->data->tebody->body->GetLinearVelocity().x) < 0.5)
		{
			teNode.append_attribute("velx") = (0);
		}
		else
		{
			teNode.append_attribute("velx") = (terrestreEnemyItem->data->tebody->body->GetLinearVelocity().x);
		}
		teNode.append_attribute("vely") = (terrestreEnemyItem->data->tebody->body->GetLinearVelocity().y);
		teNode.append_attribute("STATE") = (int)terrestreEnemyItem->data->state;
		teNode.append_attribute("DIRECTION") = (int)terrestreEnemyItem->data->direction;

		terrestreEnemyItem = terrestreEnemyItem->next;
	}


	return true;
}