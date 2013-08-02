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

/** 
 *   \file linestream.c Read lines from a file, pipe or buffer
 *   \author Adapted by Lukas Habegger (lukas.habegger@yale.edu)
 */

/* 
   Module lineStream
   Transparent way to read lines from a file, pipe or buffer
   The lines always come without \n at the end
*/

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

/**
 * Creates a line stream from a file.
 * @param[in] fn File name ("-" means stdin)
 * @return A line stream object, NULL if file could not been opened; to learn 
 *         details call warnReport() from module log.c
 */
LineStream* LineStream::FromFile(const char* filename) {
  return new FileLineStream(filename);
}

/** 
 * Creates a line stream from a pipe.
 * Example: ls_createFromPipe ("zcat test.dat.Z");
 * @param[in] command As it would be written on the command line
 * @return A line stream object, NULL if the pipe could not been opened
 * @post warnCount(NULL,NULL) !=0 if problem occured
 */
LineStream* LineStream::FromPipe(const char* command) {
  return new PipeLineStream(command);
}

/**
 * Creates a line stream from a buffer.
 * @param[in] buffer  A buffer pointer, must not be NULL
 * @return A line stream object
 * @note The buffer will be destroyed after using ls_nextLine (newline 
 *       characters are replaced by null-termination characters). 
   Work on a copy of the buffer if you want to use it again.
 */
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

/**
 * Get the next line from a line stream object.
 * This function is called from the application programs independently whether 
 * the stream is from a file, pipe or buffer
 * @param[in] this1 A line stream 
 * @return A line without trailing newlines if there is still a line, else 
 *         NULL.
 * @note The memory returned belongs to this routine; it may be read and 
 *       written to, but not free'd or realloc'd by the user of this routine; 
 *       it stays stable until the next call ls_nextLine(this1).
  */
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

/**
 * Push back 'lineCnt' lines.
 * @param[in] this1 A line stream 
 * @param[in] lineCnt  How many lines should ls_nextLine() repeat (currently, 
 *            only lineCnt==1 is supported)
 * @pre ls_bufferSet() was called.
 * @post Next call to ls_nextLine() will return the same line again
*/
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

/**
 * Set how many lines the linestream should buffer.
 * @param[in] this1 A line stream 
 * @param[in] lineCnt How many lines should ls_nextLine() repeat (currently, 
 *            only lineCnt==1 is supported)
 * @pre ls_create*
 * @post ls_back() will work
 */
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

/** 
 * Returns the number of the current line.
 * @param[in] this1 A line stream 
*/
int LineStream::GetLineCount() {
  return count_;
}

/**
 * Redirect a linestream.
 * @param[in] this1 A line stream created by one of ls_create*()
 * @param[in] filename  Name of file to write lines to; special cases: '-'  
 *            means stdout, NULL means /dev/null (discard) 
 * @post this1 contains no more lines; file 'filename' contains the contents of 
 *       'this1'
 */
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
    while (line = GetNextLine()) {
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

/** 
 * Returns the next line of a file and closes the file if no further line was 
 * found. The line can be of any length. A trailing \n or \r\n is removed.
 * @param[in] this1 line stream object
 * @return The line, NULL if no further line was found
 * @note Memory managed by this routine
 */
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

/**
 * Skips remainder of line stream and returns exit status which only meaningful 
 * when created from a pipe - exit status for file and buffer will always be 0.
 * @param[in] this1 A line stream 
 * @return If 'this1' was created by ls_createFromPipe(command), the exit 
 *         status of 'command' is returned; else 0 
 * @post line stream is read to its end - ls_nextLine() must not be called 
 *       anymore.
 * @note For reasons of efficiency, skip does not actually read the linestream 
 *       therefore ls_lineCountGet() 
   will not return the correct line number after this function has been called.
 */
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

/* returns the next line from the pipe and closes the pipe if
   no further line was found. The line can be of any length and
   is returned without \n at the end
   input: line stream object
   output: the line
           NULL if no further line was found
*/
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

/**
 * Skips remainder of line stream and returns exit status which only meaningful 
 * when created from a pipe - exit status for file and buffer will always be 0.
 * @param[in] this1 A line stream 
 * @return If 'this1' was created by ls_createFromPipe(command), the exit 
 *         status of 'command' is returned; else 0 
 * @post line stream is read to its end - ls_nextLine() must not be called 
 *       anymore.
 * @note For reasons of efficiency, skip does not actually read the linestream 
 *       therefore ls_lineCountGet() 
   will not return the correct line number after this function has been called.
 */
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

/* 
returns the next line of a buffer. The line can be of any length and
a trailing \n or \r\n is removed.
input: line stream object
output: the line
        NULL if no further line was found
*/
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

/**
 * Skips remainder of line stream and returns exit status which only meaningful 
 * when created from a pipe - exit status for file and buffer will always be 0.
 * @param[in] this1 A line stream 
 * @return If 'this1' was created by ls_createFromPipe(command), the exit 
 *         status of 'command' is returned; else 0 
 * @post line stream is read to its end - ls_nextLine() must not be called 
 *       anymore.
 * @note For reasons of efficiency, skip does not actually read the linestream 
 *       therefore ls_lineCountGet() 
   will not return the correct line number after this function has been called.
 */
int BufferLineStream::SkipGetStatus() {
  if (word_iter_ != NULL) {
    delete word_iter_;
    word_iter_ = NULL;
  }
  return status_;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
