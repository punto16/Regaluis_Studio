
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"
#include "Scene.h"
#include "Defs.h"
#include "Log.h"
#include "Window.h"
#include <string.h>

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Map::Map(bool startEnabled) : Module(startEnabled), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;


    return ret;
}

void Map::Draw()
{
    if(mapLoaded == false)
        return;

    // L05: DONE 5: Prepare the loop to draw all tiles in a layer + DrawTexture()

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        //L06: DONE 7: use GetProperty method to ask each layer if your “Draw” property is true.



        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    // L05: DONE 9: Complete the draw function
                    int gid = mapLayerItem->data->Get(x, y);

                    //L06: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);

                    if (mapLayerItem->data->id == 5)     //PARALAX STUFF
                    {
                        app->render->DrawTexture(tileset->texture,
                            pos.x - (app->render->camera.x) * 0.3 / app->win->GetScale(),
                            pos.y,
                            &r);
                    
                    }
                    else
                    {
                        app->render->DrawTexture(tileset->texture,
                            pos.x,
                            pos.y,
                            &r);
                    }
                }
            }
        }
        mapLayerItem = mapLayerItem->next;

    }


}

void Map::Collisions(pugi::xml_node mapFile) // it creates the collisions lol omg xd
{
    pugi::xml_node objectGroupNode = mapFile.child("map").child("objectgroup");


    while (objectGroupNode != NULL) {

        pugi::xml_node objectNode = objectGroupNode.child("object");

        //L06: DONE 7: use GetProperty method to ask each layer if your “Draw” property is true.
        if (!strcmp(objectGroupNode.attribute("name").as_string(),"FloorCollisions")) {

            while (objectNode != NULL)
            {
                PhysBody* c1 = app->physics->CreateRectangleSensor( objectNode.attribute("x").as_int() + objectNode.attribute("width").as_int() / 2,
                                                                    objectNode.attribute("y").as_int() + objectNode.attribute("height").as_int() / 2, 
                                                                    objectNode.attribute("width").as_int(), 
                                                                    objectNode.attribute("height").as_int(), 
                                                                    bodyType::STATIC);
                c1->ctype = ColliderType::PLATFORM;
                collisions.Add(c1);

                objectNode = objectNode.next_sibling("object");
            }
        }
        else if (!strcmp(objectGroupNode.attribute("name").as_string(),"WallCollisions"))
        {
            while (objectNode != NULL)
            {
                PhysBody* c1 = app->physics->CreateRectangle(       objectNode.attribute("x").as_int() + objectNode.attribute("width").as_int() / 2,
                                                                    objectNode.attribute("y").as_int() + objectNode.attribute("height").as_int() / 2,
                                                                    objectNode.attribute("width").as_int(),
                                                                    objectNode.attribute("height").as_int(),
                                                                    bodyType::STATIC);
                c1->ctype = ColliderType::WALL;
                collisions.Add(c1);

                objectNode = objectNode.next_sibling("object");
            }
        }
        else if (!strcmp(objectGroupNode.attribute("name").as_string(), "WaterCollisions"))
        {
            while (objectNode != NULL)
            {
                PhysBody* c1 = app->physics->CreateRectangleSensor( objectNode.attribute("x").as_int() + objectNode.attribute("width").as_int() / 2,
                                                                    objectNode.attribute("y").as_int() + objectNode.attribute("height").as_int() / 2,
                                                                    objectNode.attribute("width").as_int(),
                                                                    objectNode.attribute("height").as_int(),
                                                                    bodyType::STATIC);
                c1->ctype = ColliderType::WATER;
                collisions.Add(c1);

                objectNode = objectNode.next_sibling("object");
            }
        }
        else if (!strcmp(objectGroupNode.attribute("name").as_string(), "WinCollisions"))
        {
            while (objectNode != NULL)
            {
                PhysBody* c1 = app->physics->CreateRectangleSensor( objectNode.attribute("x").as_int() + objectNode.attribute("width").as_int() / 2,
                                                                    objectNode.attribute("y").as_int() + objectNode.attribute("height").as_int() / 2,
                                                                    objectNode.attribute("width").as_int(),
                                                                    objectNode.attribute("height").as_int(),
                                                                    bodyType::STATIC);
                c1->ctype = ColliderType::VICTORY;
                collisions.Add(c1);

                objectNode = objectNode.next_sibling("object");
            }
        }
        else if (!strcmp(objectGroupNode.attribute("name").as_string(), "FloatingTerrainCollisions"))
        {
            while (objectNode != NULL)
            {
                PhysBody* c1 = app->physics->CreateRectangle(   objectNode.attribute("x").as_int() + objectNode.attribute("width").as_int() / 2,
                                                                objectNode.attribute("y").as_int() + objectNode.attribute("height").as_int() / 2,
                                                                objectNode.attribute("width").as_int(),
                                                                objectNode.attribute("height").as_int(),
                                                                bodyType::STATIC);
                    c1->ctype = ColliderType::FLOATINGTERRAIN;
                    collisions.Add(c1);

                    objectNode = objectNode.next_sibling("object");
            }
        }
        else if (!strcmp(objectGroupNode.attribute("name").as_string(), "BridgePlatformCollision"))
        {
            //getting the array of points from xml

            SString pointsString = (objectNode.child("polygon").attribute("points").as_string());
            int size = 0;
            int points[MAX_ARRAY_ELEMENTS];
            bool negative = false;

            //capped to maximum of a coord = 9999
            int toadditerator = 0;
            int iterator = 0;
            while ( pointsString.GetTerm(iterator) != '\0')
            {
                if (pointsString.GetTerm(iterator) == '-')
                {
                    negative = true;
                }
                else if (pointsString.GetTerm(iterator) != ',' && pointsString.GetTerm(iterator) != ' ')
                {
                    points[size] = (int)pointsString.GetTerm(iterator) - 48;

                    if (pointsString.GetTerm(iterator + 1) != '\0' && pointsString.GetTerm(iterator + 1) != ',' && pointsString.GetTerm(iterator + 1) != ' ')
                    {
                        points[size] *= 10;
                        points[size] += (int)pointsString.GetTerm(iterator + 1) - 48;
                        toadditerator++;
                        if (pointsString.GetTerm(iterator + 2) != '\0' && pointsString.GetTerm(iterator + 2) != ',' && pointsString.GetTerm(iterator + 2) != ' ')
                        {
                            points[size] *= 10;
                            points[size] += (int)pointsString.GetTerm(iterator + 2) - 48;
                            toadditerator++;
                            if (pointsString.GetTerm(iterator + 3) != '\0' && pointsString.GetTerm(iterator + 3) != ',' && pointsString.GetTerm(iterator + 3) != ' ')
                            {
                                points[size] *= 10;
                                points[size] += (int)pointsString.GetTerm(iterator + 3) - 48;
                                toadditerator++;
                            }
                        }
                    }
                    if (negative)
                    {
                        points[size] *= -1;
                    }
                    size++;
                }
                iterator += toadditerator + 1;
                toadditerator = 0;
            }
            while (objectNode != NULL)
            {
                PhysBody* c1 = app->physics->CreateChain(   objectNode.attribute("x").as_int(),
                                                            objectNode.attribute("y").as_int(),
                                                            points,
                                                            size,
                                                            bodyType::STATIC,
                                                            true);
                c1->ctype = ColliderType::PLATFORM;
                collisions.Add(c1);
                objectNode = objectNode.next_sibling("object");
            }
        }
        else if (!strcmp(objectGroupNode.attribute("name").as_string(), "BridgeWallCollision"))
        {
            //getting the array of points from xml

            SString pointsString = (objectNode.child("polygon").attribute("points").as_string());
            int size = 0;
            int points[MAX_ARRAY_ELEMENTS];
            bool negative = false;

            //capped to maximum of a coord = 9999
            int toadditerator = 0;
            int iterator = 0;
            while (pointsString.GetTerm(iterator) != '\0')
            {
                if (pointsString.GetTerm(iterator) == '-')
                {
                    negative = true;
                }
                else if (pointsString.GetTerm(iterator) != ',' && pointsString.GetTerm(iterator) != ' ')
                {
                    points[size] = (int)pointsString.GetTerm(iterator) - 48;

                    if (pointsString.GetTerm(iterator + 1) != '\0' && pointsString.GetTerm(iterator + 1) != ',' && pointsString.GetTerm(iterator + 1) != ' ')
                    {
                        points[size] *= 10;
                        points[size] += (int)pointsString.GetTerm(iterator + 1) - 48;
                        toadditerator++;
                        if (pointsString.GetTerm(iterator + 2) != '\0' && pointsString.GetTerm(iterator + 2) != ',' && pointsString.GetTerm(iterator + 2) != ' ')
                        {
                            points[size] *= 10;
                            points[size] += (int)pointsString.GetTerm(iterator + 2) - 48;
                            toadditerator++;
                            if (pointsString.GetTerm(iterator + 3) != '\0' && pointsString.GetTerm(iterator + 3) != ',' && pointsString.GetTerm(iterator + 3) != ' ')
                            {
                                points[size] *= 10;
                                points[size] += (int)pointsString.GetTerm(iterator + 3) - 48;
                                toadditerator++;
                            }
                        }
                    }
                    if (negative)
                    {
                        points[size] *= -1;
                    }
                    size++;
                }
                iterator += toadditerator + 1;
                toadditerator = 0;
            }
            while (objectNode != NULL)
            {
                PhysBody* c1 = app->physics->CreateChain(   objectNode.attribute("x").as_int(),
                                                            objectNode.attribute("y").as_int(),
                                                            points,
                                                            size,
                                                            bodyType::STATIC);
                c1->ctype = ColliderType::WALL;
                collisions.Add(c1);
                objectNode = objectNode.next_sibling("object");
            }
        }
        objectGroupNode = objectGroupNode.next_sibling("objectgroup");
    }
}

