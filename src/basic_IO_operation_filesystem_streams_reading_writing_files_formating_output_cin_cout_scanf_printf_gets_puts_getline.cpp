// Demos for docs/basic_IO_operation.md.
//
// Robotics framing: most file examples use a small CSV-style telemetry log
// (timestamp, joint position) written to /tmp, plus a config-style key/value
// parse via stringstream. Where the doc shows std::cin, we drive an
// std::istringstream instead so the binary runs non-interactively.

#include <bitset>
#include <complex>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>

namespace {

// Path helpers so examples don't fight over the same file.
const std::filesystem::path kTmpDir = std::filesystem::temp_directory_path();
const auto kTelemetryIn = kTmpDir / "joint_positions_in.csv";
const auto kTelemetryOut = kTmpDir / "joint_positions_out.csv";
const auto kLogFile = kTmpDir / "robot_log.txt";

// Snapshots cout state and restores it on destruction so formatting demos
// don't leak (precision, fill, flags) into later sections.
class CoutGuard {
public:
  CoutGuard() : flags_(std::cout.flags()), fill_(std::cout.fill()),
                precision_(std::cout.precision()) {}
  ~CoutGuard() {
    std::cout.flags(flags_);
    std::cout.fill(fill_);
    std::cout.precision(precision_);
  }

private:
  std::ios::fmtflags flags_;
  char fill_;
  std::streamsize precision_;
};

void section(const char *title) {
  std::cout << "\n===== " << title << " =====\n";
}

// Seeds the input CSV we read in later examples.
void seedTelemetryFile() {
  std::ofstream f(kTelemetryIn);
  f << 0.0123f << '\n' << 0.0456f << '\n' << 0.0789f << '\n' << 0.1011f << '\n';
}

} // namespace

// ====================================================================
// Reading & Writing Files
// ====================================================================
//
// Mode flags are bitsets and can be OR'd: ios::out | ios::app, etc.
// Common ones: app, ate, binary, in, out, trunc.
void readingWritingFilesExample() {
  section("Reading & writing files (joint position telemetry)");

  // Simple reader/writer: copy floats from telemetry input to a formatted
  // output log.
  {
    std::ifstream inputfile(kTelemetryIn);
    std::ofstream outputfile(kTelemetryOut);
    float f;
    while (inputfile >> f) { // detects end-of-file and exits loop
      outputfile << "joint_pos = " << f << '\n';
    }
  }

  // Simple writing: append a status line plus a bitset and a complex sample.
  {
    std::ofstream myfile;
    myfile.open(kLogFile, std::ofstream::out | std::ofstream::app);
    myfile << "Writing this to a file.\n";
    myfile << std::bitset<8>(14) << '\n';           // 00001110
    myfile << std::complex<double>(2, 3) << '\n';   // (2,3)
  }

  // Advanced writing with seekp. The doc opens in `in|out`, but for that to
  // succeed the file must already exist (the runtime won't create it). We
  // pad it to a known size first, then seek around inside.
  {
    {
      std::ofstream pad(kLogFile, std::ios::out | std::ios::trunc);
      pad << std::string(64, '.'); // 64 dots so seeks are safe
    }
    std::ofstream myfile(kLogFile, std::ofstream::in | std::ofstream::out);
    myfile.seekp(10, std::ios::beg);
    myfile << "BEGIN";
    myfile.seekp(-5, std::ios::end);
    myfile << "ENDOK";
  }

  // Read first int from the log (will fail because the file now starts with
  // dots — the doc deliberately uses this to show success/failure paths).
  {
    std::ifstream myfile(kLogFile);
    int i;
    myfile >> i;
    std::cout << (myfile.good() ? "Reading succeeded\n" : "Reading failed\n");
  }

  // Read whole file at once via rdbuf into a stringstream.
  {
    std::ifstream in(kTelemetryIn);
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string contents(buffer.str());
    std::cout << "Full file:\n" << contents;
  }

  // Read line by line.
  {
    std::string line;
    std::ifstream myfile(kTelemetryIn);
    if (myfile.is_open()) {
      while (std::getline(myfile, line)) {
        std::cout << "..." << line << "...\n";
      }
    } else {
      std::cout << "Unable to open file\n";
    }
  }
}

