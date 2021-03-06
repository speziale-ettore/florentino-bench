

#ifndef FLORENTINO_CLOCK_H
#define FLORENTINO_CLOCK_H

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <cassert>

namespace florentino {

// A statistic about time.
class TimeStat {
public:
  // Record the time an event occurs, in nanoseconds.
  class Tick {
  public:
    Tick(unsigned long long val = 0) : _val(val) { }

  public:
    operator double() const { return _val * 1e-9; }

  private:
    unsigned long long _val;

    friend Tick operator-(const Tick &a, const Tick &b) {
      return a._val - b._val;
    }

    friend std::ostream &operator<<(std::ostream &os, const Tick &tick) {
      return os << (tick._val * 1e-9);
    }
  };

  typedef std::vector<Tick>::const_iterator iterator;

public:
  iterator begin() const { return _values.begin(); }
  iterator end() const { return _values.end(); }

public:
  TimeStat() : _valid(false) { }

  TimeStat(const std::string &desc) : _valid(true),
                                      _desc(desc) { }

public:
  const Tick &operator[](int i) const {
    return _values[i];
  }

public:
  bool valid() const {
    return _valid;
  }

  const std::string &description() const {
    return _desc;
  }

  size_t size() const {
    return _values.size();
  }

  double avg() const {
    return std::accumulate(_values.begin(), _values.end(), 0.0) /
           _values.size();
  }

protected:
  bool _valid;

  std::string _desc;
  std::vector<Tick> _values;

  friend TimeStat operator-(const TimeStat &a, const TimeStat &b) {
    assert(a.valid() && b.valid() && "invalid stats");

    TimeStat c("CIAO");
    std::vector<Tick> &values = c._values;

    for(iterator i = a.begin(), e = a.end(), j = b.begin(), f = b.end();
                 i != e && j != f;
                 ++i, ++j)
      values.push_back(*i - *j);

    return c;
  }
};

// A Clock is simple a TimeStat that exposes a member function to read the
// current time.
class Clock : public TimeStat {
public:
  Clock() { }

  Clock(const std::string &desc) : TimeStat(desc) { }

public:
  void record() {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    _values.push_back(ts.tv_sec * static_cast<unsigned long long>(1e9) +
                      ts.tv_nsec);
  }
};

// A collection of clocks. Collection can be iterated and provides accessors
// member functions to record current time on a specific clock.
class Clocks {
public:
  typedef std::vector<Clock>::const_iterator iterator;

public:
  iterator begin() const { return _clocks.begin(); }
  iterator end() const { return _clocks.end(); }

public:
  void reserve(unsigned id, const std::string &desc) {
    assert(_clocks.size() <= id || !_clocks[id].valid() && "invalid clock id");

    if(_clocks.size() <= id)
      _clocks.resize(id + 1);

    _clocks[id] = Clock(desc);
  }

  void record(unsigned id) {
    assert(_clocks.size() > id && "invalid clock id");
    _clocks[id].record();
  }

public:
  Clock &operator[](int id) {
    assert(_clocks.size() > id && "invalid clock id");
    return _clocks[id];
  }

  const Clock &operator[](int id) const {
    assert(_clocks.size() > id && "invalid clock id");
    return _clocks[id];
  }

private:
  std::vector<Clock> _clocks;
};

} // End namespace florentino.

#endif // FLORENTINO_CLOCK_H
