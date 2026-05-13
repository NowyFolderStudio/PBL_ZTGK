#pragma once

struct GameStateViewData {
    int score = 0;
    int lives = 0;
};

struct GameStateView {
    GameStateViewData data;
};
