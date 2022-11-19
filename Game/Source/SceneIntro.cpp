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

#include "Defs.h"
#include "Log.h"

SceneIntro::SceneIntro(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
SceneIntro::~SceneIntro()
{}

// Called before render is available
bool SceneIntro::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneIntro");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool SceneIntro::Start()
{

	StartImage = app->tex->Load("Assets/Textures/screenIntro.png");
	WinImage = app->tex->Load("Assets/Textures/VictoryScreen.png");
	LoseImage = app->tex->Load("Assets/Textures/LoseScreen.png");

	

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	

	app->audio->PlayMusic("");
	
	// L03: DONE: Load map
	//app->map->Load();

	//app->entityManager->DisableEntities();

	// L04: DONE 7: Set the window title with map/tileset info
	/*SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());*/

	return true;
}

// Called each loop iteration
bool SceneIntro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneIntro::Update(float dt)
{

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN ||
		app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		app->fade->FadeToBlack(this, (Module*)app->scene, 60);
	}


	if (beforePlay)
	{
		app->render->DrawTexture(StartImage, 0, 0, 0, 1.0f, 0.0, 2147483647, 2147483647, true);
	}
	else
	{
		if (Win)
		{
			app->render->DrawTexture(WinImage, 0, 0, 0, 1.0f, 0.0, 2147483647, 2147483647, true);
		}
		else
		{
			app->render->DrawTexture(LoseImage, 0, 0, 0, 1.0f, 0.0, 2147483647, 2147483647, true);
		}
	}


	// Draw map
	//app->map->Draw();

	return true;
}

// Called each loop iteration
bool SceneIntro::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;


	return ret;
}

// Called before quitting
bool SceneIntro::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(StartImage);
	app->tex->UnLoad(WinImage);
	app->tex->UnLoad(LoseImage);

	return true;
}
