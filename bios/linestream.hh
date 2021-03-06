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

/// @file bed.cc
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// Transparent way to read lines from a file, pipe, or buffer.

#ifndef BIOS_LINESTREAM_H__
#define BIOS_LINESTREAM_H__

#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <memory>
#include <cstring>
#include <unistd.h>

namespace bios {

/// @class LineStream
/// @brief Base class for reading lines from an input source.
class LineStream {
 public:
  LineStream();
  virtual ~LineStream();
  
  /// Get the next line from a line stream object.
  /// This function is called from the application programs independently whether 
  /// the stream is from a file, pipe or buffer
  /// @param[in] this1 A line stream 
  /// @return A line without trailing newlines if there is still a line, else 
  ///         NULL.
  /// @note The memory returned belongs to this routine; it may be read and 
  ///       written to, but not free'd or realloc'd by the user of this routine; 
  ///       it stays stable until the next call ls_nextLine(this1).
  bool GetLine(std::string& line);

  /// Returns the number of the current line.
  /// @param[in] this1 A line stream 
  int GetLineCount();

  /// Set how many lines the linestream should buffer.
  /// @param[in] this1 A line stream 
  /// @param[in] lineCnt How many lines should ls_nextLine() repeat (currently, 
  ///            only lineCnt==1 is supported)
  /// @pre ls_create*
  /// @post ls_back() will work
  void SetBuffer(int line_count);

  /// Push back 'lineCnt' lines.
  /// @param[in] this1 A line stream 
  /// @param[in] lineCnt  How many lines should ls_nextLine() repeat (currently, 
  ///            only lineCnt==1 is supported)
  /// @pre ls_bufferSet() was called.
  /// @post Next call to ls_nextLine() will return the same line again
  void Back(std::string& str);

  /// @brief Returns whether the stream has reached the end of file.
  ///
  /// @return true if end of file is reached, false otherwise.
  virtual bool IsEof() const;

  bool operator!() const {
    return IsEof();
  }

  operator bool() const {
    return IsEof();
  }

 protected: 
  /// Returns the next line of a file and closes the file if no further line was 
  /// found. The line can be of any length. A trailing \n or \r\n is removed.
  /// @param[in] this1 line stream object
  /// @return The line, NULL if no further line was found
  /// @note Memory managed by this routine
  virtual bool GetNextLine(std::string& line);

 protected:
  int count_;
  int status_;
  std::deque<std::string> buffer_;
  unsigned int buffer_size_;
};

/// @class FileLineStream
/// @brief Line stream class for reading lines from a file or standard input.
class FileLineStream : public LineStream {
 public:
  FileLineStream(const char* filename);
  ~FileLineStream();
  
  bool IsEof() const;

 protected:
  bool GetNextLine(std::string& line);

 private:
  std::istream* stream_;
  std::ifstream* file_;
};

// Adapted from code provided by ihuk for the thread:
// http://stackoverflow.com/questions/1683051/file-and-istream-connect-the-two
class pipe_streambuf : public std::streambuf {
 public:
  pipe_streambuf();
  ~pipe_streambuf();

  pipe_streambuf* open(const char* command, const char* mode);
  void close();

 protected:
  std::streamsize xsgetn(char_type* ptr, std::streamsize n);
  int_type underflow();
  std::streamsize showmanyc();
 
 private:
  enum {
    kBufferSize = 1024
  };

  FILE* fp_;
  char_type* buffer_;
};

/// @class PipeLineStream
/// @brief Line stream class for reading lines from the output of a command.
class PipeLineStream : public LineStream {
 public:
  PipeLineStream(const char* pipe);
  ~PipeLineStream();

  bool IsEof() const;

 protected: 
  bool GetNextLine(std::string& line);

 private:
  pipe_streambuf* pipe_;
  std::istream* stream_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_LINESTREAM_H__ */
