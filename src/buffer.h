#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <memory>

class Buffer {
private:
    struct BufferData {
        std::vector<char> data;
        size_t loaded_size = 0;

        explicit BufferData(size_t size)
            : data(size)
        {}
    };

    std::ifstream file;

    std::unique_ptr<BufferData> buffers[2];
    BufferData* current_buf = nullptr;
    BufferData* loading_buf = nullptr;

    char* current_pos = nullptr;

    static constexpr size_t BUFFER_SIZE = 50;

    void LoadBuffer(BufferData* buf);
    void SwitchBuffers();

public:
    explicit Buffer(std::ifstream&& file_);
    ~Buffer() = default;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    bool GetLine(std::string& str);
    std::string GetLine();
    bool IsEnd() const;
};