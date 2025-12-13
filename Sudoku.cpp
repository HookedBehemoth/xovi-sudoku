#include "Sudoku.hpp"

#include <cstdint>
#include <QResource>
#include <QFile>
#include <QRandomGenerator>

constexpr const uchar HEADER[8] = { 'S', 'U', 'D', 'O', 'K', 'U', '0', '0' };
constexpr const size_t CELL_COUNT = 81;
constexpr const size_t PUZZLE_SIZE = static_cast<size_t>(CELL_COUNT / 2) + 1;
constexpr const size_t HINT_SIZE = static_cast<size_t>(CELL_COUNT / 8) + 1;
constexpr const size_t ELEMENT_SIZE = PUZZLE_SIZE + HINT_SIZE;

static_assert(PUZZLE_SIZE == 41);
static_assert(HINT_SIZE == 11);
static_assert(ELEMENT_SIZE == 52);

static constexpr std::optional<Sudoku> load(
    const uchar* data,
    const size_t size,
    std::optional<int> index = std::nullopt) {
    if (size < (sizeof(HEADER) + 4)) {
        printf("Invalid Sudoku file: %zu bytes\n", size);
        return std::nullopt;
    }

    if (std::memcmp(data, HEADER, sizeof(HEADER)) != 0) {
        printf("Invalid Sudoku file: bad header\n");
        return std::nullopt;
    }

    uint32_t count =
        static_cast<uint32_t>(data[8]) |
        (static_cast<uint32_t>(data[9]) << 8) |
        (static_cast<uint32_t>(data[10]) << 16) |
        (static_cast<uint32_t>(data[11]) << 24);

    const size_t puzzleIndex = index.has_value() ?
        static_cast<size_t>(index.value()) :
        static_cast<size_t>(QRandomGenerator::global()->bounded(count));

    if (puzzleIndex >= count) {
        printf("Invalid Sudoku file: index %zu out of range (max %u)\n", puzzleIndex, count);
        return std::nullopt;
    }

    const uchar* puzzleData = data + sizeof(HEADER) + 4 + (puzzleIndex * ELEMENT_SIZE);
    const uchar* hintData = puzzleData + 41;

    Sudoku sudoku = {};
    for (size_t i = 0; i < 81; ++i) {
        sudoku.Number[i] = (i % 2) == 0
            ? puzzleData[i / 2] >> 4
            : puzzleData[i / 2] & 0x0F;
        // printf("%d ", sudoku.Number[i]);
    }

    for (size_t i = 0; i < 81; ++i) {
        size_t nibble = i % 8;
        size_t byteIndex = i / 8;
        sudoku.HintMask[i] = (hintData[byteIndex] >> nibble) & 0x01;
    }

    return sudoku;
}

std::optional<Sudoku> Sudoku::loadFromResource(
    Sudoku::Difficulty level,
    std::optional<int> index) {
    const char* resourcePath = nullptr;

    switch (level) {
    case Difficulty::Easy:
        resourcePath = ":/bin/res/easy.bin";
        break;
    case Difficulty::Medium:
        resourcePath = ":/bin/res/medium.bin";
        break;
    case Difficulty::Hard:
        resourcePath = ":/bin/res/hard.bin";
        break;
    case Difficulty::Expert:
        resourcePath = ":/bin/res/expert.bin";
        break;
    default:
        printf("Invalid Sudoku difficulty level\n");
        return std::nullopt;
    }
    printf("Loading Sudoku from resource: %s\n", resourcePath);

    QResource res(resourcePath);

    if (!res.isValid()) {
        printf("Failed to load Sudoku resource: %s\n", resourcePath);
        return std::nullopt;
    }

    return load(res.data(), res.size(), index);
}

std::optional<Sudoku> Sudoku::loadFromFile(
    const char* path,
    std::optional<int> index) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return std::nullopt;
    }

    QByteArray fileData = file.readAll();
    return load(
        reinterpret_cast<const uchar*>(fileData.constData()),
        static_cast<size_t>(fileData.size()),
        index);
}

