/*
Copyright (c) 2020 - 2021 Adam Kaniewski

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

#include "ModuleManager.h"

#include <vector>
#include <stdio.h>
#include <string>
#include <string.h>

const int SACS_PORT = 8079;

struct ModuleProperty {
  enum Type {
    BUTTON = 0,
    BUTTON_SW,
    BUTTON_SW_IN,
    TEXT,
    TEXT_BT,
    TEXT_RO,
    TEXT_AREA,
    TEXT_AREA_BT,
    TEXT_AREA_RO,
    LOG,
    SCRIPT
  };
  ModuleProperty(Type type,
                 const std::string& name,
                 const std::string& defualt_value = {})
    : _type(type)
    , _name(name)
    , _default_value(defualt_value) {
  }
  Type _type;
  std::string _name;
  std::string _default_value;
};

class SacsWrapper {
private :
  std::shared_ptr<ModuleManager> _mgr;

protected:
  SacsWrapper() {
    _mgr = ModuleManager::Create(SACS_PORT);
  }

  char IsPropertyTypeForReadOnly(ModuleProperty::Type type) {
    if (type == ModuleProperty::Type::SCRIPT)
      return 1;
    return 0;
  }

public:
  static SacsWrapper& Instance() {
    static SacsWrapper& instance = *new SacsWrapper();
    return instance;
  }

  bool IsValid() {
    return true;
  }

  int RegisterModule(const std::string& name,
                     const std::vector<ModuleProperty>& properties,
                     void(callback)(int, int, const int*, const char* const*)) {

    std::vector<Module::Property> vec_props;
    for(auto& mod_prop : properties) {
      vec_props.emplace_back((int)mod_prop._type, mod_prop._name, mod_prop._default_value, IsPropertyTypeForReadOnly(mod_prop._type));
    }

    return _mgr->RegisterModule(name, vec_props, callback);
  }

  void UnregisterModule(int module_id) {
    _mgr->UnregisterModule(module_id);
  }

  void UpdateProperties(int module_id, const std::vector<std::pair<int, std::string> >& properties) {
   _mgr->UpdateProperties(module_id, properties);
  }
};
