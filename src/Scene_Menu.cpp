#include "Scene_Menu.h"
#include "Scene_House.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

Scene_Menu::Scene_Menu(GameEngine * gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{
    registerAction(sf::Keyboard::W,     "UP");
    registerAction(sf::Keyboard::S,     "DOWN");
    registerAction(sf::Keyboard::D,     "ZELDA");
    registerAction(sf::Keyboard::U,     "UPDATE");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_mainLevels.push_back("levels/main/level1.txt");
    m_mainLevels.push_back("levels/main/level2.txt");
    m_mainLevels.push_back("levels/main/level3.txt");
    m_mainLevels.push_back("levels/main/final_boss.txt");

    m_title = "Visitors";
    updateLevels();
    m_menuText.setFont(m_game->assets().getFont("Visitors"));
    m_menuText.setCharacterSize(64);

    m_game->playSound("MusicTitle");
}

void Scene_Menu::updateLevels()
{
    m_levelPaths.clear();
    m_menuStrings.clear();
    m_progress.clear();

    std::string filename = "progress.txt";
    std::string level;
    std::ifstream fin(filename);
	while (fin.good())
    {
        fin >> level;
        m_progress.push_back(level);
    }

    std::string path = "levels/custom/";
    std::string levels = "levels/custom/";
    std::string txt = ".txt";
    std::vector<std::string> customLevels;
    for (const auto & entry : fs::directory_iterator(path))
    {
        customLevels.push_back(entry.path().string()); 
    }
    loadLevels(customLevels, levels, txt, 1000);
    path = "levels/main/";
    levels = "levels/main/";
    txt = ".txt";
    loadLevels(m_mainLevels, levels, txt, m_progress.size());
}

void Scene_Menu::loadLevels(std::vector<std::string> path, std::string levels, std::string txt, size_t prg)
{
 
    for (const auto & entry : path)
    {
        std::string levelName = entry;

        std::string::size_type i = levelName.find(txt);
        std::string::size_type n = levelName.find(levels);
        if (i != std::string::npos)
        {
            levelName.erase(i, txt.length());
        }
        if (n != std::string::npos)
        {
            levelName.erase(n, levels.length());
        }
        if (prg > 0)
        {
            m_levelPaths.push_back(entry);
            m_menuStrings.push_back(levelName);
            prg--;
        }
    }
}

void Scene_Menu::update()
{
    m_entityManager.update();
}

void Scene_Menu::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "UP")
        {
            if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
            else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
        }
        else if (action.name() == "DOWN")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        }
        else if (action.name() == "ZELDA")
        {
            m_game->changeScene("ZELDA", std::make_shared<Scene_House>(m_game, m_levelPaths[m_selectedMenuIndex]));
            m_game->assets().getSound("MusicTitle").stop();
        }
        else if (action.name() == "UPDATE")
        {
            updateLevels();
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void Scene_Menu::sRender()
{
    // clear the window to a blue
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(11, 15, 20));

    // draw the game title in the middle of the screen
    m_menuText.setCharacterSize(200);
    m_menuText.setString(m_title);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setOrigin((m_menuText.getGlobalBounds().width /2) , (m_menuText.getGlobalBounds().height /2));
    m_menuText.setPosition(m_game->window().getSize().x/2.0f,20);
    m_game->window().draw(m_menuText);
    
    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setCharacterSize(100);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(100, 100, 100));
        m_menuText.setOrigin((m_menuText.getGlobalBounds().width /2) , (m_menuText.getGlobalBounds().height /2));
        m_menuText.setPosition(m_game->window().getSize().x/2.0f,220 + i * 72);
        m_game->window().draw(m_menuText);
    }

    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(50);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setString("up: w     down: s    play: d      back: esc       update levels: u");
    m_menuText.setOrigin((m_menuText.getGlobalBounds().width / 2), (m_menuText.getGlobalBounds().height / 2));
    m_menuText.setPosition(sf::Vector2f(m_game->window().getSize().x / 2.0f, 690));
    m_game->window().draw(m_menuText);

    //draw edit level header
    m_menuText.setCharacterSize(50);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setString("LEVEL EDITOR");
    m_menuText.setOrigin((m_menuText.getGlobalBounds().width / 2), (m_menuText.getGlobalBounds().height / 2));
    m_menuText.setPosition(m_game->window().getSize().x / 2.0f - 400, 250);
    m_game->window().draw(m_menuText);

    // draw edit level info
    m_menuText.setCharacterSize(30);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setString("Level editor is available in all levels! \n\n To enter level edit mode: G \n\n Level edit help: H");
    m_menuText.setOrigin((m_menuText.getGlobalBounds().width / 2), (m_menuText.getGlobalBounds().height / 2));
    m_menuText.setPosition(m_game->window().getSize().x / 2.0f - 400, 390);
    m_game->window().draw(m_menuText);

    //draw difficulty header
    m_menuText.setCharacterSize(50);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setString("DIFFICULTY OPTIONS");
    m_menuText.setOrigin((m_menuText.getGlobalBounds().width / 2), (m_menuText.getGlobalBounds().height / 2));
    m_menuText.setPosition(m_game->window().getSize().x / 2.0f + 400, 250);
    m_game->window().draw(m_menuText);

    // draw difficulty options info
    m_menuText.setCharacterSize(30);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setOrigin((m_menuText.getGlobalBounds().width / 2), (m_menuText.getGlobalBounds().height / 2));
    m_menuText.setString("Increase Difficulty: U     Decrease Difficulty: J");
    m_menuText.setPosition(m_game->window().getSize().x / 2.0f + 300, 320);
    m_game->window().draw(m_menuText);
}



void Scene_Menu::onEnd()
{
    m_hasEnded = true;
    m_game->quit();
}