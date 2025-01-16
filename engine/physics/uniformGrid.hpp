#include "core/common.hpp"
#include <unordered_map>
#include <vector>

class UniformGrid {
public:
    UniformGrid(float cellSize) : cellSize(cellSize) {}

    void clear() {
        grid.clear();
    }

    void insert(const Particle& particle) {
        std::lock_guard<std::mutex> lock(grid_mutex);
        const auto minCell = computeCell(particle.curr_pos - vec2(particle.radius, particle.radius));
        const auto maxCell = computeCell(particle.curr_pos + vec2(particle.radius, particle.radius));

        const int cellCount = (maxCell.x - minCell.x + 2) * (maxCell.y - minCell.y + 1);

        for (int x = minCell.x; x <= maxCell.x; ++x) {
            for (int y = minCell.y; y <= maxCell.y; ++y) {
                auto& cell = grid[{x, y}];
                if (cell.capacity() == 0) {
                    cell.reserve(8);
                }
                cell.push_back(&particle);
            }
        }
    }

    void getNeighbors(const Particle& particle, std::vector<const Particle*>& neighbors) {
        std::lock_guard<std::mutex> lock(grid_mutex);
        neighbors.clear();

        const auto minCell = computeCell(particle.curr_pos - vec2(particle.radius, particle.radius));
        const auto maxCell = computeCell(particle.curr_pos + vec2(particle.radius, particle.radius));

        const int expectedNeighbors = (maxCell.x - minCell.x + 1) * (maxCell.y - minCell.y + 1) * 4;
        neighbors.reserve(expectedNeighbors);

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
            return (static_cast<size_t>(cell.first) << 32) | static_cast<size_t>(cell.second);
        }
    };

    vec2i computeCell(const vec2& position) const {
        return vec2i(static_cast<int>(std::floor(position.x / cellSize)),
                     static_cast<int>(std::floor(position.y / cellSize)));
    }

    float cellSize;
    std::unordered_map<std::pair<int, int>, std::vector<const Particle*>, CellHash> grid;
};