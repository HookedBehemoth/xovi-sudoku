#pragma once

#include <optional>

class Sudoku {
public:
    enum class Difficulty {
        Easy,
        Medium,
        Hard,
        Expert
    };

public:
    char Number[81];
    bool HintMask[81];

    static std::optional<Sudoku> loadFromResource(Sudoku::Difficulty level, std::optional<int> index);
    static std::optional<Sudoku> loadFromFile(const char* path, std::optional<int> index);

    constexpr bool operator==(const Sudoku& other) const {
        for (size_t i = 0; i < 81; ++i) {
            if (Number[i] != other.Number[i] ||
                HintMask[i] != other.HintMask[i]) {
                return false;
            }
        }
        return true;
    }
};
