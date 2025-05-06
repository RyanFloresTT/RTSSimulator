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
        // remove
    }

public:
    EntityManager() = default;

    void update() {
        // TODO: add entities from m_entitiesToAdd to the proper locations
        //  - add them to the vector of all entities
        //  - add them to the EntityMap

        removeDeadEntities(m_entities);

        for (auto &[tag, entityVec]: m_entityMap) {
            removeDeadEntities(entityVec);
        }
    }

    std::shared_ptr<Entity> addEntity(const std::string &tag) {
        auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

        m_entitiesToAdd.push_back(entity);

        if (m_entityMap.find(tag) == m_entityMap.end()) { m_entityMap[tag] = EntityVec(); }
        m_entityMap[tag].push_back(entity);

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