constexpr const std::array<uchar, 116> compressedSudokuPuzzle = {
    // SUDOKU00
    0x53, 0x55, 0x44, 0x4f, 0x4b, 0x55, 0x30, 0x30,
    // count
    0x02, 0x00, 0x00, 0x00,

    // puzzle 0
    0x17, 0x92, 0x38, 0x64,
    0x56, 0x38, 0x45, 0x72, 0x19, 0x25, 0x49, 0x61,
    0x87, 0x37, 0x46, 0x58, 0x39, 0x21, 0x31, 0x26,
    0x49, 0x58, 0x79, 0x85, 0x17, 0x24, 0x36, 0x49,
    0x38, 0x16, 0x75, 0x25, 0x67, 0x32, 0x41, 0x98,
    0x82, 0x17, 0x95, 0x36, 0x40,
    // hints 0
    0x00, 0x51, 0x07, 0x88, 0x58, 0x03, 0x64, 0x29, 0x29, 0x61, 0x00,

    // puzzle 1
    0x57, 0x81, 0x29, 0x64, 0x39, 0x32, 0x65, 0x41,
    0x87, 0x16, 0x48, 0x73, 0x95, 0x22, 0x87, 0x96,
    0x15, 0x34, 0x69, 0x53, 0x47, 0x82, 0x13, 0x41,
    0x28, 0x57, 0x96, 0x41, 0x37, 0x98, 0x26, 0x58,
    0x56, 0x41, 0x23, 0x79, 0x72, 0x95, 0x36, 0x41,
    0x80,
    // hints 1
    0x91, 0x84, 0x15, 0x19, 0x73, 0x00, 0xbc, 0x0b, 0x0c, 0x48, 0x00
};
constexpr const Sudoku decompressed0 = {
    // 179238645638457219254961873746583921312649587985172436493816752567324198821795364
    Number: {
        1, 7, 9,  2, 3, 8,  6, 4, 5,
        6, 3, 8,  4, 5, 7,  2, 1, 9,
        2, 5, 4,  9, 6, 1,  8, 7, 3,
        7, 4, 6,  5, 8, 3,  9, 2, 1,
        3, 1, 2,  6, 4, 9,  5, 8, 7,
        9, 8, 5,  1, 7, 2,  4, 3, 6,
        4, 9, 3,  8, 1, 6,  7, 5, 2,
        5, 6, 7,  3, 2, 4,  1, 9, 8,
        8, 2, 1,  7, 9, 5,  3, 6, 4,
    },
    // --------5---4-7-192--------7---8---13-2-49--------2--64-3--6-5--6--2-1--8----53--
    HintMask: {
        false, false, false, false, false, false, false, false, true,
        false, false, false, true, false, true, false, true, true,
        true, false, false, false, false, false, false, false, false,
        true, false, false, false, true, false, false, false, true,
        true, false, true, false, true, true, false, false, false,
        false, false, false, false, false, true, false, false, true,
        true, false, true, false, false, true, false, true, false,
        false, true, false, false, true, false, true, false, false,
        true, false, false, false, false, true, true, false, false, 
    },
};

constexpr const Sudoku decompressed1 = {
    // 578129643932654187164873952287961534695347821341285796413798265856412379729536418
    Number: {
        5, 7, 8,  1, 2, 9,  6, 4, 3,
        9, 3, 2,  6, 5, 4,  1, 8, 7,
        1, 6, 4,  8, 7, 3,  9, 5, 2,

        2, 8, 7,  9, 6, 1,  5, 3, 4,
        6, 9, 5,  3, 4, 7,  8, 2, 1,
        3, 4, 1,  2, 8, 5,  7, 9, 6,

        4, 1, 3,  7, 9, 8,  2, 6, 5,
        8, 5, 6,  4, 1, 2,  3, 7, 9,
        7, 2, 9,  5, 3, 6,  4, 1, 8,
    },
    // 5---2--4--3----18-1-4---9--28---15--695-----------5796-137-8------41-------5--4--
    HintMask: {
        true, false, false, false, true, false, false, true, false,
        false, true, false, false, false, false, true, true, false,
        true, false, true, false, false, false, true, false, false,
        true, true, false, false, false, true, true, false, false,
        true, true, true, false, false, false, false, false, false,
        false, false, false, false, false, true, true, true, true,
        false, true, true, true, false, true, false, false, false,
        false, false, false, true, true, false, false, false, false,
        false, false, false, true, false, false, true, false, false, 
    },
};

static_assert(
    load(
        compressedSudokuPuzzle.data(),
        compressedSudokuPuzzle.size(),
        0
    ).value() == decompressed0
);

static_assert(
    load(
        compressedSudokuPuzzle.data(),
        compressedSudokuPuzzle.size(),
        1
    ).value() == decompressed1
);
