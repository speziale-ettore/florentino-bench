
#ifndef FLORENTINO_LOGSTREAM_H
#define FLORENTINO_LOGSTREAM_H

#include <iostream>

namespace florentino {

// There is always space for emitting something in this buffer, indeed it drops
// every character it receives. Used to silence benchmark messages.
class logbuf : public std::streambuf {
public:
  logbuf() { }

protected:
  virtual int overflow(int c) { return c; }
};

// The stream used to log all benchmark operations. It employs two buffer. When
// run in verbose mode, it employs the standard log buffer. When run in silent
// mode, it used a special buffer that drops every written character. The mode
// can be changed on fly.
class logstream : public std::ostream {
public:
  logstream() : std::ostream(std::clog.rdbuf()),
                _buf(new logbuf()) { }

  virtual ~logstream() { delete _buf; }

public:
  void verbose(bool verb) {
    rdbuf(verb ? std::clog.rdbuf() : _buf);
  }

  bool verbose() const {
    return rdbuf() == std::clog.rdbuf();
  }

private:
  logbuf *_buf;
};

} // End namespace florentino.

#endif // FLORENTINO_LOGSTREAM_H
