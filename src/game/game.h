#ifndef GAME_H
#define GAME_H

#include <ae/app.h>

class Game : public ae::App
{
public:
    Game(int32_t width, int32_t height, const std::string &title);
    ~Game() = default;
};

#endif // GAME_H
