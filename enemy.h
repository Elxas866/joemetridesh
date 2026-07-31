#ifndef ENEMY_H
#define ENEMY_H

enum class EnemyType {
    SPIKE,
    CUBE
};

struct Enemy {
    float x;
    float y;
    float width;
    float height;
    EnemyType type;
};

#endif