// ====================================================================
// stringstream
// ====================================================================
void stringstreamExample() {
  section("stringstream");

  std::stringstream ss;
  std::string name = "behnam";
  int age = 33;
  ss << "name: " << name << " age: " << age;

  std::cout << "via .str(): " << ss.str() << '\n';

  // Extraction via >> reads until whitespace.
  std::cout << "via >>:\n";
  std::stringstream nums("23 4 5.0");
  std::string word;
  while (nums >> word) {
    std::cout << "  " << word << '\n';
  }

  // Word frequency. Useful for, say, counting log severity tags.
  std::cout << "--- word frequency ---\n";
  std::stringstream wordsFrequencyStream("a b bb c a dd d");
  std::map<std::string, int> wordsFrequency;
  while (wordsFrequencyStream >> word) {
    wordsFrequency[word]++;
  }
  for (auto const &kv : wordsFrequency) {
    std::cout << "  " << kv.first << ":" << kv.second << '\n';
  }

  // Hex/decimal round-trip.
  std::cout << "--- hex/decimal ---\n";
  std::stringstream hexDecimalStream;
  hexDecimalStream << std::hex << 12;
  std::cout << "0x" << hexDecimalStream.str() << '\n';

  unsigned int x;
  hexDecimalStream >> std::hex >> x;
  std::cout << x << '\n';

  std::cout << "0x" << std::setbase(16) << 12 << std::dec << '\n';
}

// ====================================================================
// streambuf — redirecting cout to a file, and a portable cin re-attach
// ====================================================================
//
// The doc reads from std::cin via a wrapped istream. We mirror the API but
// feed an istringstream so the binary doesn't block on stdin.
void streambufExample() {
  section("streambuf redirection");

  // Backup the original cout buffer so we can restore it.
  std::streambuf *stream_buffer_cout = std::cout.rdbuf();

  // --- Wrap an input stream's streambuf, same idiom as std::cin would use.
  // std::cin works the same way; just pass std::cin.rdbuf() instead.
  std::istringstream input("42");
  std::streambuf *inbuf = input.rdbuf();
  std::istream my_cin(inbuf);
  int x = 0;
  my_cin >> x;

  // --- Wrap cout's streambuf into a second ostream. Both still flush to the
  // same terminal.
  std::ostream my_cout(stream_buffer_cout);
  my_cout << "wrapped cout, x=" << x << '\n';

  // --- Redirect cout to a file, then put it back.
  {
    std::fstream file(kLogFile, std::ios::out | std::ios::trunc);
    std::streambuf *stream_buffer_file = file.rdbuf();

    std::cout.rdbuf(stream_buffer_file);
    std::cout << "This line is written to the file\n";

    std::cout.rdbuf(stream_buffer_cout); // restore before file closes
  }
  std::cout << "This line is back on screen\n";
}

// ====================================================================
// flush — buffered cout vs. a progress dot
// ====================================================================
//
// Doc shows an infinite progress-dot loop. We bound it so the demo finishes.
void flushExample() {
  section("flush (bounded progress dots)");

  for (int step = 0; step < 5; ++step) {
    // Pretend to do expensive work; volatile keeps the loop from being
    // optimized away.
    volatile std::size_t j = 0;
    for (std::size_t i = 0; i < 1'000'000; i++) {
      j = 2 * i;
    }
    std::cout << '.';
    std::flush(std::cout); // without this, dots may not appear in real-time
  }
  std::cout << " done\n";
}

// ====================================================================
// Manipulators
// ====================================================================
//
// std::ws skips leading whitespace on the *next* read. Crucial when mixing
// >> and std::getline.
void wsExample() {
  section("std::ws");

  int age = 0;
  std::string fullname;

  // Without std::ws: the newline left by `>> age` ends up as the entire
  // getline result on some inputs.
  std::stringstream ss1("     47 \n \n mumbo jumbo");
  if (ss1 >> age && std::getline(ss1, fullname)) {
    std::cout << "without ws: age=" << age << " fullname='" << fullname
              << "'\n";
  }

  // With std::ws: whitespace (including \n) is consumed first.
  std::stringstream ss2("     47 \n \n mumbo jumbo");
  if (ss2 >> age && std::getline(ss2 >> std::ws, fullname)) {
    std::cout << "with ws:    age=" << age << " fullname='" << fullname
              << "'\n";
  }
}

void setwExample() {
  section("setw");
  CoutGuard g;
  std::cout << std::setw(10) << 77 << std::setw(8) << 15 << '\n';
}

void setfillExample() {
  section("setfill");
  CoutGuard g;
  std::cout << std::setfill('x') << std::setw(10) << 77 << std::setw(10) << 12
            << '\n';
}

