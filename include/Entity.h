#pragma once

#include <string>
#include <utility>
#include "components/ComponentTuple.h"

class EntityManager;

class Entity {
    friend class EntityManager;

    ComponentTuple m_components;
    bool           m_isActive = true;
    std::string    m_tag      = "default";
    size_t         m_id       = 0;

    Entity(const size_t &id, const std::string &tag) : m_id(id), m_tag(tag) {}

public:
    bool isActive() const {
        return m_isActive;
    }

    void destroy() {
        m_isActive = false;
    }

    size_t id() const {
        return m_id;
    }

    const std::string &tag() const {
        return m_tag;
    }

    template<typename T>
    bool has() const {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        return get<T>().exists;
    }

    template<typename T, typename ...TArgs>
    T &add(TArgs &&... mArgs) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto &component = get<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.exists = true;
        return component;
    }

    template<typename T>
    const T &get() const {
        return std::get<T>(m_components);
    }

    template<typename T>
    T &get() {
        return std::get<T>(m_components);
    }

    template<typename T>
    void remove(T) {
        get<T>() = T();
    }
};
