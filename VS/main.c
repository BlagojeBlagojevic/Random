#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h" // Required for vector math

#define MAX_PARTICLES 1000
#define SPAWN_INTERVAL 0.015f

// --- Data Structures ---

typedef enum {
    E_Enemy,
    E_Player,
    E_Bullet,
    E_Experience
} EntityType;

typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float alpha;
    float size;
    float decay;
    bool active;
} Particle;

typedef struct Entity {
    EntityType type;
    struct Entity *prev, *next;
    
    // Physics
    Vector2 position;
    Vector2 velocity;
    float radius;
    float speed;
    
    // Properties
    Color color;
    float health;
    float maxHealth;
    bool active;
    
    // Enemy specific
    int enemyType;
    
    // Bullet specific
    float lifetime;
    int damage;
    
    // Experience specific
    int value;
    float bobOffset; // For floating animation
    
} Entity;

typedef struct {
    int maxHealth;
    float moveSpeed;
    float fireRate;
    float lastFireTime; // Moved here to persist between frames
    int damage;
    int bulletCount;
    float bulletSpread;
} PlayerStats;

typedef struct {
    int width;
    int height;
    
    // Entity Management
    Entity *list;
    Entity *player;
    Particle particles[MAX_PARTICLES];
    
    // Game State
    int score;
    float gameTime;
    float spawnTimer;
    bool gameOver;
    int level;
    int experience;
    int expToNextLevel;
    
    // Persistent Player Stats
    PlayerStats stats;
    
    // Camera & VFX
    Camera2D camera;
    float screenShake;
    
} Engine;

// --- Prototypes ---
Engine *engine_init(int width, int height);
void engine_add_entity(Engine *engine, Entity *entity);
void engine_remove_entity(Engine *engine, Entity *entity);
void spawn_particle(Engine *engine, Vector2 pos, Color color, int count, float speed);
Entity* create_bullet(Vector2 position, Vector2 direction, int damage);

// --- Math Helpers ---


// --- Implementation ---

Engine *engine_init(int width, int height) {
    Engine *engine = malloc(sizeof(Engine));
    engine->width = width;
    engine->height = height;
    engine->list = NULL;
    engine->player = NULL;
    
    engine->score = 0;
    engine->gameTime = 0;
    engine->spawnTimer = 0;
    engine->gameOver = false;
    engine->level = 1;
    engine->experience = 0;
    engine->expToNextLevel = 20;
    engine->screenShake = 0.0f;
    
    // Init Particles
    for(int i=0; i<MAX_PARTICLES; i++) engine->particles[i].active = false;
    
    // Init Camera
    engine->camera.target = (Vector2){ 0, 0 };
    engine->camera.offset = (Vector2){ width/2.0f, height/2.0f };
    engine->camera.rotation = 0.0f;
    engine->camera.zoom = 1.0f;

    // Init Base Stats
    engine->stats.maxHealth = 100;
    engine->stats.moveSpeed = 250.0f;
    engine->stats.fireRate = 4.0f;
    engine->stats.lastFireTime = 0.0f;
    engine->stats.damage = 15;
    engine->stats.bulletCount = 1;
    engine->stats.bulletSpread = 0.2f;
    
    return engine;    
}

void engine_add_entity(Engine *engine, Entity *entity) {
    entity->next = engine->list;
    entity->prev = NULL;
    if (engine->list) engine->list->prev = entity;
    engine->list = entity;
    if (entity->type == E_Player) engine->player = entity;
}

Entity* create_player(Vector2 position) {
    Entity *e = malloc(sizeof(Entity));
    e->type = E_Player;
    e->position = position;
    e->velocity = (Vector2){0,0};
    e->radius = 12.0f;
    e->color = SKYBLUE;
    e->health = 100.0f;
    e->maxHealth = 100.0f;
    e->active = true;
    return e;
}

