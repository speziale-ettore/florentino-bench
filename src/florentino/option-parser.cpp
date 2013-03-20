
#include "florentino/option-parser.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <cstring>

#include <unistd.h>

using namespace florentino;

namespace {

// The help handler is a little bit different from the other handlers: it does
// not parse the help option, it just build and throw an usage message.
void helpHandler(void *arg, const char *optArg) {
  OptionParser *parser = reinterpret_cast<OptionParser *>(arg);
  std::ostringstream os;

  os << "Usage: " << parser->argv(0);

  if(!parser->empty()) {
    os << " [OPTIONS]";

    size_t maxLen = 0;

    // First we have to computed the first field length.
    for(OptionParser::iterator i = parser->begin(),
                               e = parser->end();
                               i != e;
                               ++i) {
      size_t curLen = std::strlen(i->help());

      if(maxLen < curLen) maxLen = curLen;
    }

    // Now build the aligned message.
    for(OptionParser::iterator i = parser->begin(),
                               e = parser->end();
                               i != e;
                               ++i) {
      os << std::endl << "  " << std::setw(maxLen) << i->help() << ": "
                              << i->description();
    }
  }

  throw std::runtime_error(os.str());
}

} // End anonymous namespace.

OptionParser::OptionParser(int argc, char **argv) : _argc(argc),
                                                    _argv(argv) {
  add(Option('h', Option::NO_ARGUMENT,
             helpHandler, this,
             "-h", "print help message"));
}

void OptionParser::parse() {
  std::string optString = buildOptString();
  int optCode;

  // Reset optind, to be safe with respect to multiple calls.
  optind = 1;

  // Do not print error messages when an unknown options is detected.
  opterr = 0;

  // See loop last statement.
  optarg = 0;

  while((optCode = getopt(_argc, _argv, optString.c_str())) != -1) {
     // Unknonw option: generate usage message and terminate options processing.
     if(optCode == '?') usage();

     // By construction, the entry is there. Invoke the handler. If it does not
     // like the option, it can throw and exception and interrupt option
     // processing here.
     Option &opt = _options[optCode];
     opt.process(optarg);

     // Reset optarg: in that way, when a flag option is parser, 0 is
     // automatically passed to the option handler.
     optarg = 0;
  }

  // Some name argument left on the command line: error!
  if(optind < _argc) usage();
}

std::string OptionParser::buildOptString() const {
  std::string opts;

  for(iterator i = _options.begin(), e = _options.end(); i != e; ++i) {
    opts += i->option();

    switch(i->value()) {
    case Option::REQUIRED_ARGUMENT:
      opts += ":";
      break;

    case Option::OPTIONAL_ARGUMENT:
      opts += "::";
      break;
    }
  }

  return opts;
}

void OptionParser::usage() const {
  // Re-use the help handler, which actually print an usage message.
  helpHandler(const_cast<OptionParser *>(this), 0);
}
