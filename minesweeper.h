#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <random>

class Minesweeper {
public:
    struct Cell {
        int64_t x = 0;
        int64_t y = 0;
    };

    enum class GameStatus {
        NOT_STARTED,
        IN_PROGRESS,
        VICTORY,
        DEFEAT,
    };

    using RenderedField = std::vector<std::string>;
    using Game = std::map<std::pair<int64_t, int64_t>, std::string>;

    Minesweeper(size_t width, size_t height, size_t mines_count);
    Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void NewGame(size_t width, size_t height, size_t mines_count);
    void NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void OpenCell(const Cell& cell);
    void MarkCell(const Cell& cell);

    GameStatus GetGameStatus() const;
    clock_t GetGameTime() const;

    RenderedField RenderField();
    Game game_;

private:
    GameStatus game_status_ = GameStatus::NOT_STARTED;
    size_t h_;
    size_t w_;
    bool start_ = false;
    clock_t time_start_ = 0;
    clock_t time_end_ = 0;
    std::set<std::pair<int64_t, int64_t>> opened_;
    Game bombs_;
    int NeighboursMines(int64_t x, int64_t y) {
        int ans = 0;
        std::vector<std::pair<int64_t, int64_t>> coord = {{x + 1, y + 1}, {x + 1, y - 1}, {x - 1, y + 1},
                                                          {x - 1, y - 1}, {x, y + 1},     {x + 1, y},
                                                          {x, y - 1},     {x - 1, y}};
        for (const auto& x_y : coord) {
            if (x_y.first < h_ && x_y.first >= 0 && x_y.second < w_ && x_y.second >= 0) {
                if (bombs_.find(x_y) != bombs_.end() && bombs_.at(x_y) == "bomb") {
                    ++ans;
                }
            }
        }

        return ans;
    }
    std::deque<std::pair<int64_t, int64_t>> Neighbours(int64_t x, int64_t y) {
        std::deque<std::pair<int64_t, int64_t>> ans;
        std::deque<std::pair<size_t, size_t>> coord = {{x + 1, y + 1}, {x + 1, y - 1}, {x - 1, y + 1}, {x - 1, y - 1},
                                                       {x, y + 1},     {x + 1, y},     {x, y - 1},     {x - 1, y}};
        for (const auto& x_y : coord) {
            if (x_y.first < h_ && x_y.first >= 0 && x_y.second < w_ && x_y.second >= 0 &&
                opened_.find(x_y) == opened_.end() &&
                ((game_.find({x_y.first, x_y.second}) != game_.end() && game_.at({x_y.first, x_y.second}) != "flag") ||
                 game_.find({x_y.first, x_y.second}) == game_.end())) {
                ans.push_back(x_y);
            }
        }
        return ans;
    }
};
