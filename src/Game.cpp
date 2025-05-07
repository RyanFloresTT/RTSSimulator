#include "Game.h"

#include <iostream>

#include "imgui-SFML.h"
#include "imgui.h"
#include "SFML/Window/Event.hpp"

Game::Game(const std::string& config) {
    init(config);
}

void Game::init(const std::string& config) {
    m_window.create(sf::VideoMode(1280, 720), "CoolGame");
    m_window.setFramerateLimit(60);

    ImGui::SFML::Init(m_window);

    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    spawnPlayer();
    spawnEnemy();
}

std::shared_ptr<Entity> Game::player() {
    auto& players = m_entities.getEntities("player");
    assert(players.size() == 1);
    return players.front();
}

void Game::run() {
    // TODO: add pause function here
    while (m_isRunning) {
        m_entities.update();

        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        if (!m_isPaused) {
            sEnemySpawner();
            sMovement();
            sCollision();
            sUserInput();
            m_currentFrame++;
        }
        sGUI();
        sRender();
    }
}

void Game::spawnPlayer() {
    const auto entity = m_entities.addEntity("player");

    entity->add<CTransform>(sf::Vector2f(200.0f, 200.0f), sf::Vector2(0.0f, 0.0f), 0.0f);
    entity->add<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
    entity->add<CInput>();
}

void Game::spawnEnemy() {
    const auto enemy = m_entities.addEntity("enemy");

    enemy->add<CTransform>(sf::Vector2f(400.0f, 400.0f), sf::Vector2(0.0f, 0.0f), 0.0f);
    enemy->add<CShape>(32.0f, 4, sf::Color(10, 10, 10), sf::Color(0, 255, 0), 4.0f);
    enemy->add<CCollision>(32.0f);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
    // spawn number of enemies to count of edges on largerEnemy
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f& mousePos) {}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {}

void Game::sMovement() {
    auto& transform = player()->get<CTransform>();
    transform.position += transform.velocity;
}

void Game::sLifespan() {
    for (auto entity : m_entities.getEntities()) {
        entity->get<CLifespan>().remaining--;
    }
}

void Game::sCollision() {}

void Game::sEnemySpawner() {}

void Game::sGUI() {
    ImGui::Begin("Yeah Yeah Gang");
    ImGui::SliderFloat("Player Speed", &m_playerSpeed, 1.0f, 10.0f);
    ImGui::Text("Yup Yup");
    ImGui::End();
}

void Game::sRender() {
    m_window.clear();

    for (const auto& entity : m_entities.getEntities()) {
        entity->get<CShape>().circle.setPosition(entity->get<CTransform>().position);
        m_window.draw(entity->get<CShape>().circle);
    }

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
            default: ;
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
            default: ;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            const auto mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
            std::cout << "Mouse Position" << event.mouseButton.x << ", " << event.mouseButton.y << "\n";
        }

        if (event.type == sf::Event::MouseButtonReleased) {}
    }
}

void Game::setPaused(const bool paused) {
    m_isPaused = paused;
}
