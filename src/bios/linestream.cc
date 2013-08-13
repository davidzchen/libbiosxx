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
  if (buffer_ != NULL) {
    delete buffer_;
  }
}

LineStream* LineStream::FromFile(const char* filename) {
  return new FileLineStream(filename);
}

LineStream* LineStream::FromPipe(const char* command) {
  return new PipeLineStream(command);
}

LineStream* LineStream::FromBuffer(char* buffer) {
  return new BufferLineStream(buffer);
}

char* LineStream::GetNextLine() {
  return NULL;
}

bool LineStream::IsEof() {
  return false;
}

int LineStream::SkipGetStatus() {
  return 0;
}

std::string LineStream::GetLine() {
  if (buffer_size_ > 0 && buffer_.size() > 0) {
    std::string line = buffer_.top();
    buffer_.pop();
    return line;
  }
  return GetNextLine();
}

void LineStream::Back(std::string str) {
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

FileLineStream::FileLineStream(const char* filename) : LineStream()
  if (filename == NULL) {
    return;
  }
  if (strcmp(filename, "-") == 0) {
    stream_.reset(&std::cin, noop());
  } else {
    file_ = new std::ifstream(filename);
    stream_.reset(file_);
  }
}

FileLineStream::~FileLineStream() {
  if (file_->is_open()) {
    file_->close();
    delete file_;
  }
}

std::string FileLineStream::GetNextLine() {
  std::string line;
  if (stream_.eof()) {
    return line;
  }
  std::getline(stream_, line);
  if (line.size() > 1 && line[line.size() - 2] == '\r') {
    line[line.size() - 2] = '\0';
  } else if (line.size() > 0 && line[line.size() - 1] == '\n') {
    line[line.size() - 1] = '\0';
  }
  ++count_;
  return line;
}

bool FileLineStream::IsEof() {
  return stream_.eof();
}

//-----------------------------------------------------------------------------
// PipeLineStream methods
//-----------------------------------------------------------------------------

PipeLineStream::PipeLineStream(const char* command) : LineStream() {
  namespace io = boost::iostreams;
  pipe_ = popen(command, "r");
  if (fp_ == NULL) {
    return;
  }
  io::stream_buffer<io::file_descriptor_source> fpstream(fileno(pipe_));
  stream_ = std::istream(&fpstream);
}

PipeLineStream::~PipeLineStream() {
  if (pipe_ != NULL) {
    pclose(pipe_);
    pipe_ = NULL;
  }
}

std::string PipeLineStream::GetNextLine() {
  int ll = getLine(fp_, &line_, &line_len_);
  if (ll == 0) {
    status_ = pclose(fp_);
    fp_ = NULL;
    free(line_);
    return NULL;
  }
  if (ll > 1 && line_[ll - 2] == '\r') {
    line_[ll - 2] = '\0';
  } else if (ll > 0 && line_[ll - 1] == '\n') {
    line_[ll - 1] = '\0';
  }
  ++count_;
  return line_;
}

bool PipeLineStream::IsEof() {
  return fp_ == NULL;
}

//-----------------------------------------------------------------------------
// BufferLinestream methods
//-----------------------------------------------------------------------------

BufferLineStream::BufferLineStream(char* buffer) 
    : LineStream(),
      word_iter_(NULL) { 
  if (buffer == NULL) {
    return;
  }
  bool collapse_seps = true;
  int len = strlen(buffer) ;
  if (len > 0) {
    if (buffer[len - 1] == '\n') {
      buffer[--len] = '\0' ;
    }
  } else {
    collapse_seps = true; /* immediately return NULL */
  }
  word_iter_ = new WordIter(buffer, "\n", collapse_seps);
}

BufferLineStream::~BufferLineStream() {
  if (word_iter_ != NULL) {
    delete word_iter_;
    word_iter_ = NULL;
  }
}

char* BufferLineStream::GetNextLine() {
  int len;
  char* token = word_iter_->Next(&len);
  if (token == NULL) {
    delete word_iter_;
    word_iter_ = NULL;
    return NULL;
  }
  ++count_;
  if (len && token[len - 1] == '\r') {
    token[len-1] = '\0';
  }
  return token;
}

bool BufferLineStream::IsEof() {
  return word_iter_ == NULL;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