// L05: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

iPoint Map::WorldToMap(int x, int y)
{
    iPoint ret(0, 0);

    if (mapData.type == MAPTYPE_ORTHOGONAL)
    {
        ret.x = x / mapData.tileWidth;
        ret.y = y / mapData.tileHeight;
    }
    else if (mapData.type == MAPTYPE_ISOMETRIC)
    {
        float halfWidth = mapData.tileWidth * 0.5f;
        float halfHeight = mapData.tileHeight * 0.5f;
        ret.x = int((x / halfWidth + y / halfHeight) / 2);
        ret.y = int((y / halfHeight - x / halfWidth) / 2);
    }
    else
    {
        LOG("Unknown map type");
        ret.x = x; ret.y = y;
    }

    return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    // L05: DONE 7: Get relative Tile rectangle
    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}


// L06: DONE 2: Pick the right Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L04: DONE 2: Make sure you clean up any memory allocated from tilesets/map
    ListItem<TileSet*>* item;
    item = mapData.tilesets.start;

    while (item != NULL)
    {
        RELEASE(item->data);
        item = item->next;
    }

    mapData.tilesets.Clear();

    // L05: DONE 2: clean up all layer data
    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    mapData.maplayers.Clear();

    //REMOVE ALL COLLIDERS
    ListItem<PhysBody*>* collisionsItem;
    collisionsItem = collisions.start;

    while (collisionsItem != NULL)
    {
        collisionsItem->data->body->DestroyFixture(collisionsItem->data->body->GetFixtureList());
        RELEASE(collisionsItem->data);
        collisionsItem = collisionsItem->next;
    }
    collisions.Clear();

    //REMOVE PLAYER
    ListItem<PhysBody*>* playerItem;
    playerItem = player.start;

    while (playerItem != NULL)
    {
        playerItem->data->body->DestroyFixture(playerItem->data->body->GetFixtureList());
        RELEASE(playerItem->data);
        playerItem = playerItem->next;
    }
    player.Clear();

    //REMOVE ENEMIES
    ListItem<PhysBody*>* enemyItem;
    enemyItem = enemies.start;

    while (enemyItem != NULL)
    {
        enemyItem->data->body->DestroyFixture(enemyItem->data->body->GetFixtureList());
        RELEASE(enemyItem->data);
        enemyItem = enemyItem->next;
    }
    enemies.Clear();

    return true;
}

