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

#include "Module.h"

Module::Module(int id,
               const std::string& name,
               const std::vector<Property>& props,
               void(*callback)(int, int, const int*, const char* const*))
    : _id(id)
    , _name(name)
    , _properties(props)
    , _callback(callback) {
}

void Module::SetPropertyValue(int property_id, const std::string& value) {
  if(property_id < (int)_properties.size()) {
    _properties.at(property_id).SetValue(value);
  }
}

Module::Property::Property(int type,
                           const std::string& name,
                           const std::string& defualt_value,
                           bool  read_only)
    : _type(type)
    , _name(name)
    , _value(defualt_value)
    , _read_only(read_only) {
}

void Module::Property::SetValue(const std::string& value) {
  if(!_read_only)
    _value = value;
}
