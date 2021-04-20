#include "Scene_House.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Node.h"
#include <math.h> 
#include <queue> 

//How to edit level:
// key G to place tile in grid
// key 1 to select tiles, key 2 to select npc
// mouse Wheel to select tile
// mouse Wheel press to place selected
// press 3 on tile if want to make it block movement
// press on tile if want to make it block vision
// L to save the level

Scene_House::Scene_House(GameEngine* game, const std::string& levelPath)
    : Scene(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}


void Scene_House::init(const std::string& levelPath)
{
    loadLevel(levelPath);
    loadTiles();


    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::A,     "PLAYER_LEFT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::E, "TOGGLE_HIDDEN");
    registerAction(sf::Keyboard::W,     "PLAYER_UP");
    registerAction(sf::Keyboard::S,     "PLAYER_DOWN");
    registerAction(sf::Keyboard::D,     "PLAYER_RIGHT");
    registerAction(sf::Keyboard::F, "SHOOT");
    registerAction(sf::Keyboard::Num1,     "TILE");
    registerAction(sf::Keyboard::Num2,     "NPC");
    registerAction(sf::Keyboard::Num3,     "BM");
    registerAction(sf::Keyboard::Num4,     "BV");
    registerAction(sf::Keyboard::Space, "SWORD");
    registerActionMouse(sf::Mouse::Right, "CLICK");
    registerActionMouse(sf::Mouse::Middle, "PLACE");
    registerActionMouse(sf::Mouse::XButton1, "DELETE");
    registerActionMouse(sf::Mouse::Wheel::VerticalWheel, "ROLL");
    registerAction(sf::Keyboard::L, "SAVE_LEVEL");
    registerAction(sf::Keyboard::U, "INCREASE_DIFFICULTY");
    registerAction(sf::Keyboard::J, "DECREASE_DIFFICULTY");
    registerAction(sf::Keyboard::H, "HELP");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));

    editText.setCharacterSize(17);
    editText.setFont(m_game->assets().getFont("Arial"));
    
}

void Scene_House::loadTiles()
{

    std::string componentName, animationIn;
	std::ifstream fin("tiles.txt");
    while (!myqueue.empty())
    {
        myqueue.pop();
    }

    	while (fin.good()) {
		fin >> componentName;
        if (componentName ==  insert ) {
            fin >> animationIn;
            myqueue.push(animationIn);
		}

    }

}

void Scene_House::loadLevel(const std::string& filename)
{
    m_entityManager = EntityManager();

    int TX, TY, RX, RY, BM, BV, PN, H, D;
	float followSpeed, PS;
    std::string componentName, animationIn, AI;
	Vec2 patrolPos;
	std::ifstream fin(filename);
    std::vector<Entity> tiles;
	while (fin.good()) {
		fin >> componentName;

		if (componentName == "Player") {
			fin >> m_playerConfig.X >> m_playerConfig.Y>> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.HEALTH >> m_playerConfig.JUMP >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY;
		}
        if (componentName == "NPC") {
			auto npc = m_entityManager.addEntity("npc");
			fin >> animationIn >> RX >> RY >> TX >> TY >> BM >> BV >> H >> D >> AI;

            auto animation = m_game->assets().getAnimation(animationIn);

			npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(animationIn).getSize(), BM, BV);
			npc->addComponent<CTransform>(Vec2(TX,TY));
			npc->addComponent<CAnimation>(animation, true);
            npc->addComponent<CHealth>(H, H);
            npc->addComponent<CDamage>(D);
            npc->getComponent<CTransform>().RX = RX;
            npc->getComponent<CTransform>().RY = RY;
			
			if (AI == "Patrol") {
				fin >> PS >> PN;
				std::vector<Vec2> positions;

				npc->addComponent<CPatrol>(positions, PS);
				
				for (auto i = 0; i < PN; i++) {
					fin >> patrolPos.x >> patrolPos.y;
                    npc->getComponent<CTransform>().positions.push_back(patrolPos);
					npc->getComponent<CPatrol>().positions.push_back(patrolPos);
                    npc->getComponent<CTransform>().PS = PS;
                    npc->getComponent<CTransform>().PN = PN;
				}	
			}
			if (AI == "Follow") {
				fin >> followSpeed;
				npc->addComponent<CFollowPlayer>(npc->getComponent<CTransform>().pos, followSpeed);
				npc->getComponent<CFollowPlayer>().home = npc->getComponent<CTransform>().pos;
                npc->addComponent<CDraggable>();
			}

            if (AI == "Find")
            {
                npc->addComponent<CFind>().find = true;
            }
		}
		if (componentName == "Tile") {
            fin >> animationIn >> RX >> RY >> TX >> TY >> BM >> BV;
			auto tile = m_entityManager.addEntity("tile");
            auto& animation = m_game->assets().getAnimation(animationIn);

			tile->addComponent<CAnimation>(animation, true);
			tile->addComponent<CBoundingBox>(animation.getSize(), BM, BV);
			tile->addComponent<CTransform>(Vec2(TX, TY));
            tile->getComponent<CTransform>().prevPos.x = tile->getComponent<CTransform>().pos.x; 
            tile->getComponent<CTransform>().prevPos.y = tile->getComponent<CTransform>().pos.y;
            tile->getComponent<CTransform>().RX = RX;
            tile->getComponent<CTransform>().RY = RY;
            tile->getComponent<CTransform>().TY = TY;
            tile->getComponent<CTransform>().TX = TX;
            tile->addComponent<CDraggable>();
            tiles.push_back(*tile);
		}
        if (componentName == "MovingTile") {
            fin >> animationIn >> RX >> RY >> TX >> TY >> BM >> BV >> PS >> PN;
            auto tile = m_entityManager.addEntity("tile");
            auto& animation = m_game->assets().getAnimation(animationIn);

            tile->addComponent<CAnimation>(animation, true);
            tile->addComponent<CBoundingBox>(animation.getSize(), BM, BV);
            tile->addComponent<CTransform>(Vec2(TX, TY));
            tile->getComponent<CTransform>().prevPos.x = tile->getComponent<CTransform>().pos.x;
            tile->getComponent<CTransform>().prevPos.y = tile->getComponent<CTransform>().pos.y;
            tile->getComponent<CTransform>().RX = RX;
            tile->getComponent<CTransform>().RY = RY;
            tile->getComponent<CTransform>().TY = TY;
            tile->getComponent<CTransform>().TX = TX;
            tile->addComponent<CDraggable>();

            std::vector<Vec2> positions;

            tile->addComponent<CPatrol>(positions, PS);

            for (auto i = 0; i < PN; i++) {
                fin >> patrolPos.x >> patrolPos.y;
                tile->getComponent<CTransform>().positions.push_back(patrolPos);
                tile->getComponent<CPatrol>().positions.push_back(patrolPos);
                tile->getComponent<CTransform>().PS = PS;
                tile->getComponent<CTransform>().PN = PN;
            }

            tiles.push_back(*tile);
        }
	}
    generateMap(tiles);
    spawnPlayer();
    m_game->playSound("MusicLevel");
}

Vec2 Scene_House::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    auto & vech = entity -> getComponent<CAnimation>().animation.getSize();

    return Vec2((m_gridSize.x*(gridX+(vech.x / m_gridSize.x))-(vech.x/2)), (m_gridSize.y*(11-gridY+1)-(vech.y/2)));
}

void Scene_House::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(
		Vec2(m_playerConfig.X, m_playerConfig.Y)
	);
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
    m_player->addComponent<CState>("up");
	m_player->addComponent<CInput>();
    m_player->addComponent<CDraggable>();
    m_player->addComponent<CInventory>();
}

void Scene_House::spawnSword(std::shared_ptr<Entity> entity)
{
	auto sword = m_entityManager.addEntity("sword");
    sword->addComponent<CDamage>(2);
	sword->addComponent<CTransform>(entity->getComponent<CTransform>().pos);
	sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"),true);
	sword->addComponent<CBoundingBox>(sword->getComponent<CAnimation>().animation.getSize(), false, false);
	sword->addComponent<CLifeSpan>(10, m_currentFrame);
    m_game->playSound("Slash");
    entity->getComponent<CInput>().attack = true;
}

void Scene_House::spawnBullet(std::shared_ptr<Entity> entity)
{
    if (m_player->getComponent<CTransform>().scale.x == -1)// if the player is looking left spawn the bullet tothe left
    {
        auto bullet = m_entityManager.addEntity("leftbullet");
        bullet->addComponent<CAnimation>(m_game->assets().getAnimation("Salt"), true);
        bullet->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>().pos.x, entity->getComponent<CTransform>().pos.y));
        bullet->addComponent<CLifeSpan>(25, m_currentFrame);
        bullet->getComponent<CTransform>().velocity.x = m_playerConfig.SPEED * 3;
    }

    if (m_player->getComponent<CTransform>().scale.x == 1)// if the player is looking right spawn the bullet to the right
    {
        auto bullet = m_entityManager.addEntity("rightbullet");
        bullet->addComponent<CAnimation>(m_game->assets().getAnimation("Salt"), true);
        bullet->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>().pos.x, entity->getComponent<CTransform>().pos.y));
        bullet->addComponent<CLifeSpan>(25, m_currentFrame);
        bullet->getComponent<CTransform>().velocity.x = m_playerConfig.SPEED * 3;
    }
}

