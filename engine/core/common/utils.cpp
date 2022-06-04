#include "utils.h"
#include <fstream>
#include <intsafe.h>

void read_binary_file(std::vector<uint8_t> &buffer, const std::string file_name)
{
    // open the file:
    std::ifstream file(file_name, std::ios::binary);

    file.unsetf(std::ios::skipws);
    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer.reserve(fileSize);

    // read the data:
    buffer.insert(buffer.begin(),
        std::istream_iterator<BYTE>(file),
        std::istream_iterator<BYTE>());
}