void leftRightExample() {
  section("left / right");
  CoutGuard g;
  std::cout << std::left << std::setw(15) << "John Smith" << std::right
            << std::setw(7) << 23 << '\n';
  std::cout << std::left << std::setw(15) << "Sam Brown" << std::right
            << std::setw(7) << 8 << '\n';
}

void setprecisionExample() {
  section("setprecision");
  CoutGuard g;
  double number = 3.1914534559;

  std::cout << "default precision: " << std::cout.precision() << '\n';
  std::cout << number << '\n';
  std::cout << "setprecision(1):   " << std::setprecision(1) << number << '\n';
  std::cout << "fixed, prec 7:     " << std::fixed << std::setprecision(7)
            << number << '\n';
}

void hexDecOctSetBaseShowBase() {
  section("setbase / hex / dec / showbase");
  CoutGuard g;
  std::cout << std::setbase(16) << 110 << '\n';
  std::cout << std::hex << 110 << '\n';
  std::cout << std::hex << std::showbase << 110 << '\n';
  std::cout << std::dec << 0xc1 << '\n';
  std::cout << std::setbase(10) << 0xc1 << '\n';
}

// ====================================================================
// cin / cout examples — fed from stringstreams so we don't block.
// std::cin works the same way for every >> / getline / get / ignore /
// peek / putback call below.
// ====================================================================

void cinGetlineExample() {
  section("getline");

  std::stringstream ss("this is a stringstream");
  std::string token;
  char delim = ' ';
  while (std::getline(ss, token, delim)) {
    std::cout << token << '\n';
  }

  // std::cin works the same way; we use an istringstream so the demo runs.
  std::istringstream fake_cin("Wall-E Robot\n");
  std::string name;
  std::getline(fake_cin, name);
  std::cout << "Hello, " << name << "!\n";
}

void cinExtractOperatorExample() {
  section("cin extract operator >>");

  // std::cin works the same way; an istringstream lets the demo run
  // non-interactively. The doc warns: `>>` leaves the trailing '\n' in the
  // buffer, so a subsequent unguarded getline would read an empty line.
  std::istringstream fake_cin("pizza with olives\n42\nmaintenance\n");

  int n1;
  std::string favouriteFood, key;

  std::getline(fake_cin >> std::ws, favouriteFood);
  std::cout << "food: " << favouriteFood << '\n';

  fake_cin >> n1;
  std::cout << "n1:   " << n1 << '\n';

  std::getline(fake_cin >> std::ws, key);
  std::cout << "key:  " << key << '\n';
}

void cinIgnoreExample() {
  section("cin ignore");

  // std::cin works the same way. ignore(N, delim) discards up to N chars or
  // until it hits the delimiter (whichever comes first).
  std::istringstream fake_cin("garbage to skip\nreal payload\n");
  fake_cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  std::string line;
  std::getline(fake_cin, line);
  std::cout << "after ignore: '" << line << "'\n";

  // In C: while ((getchar()) != '\n');
  // ignore(5): discards 5 chars.
  // ignore(5, delim): discards up to 5 chars or until delim.
}

void cinPeekExample() {
  section("cin peek");

  // peek() looks at the next char without consuming it.
  // std::cin.peek() works the same way.
  std::istringstream fake_cin("Hello");
  char next = static_cast<char>(fake_cin.peek());
  std::cout << "next char (not consumed): " << next << '\n';

  std::string rest;
  fake_cin >> rest;
  std::cout << "stream still has: " << rest << '\n';
}

void cinPutbackExample() {
  section("cin putback");

  // putback() pushes a character back onto the input stream. Portable use:
  // putback the same char you just got (or call unget()), so the streambuf
  // is guaranteed to have room.
  // std::cin.putback() works the same way.
  std::istringstream fake_cin("Hello");
  char first = 0;
  fake_cin.get(first);          // consume 'H'
  fake_cin.putback(first);      // push it back
  std::string s;
  fake_cin >> s;
  std::cout << "after putback('" << first << "'): " << s << '\n';
}

void cinGetExample() {
  section("cin get");

  // get(buf, N) reads a c-string up to N-1 chars or until '\n'.
  // std::cin.get() works the same way.
  std::istringstream fake_cin("calibration.yaml\n");
  char str[256];
  fake_cin.get(str, 256);
  std::cout << "filename: " << str << '\n';

  // Stream a file char by char (just write something we can read back).
  std::ofstream tmp(kTelemetryOut);
  tmp << "abc";
  tmp.close();

  std::ifstream is(kTelemetryOut);
  char c;
  std::cout << "file chars: ";
  while (is.get(c)) {
    std::cout << c;
  }
  std::cout << '\n';
}

