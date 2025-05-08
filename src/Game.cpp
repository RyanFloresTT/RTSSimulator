#include "Game.h"

#include <iostream>
#include <random>
#include <cmath>

#include "imgui-SFML.h"
#include "imgui.h"
#include "SFML/Window/Event.hpp"
#include "Util/VectorUtils.h"

const double PI = 3.141592653589793;

Game::Game(const std::string &config) {
    init(config);
}

void Game::init(const std::string &config) {
    m_window.create(sf::VideoMode(1280, 720), "CoolGame");
    m_window.setFramerateLimit(60);

    ImGui::SFML::Init(m_window);

    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    if (!m_font.loadFromFile("assets/fonts/Roboto-Regular.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    m_text.setFont(m_font);
    m_text.setCharacterSize(32);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(10.f, 10.f);

    spawnPlayer();
}

std::shared_ptr<Entity> Game::player() {
    auto &players = m_entities.getEntities("player");
    assert(players.size() == 1);
    return players.front();
}

void Game::run() {
    while (m_isRunning) {
        m_entities.update();

        if (!m_isPaused) { sLifespan(); }
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        if (!m_isPaused) {
            if (m_canSpawn) { sEnemySpawner(); }
            if (m_isCollisionsOn) { sCollision(); }
            sMovement();
            sRotation();
            m_currentFrame++;
        }
        sGUI();
        sRender();
        sUserInput();
    }
}

void Game::spawnPlayer() {
    const auto entity = m_entities.addEntity("player");

    entity->add<CTransform>(sf::Vector2f(640.0f - 16.0f, 360.0f - 16.0f), sf::Vector2(0.0f, 0.0f));
    entity->add<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
    entity->add<CCollision>(32.0f);
    entity->add<CInput>();
}

void Game::spawnEnemy() {
    const auto enemy = m_entities.addEntity("enemy");

    std::random_device rd;
    std::mt19937       gen(rd());

    // Assuming screenWidth and screenHeight are defined
    std::uniform_int_distribution<> distP(0, 8);
    std::uniform_int_distribution<> distX(32, 1280 - 32);
    std::uniform_int_distribution<> distY(32, 720 - 32);
    std::uniform_int_distribution<> distRGB(0, 256);

    int points = distP(gen);
    int posX   = distX(gen);
    int posY   = distY(gen);
    int red    = distRGB(gen);
    int green  = distRGB(gen);
    int blue   = distRGB(gen);

    auto enemyVector = VectorUtils::generateNormalizedVector();

    enemyVector.x *= 2;
    enemyVector.y *= 2;

    enemy->add<CTransform>(sf::Vector2f(posX, posY), enemyVector);
    enemy->add<CShape>(32.0f, points, sf::Color(red, green, blue), sf::Color(red, green, blue), 4.0f);
    enemy->add<CCollision>(32.0f);
    enemy->add<CRotation>(9 - points);
    enemy->add<CScore>(points * 100);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(const std::shared_ptr<Entity> &entity) {
    size_t   points = entity->get<CShape>().circle.getPointCount();
    for (int i      = 0; i < points; ++i) {
        float angle_degrees = (360.0f / points) * i;
        float angle_radians = angle_degrees * (PI / 180.0f);

        float x = std::cos(angle_radians);
        float y = std::sin(angle_radians);

        auto entityShape     = entity->get<CShape>();
        auto smallEnemyColor = entityShape.circle.getFillColor();
        smallEnemyColor.a = 128;
        auto smallEnemy = m_entities.addEntity("small enemy");
        smallEnemy->add<CTransform>(entity->get<CTransform>().position, sf::Vector2f(x, y));
        smallEnemy->add<CShape>(4.0f, points, smallEnemyColor,
                                entityShape.circle.getOutlineColor(), 4.0f);
        smallEnemy->add<CCollision>(4.0f);
        smallEnemy->add<CScore>(points * 100);
        smallEnemy->add<CLifespan>(1);
    }
}

void Game::spawnBullet(const std::shared_ptr<Entity> &entity, const sf::Vector2f &mousePos) {
    const sf::Vector2f playerPos = player()->get<CTransform>().position;

    sf::Vector2f direction = mousePos - playerPos;

    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        direction /= distance;
    }

    direction.x *= 6;
    direction.y *= 6;

    entity->add<CTransform>(playerPos, direction);
    entity->add<CShape>(4.0f, 24, sf::Color(255, 255, 255), sf::Color(255, 255, 255), 4.0f);
    entity->add<CCollision>(4.0f);
    entity->add<CLifespan>(3.0f);
}

void Game::spawnSpecialWeapon(const std::shared_ptr<Entity> &entity) {
}

void Game::sMovement() {
    for (auto &entity: m_entities.getEntities()) {
        auto &transform = entity->get<CTransform>();
        transform.position += transform.velocity;
    }
}

void Game::sLifespan() {
    for (const auto &entity: m_entities.getEntities()) {
        auto &lifespan = entity->get<CLifespan>();
        lifespan.remaining -= m_deltaClock.getElapsedTime().asSeconds();
        if (entity->tag() == "bullet") {
            std::cout << lifespan.remaining << "\n";
        }
        if (entity->has<CLifespan>() && lifespan.remaining < 0) {
            entity->destroy();
        }
    }
}

void Game::sCollision() {
    bool playerDestroyed = false;

    // handle collisions between entities
    for (const auto &el: m_entities.getEntities()) {
        for (const auto &er: m_entities.getEntities()) {

            if (el->id() == er->id()) continue;
            if ((el->tag() == "player" && er->tag() == "bullet") ||
                (er->tag() == "player" && el->tag() == "bullet"))
                continue;
            if (el->tag() == "enemy" && er->tag() == "enemy") continue;
            if (el->tag() == "small enemy" && er->tag() == "small enemy") continue;

            auto  diff     = el->get<CTransform>().position - er->get<CTransform>().position;
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (el->get<CCollision>().radius + er->get<CCollision>().radius > distance) {

                if (el->tag() == "player" || er->tag() == "player") {
                    playerDestroyed = true;

                    el->destroy();
                    er->destroy();
                    break;
                }

                if (el->tag() == "enemy") {
                    spawnSmallEnemies(el);
                }

                if (er->tag() == "enemy") {
                    spawnSmallEnemies(er);
                }

                el->destroy();
                er->destroy();

                m_score += el->get<CScore>().score;
                m_score += er->get<CScore>().score;
            }
        }
    }

    // handle collisions between entities and the screen bounds
    for (auto &entity: m_entities.getEntities()) {
        auto       &transform = entity->get<CTransform>();
        const auto collider   = entity->get<CCollision>();
        if (transform.position.x + collider.radius >= 1280 || transform.position.x - collider.radius <= 0) {
            transform.velocity.x *= -1;
        }

        if (transform.position.y + collider.radius >= 720 || transform.position.y - collider.radius <= 0) {
            transform.velocity.y *= -1;
        }
    }

    if (playerDestroyed) {
        spawnPlayer();
    }
}

void Game::sEnemySpawner() {
    if (m_currentFrame - m_lastEnemySpawnTime >= 60) {
        spawnEnemy();
    }
}

void Game::sGUI() {
    ImGui::Begin("Game");
    ImGui::SliderFloat("Player Speed", &m_playerSpeed, 1.0f, 10.0f);
    ImGui::Checkbox("Pause", &m_isPaused);
    ImGui::Checkbox("Enemy Spawner", &m_canSpawn);
    ImGui::Checkbox("Collisions", &m_isCollisionsOn);
    if (ImGui::Button("Spawn Enemy")) {
        spawnEnemy();
    }
    ImGui::End();
}

void Game::sRender() {
    m_window.clear();

    for (const auto &entity: m_entities.getEntities()) {
        entity->get<CShape>().circle.setPosition(entity->get<CTransform>().position);
        entity->get<CShape>().circle.setRotation(entity->get<CRotation>().angle);
        m_window.draw(entity->get<CShape>().circle);
    }

    m_text.setString("Score: " + std::to_string(m_score));
    m_window.draw(m_text);

    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sUserInput() {
    sf::Event event{};
    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(m_window, event);

        if (event.type == sf::Event::Closed) {
            m_isRunning = false;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    player()->get<CTransform>().velocity.y = -m_playerSpeed;
                    break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    player()->get<CTransform>().velocity.y = m_playerSpeed;
                    break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    player()->get<CTransform>().velocity.x = -m_playerSpeed;
                    break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    player()->get<CTransform>().velocity.x = m_playerSpeed;
                    break;
                default:;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    player()->get<CTransform>().velocity.y = 0.0f;
                    break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    player()->get<CTransform>().velocity.x = 0.0f;
                    break;
                default:;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            const sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            auto               bulletEntity = m_entities.addEntity("bullet");
            spawnBullet(bulletEntity, mousePos);
        }

        if (event.type == sf::Event::MouseButtonReleased) {}
    }
}

void Game::setPaused(const bool paused) {
    m_isPaused = paused;
}

void Game::sRotation() {
    for (auto &entity: m_entities.getEntities()) {
        auto &rotation = entity->get<CRotation>();
        rotation.angle += rotation.speed;
    }
}