Entity* create_enemy(Vector2 position, int level) {
    Entity *e = malloc(sizeof(Entity));
    e->type = E_Enemy;
    e->position = position;
    e->velocity = (Vector2){0,0};
    e->radius = 10.0f;
    
    // Varied enemy types based on randomization
    int type = GetRandomValue(0, 2);
    if(type == 0) { // Fast, weak
        e->color = (Color){230, 41, 55, 255}; 
        e->speed = 120.0f + (level * 2);
        e->health = 15.0f + (level * 2);
    } else { // Slow, tanky
        e->color = (Color){100, 0, 0, 255};
        e->speed = 60.0f + (level * 1);
        e->health = 40.0f + (level * 5);
        e->radius = 14.0f;
    }
    
    e->maxHealth = e->health;
    e->active = true;
    return e;
}

Entity* create_bullet(Vector2 position, Vector2 direction, int damage) {
    Entity *e = malloc(sizeof(Entity));
    e->type = E_Bullet;
    e->position = position;
    e->velocity = Vector2Scale(direction, 600.0f);
    e->radius = 4.0f;
    e->color = YELLOW;
    e->lifetime = 1.5f;
    e->damage = damage;
    e->active = true;
    return e;
}

Entity* create_experience(Vector2 position, int value) {
    Entity *e = malloc(sizeof(Entity));
    e->type = E_Experience;
    e->position = position;
    e->velocity = (Vector2){0,0};
    e->radius = 4.0f;
    e->color = (Color){ 0, 228, 48, 255 };
    e->value = value;
    e->active = true;
    e->bobOffset = (float)GetRandomValue(0, 100) / 10.0f;
    e->speed = 400.0f; // Magnet speed
    return e;
}

void spawn_particle(Engine *engine, Vector2 pos, Color color, int count, float speed) {
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < count; i++) {
        if (!engine->particles[i].active) {
            Particle *p = &engine->particles[i];
            p->active = true;
            p->position = pos;
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float spd = (float)GetRandomValue(50, 150) / 100.0f * speed;
            p->velocity = (Vector2){ cosf(angle) * spd, sinf(angle) * spd };
            p->color = color;
            p->alpha = 1.0f;
            p->size = (float)GetRandomValue(2, 5);
            p->decay = (float)GetRandomValue(2, 5) * 0.01f;
            spawned++;
        }
    }
}

// --- Update Logic ---

void update_player(Engine *engine, Entity *player) {
    Vector2 input = {0, 0};
    if (IsKeyDown(KEY_W)) input.y -= 1;
    if (IsKeyDown(KEY_S)) input.y += 1;
    if (IsKeyDown(KEY_A)) input.x -= 1;
    if (IsKeyDown(KEY_D)) input.x += 1;
    
    if (Vector2Length(input) > 0) input = Vector2Normalize(input);
    
    player->velocity = Vector2Scale(input, engine->stats.moveSpeed);
    player->position = Vector2Add(player->position, Vector2Scale(player->velocity, GetFrameTime()));
    
    // Auto-fire
    if (GetTime() - engine->stats.lastFireTime >= 1.0f / engine->stats.fireRate) {
        // Find nearest enemy to aim at
        Entity *nearest = NULL;
        float minDist = 10000.0f;
        Entity *curr = engine->list;
        while(curr) {
            if(curr->type == E_Enemy && curr->active) {
                float d = Vector2Distance(player->position, curr->position);
                if(d < minDist) { minDist = d; nearest = curr; }
            }
            curr = curr->next;
        }

        Vector2 targetDir = {1, 0}; // Default aim right
        if (nearest) {
            targetDir = Vector2Normalize(Vector2Subtract(nearest->position, player->position));
        } else {
            // If no enemy, aim at mouse
            Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), engine->camera);
            targetDir = Vector2Normalize(Vector2Subtract(mouseWorld, player->position));
        }

        for (int i = 0; i < engine->stats.bulletCount; i++) {
            float angleOffset = (i - (engine->stats.bulletCount - 1) / 2.0f) * engine->stats.bulletSpread;
            Vector2 rotDir = {
                targetDir.x * cosf(angleOffset) - targetDir.y * sinf(angleOffset),
                targetDir.x * sinf(angleOffset) + targetDir.y * cosf(angleOffset)
            };
            engine_add_entity(engine, create_bullet(player->position, rotDir, engine->stats.damage));
        }
        engine->stats.lastFireTime = GetTime();
    }
}

