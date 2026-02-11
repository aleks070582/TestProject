#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Buffer {
private:
    struct BufferData {
        std::vector<char> data;
        size_t loaded_size = 0;
        bool ready = false;
        explicit BufferData(size_t size)
            : data(size)
        {}
    };
    static constexpr size_t BUFFER_SIZE = 50;
    std::ifstream file;
    std::unique_ptr<BufferData> buffers[2];
    BufferData* current_buf = nullptr;
    BufferData* next_buf = nullptr;
    char* current_pos = nullptr;
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};
    std::atomic<bool> eof{false};
    void WorkerThread();
    void Load(BufferData* buf);
    void SwitchBuffers();
public:
    explicit Buffer(std::ifstream&& file_);
    ~Buffer();
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    bool GetLine(std::string& str);
    std::string GetLine();
};
