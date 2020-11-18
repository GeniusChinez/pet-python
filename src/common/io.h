#pragma once

#include <fstream>
#include <string>

/**
 * @brief      Attempts to open a file with the given name using the 
 *  given input stream.
 *
 * @param[in]  fileName     The file name.
 * @param      inputStream  The stream using which to open the file.
 *
 * @return     An indication of whether the file was indeed opened 
 *  or not.
 */
bool openFile(const std::string& fileName, std::ifstream& inputStream) {
    inputStream.open(fileName.data());
    return inputStream.is_open();
}

/**
 * @brief      Reads a data from stream into buffer.
 *
 * @param      sourceStream  The stream to read data from.
 * @param      destBuffer    The destination buffer.
 */
void readDataFromStreamIntoBuffer(
    std::ifstream& sourceStream, 
    std::string& destBuffer
) {
    destBuffer.clear();
    
    sourceStream.seekg(0, std::ios::end);
    destBuffer.reserve(sourceStream.tellg());
    sourceStream.seekg(0, std::ios::beg);

    destBuffer.assign(
        std::istreambuf_iterator<char>(sourceStream), 
        std::istreambuf_iterator<char>()
    );
}

/**
 * @brief      Reads a file's data into buffer.
 *
 * @param[in]  fileName    The file name.
 * @param      destBuffer  The destination buffer.
 *
 * @return     An indication of whether or not the file was read.
 */
bool readFileDataIntoBuffer(const std::string& fileName, std::string& destBuffer) {
    std::ifstream inputStream;

    if (not openFile(fileName, inputStream)) {
        return false;
    }

    readDataFromStreamIntoBuffer(inputStream, destBuffer);
    inputStream.close();

    return true;
}

/**
 * @brief      Reads a file.
 *             
 * @param[in]  fileName  The name of the file.
 *
 * @return     the file's contents.
 */
std::string readFile(const std::string& fileName) {
    std::string buffer;
    readFileDataIntoBuffer(fileName, buffer);
    return buffer;
}
