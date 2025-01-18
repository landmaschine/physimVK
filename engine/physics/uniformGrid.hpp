#include "core/common.hpp"
#include <unordered_map>
#include <vector>

class UniformGrid {
public:
    UniformGrid(float cellSize) : cellSize(cellSize) {}

    void clear() {
        grid.clear();
    }

    void insert(const Particles& particles, size_t index) {
        std::lock_guard<std::mutex> lock(grid_mutex);

        float radius = particles.radius[index];
        vec2 position{particles.curr_pos_x[index], particles.curr_pos_y[index]};

        const auto minCell = computeCell(position - vec2(radius, radius));
        const auto maxCell = computeCell(position + vec2(radius, radius));

        for (int x = minCell.x; x <= maxCell.x; ++x) {
            for (int y = minCell.y; y <= maxCell.y; ++y) {
                auto& cell = grid[{x, y}];
                if (cell.capacity() == 0) {
                    cell.reserve(8);
                }
                cell.push_back(index);
            }
        }
    }

    void getNeighbors(const Particles& particles, size_t index, std::vector<size_t>& neighbors) {
        neighbors.clear();

        float radius = particles.radius[index];
        vec2 position{particles.curr_pos_x[index], particles.curr_pos_y[index]};

        vec2 minPos = position - vec2(radius, radius);
        vec2 maxPos = position + vec2(radius, radius);
        auto minCell = computeCell(minPos);
        auto maxCell = computeCell(maxPos);

        for (int x = minCell.x; x <= maxCell.x; ++x) {
            for (int y = minCell.y; y <= maxCell.y; ++y) {
                auto cell = grid.find({x, y});
                if (cell != grid.end()) {
                    neighbors.insert(neighbors.end(), cell->second.begin(), cell->second.end());
                }
            }
        }
    }

private:
    mutable std::mutex grid_mutex;

    struct CellHash {
        std::size_t operator()(const std::pair<int, int>& cell) const {
            std::size_t h1 = std::hash<int>{}(cell.first);
            std::size_t h2 = std::hash<int>{}(cell.second);
            return h1 ^ (h2 << 1);
        }
    };

    inline vec2i computeCell(const vec2& position) const {
        return vec2i(static_cast<int>(std::floor(position.x / cellSize)),
                    static_cast<int>(std::floor(position.y / cellSize)));
    }

    float cellSize;
    std::unordered_map<std::pair<int, int>, std::vector<size_t>, CellHash> grid;
};