void Scene_House::update()
{
    m_entityManager.update();

   if (!m_paused)
    {
        sAI();
        sMovement();
        sCollision();
        sAnimation();
        sStatus();
    }

    sDrag();
    sCamera();
    sRender();

    m_currentFrame++;
}

void Scene_House::sMovement()
{
   if (m_player->getComponent<CInput>().left)
    {
        m_player->getComponent<CTransform>().scale.x = -1;
        m_player->getComponent<CTransform>().velocity.x = -m_playerConfig.SPEED;
    }
    else if (m_player->getComponent<CInput>().right)
    {
        m_player->getComponent<CTransform>().scale.x = 1;
        m_player->getComponent<CTransform>().velocity.x = m_playerConfig.SPEED;
    }  
    else
    {
        m_player->getComponent<CTransform>().velocity.x = 0;
    }

    if (m_player->getComponent<CInput>().canJump)
    {
        if (m_player->getComponent<CInput>().up) 
        {
            m_player->getComponent<CTransform>().velocity.y = m_playerConfig.JUMP;
            m_player->getComponent<CInput>().canJump = false;
            m_player->getComponent<CState>().state = "up";
        }
    }
    // update prev position
    m_player->getComponent<CTransform>().prevPos.x = m_player->getComponent<CTransform>().pos.x; 
    m_player->getComponent<CTransform>().prevPos.y = m_player->getComponent<CTransform>().pos.y;
    
    // Maximum player speed in the x direction
    if (m_player->getComponent<CTransform>().velocity.x < m_playerConfig.MAXSPEED)
    {
        m_player->getComponent<CTransform>().pos.x += m_player->getComponent<CTransform>().velocity.x;
    }
    else 
    {
        m_player->getComponent<CTransform>().pos.x += m_playerConfig.MAXSPEED;
    }
    // Maximum player speed in the y direction
    if (m_player->getComponent<CTransform>().velocity.y < m_playerConfig.MAXSPEED)
    {
        m_player->getComponent<CTransform>().velocity.y += m_playerConfig.GRAVITY;
        m_player->getComponent<CTransform>().pos.y += m_player->getComponent<CTransform>().velocity.y;
    }
    else 
    {
        m_player->getComponent<CTransform>().pos.y += m_playerConfig.MAXSPEED;
    }
    //attacking
    if (m_player->getComponent<CInput>().attack)
	{
		for (auto sword : m_entityManager.getEntities("sword"))
		{
			sword->getComponent<CTransform>().pos = m_player->getComponent<CTransform>().pos;

			if ( m_player->getComponent<CTransform>().scale.x == 1)
			{
				sword->getComponent<CTransform>().pos.x += 140;
				sword->getComponent<CTransform>().scale = Vec2(1,1);
				sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
				m_player->getComponent<CState>().state = "swordRight";
			}
			else if ( m_player->getComponent<CTransform>().scale.x == -1)
			{
				sword->getComponent<CTransform>().pos.x -= 140;
				sword->getComponent<CTransform>().scale = Vec2(-1, 1);
				sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
				m_player->getComponent<CState>().state = "swordRight";
			}
		}
	}
    for (auto e : m_entityManager.getEntities())// for all of the bullets, move them at a velocity that is a multiple of the players velocity, and set the previous position.
    {
        if (e->tag() == "leftbullet")
        {

            e->getComponent<CTransform>().prevPos.x = e->getComponent<CTransform>().pos.x;
            e->getComponent<CTransform>().prevPos.y = e->getComponent<CTransform>().pos.y;
            e->getComponent<CTransform>().velocity.x = e->getComponent<CTransform>().velocity.x;
            e->addComponent<CBoundingBox>(m_game->assets().getAnimation("Salt").getSize());
            e->getComponent<CTransform>().pos.x -= e->getComponent<CTransform>().velocity.x;
            if (e->getComponent<CTransform>().velocity.y < m_playerConfig.MAXSPEED)
            {
                e->getComponent<CTransform>().velocity.y += m_playerConfig.GRAVITY;
                e->getComponent<CTransform>().pos.y += e->getComponent<CTransform>().velocity.y;
            }
        }
        if (e->tag() == "rightbullet")
        {
            e->getComponent<CTransform>().prevPos.x = e->getComponent<CTransform>().pos.x;
            e->getComponent<CTransform>().prevPos.y = e->getComponent<CTransform>().pos.y;

            e->addComponent<CBoundingBox>(m_game->assets().getAnimation("Salt").getSize());
            e->getComponent<CTransform>().pos.x += e->getComponent<CTransform>().velocity.x;
            if (e->getComponent<CTransform>().velocity.y < m_playerConfig.MAXSPEED)
            {
                e->getComponent<CTransform>().velocity.y += m_playerConfig.GRAVITY;
                e->getComponent<CTransform>().pos.y += e->getComponent<CTransform>().velocity.y;
            }
        }
    }
    for (auto& mtile : m_entityManager.getEntities("tile"))
    {
        if (mtile->hasComponent<CPatrol>())
        {
            size_t npcCurrentPos = mtile->getComponent<CPatrol>().currentPosition;
            size_t npcNextPos = npcCurrentPos + 1;
            // back to inintial point
            if (npcCurrentPos >= mtile->getComponent<CPatrol>().positions.size() - 1)
            {
                npcNextPos = 0;
            }
            //move from point to point
            if (mtile->getComponent<CTransform>().pos.dist(mtile->getComponent<CPatrol>().positions[npcNextPos]) < 5)
            {
                mtile->getComponent<CPatrol>().currentPosition = npcNextPos;
            }

            Vec2 dirVec = mtile->getComponent<CPatrol>().positions[npcNextPos] - mtile->getComponent<CPatrol>().positions[npcCurrentPos];
            Vec2 stepVec = Vec2(dirVec.x / (sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2))), dirVec.y / (sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2)))) * mtile->getComponent<CPatrol>().speed;

            mtile->getComponent<CTransform>().pos += stepVec;
        }
    }
}

void Scene_House::sDrag()
{
    
	if (m_drag == NULL) { return; }
    
    if(m_drawGrid)
	{		
        auto xpos = mouse.x;
		auto ypos = mouse.y;

		auto xposGrid = floor(xpos/64) * 64 + 32;
		auto yposGrid  = floor(ypos/64) * 64 + 32;
		m_drag->getComponent<CTransform>().pos = Vec2(xposGrid, yposGrid);
        m_drag->getComponent<CDraggable>().canDrag = false;
	} else 
    {
        if (m_drag->getComponent<CDraggable>().canDrag == true)
	    {
		    m_drag->getComponent<CTransform>().pos = mouse;
	    }
         m_drag->getComponent<CDraggable>().canDrag = false;
    }
	
}

