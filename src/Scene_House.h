#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <queue> 
#include "Node.h"
#include <vector>

#include "EntityManager.h"

class Scene_House : public Scene
{

    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, HEALTH, JUMP, MAXSPEED, GRAVITY;
        std::string WEAPON;
    };

protected:

    std::shared_ptr<Entity> m_player;
    std::shared_ptr<Entity> m_drag;
    std::shared_ptr<Entity> m_add;
    std::shared_ptr<Entity> m_addPrev;
    std::queue<std::string  >      myqueue;
    std::string             m_levelPath;
    std::string             m_addAnimation = "";
    std::string				insert = "Tile";
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = false;
    bool					isMoving = false;
	bool					isDeleting = false;
	bool					drawGrid = false;
	bool					placeable = false;
	bool					toDelete = false;
	bool					changeAsset = false;
	Vec2				    mouse;
    bool                    canhide = false;
    bool                    ishidden = false;
    bool	                m_drawGrid = false;
    bool	                m_drawHelp = false;
    bool                    m_gameOver = false;
    const Vec2              m_gridSize = { 64, 64 };
    sf::Text                editText;
    sf::Text                m_gridText;
    std::vector<std::vector<int>>     m_levelRep;
    
    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);
    void loadTiles();

    void onEnd();
    void saveLevel();
    void update();
    void sDrag();
    void deleteEntity();
    void spawnPlayer();
    void spawnSword(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity);
    void drawLine(const Vec2 & p1, const Vec2 & p2);
    Vec2 gridToMidPixel(float x, float y, std::shared_ptr<Entity> entity);
    void sMovement();
    void sAI();
    void sStatus();
    void sAnimation();
    void sCollision();
    void sCamera();
    void gameOver();
    void sUserInput();
    void increaseDifficulty();
    void decreaseDifficulty();
    void generateMap(std::vector<Entity>);
    static bool compare_points(const Vec2 a, const Vec2 b);


    float estimateCost(int startx,int starty, int goalx, int goaly);
    std::vector<Vec2> pathfind(int startx, int starty, int goalx, int goaly);

    bool isLegalAction(Vec2 action, Vec2 state);
    

public:

    Scene_House(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);
};