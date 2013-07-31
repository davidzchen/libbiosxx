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
 *   \file linestream.h
 *   \author Adapted by Lukas Habegger (lukas.habegger@yale.edu)
 */


#ifndef BIOINFO_LINESTREAM_H__
#define BIOINFO_LINESTREAM_H__

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "log.h"
#include "worditer.h"
#include "misc.h"

class LineStream {
 public:
  LineStream();
  ~LineStream();

  static LineStream* FromFile(const char* filename);
  static LineStream* FromPipe(const char* command);
  static LineStream* FromBuffer(char* buffer);

  char* GetLine();
  int GetLineCount();
  void Cat(const char* filename);
  void SetBuffer(int line_count);
  void Back(int line_count);

  virtual bool IsEof();
  virtual int SkipGetStatus();

 protected:
  virtual char* GetNextLine();

 protected:
  int count_;
  int status_;
  std::string* buffer_;
  int buffer_back_;
};

class FileLineStream : public LineStream {
 public:
  FileLineStream(const char* filename);
  ~FileLineStream();

  bool IsEof();
  int SkipGetStatus();

 protected:
  char* GetNextLine();

 private:
  FILE* fp_;
  char* line_;
  int line_len_;
};

class PipeLineStream : public LineStream {
 public:
  PipeLineStream(const char* pipe);
  ~PipeLineStream();

  bool IsEof();
  int SkipGetStatus();

 protected: 
  char* GetNextLine();

 private:
  FILE* fp_;
  char* line_;
  int line_len_;
};

class BufferLineStream : public LineStream {
 public:
  BufferLineStream(char* buffer);
  ~BufferLineStream();

  bool IsEof();
  int SkipGetStatus();
  
 protected:
  char* GetNextLine();

 private:
  WordIter* word_iter_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_LINESTREAM_H__ */
