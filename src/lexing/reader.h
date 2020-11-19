#pragma once

#include <string>
#include <cstdint>

class Reader {
public:
    Reader() = default;
    Reader(const Reader&) = default;
    Reader(Reader&&) = default;
    ~Reader() = default;

    [[nodiscard]]
    bool openFile(std::string fileName) {
        if (not readFileDataIntoBuffer(fileName, this->fileData)) {
            return false;
        }

        this->fileName = std::move(fileName);
        fileDataIterator = std::begin(fileData);

        return true;
    }

    uint32_t getCharacter() {
        if (fileDataIterator == fileData.end()) {
            return 0;
        }

        return GeniusC::GetUtf8Character(
            fileDataIterator, 
            std::end(fileData)
        );
    }

    const std::string& getFileName() const {
        return fileName;
    }

private:
    std::string fileName;
    std::string fileData;
    std::string::iterator fileDataIterator;
};
