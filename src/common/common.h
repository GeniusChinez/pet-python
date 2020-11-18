#pragma once
#include <iostream>
#include <sstream>
#include <string>

/**
 * @brief      Acts as the base case for the other 'printToStream' function.
 *
 * @param      out   The output stream.
 */
void printToStream(std::ostream& out) {}

/**
 * @brief      Feeds the input to the given output stream.
 *
 * @param      out   The output stream.
 * @param[in]  arg   The argument which to print.
 * @param      rest  The rest of the arguments which to print.
 *
 * @tparam     T     a generic type deduced from the passed-in object.
 * @tparam     Rest  a generic parameter pack.
 */
template <typename T, typename ...Rest>
void printToStream(
    std::ostream& out, 
    const T& arg, 
    Rest&&... rest
) {
    out << arg;
    printToStream(out, std::forward<Rest>(rest)...);
}

/**
 * @brief      Concatenates the given arguments into a string.
 *
 * @param      args  The arguments which to concatenate into a string.
 *
 * @tparam     Args  a generic parameter pack.
 */
template <typename ...Args>
std::string formatAsString(Args&&... args) {
    std::ostringstream ss;
    printToStream(ss, std::forward<Args>(args)...);
    return ss.str();
}

/**
 * @brief      Converts the given codepoint to hexadecimal form.
 *
 * @param[in]  codePoint  The code point.
 *
 * @return     A hexadecimal rendering of the given code point.
 */
std::string convertCodepointToHex(uint32_t codePoint) {
    return formatAsString("0x", std::hex, codePoint);
}
