/*
  Copyright 2025 Jan Schlieper

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
 */

#ifndef UTILS_H
#define UTILS_H

#include <set>
#include <map>
#include <regex>
#include <Arduino.h>
#include <SD.h>
#include <Update.h>
#include <HardwareSerial.h>

enum class FinderType { FOO, T1, T2, T3, T_float };

class Utils {
    public:
        Utils();
        String configToString(SerialConfig value);
        SerialConfig stringToConfig(String value);
        String typeToString(FinderType value);
        FinderType stringToType(String value);
        std::set<String>* listDirs(String path, const std::regex re);
        std::map<String, int>* listFiles(String path, const std::regex re);
        void setUpdateFilename(String filename);
        void setUpdateCommand(int command);
        bool updateAvailable();
        void update();

    private:
        String updateFilename;
        int updateCommand;
};

#endif
