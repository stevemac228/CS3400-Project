#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class Scene_Menu : public Scene
{

protected:

    std::string                 m_title;
    std::vector<std::string>    m_menuStrings;
    std::vector<std::string>    m_levelPaths;
    std::vector<std::string>    m_mainLevels;
    sf::Text                    m_menuText;
    std::vector<std::string>    m_progress;
    size_t                      m_selectedMenuIndex = 0;
    
    void init();
    void update();
    void onEnd();
    void updateLevels();
    void sDoAction(const Action& action);
    void loadLevels(std::vector<std::string> path, std::string levels, std::string txt, size_t prg);

public:

    Scene_Menu(GameEngine * gameEngine = nullptr);
    void sRender();

};