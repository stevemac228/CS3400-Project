#include "Scene.h"
#include "GameEngine.h"

Scene::Scene()
{

}

Scene::Scene(GameEngine * gameEngine)
    : m_game(gameEngine)
{ 
    
}

void Scene::setPaused(bool paused)
{
    m_paused = paused;
}

size_t Scene::width() const
{
    return m_game->window().getSize().x;
}

size_t Scene::height() const
{
    return m_game->window().getSize().y;
}

size_t Scene::currentFrame() const
{
    return m_currentFrame;
}

bool Scene::hasEnded() const
{
    return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
    return m_actionMap;
}

void Scene::registerAction(int inputKey, const std::string& actionName)
{
    m_actionMap[inputKey] = actionName;
}

void Scene::registerActionMouse(int inputKey, const std::string& actionName)
{
    m_actionMapMouse[inputKey] = actionName;
}

const ActionMap& Scene::getActionMapMouse() const
{
    return m_actionMapMouse;
}

void Scene::doAction(const Action& action)
{
    // ignore null actions
    if (action.name() == "NONE") { return; }

    sDoAction(action);
}

void Scene::simulate(const size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        update();
    }
}
