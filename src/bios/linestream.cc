/*****************************************************************************
* Copyright (C) 2002,  F. Hoffmann-La Roche & Co., AG, Basel, Switzerland.   *
*                                                                            *
* This file is part of "Roche Bioinformatics Software Objects and Services"  *
*                                                                            *
* This file is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                 *
* License as published by the Free Software Foundation; either               *
* version 2.1 of the License, or (at your option) any later version.         *
*                                                                            *
* This file is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
* Lesser General Public License for more details.                            *
*                                                                            *
* To obtain a copy of the GNU Lesser General Public License                  *
* please write to the Free Software                                          *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *
* or visit the WWW site http://www.gnu.org/copyleft/lesser.txt               *
*                                                                            *
* SCOPE: this licence applies to this file. Other files of the               *
*        "Roche Bioinformatics Software Objects and Services" may be         *
*        subject to other licences.                                          *
*                                                                            *
* CONTACT: clemens.broger@roche.com or detlef.wolf@roche.com                 *
*                                                                            *
*****************************************************************************/

/// @file bed.cc
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// Transparent way to read lines from a file, pipe, or buffer.

#include "linestream.hh"

namespace bios {

LineStream::LineStream()
    : count_(0),
      status_(0),
      buffer_(NULL),
      buffer_back_(0) {
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

char* LineStream::GetLine() { 
  char *line ;
  if (buffer_ != NULL) {
    if (buffer_back_ == true) {
      buffer_back_ = false;
      line = (char*) buffer_->c_str();
    } else {
      /* only get the next line if there we did not yet see the
         end of file */
      if (buffer_->empty() == false) {
        line = GetNextLine();
      } else {
        line = NULL;
      }
      if (line != NULL) {
        *buffer_ = line;
      } else {
        buffer_->clear();
      }
    }
  } else {
    line = GetNextLine();
  }
  return line;
}

void LineStream::Back(int line_count) {
  if (buffer_ == NULL) {
    std::cerr << "ls_back() without preceeding ls_bufferSet()" << std::endl;
    return;
  }
  if (buffer_back_ > 0) {
    std::cerr << "ls_back() twice in a row" << std::endl;
    return;
  }
  if (line_count != 1) {
    std::cerr << "ls_back: sorry, not yet implemented" << std::endl;
    return;
  }
  buffer_back_ = 1 ;
}

void LineStream::SetBuffer(int line_count) { 
  if (buffer_ != NULL || count_ != 0) {
    std::cerr << "ls_bufferSet() more than once or too late" << std::endl;
    return;
  }
  if (line_count != 1) {
    std::cerr << "ls_bufferSet() sorry, not yet implemented" << std::endl;
    return;
  }
  buffer_ = new std::string;
  buffer_back_ = 0;
}

int LineStream::GetLineCount() {
  return count_;
}

void LineStream::Cat(const char* filename)  { 
  if (filename != NULL) {
    char* line;
    FILE* fp;
    if (strcmp(filename, "-") == 0) {
      fp = stdout;
    } else {
      fp = fopen(filename, "w");
    }
    if (fp == NULL) {
      return;
    }
    while ((line = GetNextLine()) != NULL) {
      fputs(line, fp) ;
      putc('\n', fp) ;
    }
    if (fp != stdout) {
      fclose(fp);
    }
  } else {
    while (GetNextLine())
      ;
  }
}

FileLineStream::FileLineStream(const char* filename)
    : LineStream(),
      fp_(NULL),
      line_(NULL) {
  if (filename == NULL) {
    return;
  }
  if (strcmp(filename, "-") == 0) {
    fp_ = stdin;
  } else {
    fp_ = fopen(filename, "r");
  }
}

FileLineStream::~FileLineStream() {
  if (fp_ != NULL) {
    if (!isatty(fileno(fp_))) {
      char line[1024];
      while (fgets(line, sizeof(line), fp_))
        ;
    }
    fclose(fp_);
  }
  if (line_ != NULL) {
    free(line_);
  }
}

char* FileLineStream::GetNextLine() { 
  int ll = getLine(fp_, &line_, &line_len_);
  if (ll == 0) {
    fclose(fp_);
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

bool FileLineStream::IsEof() {
  return fp_ == NULL;
}

int FileLineStream::SkipGetStatus() {
  if (fp_ != NULL) {
    fclose(fp_);
    fp_ = NULL;
    free(line_);
  }
  return status_;
}

PipeLineStream::PipeLineStream(const char* command) 
    : LineStream(),
      fp_(NULL),
      line_(NULL) { 
  status_ = -2;
  fp_ = popen(command, "r");
  if (fp_ == NULL) {
    return;
  }
}

PipeLineStream::~PipeLineStream() {
  if (fp_ != NULL) {
    char line[1024];
    while (fgets(line, sizeof(line), fp_))
      ;
    pclose(fp_);
  }
  if (line_ != NULL) {
    free(line_);
  }
}

char* PipeLineStream::GetNextLine() {
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

int PipeLineStream::SkipGetStatus() {
  if (fp_ != NULL) {
    while (GetNextLine())
      ;
  }
  return status_;
}

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

int BufferLineStream::SkipGetStatus() {
  if (word_iter_ != NULL) {
    delete word_iter_;
    word_iter_ = NULL;
  }
  return status_;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
