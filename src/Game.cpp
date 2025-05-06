#include "Game.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include "SFML/Window/Event.hpp"

Game::Game(const std::string &config) {
    init(config);
}

void Game::init(const std::string &config) {
    m_window.create(sf::VideoMode(1280, 720), "CoolGame");
    m_window.setFramerateLimit(60);

    ImGui::SFML::Init(m_window);

    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    spawnPlayer();
}

std::shared_ptr<Entity> Game::player() {
    auto &players = m_entities.getEntities("player");
    assert(players.size() == 1);
    return players.front();
}

void Game::run() {
    // TODO: add pause function here
    while (m_isRunning) {
        m_entities.update();

        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sGUI();
        sRender();

        m_currentFrame++; // move when pause is implemented
    }
}

void Game::spawnPlayer() {
    auto entity = m_entities.addEntity("player");

    entity->add<CTransform>(sf::Vector2f(200.0f, 200.0f), sf::Vector2(1.0f, 1.0f), 0.0f);
    entity->add<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
    entity->add<CInput>();
}

void Game::spawnEnemy() {
    // TODO: make sure enemy is completely in bounds of screen

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
    // spawn number of enemies to count of edges on largerEnemy
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f &mousePos) {

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {

}

void Game::sMovement() {
    auto &transform = player()->get<CTransform>();
    transform.position += transform.velocity;
}

void Game::sLifespan() {}

void Game::sCollision() {}

void Game::sEnemySpawner() {}

void Game::sGUI() {
    ImGui::Begin("Yeah Yeah Gang");
    ImGui::Text("Yup Yup");
    ImGui::End();
}

void Game::sRender() {
    m_window.clear();
    player()->get<CShape>().circle.setPosition(player()->get<CTransform>().position);

    player()->get<CTransform>().angle += 1.0f;
    player()->get<CShape>().circle.setRotation(player()->get<CTransform>().angle);

    m_window.draw(player()->get<CShape>().circle);

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

        }

        if (event.type == sf::Event::KeyReleased) {

        }

        if (event.type == sf::Event::MouseButtonPressed) {

        }

        if (event.type == sf::Event::MouseButtonReleased) {

        }
    }
}

void Game::setPaused(bool paused) {
    m_isPaused = paused;
}
