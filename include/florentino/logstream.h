
#ifndef FLORENTINO_LOGSTREAM_H
#define FLORENTINO_LOGSTREAM_H

#include <iostream>

namespace florentino {

class logbuf : public std::streambuf {
public:
  logbuf() { }

protected:
  virtual int overflow(int c) { return c; }
};

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
