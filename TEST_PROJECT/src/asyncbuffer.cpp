#include "asyncbuffer.h"
#include <algorithm>

Buffer::Buffer(std::ifstream&& file_)
    : file(std::move(file_))
{
    buffers[0] = std::make_unique<BufferData>(BUFFER_SIZE);
    buffers[1] = std::make_unique<BufferData>(BUFFER_SIZE);
    current_buf = buffers[0].get();
    next_buf    = buffers[1].get();
    worker = std::thread(&Buffer::WorkerThread, this);
    Load(current_buf);
    current_pos = current_buf->data.data();
    {
        std::lock_guard lock(mtx);
        next_buf->ready = false;
        cv.notify_all();
    }
}
Buffer::~Buffer() {
    stop = true;
    cv.notify_all();
    if (worker.joinable())
        worker.join();
}
void Buffer::Load(BufferData* buf) {
    file.read(buf->data.data(), buf->data.size());
    buf->loaded_size = static_cast<size_t>(file.gcount());
    if (buf->loaded_size == 0)
        eof = true;
    buf->ready = true;
}

void Buffer::WorkerThread() {
    while (!stop) {
        std::unique_lock lock(mtx);
        cv.wait(lock, [&] {
            return stop || (!next_buf->ready && !eof);
        });
        if (stop || eof)
            return;
        BufferData* target = next_buf;
        lock.unlock();
        Load(target);
        lock.lock();
        cv.notify_all();
    }
}

void Buffer::SwitchBuffers() {
    std::unique_lock lock(mtx);
    cv.wait(lock, [&] {
        return next_buf->ready || eof;
    });
    if (!next_buf->ready)
        return;
    current_buf->loaded_size = 0;
    std::swap(current_buf, next_buf);
    current_pos = current_buf->data.data();
    next_buf->ready = false;
    cv.notify_all();
}


bool Buffer::GetLine(std::string& str) {
    str.clear();
    while (true) {
        char* buffer_begin = current_buf->data.data();
        char* buffer_end   = buffer_begin + current_buf->loaded_size;
        if (current_pos >= buffer_end) {
            if (eof)
                return !str.empty();
            SwitchBuffers();
            continue;
        }
        char* newline =std::find(current_pos, buffer_end, '\n');
        if (newline != buffer_end) {
            str.append(current_pos, newline - current_pos);
            current_pos = newline + 1;
            return true;
        }
        str.append(current_pos, buffer_end - current_pos);
        current_pos = buffer_end;
        if (eof)
            return !str.empty();
        SwitchBuffers();
    }
}


std::string Buffer::GetLine() {
    std::string s;
    GetLine(s);
    return s;
}