void update_enemies(Engine *engine) {
    Entity *e = engine->list;
    while(e) {
        if(e->type == E_Enemy && e->active) {
            // 1. Move towards player
            if(engine->player && engine->player->active) {
                Vector2 dir = Vector2Normalize(Vector2Subtract(engine->player->position, e->position));
                e->velocity = Vector2Scale(dir, e->speed);
            }

            // 2. Separation Logic (Boids: prevent stacking)
            Vector2 separation = {0,0};
            int neighbors = 0;
            Entity *other = engine->list;
            while(other) {
                if(other != e && other->type == E_Enemy && other->active) {
                    float dist = Vector2Distance(e->position, other->position);
                    if(dist < e->radius * 2.5f) { // Too close
                        Vector2 push = Vector2Subtract(e->position, other->position);
                        push = Vector2Normalize(push);
                        push = Vector2Scale(push, (e->radius * 3.0f) / (dist + 0.1f)); // Stronger push when closer
                        separation = Vector2Add(separation, push);
                        neighbors++;
                    }
                }
                other = other->next;
            }
            
            if(neighbors > 0) {
                separation = Vector2Scale(separation, 50.0f); // Separation strength
                e->velocity = Vector2Add(e->velocity, separation);
            }

            e->position = Vector2Add(e->position, Vector2Scale(e->velocity, GetFrameTime()));
        }
        e = e->next;
    }
}

void check_collisions(Engine *engine) {
    Entity *a = engine->list;
    while (a) {
        if (!a->active) { a = a->next; continue; }
        
        // Bullet Hit Enemy
        if (a->type == E_Bullet) {
            Entity *b = engine->list;
            while (b) {
                if (b->type == E_Enemy && b->active) {
                    if (CheckCollisionCircles(a->position, a->radius, b->position, b->radius)) {
                        b->health -= a->damage;
                        a->active = false; // Destroy bullet
                        
                        // Blood effect
                        spawn_particle(engine, b->position, RED, 5, 200.0f);

                        if (b->health <= 0) {
                            b->active = false; // Kill enemy
                            engine->score += 10;
                            spawn_particle(engine, b->position, RED, 15, 300.0f); // Explosion
                            engine_add_entity(engine, create_experience(b->position, 5 + engine->level));
                        }
                        break;
                    }
                }
                b = b->next;
            }
        }
        
        // Player Hit Exp
        if (a->type == E_Experience && engine->player) {
            float dist = Vector2Distance(a->position, engine->player->position);
            
            // Magnet effect
            if (dist < 100.0f) {
                Vector2 dir = Vector2Normalize(Vector2Subtract(engine->player->position, a->position));
                a->position = Vector2Add(a->position, Vector2Scale(dir, a->speed * GetFrameTime()));
            }
            
            if (dist < engine->player->radius + a->radius) {
                a->active = false;
                engine->experience += a->value;
                if (engine->experience >= engine->expToNextLevel) {
                    // Level Up!
                    engine->experience -= engine->expToNextLevel;
                    engine->level++;
                    engine->expToNextLevel *= 1.2f;
                    
                    // Buff Stats
                    engine->stats.bulletCount++; 
                    if(engine->stats.bulletCount > 5) {
                        engine->stats.bulletCount = 5;
                        engine->stats.fireRate += 1.0f;
                        engine->stats.damage += 5;
                    }
                    
                    // Visual Heal
                    engine->player->health = engine->stats.maxHealth;
                    spawn_particle(engine, engine->player->position, GOLD, 30, 400.0f);
                }
            }
        }
        
        // Enemy Hit Player
        if (a->type == E_Enemy && engine->player && engine->player->active) {
            if (CheckCollisionCircles(a->position, a->radius, engine->player->position, engine->player->radius)) {
                engine->player->health -= 20.0f * GetFrameTime();
                engine->screenShake = 5.0f; // Add shake
                
                if (engine->player->health <= 0) {
                    engine->player->active = false;
                    engine->gameOver = true;
                    spawn_particle(engine, engine->player->position, SKYBLUE, 50, 500.0f);
                }
            }
        }
        
        a = a->next;
    }
}

