#pragma once
#include "raylib.h"

class GameObject {
protected:
    Vector2 position;
    bool active; 

public:
    GameObject(float x, float y) : position{x, y}, active(true) {}
    virtual ~GameObject() = default;

    virtual void Update() = 0;
    virtual void Draw() = 0;
    
    // colisões
    virtual Rectangle GetBounds() const = 0;

    // Getters e Setters básicos
    Vector2 GetPosition() const { return position; }
    bool IsActive() const { return active; }
    void SetActive(bool state) { active = state; }
};