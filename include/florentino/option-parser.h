
#ifndef FLORENTINO_OPTION_PARSER_H
#define FLORENTINO_OPTION_PARSER_H

#include <map>
#include <string>

#include <cassert>
#include <cstddef>

namespace florentino {

class Option {
public:
  enum Value {
    NO_ARGUMENT,
    REQUIRED_ARGUMENT,
    OPTIONAL_ARGUMENT
  };

  typedef void (*Handler)(void *, const char *);

public:
  Option() : _opt('\0'), _value(NO_ARGUMENT),
             _handler(0), _arg(0),
             _help(""), _desc("") { }

  Option(char opt, Option::Value value,
         Option::Handler handler, void *arg,
         const char *help, const char *desc) : _opt(opt), _value(value),
                                               _handler(handler), _arg(arg),
                                               _help(help), _desc(desc) { }

public:
  void process(const char *optArg) { _handler(_arg, optArg); }

public:
  char option() const { return _opt; }
  Value value() const { return _value; }

  const char *help() const { return _help; }
  const char *description() const { return _desc; }

private:
  char _opt;
  Value _value;

  Handler _handler;
  void *_arg;

  const char *_help;
  const char *_desc;
};

class OptionParser {
public:
  static const bool DEFAULT_HELP = false;

public:
  class iterator {
  public:
    typedef const Option value_type;
    typedef ptrdiff_t difference_type;

    typedef value_type *pointer;
    typedef value_type &reference;

    typedef std::forward_iterator_tag iterator_category;

  public:
    iterator() { }
    iterator(const iterator &that) : _cur(that._cur) { }

    const iterator &operator=(const iterator &that) {
      if(this != &that)
        _cur = that._cur;

      return *this;
    }

  public:
    bool operator==(const iterator &that) const { return _cur == that._cur; }
    bool operator!=(const iterator &that) const { return _cur != that._cur; }

  public:
    reference operator*() const { return _cur->second; }
    pointer operator->() const { return &_cur->second; }

  public:
    iterator &operator++() {
      ++_cur;
      return *this;
    }

    iterator operator++(int ign) {
      iterator prev = *this;
      ++*this;
      return *this;
    }

  private:
    iterator(std::map<char, Option>::const_iterator i) : _cur(i) { }

  private:
    std::map<char, Option>::const_iterator _cur;

    friend class OptionParser;
  };

public:
  iterator begin() const { return iterator(_options.begin()); }
  iterator end() const { return iterator(_options.end()); }

public:
  OptionParser(int argc, char **argv);

private:
  OptionParser(const OptionParser &that); // Do not implement.
  const OptionParser &operator=(const OptionParser &that); // Do not implement.

public:
  void add(const Option &opt) {
    assert(!_options.count(opt.option()) && "option already registered");

    _options.insert(std::make_pair(opt.option(), opt));
  }

  void parse();

public:
  int argc() const {
    return _argc;
  }

  char *argv(unsigned i) const {
    assert(i < _argc && "invalid argv index");
    return _argv[i];
  }

  bool empty() const { return _options.empty(); }
  size_t size() const { return _options.size(); }

private:
  std::string buildOptString() const;
  void usage() const;

private:
  int _argc;
  char **_argv;

  std::map<char, Option> _options;
};

} // End namespace florentino.

#endif // FLORENTINO_OPTION_PARSER_H