void engine_update(Engine *engine) {
    if (engine->gameOver) return;

    engine->gameTime += GetFrameTime();
    
    // Decrease Screen Shake
    if (engine->screenShake > 0) engine->screenShake -= GetFrameTime() * 15.0f;
    if (engine->screenShake < 0) engine->screenShake = 0;

    // Update Player
    if (engine->player && engine->player->active) {
        update_player(engine, engine->player);
    }
    
    // Camera Smooth Follow
    if (engine->player) {
        Vector2 target = engine->player->position;
        // Add Shake
        target.x += GetRandomValue(-100, 100) / 100.0f * engine->screenShake;
        target.y += GetRandomValue(-100, 100) / 100.0f * engine->screenShake;
        
        engine->camera.target.x = Lerp(engine->camera.target.x, target.x, 0.1f);
        engine->camera.target.y = Lerp(engine->camera.target.y, target.y, 0.1f);
    }
    
    // Update Enemies & Physics
    update_enemies(engine);
    
    // Update Bullets & Exp cleanup
    Entity *curr = engine->list;
    while(curr) {
        if(curr->type == E_Bullet && curr->active) {
            curr->position = Vector2Add(curr->position, Vector2Scale(curr->velocity, GetFrameTime()));
            curr->lifetime -= GetFrameTime();
            if(curr->lifetime <= 0) curr->active = false;
        }
        curr = curr->next;
    }
    
    check_collisions(engine);
    
    // Update Particles
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(engine->particles[i].active) {
            Particle *p = &engine->particles[i];
            p->position = Vector2Add(p->position, Vector2Scale(p->velocity, GetFrameTime()));
            p->alpha -= p->decay;
            if(p->alpha <= 0) p->active = false;
        }
    }

    // Spawner
    engine->spawnTimer -= GetFrameTime();
    if (engine->spawnTimer <= 0) {
        float angle = (float)GetRandomValue(0, 360);
        float dist = sqrtf(pow(engine->width,2) + pow(engine->height,2)); // Spawn outside screen
        Vector2 spawnPos = {
            engine->player->position.x + cosf(angle) * 600.0f,
            engine->player->position.y + sinf(angle) * 600.0f
        };
        engine_add_entity(engine, create_enemy(spawnPos, engine->level));
        engine->spawnTimer = SPAWN_INTERVAL / (1.0f + engine->level * 0.1f);
    }
    
    // Cleanup Inactive
    Entity *c = engine->list;
    while(c) {
        Entity *next = c->next;
        if(!c->active) {
            if(c->prev) c->prev->next = c->next;
            else engine->list = c->next;
            if(c->next) c->next->prev = c->prev;
            free(c);
        }
        c = next;
    }
}

