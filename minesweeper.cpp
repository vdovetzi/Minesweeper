#include "minesweeper.h"
// first constructor
Minesweeper::Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    h_ = height;
    w_ = width;
    for (const Cell& pos : cells_with_mines) {
        bombs_.insert({{pos.y, pos.x}, "bomb"});
    }
}

// second constructor
Minesweeper::Minesweeper(size_t width, size_t height, size_t mines_count) {
    w_ = width;
    h_ = height;
    std::vector<int64_t> coord1;
    coord1.reserve(std::max(height, width));
    std::vector<int64_t> coord2;
    coord2.reserve(std::min(height, width));
    for (int64_t i = 0; i < std::max(height, width); ++i) {
        coord1.push_back(i);
    }
    for (int64_t i = 0; i < std::min(height, width); ++i) {
        coord2.push_back(i);
    }
    std::mt19937 gen{std::random_device{}()};
    std::shuffle(coord1.begin(), coord1.end(), gen);
    std::shuffle(coord2.begin(), coord2.end(), gen);
    size_t cnt = 0;
    for (size_t i = 0; i < coord1.size(); ++i) {
        if (cnt == mines_count) {
            break;
        }
        for (size_t j = 0; j < coord2.size(); ++j) {
            ++cnt;
            bombs_[{coord1[i], coord2[j]}] = "bomb";
            if (cnt == mines_count) {
                break;
            }
        }
    }
}

// game initialization-1
void Minesweeper::NewGame(size_t width, size_t height, size_t mines_count) {
    game_.clear();
    opened_.clear();
    start_ = false;
    time_end_ = 0;
    time_start_ = 0;
    bombs_.clear();
    w_ = 0;
    h_ = 0;
    game_status_ = GameStatus::NOT_STARTED;
    *this = Minesweeper(width, height, mines_count);
}

// game initialization-2
void Minesweeper::NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    game_.clear();
    opened_.clear();
    start_ = false;
    time_end_ = 0;
    time_start_ = 0;
    bombs_.clear();
    w_ = 0;
    h_ = 0;
    game_status_ = GameStatus::NOT_STARTED;
    *this = Minesweeper(width, height, cells_with_mines);
}

Minesweeper::RenderedField Minesweeper::RenderField() {
    RenderedField map = RenderedField(h_, std::string(w_, '-'));
    if (!opened_.empty()) {
        if (game_status_ != GameStatus::DEFEAT) {
            for (size_t i = 0; i < h_; ++i) {
                std::string row;
                for (size_t j = 0; j < w_; ++j) {
                    if (game_.find({i, j}) != game_.end()) {
                        if (game_.at({i, j}) == "flag") {
                            row.push_back('?');
                        } else {
                            if (game_.at({i, j}) == "0") {
                                row.push_back('.');
                            } else {
                                row += game_.at({i, j});
                            }
                        }
                    } else {
                        row.push_back('-');
                    }
                }
                map[i] = row;
            }
        } else {
            for (size_t i = 0; i < h_; ++i) {
                std::string row;
                for (size_t j = 0; j < w_; ++j) {
                    if (bombs_.find({i, j}) != bombs_.end()) {
                        row.push_back('*');
                    } else if (game_.find({i, j}) != game_.end() && game_.at({i, j}) == "flag") {
                        row.push_back('?');
                    } else if (game_.find({i, j}) != game_.end() && game_.at({i, j}) == "0") {
                        row.push_back('.');
                    } else {
                        row += game_.at({i, j});
                    }
                }
                map[i] = row;
            }
        }
    }
    return map;
}

void Minesweeper::OpenCell(const Minesweeper::Cell& cell) {
    if (!start_) {
        start_ = true;
        time_start_ = clock();
        game_status_ = GameStatus::IN_PROGRESS;
    }
    if (game_status_ != GameStatus::DEFEAT && game_status_ != GameStatus::VICTORY &&
        ((game_.find({cell.y, cell.x}) != game_.end() && game_.at({cell.y, cell.x}) != "flag") ||
         game_.find({cell.y, cell.x}) == game_.end())) {
        auto it = bombs_.find({cell.y, cell.x});
        auto it1 = game_.find({cell.y, cell.x});
        opened_.insert({cell.y, cell.x});
        int num = NeighboursMines(cell.y, cell.x);
        // если в клетке нет бомбы и рядом с ней есть бомба
        if (bombs_.find({cell.y, cell.x}) == bombs_.end() && num > 0) {
            game_[{cell.y, cell.x}] = std::to_string(num);
            // если в клетке есть мина
        } else if (it != bombs_.end() && bombs_.at({cell.y, cell.x}) == "bomb") {
            game_status_ = GameStatus::DEFEAT;
            for (int64_t i = 0; i < h_; ++i) {
                for (int64_t j = 0; j < w_; ++j) {
                    if (bombs_.find({i, j}) == bombs_.end() && game_.find({i, j}) == game_.end()) {
                        game_[{i, j}] = std::to_string(NeighboursMines(i, j));
                    }
                }
            }
            // если в клетке пусто и рядом с ней нет мин
        } else if (it1 == game_.end() && num == 0) {
            game_.insert({{cell.y, cell.x}, std::to_string(NeighboursMines(cell.y, cell.x))});
            std::deque<std::pair<int64_t, int64_t>> deq = Neighbours(cell.y, cell.x);
            while (!deq.empty()) {
                if ((game_.find(deq[0]) != game_.end() && game_.at(deq[0]) != "flag") ||
                    (game_.find(deq[0]) == game_.end())) {
                    opened_.insert(deq[0]);
                    int mines = NeighboursMines(deq[0].first, deq[0].second);
                    if (mines > 0) {
                        game_.insert({deq[0], std::to_string(mines)});
                    } else {
                        game_.insert({deq[0], std::to_string(mines)});
                        for (const auto& pos : Neighbours(deq[0].first, deq[0].second)) {
                            deq.push_back(pos);
                        }
                    }
                }
                deq.pop_front();
            }
        }

        if (opened_.size() == w_ * h_ - bombs_.size() && game_status_ != GameStatus::DEFEAT) {
            game_status_ = GameStatus::VICTORY;
        }
    }
}

/// completed
void Minesweeper::MarkCell(const Minesweeper::Cell& cell) {
    if (game_status_ != GameStatus::DEFEAT && game_status_ != GameStatus::VICTORY) {
        auto it = game_.find({cell.y, cell.x});
        if (it == game_.end() || (it != game_.end() && game_.at({cell.y, cell.x}) == "bomb")) {
            game_.insert({{cell.y, cell.x}, "flag"});
        } else if (game_.at({cell.y, cell.x}) == "flag") {
            game_.erase({cell.y, cell.x});
        }
    }
}

/// completed
Minesweeper::GameStatus Minesweeper::GetGameStatus() const {
    return game_status_;
}

clock_t Minesweeper::GetGameTime() const {
    return time_end_ - time_start_;
}