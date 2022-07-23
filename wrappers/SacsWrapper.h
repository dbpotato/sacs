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

#include <dlfcn.h>
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
  void(*_init)(int);
  int(*_register_module)(const char*, int, int*, char**, char**, char*, void(*)(int, int, const int*, const char* const*));
  void(*_unregister_module)(int);
  void(*_update_properties)(int, int, int*, char**);
  bool _is_valid;
  void* _lib_handler;

protected:
  SacsWrapper()
      : _init(nullptr)
      , _register_module(nullptr)
      , _is_valid(false)
      , _lib_handler(nullptr) {

    if(!(_lib_handler = dlopen("libsacs.so", RTLD_NOW))) {
      printf("SacsWrapper : Can't load libsacs.so : %s\n", dlerror());
    }
    else {
      _init = (void(*)(int)) dlsym(_lib_handler, "init");
      _register_module = (int(*)(const char*, int, int*, char**, char**, char*, void(*)(int, int, const int*, const char* const*))) dlsym(_lib_handler, "register_module");
      _unregister_module = (void(*)(int)) dlsym(_lib_handler, "unregister_module");
      _update_properties = (void(*)(int, int, int*, char**)) dlsym(_lib_handler, "update_properties");

      if(_init && _register_module && _update_properties && _unregister_module) {
        _init(SACS_PORT);
        _is_valid = true;
      }
      else {
        printf("SacsWrapper : Can't load some functions\n");
      }
    }
  }
  char* StrToChrA(const std::string& str) {
    char* res = nullptr;
    if(str.length()) {
      res = new char[str.length() + 1];
      res = strcpy(res, str.c_str());
    }
    else {
      res = new char[1];
      *res = 0;
    }
    return res;
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
    return _is_valid;
  }

  int RegisterModule(const std::string& name,
                     const std::vector<ModuleProperty>& properties,
                     void(callback)(int, int, const int*, const char* const*)) {
    if(!_is_valid)
      return -2;

    int props_no = properties.size();

    int* types = new int[properties.size()];
    char** names = new char*[properties.size()];
    char** vals = new char*[properties.size()];
    char* read_only = new char[properties.size()];

    for(int i = 0; i < props_no; i++) {
      types[i] = (int)properties.at(i)._type;
      names[i] = StrToChrA(properties.at(i)._name);
      vals[i] = StrToChrA(properties.at(i)._default_value);
      read_only[i] = IsPropertyTypeForReadOnly(properties.at(i)._type);
    }

    int res = _register_module(name.c_str(), props_no, types, names, vals, read_only, callback);

    delete[] types;
    delete[] read_only;
    for(int i = 0; i < props_no; i++) {
      delete[] names[i];
      delete[] vals[i];
    }
    delete[] names;
    delete[] vals;

    return res;
  }

  void UnregisterModule(int module_id) {
    if(!_is_valid)
      return;

    _unregister_module(module_id);
  }

  void UpdateProperties(int module_id, const std::vector<std::pair<int, std::string> >& properties) {
    if(!_is_valid)
      return;

    int props_no = properties.size();

    int* ids = new int[properties.size()];
    char** values = new char*[properties.size()];

    for(int i = 0; i < props_no; i++) {
      ids[i] = properties.at(i).first;
      values[i] = new char[properties.at(i).second.length() + 1];
      strcpy(values[i], properties.at(i).second.c_str());
    }

    _update_properties(module_id, props_no, ids, values);

    delete[] ids;
    for(int i = 0; i < props_no; i++) {
      delete[] values[i];
    }
    delete[] values;
  }
};