void Scene_House::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        
             if (action.name() == "PAUSE") { setPaused(!m_paused); }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "TOGGLE_FOLLOW") { m_follow = !m_follow; }
        else if (action.name() == "TOGGLE_GRID")        { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "TOGGLE_HIDDEN") {
                 if (canhide == true)
                 {
                     ishidden = !ishidden;

                 }
             }
        else if (action.name() == "TILE")        { insert = "Tile"; loadTiles();}
        else if (action.name() == "NPC")        { insert = "NPC"; loadTiles();}
         else if (action.name() == "HELP")        { m_drawHelp = !m_drawHelp;}
        else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "PLAYER_UP")          { m_player->getComponent<CInput>().up = true; }
        else if (action.name() == "PLAYER_DOWN")        { m_player->getComponent<CInput>().down = true; }
        else if (action.name() == "PLAYER_LEFT")        { m_player->getComponent<CInput>().left = true;}
        else if (action.name() == "PLAYER_RIGHT")       { m_player->getComponent<CInput>().right = true; }
        else if (action.name() == "SWORD")              { spawnSword(m_player); }
        else if (action.name() == "SAVE_LEVEL")              {saveLevel(); }
        else if (action.name() == "SHOOT" && m_player->getComponent<CInput>().canShoot == true) {
                 m_player->getComponent<CInput>().shoot = true;
                 spawnBullet(m_player);
                 m_player->getComponent<CInput>().canShoot = false;
                 m_player->getComponent<CInventory>().currentSalt -= 1;
             }
        else if (action.name() == "BM")              
        {   
            for(auto & e: m_entityManager.getEntities())
            {
				if (Physics::IsInside(mouse, e) && e->tag() != "player") 
				{
					e->getComponent<CBoundingBox>().blockMove = !e->getComponent<CBoundingBox>().blockMove;
								
				}  
            } 
        }
        else if (action.name() == "BV")              
        {   
            for(auto & e: m_entityManager.getEntities())
            {
				if (Physics::IsInside(mouse, e) && e->tag() != "player") 
				{
					e->getComponent<CBoundingBox>().blockVision = !e->getComponent<CBoundingBox>().blockVision;
								
				}  
            } 
        }
        else if (action.name() == "INCREASE_DIFFICULTY") { increaseDifficulty(); }
        else if (action.name() == "DECREASE_DIFFICULTY") { decreaseDifficulty(); }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "PLAYER_UP")               { m_player->getComponent<CInput>().up = false; }
        else if (action.name() == "PLAYER_DOWN")        { m_player->getComponent<CInput>().down = false; }
        else if (action.name() == "PLAYER_LEFT")        { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "PLAYER_RIGHT")       { m_player->getComponent<CInput>().right = false; }
        else if (action.name() == "SHOOT" && m_player->getComponent<CInput>().canShoot == false)
        {
            m_player->getComponent<CInput>().shoot = false;
            m_player->getComponent<CInput>().canShoot = true;
        }
    }
    else if (action.type() == "MSTART")
    {
       if (action.name() == "CLICK")              
       { 
               if (m_drawGrid)
    {
        for(auto & e: m_entityManager.getEntities())
            {
                if(e->hasComponent<CDraggable>())
                {  
					if (Physics::IsInside(mouse, e)) 
					{
						m_drag = e;
						m_drag->getComponent<CDraggable>().canDrag = true;
								
					}
					            
                }
                
            }
            }
        }
        if (action.name() == "DELETE")              
       { 
    if (m_drawGrid)
    {
        for(auto & e: m_entityManager.getEntities())
            {

				if (Physics::IsInside(mouse, e) && e->tag() != "player") 
				{
					e->destroy();
								
				}

                
            }
       }
        }
        if (action.name() == "ROLL"){
        if (m_drawGrid)
    {
            if (m_add != NULL){
                m_add->destroy();
            } 
            if (insert == "Tile")
            {
                m_add = m_entityManager.addEntity("tile");

            }
            if (insert == "NPC")
            {
                m_add = m_entityManager.addEntity("npc");
                m_add->addComponent<CHealth>(5, 5);
                m_add->addComponent<CDamage>(2);
                m_add->getComponent<CTransform>().RX = 0;
                m_add->getComponent<CTransform>().RY = 0;
                m_add->addComponent<CFollowPlayer>(m_add->getComponent<CTransform>().pos, 0);
                m_add->getComponent<CFollowPlayer>().home = mouse;

            }

            m_addAnimation = myqueue.front();
            auto animation = m_game->assets().getAnimation(m_addAnimation);

			m_add->addComponent<CAnimation>(animation, true);
			m_add->addComponent<CBoundingBox>(animation.getSize(), 0, 0);

            m_add->addComponent<CTransform>(mouse);
            m_add->getComponent<CTransform>().prevPos.x = m_add->getComponent<CTransform>().pos.x; 
            m_add->getComponent<CTransform>().prevPos.y = m_add->getComponent<CTransform>().pos.y;
            m_add->addComponent<CDraggable>();
            myqueue.pop();
            myqueue.push(m_addAnimation);
        }

        }
        if (action.name() == "PLACE")
        {
            if (m_drawGrid)
            {
            if (m_add != NULL && m_addAnimation != ""){
                    if (insert == "Tile")
                    {
                        m_addPrev = m_entityManager.addEntity("tile");

                    }
                    if (insert == "NPC")
                    {
                        m_addPrev = m_entityManager.addEntity("npc");
                        m_addPrev->addComponent<CHealth>(5, 5);
                        m_addPrev->addComponent<CDamage>(2);
                        m_addPrev->getComponent<CTransform>().RX = 0;
                        m_addPrev->getComponent<CTransform>().RY = 0;
                        m_addPrev->addComponent<CFollowPlayer>(m_addPrev->getComponent<CTransform>().pos, 2);
				        m_addPrev->getComponent<CFollowPlayer>().home = mouse;


                    }
                   auto animation = m_game->assets().getAnimation(m_addAnimation);

			       m_addPrev->addComponent<CAnimation>(animation, true);
			       m_addPrev->addComponent<CBoundingBox>(animation.getSize(), 0, 0);

                   m_addPrev->addComponent<CTransform>(mouse);
                    if (insert == "Tile")
                    {
                       m_addPrev->getComponent<CTransform>().prevPos.x = m_addPrev->getComponent<CTransform>().pos.x; 
                       m_addPrev->getComponent<CTransform>().prevPos.y = m_addPrev->getComponent<CTransform>().pos.y;

                    }
                   m_addPrev->addComponent<CDraggable>();
             
            } 
            if (m_add != NULL){
                m_add->destroy();
            } 

        }
        }

    }
    else if (action.type() == "MEND")
    {
        if (action.name() == "CLICK")              
        { 
			if(m_drag != NULL)
            {
                m_drag->getComponent<CDraggable>().canDrag = false;
				m_drag = NULL;
						
						

            }
        }
        if (action.name() == "ROLL"){
        }

    }
}

void Scene_House::increaseDifficulty()
{
    for (auto& e : m_entityManager.getEntities("npc"))
    {
        e->getComponent<CHealth>().max *= 2;
        e->getComponent<CHealth>().current *= 2;
    }
}

void Scene_House::decreaseDifficulty()
{
    for (auto& e : m_entityManager.getEntities("npc"))
    {
        e->getComponent<CHealth>().max /= 2;
        e->getComponent<CHealth>().current /= 2;
    }
}

std::vector<Vec2> pathOf(const Node *node)
{
    return node->parent;
}

bool Scene_House::isLegalAction(Vec2 action, Vec2 state)
{
    try
    {
        return m_levelRep.at(state.y + action.y).at(state.x + action.x) == 0;
    }
    catch (...)
    {
        return false;
    }
}

void Scene_House::generateMap(std::vector<Entity> tiles)
{
    // determine size of map
    int min[2] = { INT_MAX, INT_MAX };
    int max[2] = { INT_MIN, INT_MIN };
    for (auto& tile : tiles)
    {
        CTransform t = tile.getComponent<CTransform>();
        if (t.TX < min[0]){ min[0] = t.TX; }
        if (t.TY < min[1]){ min[1] = t.TY; }
        if (t.TX > max[0]){ max[0] = t.TX; }
        if (t.TY > max[1]){ max[1] = t.TY; }
    }
    int size[2] = { max[0] - min[0], max[1] - min[1] };
    // adjust for grid size
    size[0] /= 64;
    size[1] /= 64;
    // add padding to left, right, top and bottom to account for integer division
    size[0] += 2;
    size[1] += 2;

    // initialize map

    for (int i = 0; i < size[1]; i++)
    {
        m_levelRep.push_back(std::vector<int>());
        auto& back = m_levelRep.back();
        for (int j = 0; j < size[0]; j++)
        {
            back.push_back(0);
        }
    }

    for (auto& tile : tiles)
    {
        CTransform t = tile.getComponent<CTransform>();
        CBoundingBox b = tile.getComponent<CBoundingBox>();
        if (b.blockMove)
        {
            int xGrid = t.TX / 64;
            int yGrid = (max[1] - t.TY) / 64;
            m_levelRep.at(yGrid).at(xGrid) = 1;
        }
    }    
}

float Scene_House::estimateCost(int startx, int starty, int goalx, int goaly)
{
    return pow(startx - goalx, 2) + pow(starty - goaly, 2);
}

std::vector<Vec2> Scene_House::pathfind(int startx, int starty, int goalx, int goaly)
{
    std::vector<Node> openCopy;
    std::priority_queue<Node, std::vector<Node>, std::greater<std::vector<Node>::value_type>> open;
    std::vector<Vec2> closed;

    std::vector<Vec2> actions;
    actions.push_back(Vec2(0, 1));
    actions.push_back(Vec2(0, -1));
    actions.push_back(Vec2(1, 0));
    actions.push_back(Vec2(-1, 0));

    auto startNode = new Node(Vec2(startx, starty), Vec2(0,0), 0, estimateCost(startx, starty, goalx, goaly));
    open.push(*startNode);
    openCopy.push_back(*startNode);
    while (!open.empty())
    {
        const Node node = open.top();
        open.pop();
        for (int i = 0; i < openCopy.size(); i++)
        {
            if (openCopy.at(i).state == node.state
                && openCopy.at(i).g == node.g
                && openCopy.at(i).h == node.h
                && openCopy.at(i).action == node.action)
            {
                openCopy.erase(openCopy.begin() + i);
                break;
            }
        }
        if (node.state == Vec2(goalx, goaly)) { return pathOf(&node); }
        bool a = false;
        for (Vec2 v : closed)
        {
            if (v == node.state) { a = true; }
        }
        if (a) { continue; }
        closed.push_back(node.state);
        for (Vec2 action : actions)
        {
            if (!isLegalAction(action, node.state)) { continue; }
            Node* newNode = new Node;
            newNode->state = node.state + action;
            newNode->action = action;
            newNode->parent = node.parent;
            newNode->parent.push_back(action);
            newNode->g = node.g + 1;
            newNode->h = estimateCost(node.state.x + action.x, node.state.y + action.y, goalx, goaly);
            bool b = false;
            for (Vec2 state : closed)
            {                
                if (state == newNode->state) { b = true; }
            }
            if (b) { continue; }
            bool c = false;
            for (Node const &n : openCopy)
            {
                if (n.state == newNode->state && n.g <= newNode->g) { c = true; }
            }
            if (c) { continue; }
            open.push(*newNode);
            openCopy.push_back(*newNode);
        }
    }
    return std::vector<Vec2>();
}

