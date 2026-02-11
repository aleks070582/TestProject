#include "buffer.h"
#include <algorithm>

Buffer::Buffer(std::ifstream&& file_)
    : file(std::move(file_))
{
    buffers[0] = std::make_unique<BufferData>(BUFFER_SIZE);
    buffers[1] = std::make_unique<BufferData>(BUFFER_SIZE);

    current_buf = buffers[0].get();
    loading_buf = buffers[1].get();

    LoadBuffer(current_buf);
    current_pos = current_buf->data.data();

    LoadBuffer(loading_buf);
}

void Buffer::LoadBuffer(BufferData* buf) {
    file.read(buf->data.data(), buf->data.size());
    buf->loaded_size = static_cast<size_t>(file.gcount());
}

void Buffer::SwitchBuffers() {
    std::swap(current_buf, loading_buf);
    current_pos = current_buf->data.data();

    if (loading_buf->loaded_size != 0) {
        LoadBuffer(loading_buf);
    }
}

bool Buffer::IsEnd() const {
    return current_buf->loaded_size == 0;
}

std::string Buffer::GetLine() {
    std::string result;
    GetLine(result);
    return result;
}

bool Buffer::GetLine(std::string& str) {
    str.clear();

    while (true) {

        if (current_buf->loaded_size == 0) {
            return !str.empty();
        }

        char* buffer_end =
            current_buf->data.data() + current_buf->loaded_size;

        char* newline =
            std::find(current_pos, buffer_end, '\n');

        if (newline != buffer_end) {
            str.append(current_pos, newline - current_pos);
            current_pos = newline + 1;
            return true;
        }

        str.append(current_pos, buffer_end - current_pos);

        SwitchBuffers();
    }
}