// This file is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// To obtain a copy of the GNU Lesser General Public License,
// please write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// or visit the WWW site http://www.gnu.org/copyleft/lesser.txt

/// @file linestream.cc
/// @author David Z. Chen <d.zhekai.chen@gmail.com>
/// @version 1.0.0
/// @since 12 Aug 2013
///
/// @section DESCRIPTION
///
/// Transparent way to read lines from a file, pipe, or buffer. Rewritten from
/// the ground up while preserving most of the same interface from the libbios
/// linestream module.

#include "linestream.hh"

namespace bios {

//-----------------------------------------------------------------------------
// LineStream methods
//-----------------------------------------------------------------------------

LineStream::LineStream()
    : count_(0),
      status_(0),
      buffer_size_(0) {
  buffer_ = std::deque<std::string>();
}

LineStream::~LineStream() {
}

bool LineStream::GetNextLine(std::string& line) {
  return false;
}

bool LineStream::IsEof() const {
  return false;
}

bool LineStream::GetLine(std::string& line) {
  if (buffer_size_ > 0 && buffer_.size() > 0) {
    line = buffer_.front();
    buffer_.pop_front();
    return true;
  }
  return GetNextLine(line);
}

void LineStream::Back(std::string& str) {
  if (buffer_.size() >= buffer_size_) {
    // Throw exception;
    return;
  }
  buffer_.push_front(str);
}

void LineStream::SetBuffer(int line_count) {
  buffer_size_ = line_count;
}

int LineStream::GetLineCount() {
  return count_;
}

//-----------------------------------------------------------------------------
// FileLineStream methods
//-----------------------------------------------------------------------------

struct noop {
  void operator()(...) const { }
};

FileLineStream::FileLineStream(const char* filename)
    : LineStream(),
      stream_(NULL),
      file_(NULL) {
  if (filename == NULL) {
    return;
  }
  if (strcmp(filename, "-") == 0) {
    stream_ = &std::cin;
  } else {
    file_ = new std::ifstream(filename);
    stream_ = file_;
  }
}

FileLineStream::~FileLineStream() {
  if (file_->is_open()) {
    file_->close();
    delete file_;
  }
}

bool FileLineStream::GetNextLine(std::string& line) {
  if (stream_->eof()) {
    return false;
  }
  std::getline(*stream_, line);
  if (line.size() > 1 && line[line.size() - 2] == '\r') {
    line[line.size() - 2] = '\0';
  } else if (line.size() > 0 && line[line.size() - 1] == '\n') {
    line[line.size() - 1] = '\0';
  }
  ++count_;
  return true;
}

bool FileLineStream::IsEof() const {
  return stream_->eof();
}

//-----------------------------------------------------------------------------
// pipe_streambuf methods
//
// Adapted from code provided by ihuk for the thread:
// http://stackoverflow.com/questions/1683051/file-and-istream-connect-the-two
//-----------------------------------------------------------------------------

pipe_streambuf::pipe_streambuf()
    : fp_(NULL) {
}

pipe_streambuf::~pipe_streambuf() {
  close();
}

pipe_streambuf* pipe_streambuf::open(const char* command, const char* mode) {
  fp_ = popen(command, mode);
  if (fp_ == NULL) {
    return NULL;
  }
  buffer_ = new char_type[kBufferSize];
  if (buffer_ == NULL) {
    return NULL;
  }
  setg(buffer_, buffer_, buffer_);
  return this;
}

void pipe_streambuf::close() {
  if (fp_ != NULL) {
    pclose(fp_);
    fp_ = NULL;
  }
}

std::streamsize pipe_streambuf::xsgetn(char_type* s, std::streamsize n) {
  std::streamsize got = showmanyc();
  if (n <= got) {
    memcpy(s, gptr(), n * sizeof(char_type));
    gbump(n);
    return n;
  }
  memcpy(s, gptr(), got * sizeof(char_type));
  gbump(got);
  if (traits_type::eof() == underflow()) {
    return got;
  }
  return got + xsgetn(s + got, n - got);
}

pipe_streambuf::int_type pipe_streambuf::underflow() {
  if (gptr() == 0) {
    return traits_type::eof();
  }
  if (gptr() < egptr()) {
    return traits_type::to_int_type(*gptr());
  }
  size_t len = fread(eback(), sizeof(char_type), kBufferSize, fp_);
  setg(eback(), eback(), eback() + sizeof(char_type) * len);
  if (len == 0) {
    return traits_type::eof();
  }
  return traits_type::to_int_type(*gptr());
}

std::streamsize pipe_streambuf::showmanyc() {
  if (gptr() == 0) {
    return 0;
  }
  if (gptr() < egptr()) {
    return egptr() - gptr();
  }
  return 0;
}

//-----------------------------------------------------------------------------
// PipeLineStream methods
//-----------------------------------------------------------------------------

PipeLineStream::PipeLineStream(const char* command) : LineStream() {
  if (command == NULL) {
    return;
  }
  pipe_ = new pipe_streambuf;
  pipe_->open(command, "r");
  stream_ = new std::istream(pipe_);
}

PipeLineStream::~PipeLineStream() {
  if (pipe_ != NULL) {
    pipe_->close();
    delete pipe_;
    pipe_ = NULL;
  }
  if (stream_ != NULL) {
    delete stream_;
    stream_ = NULL;
  }
}

bool PipeLineStream::GetNextLine(std::string& line) {
  if (stream_->eof()) {
    return false;
  }
  std::getline(*stream_, line);
  if (line.size() > 1 && line[line.size() - 2] == '\r') {
    line[line.size() - 2] = '\0';
  } else if (line.size() > 0 && line[line.size() - 1] == '\n') {
    line[line.size() - 1] = '\0';
  }
  ++count_;
  return true;
}

bool PipeLineStream::IsEof() const {
  return stream_->eof();
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