void Scene_House::sAI()
{
	for (auto npc : m_entityManager.getEntities("npc"))
	{
        if (npc->hasComponent<CFind>())
        {
            if (npc->getComponent<CFind>().find && m_currentFrame > 0 && !ishidden)
            {
                int min[2] = { INT_MAX, INT_MAX };
                int max[2] = { INT_MIN, INT_MIN };
                for (auto& tile : m_entityManager.getEntities("tile"))
                {
                    CTransform t = tile->getComponent<CTransform>();
                    if (t.TX < min[0]) { min[0] = t.TX; }
                    if (t.TY < min[1]) { min[1] = t.TY; }
                    if (t.TX > max[0]) { max[0] = t.TX; }
                    if (t.TY > max[1]) { max[1] = t.TY; }
                }
                Vec2 start = npc->getComponent<CTransform>().pos ;
                Vec2 goal = m_player->getComponent<CTransform>().pos;
                std::vector<Vec2> path = pathfind(start.x/64, (max[1] - start.y)/64, goal.x/64, (max[1] - goal.y)/64);
                if (!path.empty())
                {
                    path.at(0).y *= -1;
                    npc->getComponent<CTransform>().pos += path.at(0) * 2;
                }
            }
        }
		if (npc->hasComponent<CPatrol>())
		{
			size_t npcCurrentPos = npc->getComponent<CPatrol>().currentPosition;
			size_t npcNextPos = npcCurrentPos + 1;
            // back to inintial point
            if (npcCurrentPos >= npc->getComponent<CPatrol>().positions.size() - 1)
			{
				npcNextPos = 0;
			}
            //move from point to point
			if (npc->getComponent<CTransform>().pos.dist(npc->getComponent<CPatrol>().positions[npcNextPos]) < 5)
			{
				npc->getComponent<CPatrol>().currentPosition = npcNextPos;
			}

			Vec2 dirVec = npc->getComponent<CPatrol>().positions[npcNextPos] - npc->getComponent<CPatrol>().positions[npcCurrentPos];
			Vec2 stepVec = Vec2(dirVec.x/(sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2))), dirVec.y/(sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2)))) * npc->getComponent<CPatrol>().speed;

			npc->getComponent<CTransform>().pos += stepVec;
		}
        if (npc->hasComponent<CFollowPlayer>())
		{
			bool iSeeYou = true;
            // if tile blocks vision npc can't see player
			for (auto tile : m_entityManager.getEntities("tile"))
			{
				if (tile->hasComponent<CBoundingBox>())
				{
					if (tile->getComponent<CBoundingBox>().blockVision)
					{
						if (Physics::EntityIntersect(npc->getComponent<CTransform>().pos, m_player->getComponent<CTransform>().pos, tile))
						{
							iSeeYou = false;
						}
					}	
				}
			}
			if (iSeeYou && !ishidden)
			{
                //move to player's direction
				Vec2 dirVec = m_player->getComponent<CTransform>().pos - npc->getComponent<CTransform>().pos;
				Vec2 stepVec = Vec2(dirVec.x/(sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2))), dirVec.y/(sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2)))) * npc->getComponent<CFollowPlayer>().speed;

				npc->getComponent<CTransform>().prevPos = npc->getComponent<CTransform>().pos;
				npc->getComponent<CTransform>().pos += stepVec;
			}
			else
			{
                //move to home position
				if (npc->getComponent<CTransform>().pos.dist(npc->getComponent<CFollowPlayer>().home) >= 5)
				{
					Vec2 dirVec = npc->getComponent<CFollowPlayer>().home - npc->getComponent<CTransform>().pos;
					Vec2 stepVec = Vec2(dirVec.x/(sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2))), dirVec.y/(sqrt(pow(dirVec.x, 2) + pow(dirVec.y, 2)))) * npc->getComponent<CFollowPlayer>().speed;

					npc->getComponent<CTransform>().prevPos = npc->getComponent<CTransform>().pos;
					npc->getComponent<CTransform>().pos += stepVec;
				}
			}
		}
	}
}

void Scene_House::gameOver()
{
    m_gameOver = true;
}

void Scene_House::sStatus()
{
    for (auto e : m_entityManager.getEntities())
    {
        // Implement Lifespan Here
        if (e->getComponent<CLifeSpan>().lifespan)
        {
            float remaining = e->getComponent<CLifeSpan>().lifespan - (m_currentFrame - e->getComponent<CLifeSpan>().frameCreated);
            if (remaining <= 0)
            {
                if (e->tag()=="sword")
				{
					m_player->getComponent<CInput>().attack = false;
				}
                e->destroy();
            }
        }
        // Implement Invincibility Frames
        if (e->hasComponent<CInvincibility>())
        {
            if (e->getComponent<CInvincibility>().iframes <= 0)
            {
                e->removeComponent<CInvincibility>();
            }
            e->getComponent<CInvincibility>().iframes = e->getComponent<CInvincibility>().iframes - 1;
        }

        if (e->hasComponent<CInventory>())
        {
            if (e->getComponent<CInventory>().currentSalt <= 0)
            {
                m_player->getComponent<CInput>().canShoot = false;
            }
        }
    }
}

