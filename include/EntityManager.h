#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Entity.h"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityManager {
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    size_t    m_totalEntities = 0;

    void removeDeadEntities(EntityVec &entityVec) {
        entityVec.erase(std::remove_if(entityVec.begin(), entityVec.end(),
                                       [](const auto &entity) { return !entity->isActive(); }),
                        entityVec.end());
    }

public:
    EntityManager() = default;

    void update() {
        for (auto &entity: m_entitiesToAdd) {
            if (m_entityMap.find(entity->tag()) == m_entityMap.end()) { m_entityMap[entity->tag()] = EntityVec(); }
            m_entityMap[entity->tag()].push_back(entity);

            m_entities.push_back(entity);
        }

        m_entitiesToAdd.clear();

        removeDeadEntities(m_entities);

        for (auto &[tag, entityVec]: m_entityMap) {
            removeDeadEntities(entityVec);
        }
    }

    std::shared_ptr<Entity> addEntity(const std::string &tag) {
        auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

        m_entitiesToAdd.push_back(entity);

        return entity;
    }

    EntityVec &getEntities() {
        return m_entities;
    }

    const EntityVec &getEntities(const std::string &tag) {
        if (m_entityMap.find(tag) == m_entityMap.end()) { m_entityMap[tag] = EntityVec(); }
        return m_entityMap[tag];
    }

    const EntityMap &getEntityMap() {
        return m_entityMap;
    }
};
