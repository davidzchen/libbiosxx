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

/// @file conf.hh
/// @author David Z. Chen <d.zhekai.chen@gmail.com>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing simple configuration files.

#ifndef BIOS_CONF_H__
#define BIOS_CONF_H__

#include <map>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "linestream.hh"

namespace bios {

/// @class conf
/// @brief Class for parsing simple configuration files.
class Conf {
 public:
  /// @brief Class constructor.
  ///
  /// Constructs a new Conf object.
  Conf(const char* filename);

  /// @brief Class destructor.
  ///
  /// Destroys the Conf object.
  ~Conf();

  /// @brief Returns the string value given the key.
  ///
  /// This method looks up the key and returns the string value.
  ///
  /// @return   The value associated with the key or NULL if not found.
  const char* Get(const char* key);

 private:
  /// @brief Reads the input configuration file.
  ///
  /// This method reads each line of the configuration file and parses each
  /// line with Conf::ParseLine.
  ///
  /// @return   The number of lines parsed.
  int Read();

  /// @brief Parses a line from the configuration file.
  ///
  /// This method parses a line from the configuration file and inserts the
  /// key/value pair into the map.
  ///
  /// @return   0 if successful, -1 if unsuccessful.
  int ParseLine(std::string& line, int n);

 private:
  std::map<std::string, std::string> map_;
  char* filename_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_CONF_H__ */
