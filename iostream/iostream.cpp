#include <cstdio>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <chrono>

using namespace std::chrono_literals; 

struct OStream {
    int fd;
    int mode;
    char *buf;
    size_t bufidx = 0;

    explicit OStream(int _fd, int _mode = _IOLBF, char *_buf = nullptr) : fd(_fd), mode(_mode), buf(_buf) {
        if (buf == nullptr && mode != _IONBF) {
            buf = new char[BUFSIZ];
        }
    }

    /*对应fflush*/
    void flush() {
        std::this_thread::sleep_for(1s);
        ::write(fd, buf, bufidx);
        bufidx = 0;
    }

    void putchar(char c) {
        if (mode == _IONBF) {
            ::write(fd, &c, 1);
            return;
        }
        if (bufidx == BUFSIZ)
            flush();
        buf[bufidx++] = c;
        if (mode == _IOLBF && c == '\n')
            flush();
    }

    void putstr(const std::string &s) {
        if (mode == _IONBF) {
            std::this_thread::sleep_for(1s);
            ::write(fd, s.data(), s.length());
            return;
        }
        for (char c : s) {
            if (bufidx == BUFSIZ)
                flush();
            buf[bufidx++] = c;
            if (mode == _IOLBF && c == '\n')
                flush();
        }
        if (mode == _IONBF) {
            flush();
        }
    }


    void close() {
        flush();
    }

    OStream(OStream &&) = delete;

    ~OStream() {
        close();
        delete[] buf;
    }

    // void putstr(std::string const &s) {
    //     for (char c: s) {
    //         putchar(c);
    //     }
    // }
};

int main() {
    OStream m_out(STDOUT_FILENO, _IOLBF);
    OStream m_err(STDERR_FILENO, _IONBF);
    m_err.putstr("Hello, ");
    m_err.putstr("world\n");
    m_out.putstr("Hello, ");
    m_out.putstr("world\n");
    return 0;
}
