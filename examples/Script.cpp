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

#include <unistd.h>
#include <stdlib.h>
#include <sstream>

#include "SacsWrapper.h"

std::string script = R"""(
  class ScriptTest {
    constructor(propertyObj) {
      this.img = document.createElement('img');
      this.img.src='https://icons-for-free.com/iconfiles/png/512/mario+mario+bros+mario+world+mushroom+toad+videogame+icon-1320196400529338074.png';
      this.img.width = 400;
      this.img.height = 400;
      this.img.addEventListener("click", function(){
        propertyObj.onActivated();
      });
      propertyObj.addObj(this.img);
    }

    getValue() {
      return "1";
    }

    setValue(value) {
      let vals = value.split(' ');
      this.img.width = parseInt(vals[0]);
      this.img.height = parseInt(vals[1]);
    }
  }

  return new ScriptTest(propertyObj);
)""";

void callback(int module_id, int count, const int* property_no, const char* const* property_val) {

  std::stringstream str_stream;
  str_stream << (rand() % 201 + 200) << " " << (rand() % 201 + 200);

  std::vector<std::pair<int, std::string> > properties;
  properties.emplace_back(0, str_stream.str());
  properties.emplace_back(1, str_stream.str());
  SacsWrapper::Instance().UpdateProperties(module_id, properties);
}

int main() {

 /* Register Module with two text fileds and callback
  */
  std::vector<ModuleProperty> properties;
  properties.emplace_back(ModuleProperty::Type::TEXT_RO, "Current Size", "400 400");
  properties.emplace_back(ModuleProperty::Type::SCRIPT, "", script);
  SacsWrapper::Instance().RegisterModule("Script", properties, callback);

  while(true)
    sleep(1);
}

