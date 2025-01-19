#include "core/common.hpp"
#include <unordered_map>
#include <vector>

#include <immintrin.h>

class UniformGrid {
public:
    UniformGrid(float cellSize) : cellSize(cellSize) {
        inv_cell_size = _mm256_set1_ps(1.0f / cellSize);
    }

    void clear() {
        grid.clear();
    }

    void insert(const Particles& particles, size_t index) {
        __m256 pos_x = _mm256_broadcast_ss(&particles.curr_pos_x[index]);
        __m256 pos_y = _mm256_broadcast_ss(&particles.curr_pos_y[index]);
        __m256 radius = _mm256_broadcast_ss(&particles.radius[index]);

        __m256 radius_vec = _mm256_set1_ps(particles.radius[index]);
        __m256 min_pos_x = _mm256_sub_ps(pos_x, radius_vec);
        __m256 min_pos_y = _mm256_sub_ps(pos_y, radius_vec);
        __m256 max_pos_x = _mm256_add_ps(pos_x, radius_vec);
        __m256 max_pos_y = _mm256_add_ps(pos_y, radius_vec);

        vec2i minCell = computeCell(vec2{
            _mm256_cvtss_f32(min_pos_x),
            _mm256_cvtss_f32(min_pos_y)
        });
        vec2i maxCell = computeCell(vec2{
            _mm256_cvtss_f32(max_pos_x),
            _mm256_cvtss_f32(max_pos_y)
        });

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
        neighbors.reserve(32);

        __m256 pos_x = _mm256_broadcast_ss(&particles.curr_pos_x[index]);
        __m256 pos_y = _mm256_broadcast_ss(&particles.curr_pos_y[index]);
        __m256 radius = _mm256_broadcast_ss(&particles.radius[index]);

        __m256 radius_vec = _mm256_set1_ps(particles.radius[index]);
        __m256 min_pos_x = _mm256_sub_ps(pos_x, radius_vec);
        __m256 min_pos_y = _mm256_sub_ps(pos_y, radius_vec);
        __m256 max_pos_x = _mm256_add_ps(pos_x, radius_vec);
        __m256 max_pos_y = _mm256_add_ps(pos_y, radius_vec);

        vec2i minCell = computeCell(vec2{
            _mm256_cvtss_f32(min_pos_x),
            _mm256_cvtss_f32(min_pos_y)
        });
        vec2i maxCell = computeCell(vec2{
            _mm256_cvtss_f32(max_pos_x),
            _mm256_cvtss_f32(max_pos_y)
        });

        for (int x = minCell.x; x <= maxCell.x; ++x) {
            for (int y = minCell.y; y <= maxCell.y; ++y) {
                auto cellIter = grid.find({x, y});
                if (cellIter != grid.end()) {
                    const auto& cellParticles = cellIter->second;
                    neighbors.insert(neighbors.end(), cellParticles.begin(), cellParticles.end());
                }
            }
        }
    }

private:
    __m256 inv_cell_size;

    inline vec2i computeCell(const vec2& position) const {
        __m256 pos_x = _mm256_set1_ps(position.x);
        __m256 pos_y = _mm256_set1_ps(position.y);
        
        __m256 cell_x = _mm256_mul_ps(pos_x, inv_cell_size);
        __m256 cell_y = _mm256_mul_ps(pos_y, inv_cell_size);
        
        return vec2i(
            static_cast<int>(std::floor(_mm256_cvtss_f32(cell_x))),
            static_cast<int>(std::floor(_mm256_cvtss_f32(cell_y)))
        );
    }

    struct CellHash {
        std::size_t operator()(const std::pair<int, int>& cell) const {
            std::size_t h1 = std::hash<int>{}(cell.first);
            std::size_t h2 = std::hash<int>{}(cell.second);
            return h1 ^ (h2 << 1);
        }
    };

    float cellSize;
    std::unordered_map<std::pair<int, int>, std::vector<size_t>, CellHash> grid;
};