void Scene_House::sCollision()
{
    for (auto& tile : m_entityManager.getEntities("tile"))
    {
        for (auto& player : m_entityManager.getEntities("player"))
        {
            Vec2 overlap = Physics::GetOverlap(player, tile);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(player, tile);

            if (overlap.x > 0 && overlap.y > 0)
            {
                if (tile->hasComponent<CBoundingBox>() && tile->getComponent<CBoundingBox>().blockMove)
                {
                    // collision from top 
                    if (prevOverlap.x > 0 && player->getComponent<CTransform>().pos.y < tile->getComponent<CTransform>().pos.y)
                    {
                        player->getComponent<CInput>().canJump = true;
                        player->addComponent<CState>("stand");
                        if (m_player->getComponent<CInput>().left)
                        {
                            m_player->addComponent<CState>("right");
                        }
                        else if (m_player->getComponent<CInput>().right)
                        {
                            m_player->addComponent<CState>("right");
                        }
                        player->getComponent<CTransform>().prevPos.y = player->getComponent<CTransform>().pos.y;
                        player->getComponent<CTransform>().pos.y -= overlap.y;
                        player->getComponent<CTransform>().velocity.y = 0;
                    }
                    // collision from below
                    else if (prevOverlap.x > 0 && player->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                    {
                        player->getComponent<CTransform>().velocity.y = 0;
                        player->getComponent<CTransform>().prevPos.y = player->getComponent<CTransform>().pos.y;
                        player->getComponent<CTransform>().pos.y += overlap.y;
                    }
                    // collision from left
                    if (prevOverlap.y > 0 && player->getComponent<CTransform>().pos.x < tile->getComponent<CTransform>().pos.x)
                    {
                        player->getComponent<CTransform>().prevPos.x = player->getComponent<CTransform>().pos.x;
                        player->getComponent<CTransform>().pos.x -= overlap.x;
                    }
                    // collision from right
                    if (prevOverlap.y > 0 && player->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                    {
                        player->getComponent<CTransform>().prevPos.x = m_player->getComponent<CTransform>().pos.x;
                        player->getComponent<CTransform>().pos.x += overlap.x;
                    }
                    // special case: No overlap in previous frame, push out in direction of most overlap
                    if (prevOverlap.y <= 0 && prevOverlap.x <= 0)
                    {
                        if (overlap.x > overlap.y)
                        {
                            if (player->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                            {
                                player->getComponent<CTransform>().prevPos.x = m_player->getComponent<CTransform>().pos.x;
                                player->getComponent<CTransform>().pos.x += overlap.x;
                            }
                            else
                            {
                                player->getComponent<CTransform>().prevPos.x = m_player->getComponent<CTransform>().pos.x;
                                player->getComponent<CTransform>().pos.x += overlap.x;
                            }
                        }
                        else
                        {
                            if (player->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                            {
                                player->getComponent<CTransform>().prevPos.y = m_player->getComponent<CTransform>().pos.y;
                                player->getComponent<CTransform>().pos.y += overlap.y;
                            }
                            else
                            {
                                player->getComponent<CTransform>().prevPos.y = m_player->getComponent<CTransform>().pos.y;
                                player->getComponent<CTransform>().pos.y -= overlap.y;
                            }
                        }
                    }
                }
                // entity - heart collisions and life gain logic
                if (tile->getComponent<CAnimation>().animation.getName() == "Heart")
                {
                    tile->getComponent<CAnimation>().repeat = false;
                    tile->removeComponent<CBoundingBox>();
                    m_player->getComponent<CHealth>().current = m_player->getComponent<CHealth>().max;
                    m_game->playSound("GetItem");
                }
                if (tile->getComponent<CAnimation>().animation.getName() == "Clock")
                {
                    tile->getComponent<CAnimation>().repeat = false;
                    tile->removeComponent<CBoundingBox>();
                    m_player->getComponent<CInventory>().clocks++;
                    m_game->playSound("GetItem");
                    if (m_player->getComponent<CInventory>().clocks == 2)
                    {
                        std::string filename = "progress.txt";
                        std::string level;
                        std::vector<std::string> progress;
                        std::ifstream fin(filename);
	                    while (fin.good())
                        {
                            fin >> level;
                            progress.push_back(level);
                        }
                        progress.push_back(m_levelPath);
                        std::ofstream fout;
                        fout.open(filename);
                        sort( progress.begin(), progress.end() );
                        progress.erase( unique( progress.begin(), progress.end() ), progress.end() );
                        for (int i = 0; i < progress.size(); i++)
                        {
                            fout << progress[i] << std::endl;

                        }
                        m_game->assets().getSound("MusicLevel").stop();
                        m_game->changeScene("MENU", nullptr, true);
                    }
                }
                if (tile->getComponent<CAnimation>().animation.getName() == "Key")
                {
                    tile->getComponent<CAnimation>().repeat = false;
                    tile->removeComponent<CBoundingBox>();
                    m_player->getComponent<CInventory>().key =true;
                    m_game->playSound("GetItem");
                }
                if (tile->getComponent<CAnimation>().animation.getName() == "Door")
                {
                    if (m_player->getComponent<CInventory>().key)
                    {
                        tile->addComponent<CAnimation>(m_game->assets().getAnimation("DoorOpen"),true);
                        tile->removeComponent<CBoundingBox>();
                        m_game->playSound("GetItem");

                    }
                }
                if (tile->getComponent<CAnimation>().animation.getName() == "Salt")
                {
                    m_player->getComponent<CInventory>().currentSalt += 1;
                    tile->getComponent<CAnimation>().repeat = false;
                    tile->removeComponent<CBoundingBox>();
                    m_game->playSound("GetItem");
                }
                // black tile collisions / 'teleporting'
                if (tile->getComponent<CAnimation>().animation.getName() == "Black")
                {
                    std::vector<Vec2> BlackPos;
                    for (auto& portal : m_entityManager.getEntities("tile"))
                    {
                        if (portal->getComponent<CAnimation>().animation.getName() == "Black")
                        {
                            BlackPos.push_back(portal->getComponent<CTransform>().pos);
                        }
                    }
                    // choose random portal other than current portal
                    int r = rand() % BlackPos.size();
                    if (m_player->getComponent<CTransform>().pos.x == BlackPos[r].x)
                    {
                        if (r == BlackPos.size() - 1)
                        {
                            r = r - 1;
                        }
                        else
                        {
                            r = r + 1;
                        }
                        m_player->getComponent<CTransform>().prevPos.x = m_player->getComponent<CTransform>().pos.x;
                        m_player->getComponent<CTransform>().prevPos.y = m_player->getComponent<CTransform>().pos.y;
                        m_player->getComponent<CTransform>().pos.x = BlackPos[r].x;
                        m_player->getComponent<CTransform>().pos.y = BlackPos[r].y + m_player->getComponent<CBoundingBox>().size.y + 50;
                    }
                    else
                    {
                        m_player->getComponent<CTransform>().prevPos.x = m_player->getComponent<CTransform>().pos.x;
                        m_player->getComponent<CTransform>().prevPos.y = m_player->getComponent<CTransform>().pos.y;
                        m_player->getComponent<CTransform>().pos.x = BlackPos[r].x;
                        m_player->getComponent<CTransform>().pos.y = BlackPos[r].y + m_player->getComponent<CBoundingBox>().size.y + 50;
                    }
                }
                if (tile->getComponent<CAnimation>().animation.getName() == "Bed" || tile->getComponent<CAnimation>().animation.getName() == "Fridge" || tile->getComponent<CAnimation>().animation.getName() == "Door" || tile->getComponent<CAnimation>().animation.getName() == "Boxes")
                {
                    canhide = true;
                    if (tile->getComponent<CAnimation>().animation.getName() == "Bed")
                    {
                        tile->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Bed_Hide");
                        tile->addComponent<CAnimation>(animation, true);
                    }
                    else if (tile->getComponent<CAnimation>().animation.getName() == "Fridge")
                    {
                        tile->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Fridge_Hide");
                        tile->addComponent<CAnimation>(animation, true);
                    }
                    if (tile->getComponent<CAnimation>().animation.getName() == "Boxes")
                    {
                        tile->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Boxes_Hide");
                        tile->addComponent<CAnimation>(animation, true);
                    }
                    if (tile->getComponent<CAnimation>().animation.getName() == "Door")
                    {
                        tile->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Door_Hide");
                        tile->addComponent<CAnimation>(animation, true);
                    }
                }
            }
            else if ((overlap.x < 0 || overlap.y < 0) && (tile->getComponent<CAnimation>().animation.getName() == "Bed_Hide" || tile->getComponent<CAnimation>().animation.getName() == "Fridge_Hide" || tile->getComponent<CAnimation>().animation.getName() == "Door_Hide" || tile->getComponent<CAnimation>().animation.getName() == "Boxes_Hide"))
            {

                if (tile->getComponent<CAnimation>().animation.getName() == "Bed_Hide")
                {
                    canhide = false;
                    ishidden = false;
                    tile->removeComponent<CAnimation>();
                    auto animation = m_game->assets().getAnimation("Bed");
                    tile->addComponent<CAnimation>(animation, true);
                }
                else if (tile->getComponent<CAnimation>().animation.getName() == "Fridge_Hide")
                {
                    canhide = false;
                    ishidden = false;
                    tile->removeComponent<CAnimation>();
                    auto animation = m_game->assets().getAnimation("Fridge");
                    tile->addComponent<CAnimation>(animation, true);
                }
                else if (tile->getComponent<CAnimation>().animation.getName() == "Boxes_Hide")
                {
                    canhide = false;
                    ishidden = false;
                    tile->removeComponent<CAnimation>();
                    auto animation = m_game->assets().getAnimation("Boxes");
                    tile->addComponent<CAnimation>(animation, true);
                }
                else if (tile->getComponent<CAnimation>().animation.getName() == "Door_Hide")
                {
                    canhide = false;
                    ishidden = false;
                    tile->removeComponent<CAnimation>();
                    auto animation = m_game->assets().getAnimation("Door");
                    tile->addComponent<CAnimation>(animation, true);
                }
            }
            else if ((player->getComponent<CTransform>().pos.x - player->getComponent<CBoundingBox>().halfSize.x) < 0)//The player cannot go to the left of the screen.
            {
            player->getComponent<CTransform>().pos.x += m_playerConfig.SPEED;
            }
        }
        for (auto& enemy : m_entityManager.getEntities("npc"))
        {
            Vec2 overlap = Physics::GetOverlap(enemy, tile);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(enemy, tile);

            if (overlap.x > 0 && overlap.y > 0)
            {
                if (tile->hasComponent<CBoundingBox>() && tile->getComponent<CBoundingBox>().blockMove)
                {
                    // collision from top 
                    if (prevOverlap.x > 0 && enemy->getComponent<CTransform>().pos.y < tile->getComponent<CTransform>().pos.y)
                    {
                        enemy->getComponent<CTransform>().prevPos.y = enemy->getComponent<CTransform>().pos.y;
                        enemy->getComponent<CTransform>().pos.y -= overlap.y;
                        enemy->getComponent<CTransform>().velocity.y = 0;
                    }
                    // collision from below
                    else if (prevOverlap.x > 0 && enemy->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                    {
                        enemy->getComponent<CTransform>().velocity.y = 0;
                        enemy->getComponent<CTransform>().prevPos.y = enemy->getComponent<CTransform>().pos.y;
                        enemy->getComponent<CTransform>().pos.y += overlap.y;
                    }
                    // collision from left
                    if (prevOverlap.y > 0 && enemy->getComponent<CTransform>().pos.x < tile->getComponent<CTransform>().pos.x)
                    {
                        enemy->getComponent<CTransform>().prevPos.x = enemy->getComponent<CTransform>().pos.x;
                        enemy->getComponent<CTransform>().pos.x -= overlap.x;
                    }
                    // collision from right
                    if (prevOverlap.y > 0 && enemy->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                    {
                        enemy->getComponent<CTransform>().prevPos.x = enemy->getComponent<CTransform>().pos.x;
                        enemy->getComponent<CTransform>().pos.x += overlap.x;
                    }
                    // special case: No overlap in previous frame, push out in direction of most overlap
                    if (prevOverlap.y <= 0 && prevOverlap.x <= 0)
                    {
                        if (overlap.x > overlap.y)
                        {
                            if (enemy->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                            {
                                enemy->getComponent<CTransform>().prevPos.x = enemy->getComponent<CTransform>().pos.x;
                                enemy->getComponent<CTransform>().pos.x += overlap.x;
                            }
                            else
                            {
                                enemy->getComponent<CTransform>().prevPos.x = enemy->getComponent<CTransform>().pos.x;
                                enemy->getComponent<CTransform>().pos.x += overlap.x;
                            }
                        }
                        else
                        {
                            if (enemy->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                            {
                                enemy->getComponent<CTransform>().prevPos.y = enemy->getComponent<CTransform>().pos.y;
                                enemy->getComponent<CTransform>().pos.y += overlap.y;
                            }
                            else
                            {
                                enemy->getComponent<CTransform>().prevPos.y = enemy->getComponent<CTransform>().pos.y;
                                enemy->getComponent<CTransform>().pos.y -= overlap.y;
                            }
                        }
                    }
                }

                // enemy - heart collisions and life gain logic
                if (tile->getComponent<CAnimation>().animation.getName() == "Heart")
                {
                    tile->getComponent<CAnimation>().repeat = false;
                    tile->removeComponent<CBoundingBox>();
                    enemy->getComponent<CHealth>().current = enemy->getComponent<CHealth>().max;
                    m_game->playSound("GetItem");
                }
            }
        }
        for (auto& salt : m_entityManager.getEntities("rightbullet"))
        {
            Vec2 overlap = Physics::GetOverlap(salt, tile);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(salt, tile);

            if (overlap.x > 0 && overlap.y > 0)
            {
                if (tile->hasComponent<CBoundingBox>() && tile->getComponent<CBoundingBox>().blockMove)
                {
                    // collision from top 
                    if (prevOverlap.x > 0 && salt->getComponent<CTransform>().pos.y < tile->getComponent<CTransform>().pos.y)
                    {
                        salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                        salt->getComponent<CTransform>().pos.y -= overlap.y;
                        salt->getComponent<CTransform>().velocity.y = 0;
                        salt->getComponent<CTransform>().velocity.x = 0;
                    }
                    // collision from below
                    else if (prevOverlap.x > 0 && salt->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                    {
                        salt->getComponent<CTransform>().velocity.y = 0;
                        salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                        salt->getComponent<CTransform>().pos.y += overlap.y;
                        salt->getComponent<CTransform>().velocity.x = 0;
                    }
                    // collision from left
                    if (prevOverlap.y > 0 && salt->getComponent<CTransform>().pos.x < tile->getComponent<CTransform>().pos.x)
                    {
                        salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                        salt->getComponent<CTransform>().pos.x -= overlap.x;
                        salt->getComponent<CTransform>().velocity.x = 0;
                    }
                    // collision from right
                    if (prevOverlap.y > 0 && salt->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                    {
                        salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                        salt->getComponent<CTransform>().pos.x += overlap.x;
                        salt->getComponent<CTransform>().velocity.x = 0;
                    }
                    // special case: No overlap in previous frame, push out in direction of most overlap
                    if (prevOverlap.y <= 0 && prevOverlap.x <= 0)
                    {
                        if (overlap.x > overlap.y)
                        {
                            if (salt->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                            {
                                salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                                salt->getComponent<CTransform>().pos.x += overlap.x;
                                salt->getComponent<CTransform>().velocity.x = 0;
                            }
                            else
                            {
                                salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                                salt->getComponent<CTransform>().pos.x += overlap.x;
                                salt->getComponent<CTransform>().velocity.x = 0;
                            }
                        }
                        else
                        {
                            if (salt->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                            {
                                salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                                salt->getComponent<CTransform>().pos.y += overlap.y;
                                salt->getComponent<CTransform>().velocity.x = 0;
                            }
                            else
                            {
                                salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                                salt->getComponent<CTransform>().pos.y -= overlap.y;
                                salt->getComponent<CTransform>().velocity.x = 0;
                            }
                        }
                    }
                }
            }
        }
        for (auto& salt : m_entityManager.getEntities("leftbullet"))
        {
            Vec2 overlap = Physics::GetOverlap(salt, tile);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(salt, tile);

            if (overlap.x > 0 && overlap.y > 0)
            {
                if (tile->hasComponent<CBoundingBox>() && tile->getComponent<CBoundingBox>().blockMove)
                {
                    // collision from top 
                    if (prevOverlap.x > 0 && salt->getComponent<CTransform>().pos.y < tile->getComponent<CTransform>().pos.y)
                    {
                        salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                        salt->getComponent<CTransform>().pos.y -= overlap.y;
                        salt->getComponent<CTransform>().velocity.y = 0;
                    }
                    // collision from below
                    else if (prevOverlap.x > 0 && salt->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                    {
                        salt->getComponent<CTransform>().velocity.y = 0;
                        salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                        salt->getComponent<CTransform>().pos.y += overlap.y;
                    }
                    // collision from left
                    if (prevOverlap.y > 0 && salt->getComponent<CTransform>().pos.x < tile->getComponent<CTransform>().pos.x)
                    {
                        salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                        salt->getComponent<CTransform>().pos.x -= overlap.x;
                    }
                    // collision from right
                    if (prevOverlap.y > 0 && salt->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                    {
                        salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                        salt->getComponent<CTransform>().pos.x += overlap.x;
                    }
                    // special case: No overlap in previous frame, push out in direction of most overlap
                    if (prevOverlap.y <= 0 && prevOverlap.x <= 0)
                    {
                        if (overlap.x > overlap.y)
                        {
                            if (salt->getComponent<CTransform>().pos.x > tile->getComponent<CTransform>().pos.x)
                            {
                                salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                                salt->getComponent<CTransform>().pos.x += overlap.x;
                            }
                            else
                            {
                                salt->getComponent<CTransform>().prevPos.x = salt->getComponent<CTransform>().pos.x;
                                salt->getComponent<CTransform>().pos.x += overlap.x;
                            }
                        }
                        else
                        {
                            if (salt->getComponent<CTransform>().pos.y > tile->getComponent<CTransform>().pos.y)
                            {
                                salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                                salt->getComponent<CTransform>().pos.y += overlap.y;
                            }
                            else
                            {
                                salt->getComponent<CTransform>().prevPos.y = salt->getComponent<CTransform>().pos.y;
                                salt->getComponent<CTransform>().pos.y -= overlap.y;
                            }
                        }
                    }
                }
            }

            for (auto& enemy : m_entityManager.getEntities("npc"))
            {
                Vec2 overlap = Physics::GetOverlap(salt, enemy);
                Vec2 prevOverlap = Physics::GetPreviousOverlap(salt, enemy);

                if (overlap.x > 0 && overlap.y > 0)
                {
                    enemy->getComponent<CFollowPlayer>().speed = enemy->getComponent<CFollowPlayer>().speed / 2;
                    m_game->playSound("EnemyHit");
                    salt->destroy();
                }
            }
        }
        // player - enemy collisions with appropriate damage calculations
        for (auto& enemy : m_entityManager.getEntities("npc"))
        {
            for (auto& player : m_entityManager.getEntities("player"))
            {
                Vec2 overlap = Physics::GetOverlap(player, enemy);
                Vec2 prevOverlap = Physics::GetPreviousOverlap(player, enemy);

                if (overlap.x > 0 && overlap.y > 0 && !ishidden)
                {
                    if (!player->hasComponent<CInvincibility>())
                    {
                        //player's current health is reduced by the enemy damage component 
                        player->getComponent<CHealth>().current -= enemy->getComponent<CDamage>().damage;
                        m_game->playSound("LinkHurt");
                        if (player->getComponent<CHealth>().current <= 0)
                        {
                            m_game->playSound("LinkDie");
                            player->destroy();
                            gameOver();
                            //spawnPlayer();
                        }
                        //player is given a CInvincibility component which lasts for 30 frames 
                        player->addComponent<CInvincibility>(30);
                    }
                }
            }
        }
        // Sword - NPC collisions
        for (auto& sword : m_entityManager.getEntities("sword"))
        {
            for (auto& enemy : m_entityManager.getEntities("npc"))
            {
                Vec2 overlap = Physics::GetOverlap(sword, enemy);
                Vec2 prevOverlap = Physics::GetPreviousOverlap(sword, enemy);

                if (overlap.x > 0 && overlap.y > 0 && !enemy->hasComponent<CInvincibility>())
                {
                    //enemy's current health is reduced by the enemy damage component 
                    enemy->getComponent<CHealth>().current -= sword->getComponent<CDamage>().damage;
                    m_game->playSound("EnemyHit");
                    enemy->addComponent<CInvincibility>(10);
                    if (enemy->getComponent<CHealth>().current <= 0)
                    {
                        m_game->playSound("EnemyDie");
                        auto explosion = m_entityManager.addEntity("explosion");
                        explosion->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), true);
                        explosion->getComponent<CAnimation>().repeat = false;
                        explosion->getComponent<CTransform>().pos = enemy->getComponent<CTransform>().pos;
                        enemy->destroy();
                    }
                }
            }
        }
        for (auto& salt : m_entityManager.getEntities("rightbullet"))
        {
            for (auto& enemy : m_entityManager.getEntities("npc"))
            {
                Vec2 overlap = Physics::GetOverlap(salt, enemy);
                Vec2 prevOverlap = Physics::GetPreviousOverlap(salt, enemy);

                if (overlap.x > 0 && overlap.y > 0 && !enemy->hasComponent<CSlow>())
                {
                    salt->destroy();
                    salt->removeComponent<CBoundingBox>();
                    enemy->addComponent<CSlow>(2, m_currentFrame);
                    enemy->getComponent<CFollowPlayer>().speed = enemy->getComponent<CFollowPlayer>().speed / enemy->getComponent<CSlow>().amount;
                    m_game->playSound("EnemyHit");
                }
            }
        }
        for (auto& salt : m_entityManager.getEntities("leftbullet"))
        {

            for (auto& enemy : m_entityManager.getEntities("npc"))
            {
                Vec2 overlap = Physics::GetOverlap(salt, enemy);
                Vec2 prevOverlap = Physics::GetPreviousOverlap(salt, enemy);

                if (overlap.x > 0 && overlap.y > 0 && !enemy->hasComponent<CSlow>())
                {
                    salt->destroy();
                    salt->removeComponent<CBoundingBox>();
                    enemy->addComponent<CSlow>(2, m_currentFrame);
                    enemy->getComponent<CFollowPlayer>().speed = enemy->getComponent<CFollowPlayer>().speed / enemy->getComponent<CSlow>().amount;
                    m_game->playSound("EnemyHit");
                }
            }
        }
        /*
        for (auto& dec : m_entityManager.getEntities("dec"))
        {
            for (auto& player : m_entityManager.getEntities("player"))
            {
                Vec2 overlap = Physics::GetOverlap(player, dec);
                Vec2 prevOverlap = Physics::GetPreviousOverlap(player, dec);

                
                if (overlap.x > 0 && overlap.y > 0 && dec->hasComponent<CHidable>())
                {

                    canhide = true;
                    if (dec->getComponent<CAnimation>().animation.getName() == "Bed") 
                    {
                        dec->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Bed_Hide");
                        dec->addComponent<CAnimation>(animation, true);
                    }
                    else if (dec->getComponent<CAnimation>().animation.getName() == "Fridge")
                    {
                        dec->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Fridge_Hide");
                        dec->addComponent<CAnimation>(animation, true);
                    }
                    
                }
                else if ((overlap.x < 0 || overlap.y < 0) && dec->hasComponent<CHidable>())
                {
                    
                    if (dec->getComponent<CAnimation>().animation.getName() == "Bed_Hide")
                    {
                        canhide = false;
                        ishidden = false;
                        dec->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Bed");
                        dec->addComponent<CAnimation>(animation, true);
                    }
                    else if (dec->getComponent<CAnimation>().animation.getName() == "Fridge_Hide")
                    {
                        canhide = false;
                        ishidden = false;
                        dec->removeComponent<CAnimation>();
                        auto animation = m_game->assets().getAnimation("Fridge");
                        dec->addComponent<CAnimation>(animation, true);
                    }
                }
            }
        }
        */
    }
}

void Scene_House::drawLine(const Vec2 & p1, const Vec2 & p2)
{
    sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
    m_game->window().draw(line, 2, sf::Lines);
}

void Scene_House::sAnimation()
{
	std::string state = m_player->getComponent<CState>().state;
	std::string animation = m_player->getComponent<CAnimation>().animation.getName();

    // animation for other entities
	for (auto entity : m_entityManager.getEntities())
	{
		if (entity->hasComponent<CAnimation>())
		{
			entity->getComponent<CAnimation>().animation.update();

			if (entity->getComponent<CAnimation>().repeat == false)
			{
				if (entity->getComponent<CAnimation>().animation.hasEnded())
				{
					entity->destroy();
				}
			}
		}
	}

    //player movement
	if (!m_player->getComponent<CInput>().attack)
	{
        std::string player_state = m_player->getComponent<CState>().state;
	    std::string player_animation = m_player->getComponent<CAnimation>().animation.getName();

        if (player_state == "up" && player_animation != "StandUp")
	    {
		    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true);
	    }
	    else if (player_state == "right" && player_animation != "RunRight")
	    {
		    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
	    }
	    else if (player_state == "stand" && player_animation != "StandRight")
	    {
		    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
	    }
	}
    //player attack
	else
	{
        if (state == "swordRight" && animation != "AtkRight")
		{
		    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkRight"), true);
		}
	}
}

void Scene_House::sCamera()
{
    auto & pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);
    
}

void Scene_House::saveLevel()
{
    std::cout << "Enter file you wish to save your level too (enter same name of the level if you want to save edited existing level); !Important! add custom/ or main/ before name of the level: ";

	std::ofstream fout;

	std::string fileName;

	std::cin >> fileName;

	fileName = "levels/" + fileName + ".txt";

	fout.open(fileName);

    // Saving tiles 
	for (auto tile : m_entityManager.getEntities("tile"))
	{
		std::string tileName = tile->getComponent<CAnimation>().animation.getName();
        int BV = tile->getComponent<CBoundingBox>().blockVision;
		int BM = tile->getComponent<CBoundingBox>().blockMove;

		fout << "Tile" << " " << tileName << " " << tile->getComponent<CTransform>().RX << " " << tile->getComponent<CTransform>().RY << " " << tile->getComponent<CTransform>().pos.x << " " << tile->getComponent<CTransform>().pos.y << " " << BM << " " << BV << " " << std::endl;
	}
    // Saving player
    for (auto e : m_entityManager.getEntities("player"))
	{
		fout << "Player" << " " << m_playerConfig.X << " " << m_playerConfig.Y << " " << m_playerConfig.CX << " " << m_playerConfig.CY  << " " << m_playerConfig.SPEED << " " << m_playerConfig.HEALTH << " " << m_playerConfig.JUMP << " " << m_playerConfig.MAXSPEED << " " << m_playerConfig.GRAVITY << " " << std::endl;
	}

    // Saving NPCs
	for (auto NPC : m_entityManager.getEntities("npc"))
	{
		std::string enemyName = NPC->getComponent<CAnimation>().animation.getName();
        int RX = 0;
        int RY = 0;
        int TY = NPC->getComponent<CTransform>().pos.y;
        int TX = NPC->getComponent<CTransform>().pos.x;
        int H = NPC->getComponent<CHealth>().max;
        int D = NPC->getComponent<CDamage>().damage ;
		int BV = NPC->getComponent<CBoundingBox>().blockVision;
		int BM = NPC->getComponent<CBoundingBox>().blockMove;

		std::string AI;
        // Follow NPCs

		if (NPC->hasComponent<CFollowPlayer>())
		{
			AI = "Follow";
			
			fout << "NPC" << " " << enemyName << " " << " " << RX << " " << RY << " " << TX << " " << TY << " " << BM << " " << BV << " " << H << " " << D << " " << AI << " " << NPC->getComponent<CFollowPlayer>().speed << std::endl;
		}

        // Patrolling NPCs
		if (NPC->hasComponent<CPatrol>())
		{
			AI = "Patrol";
            int PS = NPC->getComponent<CTransform>().PS;
            std::string s;
            int PN = NPC->getComponent<CTransform>().PN;
            std::vector<Vec2> positions = NPC->getComponent<CTransform>().positions;

			fout << "NPC" << " " << enemyName << " " << " " << RX << " " << RY << " " << TX << " " << TY << " " << BM << " " << BV << " " << H << " " << D << " " << AI << " " << PS << " " << PN  << s << std::endl;

		}


	}
	
	fout.close();
	std::cout << "Level saved" << std::endl;
}

void Scene_House::onEnd()
{
    m_game->assets().getSound("LinkHurt").stop();
    m_game->assets().getSound("MusicLevel").stop();
    m_game->playSound("MusicTitle");
    m_game->changeScene("MENU", nullptr, true);
}

bool Scene_House::compare_points(const Vec2 a, const Vec2 b)
{
    Vec2 lightSource = Vec2(920, 200);

    Vec2 d_a = Vec2(a.x - lightSource.x, a.y - lightSource.y);
    float theta_a = atan2f(d_a.y, d_a.x);

    Vec2 d_b = Vec2(b.x - lightSource.x, b.y - lightSource.y);
    float theta_b = atan2f(d_b.y, d_b.x);

    return (theta_a > theta_b);
}

void Scene_House::sRender()
{
    
    // RENDERING DONE FOR YOU

    if (ishidden)
    {
        m_game->window().clear(sf::Color(25, 25, 25));
    }
    else
    {
        m_game->window().clear(sf::Color(51, 25, 0));
    }
    sf::RectangleShape tick({ 1.0f, 6.0f });
    tick.setFillColor(sf::Color::Black);
    auto xpos = m_game->window().mapPixelToCoords(sf::Mouse::getPosition(m_game->window())).x;
    auto ypos = m_game->window().mapPixelToCoords(sf::Mouse::getPosition(m_game->window())).y;
    mouse = Vec2(xpos, ypos);

    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            sf::Color c = sf::Color::White;
            if (e->hasComponent<CInvincibility>())
            {
                c = sf::Color(255, 255, 255, 128);
            }

            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                animation.getSprite().setColor(c);
                m_game->window().draw(animation.getSprite());
            }
        }

        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            if (e->hasComponent<CHealth>())
            {
                auto& h = e->getComponent<CHealth>();
                Vec2 size(64, 6);
                sf::RectangleShape rect({ size.x, size.y });
                rect.setPosition(transform.pos.x - 32, transform.pos.y - 48);
                rect.setFillColor(sf::Color(96, 96, 96));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(2);
                m_game->window().draw(rect);

                float ratio = (float)(h.current) / h.max;
                size.x *= ratio;
                rect.setSize({ size.x, size.y });
                rect.setFillColor(sf::Color(255, 0, 0));
                rect.setOutlineThickness(0);
                m_game->window().draw(rect);

                for (int i = 0; i < h.max; i++)
                {
                    tick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.max, 0));
                    m_game->window().draw(tick);
                }

                // color the player red when health gets to 1
                if (h.current==1)
                {
                    sf::Shader shader;
                    if (!shader.loadFromFile("shaders/shader_red.frag", sf::Shader::Fragment))
                    {
                        std::cerr << "Error while shaders" << std::endl;
                    }
                    auto& animation = e->getComponent<CAnimation>().animation;
                    m_game->window().draw(animation.getSprite(), &shader);
                }
            }
            if (e->hasComponent<CInventory>())
            {
                auto& h = e->getComponent<CInventory>();
                Vec2 size(64, 6);
                sf::RectangleShape rect({ size.x, size.y });
                rect.setPosition(transform.pos.x - 32, transform.pos.y - 58);
                rect.setFillColor(sf::Color(96, 96, 96));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(2);
                m_game->window().draw(rect);

                float ratio = (float)(h.currentSalt) / h.maxSalt;
                size.x *= ratio;
                rect.setSize({ size.x, size.y });
                rect.setFillColor(sf::Color(255, 255, 255));
                rect.setOutlineThickness(0);
                m_game->window().draw(rect);

                for (int i = 0; i < h.maxSalt; i++)
                {
                    tick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.maxSalt, 0));
                    m_game->window().draw(tick);
                }
            }
        }
    }
        // draw the grid so that students can easily debug
    if (m_drawGrid)
    {
        float leftX = m_game->window().getView().getCenter().x - width() / 2;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);



        editText.setString("edit mode        press H to get help");
        //key 1 to select tiles, key 2 to select np  mouse Wheel to select tille mouse wheel press to place selected press 3 on tile if want to make it block movement press on tile if want to make it block vision  L to save the level

        editText.setPosition(sf::Vector2f(10, 10));
        m_game->window().draw(editText);

        if (m_drawHelp)
        {
            editText.setString("key 1 to select tiles, key 2 to select npc  \n mouse Wheel to select tile \n mouse wheel press to place selected \n press 3 on tile if want to make it block movement \n press on 4 tile if want to make it block vision  \n Button1 on mouse to delete tile \n L to save the level");

        editText.setPosition(sf::Vector2f(10, 30));
        m_game->window().draw(editText);

        }

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, height()));
        }

        for (float y = 0; y < height(); y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }

    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        sf::CircleShape dot(4);
        dot.setFillColor(sf::Color::Black);
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
                if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
                if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
                if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }

            if (e->hasComponent<CPatrol>())
            {
                auto& patrol = e->getComponent<CPatrol>().positions;
                for (size_t p = 0; p < patrol.size(); p++)
                {
                    dot.setPosition(patrol[p].x, patrol[p].y);
                    m_game->window().draw(dot);
                }
            }

            if (e->hasComponent<CFollowPlayer>())
            {
                sf::VertexArray lines(sf::LinesStrip, 2);
                lines[0].position.x = e->getComponent<CTransform>().pos.x;
                lines[0].position.y = e->getComponent<CTransform>().pos.y;
                lines[0].color = sf::Color::Black;
                lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
                lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
                lines[1].color = sf::Color::Black;
                m_game->window().draw(lines);
                dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
                m_game->window().draw(dot);
            }
        }
    }
    // HUD
    Vec2 playerPos = m_player->getComponent<CTransform>().pos;
    sf::RectangleShape hud({ 100, 50 });
    hud.setPosition(playerPos.x - 54, playerPos.y - 120);
    hud.setFillColor(sf::Color::Black);
    hud.setOutlineColor(sf::Color::Black);
    hud.setOutlineThickness(2);
    m_game->window().draw(hud);
    sf::Text hudInfo;
    hudInfo.setFont(m_game->assets().getFont("Visitors"));
    hudInfo.setCharacterSize(20);
    hudInfo.setPosition(playerPos.x - 54, playerPos.y - 120);
    hudInfo.setFillColor(sf::Color::White);
    std::string hudString = "Clocks: " + std::to_string(m_player->getComponent<CInventory>().clocks) + "\nKey: ";
    hudString += m_player->getComponent<CInventory>().key ? "Found" : "Not Found";
    hudInfo.setString(hudString);
    m_game->window().draw(hudInfo);


    // Draw light source
    Vec2 centre = Vec2(920, 200);
    std::vector<Vec2> lightPos;

    for (int i = 0; i < 360; i += 5)
    {
        int rad = i * 3.14 / 180;
        int dist = 150;
        Vec2 endPoint = Vec2(centre.x + dist * cos(i), centre.y + dist * sin(i));
        bool intersection = false;

        for (auto tile : m_entityManager.getEntities("tile"))
        {
            if (tile->hasComponent<CBoundingBox>())
            {
                //position of four corners of the tile 
                Vec2 topC = Vec2(tile->getComponent<CTransform>().pos.x - tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y - tile->getComponent<CBoundingBox>().halfSize.y);
                Vec2 topD = Vec2(tile->getComponent<CTransform>().pos.x + tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y - tile->getComponent<CBoundingBox>().halfSize.y);

                Vec2 bottomC = Vec2(tile->getComponent<CTransform>().pos.x - tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y + tile->getComponent<CBoundingBox>().halfSize.y);
                Vec2 bottomD = Vec2(tile->getComponent<CTransform>().pos.x + tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y + tile->getComponent<CBoundingBox>().halfSize.y);

                Vec2 leftC = Vec2(tile->getComponent<CTransform>().pos.x - tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y - tile->getComponent<CBoundingBox>().halfSize.y);
                Vec2 leftD = Vec2(tile->getComponent<CTransform>().pos.x - tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y + tile->getComponent<CBoundingBox>().halfSize.y);

                Vec2 rightC = Vec2(tile->getComponent<CTransform>().pos.x + tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y - tile->getComponent<CBoundingBox>().halfSize.y);
                Vec2 rightD = Vec2(tile->getComponent<CTransform>().pos.x + tile->getComponent<CBoundingBox>().halfSize.x, tile->getComponent<CTransform>().pos.y + tile->getComponent<CBoundingBox>().halfSize.y);


                if (Physics::LineIntersect(centre, endPoint, topC, topD).result)
                {
                    intersection = true;

                    if (topC.y < bottomC.y && topC.y > centre.y)
                    {

                        //if distpoint < dist
                        Vec2 DC = Vec2(topC.x - centre.x, centre.y - topC.y);
                        float distptC = sqrtf(DC.x * DC.x + DC.y * DC.y);
                        Vec2 DD = Vec2(topD.x - centre.x, centre.y - topD.y);
                        float distptD = sqrtf(DD.x * DD.x + DD.y * DD.y);
                        if (distptC < dist)
                        {
                            lightPos.push_back(topC);
                        }
                        if (distptD < dist)
                        {
                            lightPos.push_back(topD);
                        }
                    }
                }

                if (Physics::LineIntersect(centre, endPoint, bottomC, bottomD).result)
                {
                    intersection = true;

                    if (topC.y < bottomC.y && bottomC.y < centre.y)
                    {
                        Vec2 DC = Vec2(bottomC.x - centre.x, centre.y - bottomC.y);
                        float distptC = sqrtf(DC.x * DC.x + DC.y * DC.y);
                        Vec2 DD = Vec2(bottomD.x - centre.x, centre.y - bottomD.y);
                        float distptD = sqrtf(DD.x * DD.x + DD.y * DD.y);
                        if (distptC < dist)
                        {
                            lightPos.push_back(bottomC);
                        }
                        if (distptD < dist)
                        {
                            lightPos.push_back(bottomD);
                        }
                    }
                }

                if (Physics::LineIntersect(centre, endPoint, leftC, leftD).result)
                {
                    intersection = true;

                    if (leftC.x < rightC.x && leftC.x > centre.x)
                    {
                        Vec2 DC = Vec2(leftC.x - centre.x, centre.y - leftC.y);
                        float distptC = sqrtf(DC.x * DC.x + DC.y * DC.y);
                        Vec2 DD = Vec2(leftD.x - centre.x, centre.y - leftD.y);
                        float distptD = sqrtf(DD.x * DD.x + DD.y * DD.y);
                        if (distptC < dist)
                        {
                            lightPos.push_back(leftC);
                        }
                        if (distptD < dist)
                        {
                            lightPos.push_back(leftD);
                        }
                    }
                }

                if (Physics::LineIntersect(centre, endPoint, rightC, rightD).result)
                {
                    intersection = true;

                    if (leftC.x < rightC.x && rightC.x < centre.x)
                    {
                        Vec2 DC = Vec2(rightC.x - centre.x, centre.y - rightC.y);
                        float distptC = sqrtf(DC.x * DC.x + DC.y * DC.y);
                        Vec2 DD = Vec2(rightD.x - centre.x, centre.y - rightD.y);
                        float distptD = sqrtf(DD.x * DD.x + DD.y * DD.y);
                        if (distptC < dist)
                        {
                            lightPos.push_back(rightC);
                        }
                        if (distptD < dist)
                        {
                            lightPos.push_back(rightD);
                        }
                    }
                }
            }
        }

        if (!intersection)
        {
            lightPos.push_back(endPoint);
        }
    }

    // radial sort
    std::sort(lightPos.begin(), lightPos.end(), Scene_House::compare_points);

    // draw light triangles 
    for (int i = 0; i < lightPos.size() - 1; i++)
    {
        sf::ConvexShape convex;
        convex.setPointCount(3);
        convex.setPoint(0, sf::Vector2f(centre.x, centre.y));
        convex.setPoint(1, sf::Vector2f(lightPos[i].x, lightPos[i].y));
        convex.setPoint(2, sf::Vector2f(lightPos[i + 1].x, lightPos[i + 1].y));
        convex.setFillColor(sf::Color(255, 255, 255, 50));
        m_game->window().draw(convex);
    }

    // draw last light triangle
    sf::ConvexShape convex_end;
    convex_end.setPointCount(3);
    convex_end.setPoint(0, sf::Vector2f(centre.x, centre.y));
    convex_end.setPoint(1, sf::Vector2f(lightPos.back().x, lightPos.back().y));
    convex_end.setPoint(2, sf::Vector2f(lightPos[0].x, lightPos[0].y));
    convex_end.setFillColor(sf::Color(255, 255, 255, 50));
    m_game->window().draw(convex_end);

    if (m_gameOver)
    {
        sf::RectangleShape end(m_game->window().getView().getSize());
        end.setPosition(m_game->window().getView().getCenter().x - m_game->window().getView().getSize().x/2, m_game->window().getView().getCenter().y - m_game->window().getView().getSize().y / 2);
        end.setFillColor(sf::Color::Black);
        m_game->window().draw(end);
        sf::Text gameOverText;
        gameOverText.setFont(m_game->assets().getFont("Visitors"));
        gameOverText.setCharacterSize(40);
        gameOverText.setPosition(end.getPosition().x + end.getSize().x /2, end.getPosition().y + end.getSize().y/2);
        gameOverText.setFillColor(sf::Color::White);
        gameOverText.setString("GAME OVER!\n\nCreated By:\nElizaveta Makarova\nSteven MacDonald\nHilary Sinclair\nChristopher Smith");
        m_game->window().draw(gameOverText);
        m_paused = true;
    }
}
