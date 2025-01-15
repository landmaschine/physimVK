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
        const auto minCell = computeCell(particle.curr_pos - vec2(particle.radius, particle.radius));
        const auto maxCell = computeCell(particle.curr_pos + vec2(particle.radius, particle.radius));

        for (int x = minCell.x; x <= maxCell.x; ++x) {
            for (int y = minCell.y; y <= maxCell.y; ++y) {
                grid[{x, y}].push_back(&particle);
            }
        }
    }

    std::vector<const Particle*> getNeighbors(const Particle& particle) const {
        std::vector<const Particle*> neighbors;
        const auto minCell = computeCell(particle.curr_pos - vec2(particle.radius, particle.radius));
        const auto maxCell = computeCell(particle.curr_pos + vec2(particle.radius, particle.radius));

        for (int x = minCell.x; x <= maxCell.x; ++x) {
            for (int y = minCell.y; y <= maxCell.y; ++y) {
                auto cell = grid.find({x, y});
                if (cell != grid.end()) {
                    neighbors.insert(neighbors.end(), cell->second.begin(), cell->second.end());
                }
            }
        }
        return neighbors;
    }

private:
    struct CellHash {
        std::size_t operator()(const std::pair<int, int>& cell) const {
            return std::hash<int>()(cell.first) ^ std::hash<int>()(cell.second);
        }
    };

    vec2i computeCell(const vec2& position) const {
        return vec2i(static_cast<int>(std::floor(position.x / cellSize)),
                     static_cast<int>(std::floor(position.y / cellSize)));
    }

    float cellSize;
    std::unordered_map<std::pair<int, int>, std::vector<const Particle*>, CellHash> grid;
};