void coutPutExample() {
  section("cout put");
  std::cout.put(65); // 'A'
  std::cout << '\n';
}

void coutWriteExample() {
  section("cout write");
  std::string message("message for write");
  std::cout.write(message.c_str(), message.size());
  std::cout << '\n';
}

// ====================================================================
// cout.setf — flag-based formatting (snapshotted/restored)
// ====================================================================
void coutFormatting() {
  section("cout.setf formatting");
  CoutGuard g;

  std::cout.setf(std::ios::oct, std::ios::basefield);
  std::cout << 34 << '\n'; // 42 in octal
  std::cout.setf(std::ios::dec, std::ios::basefield);
  std::cout << 34 << '\n';
  std::cout.width(10);

  std::cout.setf(std::ios::scientific, std::ios::floatfield);
  std::cout << 3.14 << '\n'; // 3.140000e+00
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  std::cout << 3.14 << '\n'; // 3.140000
  std::cout.precision(1);
  std::cout << 3.14 << '\n'; // 3.1
  std::cout << "flags=" << std::cout.flags() << '\n';

  std::cout.width(6);
  std::cout.fill('*');
  std::cout.unsetf(std::ios::showbase);
}

// ====================================================================
// std::format (C++20)
// ====================================================================
void formatExample() {
  section("std::format (C++20)");
  std::cout << std::format("{} {}!\n", "Hello", "world");
  std::cout << std::format("joint[{}] = {:.3f} rad\n", 2, 1.5708);
}

// ====================================================================
// Fast IO
// ====================================================================
void fastIO() {
  section("fast IO toggle");
  // Turn off the C/C++ stdio sync and untie cin from cout. Useful for
  // competitive-programming-style large I/O. Must be called before any I/O.
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout << "sync_with_stdio(false) and cin.tie(nullptr) set\n";
  std::cout << "Prefer cout << \"\\n\" over std::endl when you don't need a "
               "flush.\n";
}

// ====================================================================
// printf / scanf — formatted I/O via C stdio
// ====================================================================
void printfScanfExample() {
  section("printf / scanf");
  // After sync_with_stdio(false), cout and stdout no longer share a buffer,
  // so interleaved output can appear out of order. Flush both sides.
  std::cout.flush();

  std::printf("printf: joint=%d angle=%.3f rad\n", 3, 0.7854);

  // scanf works on stdin in the same way; we demonstrate sscanf so we don't
  // block waiting for input.
  const char *line = "5 1.2345";
  int joint = 0;
  double angle = 0.0;
  std::sscanf(line, "%d %lf", &joint, &angle);
  std::printf("sscanf parsed: joint=%d angle=%.4f\n", joint, angle);
  std::fflush(stdout);
}

// ====================================================================
// Filesystem library
// ====================================================================
void filesystemExample() {
  section("filesystem");

  const auto sourceFile = kTelemetryIn; // already populated
  if (std::filesystem::exists(sourceFile)) {
    const auto fileSize = std::filesystem::file_size(sourceFile);
    std::cout << sourceFile << " exists, size=" << fileSize << " bytes\n";

    auto destDir = kTmpDir / "basic_io_demo";
    std::filesystem::create_directory(destDir);

    auto destFile = destDir / "joint_positions_copy.csv";
    std::filesystem::copy_file(sourceFile, destFile,
                               std::filesystem::copy_options::overwrite_existing);
    std::cout << "copied to " << destFile << '\n';

    auto space = std::filesystem::space(kTmpDir);
    std::cout << "available on " << kTmpDir << ": " << space.available
              << " bytes\n";
  } else {
    std::cout << sourceFile << " does not exist\n";
  }
}

int main() {
  seedTelemetryFile();

  readingWritingFilesExample();
  stringstreamExample();
  streambufExample();
  flushExample();

  wsExample();
  setwExample();
  setfillExample();
  leftRightExample();
  setprecisionExample();
  hexDecOctSetBaseShowBase();

  cinGetlineExample();
  cinExtractOperatorExample();
  cinIgnoreExample();
  cinPeekExample();
  cinPutbackExample();
  cinGetExample();
  coutPutExample();
  coutWriteExample();

  coutFormatting();
  formatExample();
  fastIO();
  printfScanfExample();
  filesystemExample();

  return 0;
}
