#ifndef ENEMY_H
#define ENEMY_H

enum class EnemyType {
    SPIKE,
    CUBE,
    SHIP_PORTAL,
    CUBE_PORTAL
};

struct Enemy {
    float x;
    float y;
    float width;
    float height;
    EnemyType type;
};

#endif