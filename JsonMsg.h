/*
Copyright (c) 2020 Adam Kaniewski

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Module.h"
#include "nlohmann/json.hpp"

#include <memory>
#include <vector>


class JsonMsg {
public:
  enum Type {
    UNKNOWN = 0,
    MODULE_ADD,
    MODULE_DEL,
    PROPERTY_UPDATE,
    REQ_LOAD,
    REQ_APPLY
  };

  JsonMsg();
  bool Parse(const std::string& str);
  std::string ToString();
  Type GetType();
  std::shared_ptr<Module> ToModule();
  int FindId();
  void SetId(int id);

  void GetPropertiesList(std::vector<std::pair<int, std::string> >& out_properties);

  static std::string CreateModuleAdded(std::shared_ptr<Module> module);
  static std::string CreateModuleRemoved(int module_id);
  static std::string CreatePropertyUpdated(int module_id, const std::vector<std::pair<int, std::string> >& properties);
  static std::string CreateModuleList(std::map<int, std::shared_ptr<Module> >& modules);
  static std::string Empty();
private:
  void TryDetectType();
  std::string ValueToString(const nlohmann::json& json, const std::string& key);
  int ValueToInt(const nlohmann::json& json, const std::string& key);
  static nlohmann::json ModuleToJson(std::shared_ptr<Module> module);
  nlohmann::json _json;
  bool _is_valid;
  Type _type;
};
