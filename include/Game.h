#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "EntityManager.h"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

struct PlayerConfig {
    int   SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S;
};
struct EnemyConfig {
    int   SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};
struct BulletConfig {
    int   SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};

class Game {
    sf::RenderWindow m_window;
    EntityManager    m_entities;
    sf::Font         m_font;
    sf::Text         m_text;
    PlayerConfig     m_playerConfig;
    EnemyConfig      m_enemyConfig;
    BulletConfig     m_bulletConfig;
    sf::Clock        m_deltaClock;
    int              m_score              = 0;
    int              m_currentFrame       = 0;
    int              m_lastEnemySpawnTime = 0;
    bool             m_isPaused           = false;
    bool             m_isRunning          = true;

    void init(const std::string &config);

    void setPaused(bool paused);

    void sMovement();

    void sUserInput();

    void sLifespan();

    void sRender();

    static void sGUI();

    void sEnemySpawner();

    void sCollision();

    void spawnPlayer();

    void spawnEnemy();

    void spawnSmallEnemies(std::shared_ptr<Entity> entity);

    void spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f &mousePos);

    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

    std::shared_ptr<Entity> player();

public:
    explicit Game(const std::string &config);

    void run();
};
