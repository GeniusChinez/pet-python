#pragma once

struct Location {
    Location() = default;
    Location(
        const std::string& file,
        size_t line,
        size_t column
    ) : 
        file(file),
        line(line),
        column(column) {}

    std::string file;
    size_t line;
    size_t column;
};