void engine_render(Engine *engine) {
    BeginDrawing();
    ClearBackground((Color){20, 20, 25, 255});
    
    BeginMode2D(engine->camera);
        // Draw Grid relative to camera for "infinite" feel
        int gridSize = 100;
        int camX = (int)engine->camera.target.x;
        int camY = (int)engine->camera.target.y;
        int offsetX = camX % gridSize;
        int offsetY = camY % gridSize;
        
        for (int x = camX - engine->width; x < camX + engine->width; x += gridSize) {
            DrawLine(x - offsetX, camY - engine->height, x - offsetX, camY + engine->height, (Color){255,255,255, 10});
        }
        for (int y = camY - engine->height; y < camY + engine->height; y += gridSize) {
            DrawLine(camX - engine->width, y - offsetY, camX + engine->width, y - offsetY, (Color){255,255,255, 10});
        }

        // Draw Particles
        for(int i=0; i<MAX_PARTICLES; i++) {
            if(engine->particles[i].active) {
                Particle *p = &engine->particles[i];
                DrawRectangleV(p->position, (Vector2){p->size, p->size}, Fade(p->color, p->alpha));
            }
        }

        // Draw Entities
        Entity *e = engine->list;
        while(e) {
            if(e->active) {
                if(e->type == E_Enemy) {
                    DrawCircleV(e->position, e->radius, e->color);
                    // Enemy Health Bar
                    float hpPct = e->health / e->maxHealth;
                    DrawRectangle(e->position.x-10, e->position.y-20, 20, 4, BLACK);
                    DrawRectangle(e->position.x-10, e->position.y-20, 20*hpPct, 4, RED);
                } 
                else if (e->type == E_Player) {
                    DrawCircleV(e->position, e->radius, e->color);
                    DrawCircleLines(e->position.x, e->position.y, e->radius + 5 + (sin(GetTime()*10)*2), GOLD); // Shield glow
                }
                else if (e->type == E_Bullet) {
                    DrawCircleV(e->position, e->radius, e->color);
                }
                else if (e->type == E_Experience) {
                    e->bobOffset += GetFrameTime();
                    DrawPoly(e->position, 4, e->radius, GetTime()*180, e->color);
                }
            }
            e = e->next;
        }
    EndMode2D();

    // UI Layer
    DrawRectangle(0, 0, engine->width, 60, (Color){0,0,0,150});
    
    DrawText(TextFormat("LVL %d", engine->level), 20, 20, 30, GOLD);
    DrawText(TextFormat("SCORE: %d", engine->score), 150, 25, 20, WHITE);
    
    // Player Health Bar (UI)
    if(engine->player) {
        float hpPct = engine->player->health / engine->stats.maxHealth;
        DrawRectangle(engine->width/2 - 100, 20, 200, 20, DARKGRAY);
        DrawRectangle(engine->width/2 - 100, 20, 200 * hpPct, 20, RED);
        DrawRectangleLines(engine->width/2 - 100, 20, 200, 20, WHITE);
    }
    
    // EXP Bar (Bottom)
    float expPct = (float)engine->experience / engine->expToNextLevel;
    DrawRectangle(0, engine->height-10, engine->width * expPct, 10, BLUE);

    if (engine->gameOver) {
        DrawRectangle(0, 0, engine->width, engine->height, (Color){0, 0, 0, 200});
        DrawText("YOU DIED", engine->width/2 - MeasureText("YOU DIED", 60)/2, engine->height/2 - 50, 60, RED);
        DrawText("Press R to Restart", engine->width/2 - MeasureText("Press R to Restart", 20)/2, engine->height/2 + 20, 20, RAYWHITE);
    }

    EndDrawing();
}

int main() {
    InitWindow(1280, 720, "Vampire Survivors Clone - Enhanced");
    //SetTargetFPS(60);
    
    Engine *engine = engine_init(1280, 720);
    engine_add_entity(engine, create_player((Vector2){0,0}));

    while (!WindowShouldClose()) {
        if(IsKeyPressed(KEY_R) && engine->gameOver) {
            // Lazy restart: re-init engine (leaks memory in this snippet, but simple for demo)
            engine = engine_init(1280, 720);
            engine_add_entity(engine, create_player((Vector2){0,0}));
        }
        
        engine_update(engine);
        engine_render(engine);
    }
    
    CloseWindow();
    return 0;
}