// Load new map
bool Map::Load(const char* scene)
{
    bool ret = true;

    pugi::xml_node configNode = app->LoadConfigFileToVar();
    pugi::xml_node config = configNode.child(scene).child(name.GetString());

    mapFileName = config.child("mapfile").attribute("path").as_string();
    mapFolder = config.child("mapfolder").attribute("path").as_string();

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());



    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }

    if(ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    // L05: DONE 4: Iterate all layers and load each of them
    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }
    
    // L07 DONE 3: Create colliders
    // Later you can create a function here to load and create the colliders from the map
    // 
    // //coliders manuales 
    //app->physics->CreateRectangle(224 + 128, 543 + 32, 256, 64, STATIC);
    //app->physics->CreateRectangle(352 + 64, 384 + 32, 128, 64, STATIC);
    //app->physics->CreateRectangle(256, 704 + 32, 576, 64, STATIC);

    Collisions(mapFileXML);


    if(ret == true)
    {
        // L04: DONE 5: LOG all the data loaded iterate all tilesets and LOG everything
       
        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d",mapData.width,mapData.height);
        LOG("tile_width : %d tile_height : %d",mapData.tileWidth, mapData.tileHeight);
        
        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        // L05: DONE 4: LOG the info for each loaded layer
        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if(mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

// L04: DONE 3: Implement LoadMap to load the map properties
bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
    }

    return ret;
}

// L04: DONE 4: Implement the LoadTileSet function to load the tileset properties
bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        // L04: DONE 4: Load Tileset attributes
        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        // L04: DONE 4: Load Tileset image
        SString tmp("%s%s", mapFolder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

// L05: DONE 3: Implement a function that loads a single layer layer
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();

    //L06: DONE 6 Call Load Propoerties
    LoadProperties(node, layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

// L05: DONE 4: Iterate all layers and load each of them
bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

// L06: DONE 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.list.Add(p);
    }

    return ret;
}


// L06: DONE 7: Ask for the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}


