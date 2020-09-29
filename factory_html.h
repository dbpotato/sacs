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


const char* FACTORY_HTML = R"""(
  function createTextProperty(id, prop, module_id, has_bt, is_ro) {
    var res = document.createElement("div");
    res.setAttribute("class", "horizontal");
    var text = document.createElement("input");
    res.appendChild(text);

    prop.html = text;

    text.setAttribute("class", "property_input");
    text.setAttribute("type", "text");
    text.value = prop.value;

    if(has_bt) {
      var button = document.createElement("div");
      button.innerHTML = "Send";
      button.addEventListener("click", function(){handlePopertyActivated(module_id, id, text.value)});
      button.setAttribute("class", "base_bt send_bt");
      res.appendChild(button);
    }

    if(is_ro) {
      text.classList.add("property_ro");
      text.readOnly = true;
    }
    return res;
  }

  function createTextAreaProperty(id, prop, module_id, has_bt, is_ro) {
    var res = document.createElement("div");
    res.setAttribute("class", "horizontal");
    var text_area = document.createElement("textarea");
    res.appendChild(text_area);

    prop.html = text_area;

    text_area.setAttribute("class", "property_textarea");
    text_area.innerHTML = prop.value;

    if(has_bt) {
      var button = document.createElement("div");
      button.innerHTML = "Send";
      button.addEventListener("click", function(){handlePopertyActivated(module_id, id, text_area.innerHTML)});
      button.setAttribute("class", "base_bt send_bt");
      res.appendChild(button);
    }

    if(is_ro) {
      text_area.classList.add("property_ro");
      text_area.readOnly = true;
    }
    return res;
  }

  function createButtonProperty(id, prop, module_id) {
    var button = document.createElement("div");
    prop.html = button;

    button.innerHTML = prop.name;
    button.addEventListener("click", function(){handlePopertyActivated(module_id, id, "")});
    button.setAttribute("class", "base_bt property_bt");

    return button;
  }

  function createSwitchProperty(id, prop, module_id) {
    var switch_prop = document.createElement("input");
    prop.html = switch_prop;

    switch_prop.setAttribute("type", "checkbox");
    if(prop.value == '1') {
      switch_prop.checked = true;
    }
    else {
      switch_prop.checked = false;
    }

    switch_prop.addEventListener("click", function() {
      value = "0";
      if(switch_prop.checked) {
        value = "1";
      }
      handlePopertyActivated(module_id, id, value);
    });

    switch_prop.setAttribute("class", "toggle");
    return switch_prop;
  }

  function createScript(prop, module_id) {
    var holder = document.getElementById("script_holder");
    var js = document.createElement("script");
    js.text= prop.value;
    js.setAttribute("id", "script_" + module_id);
    holder.appendChild(js);
    return window[prop.name + "_init"](module_id);
  }

  function createProperty(id, prop, module_id) {
    var prop_div = document.createElement("div");
    prop_div.setAttribute("class", "property");

    var prop_name = document.createElement("div");
    prop_name.setAttribute("class", "property_name");
    prop_div.appendChild(prop_name);

    if(prop.type != Property.BUTTON) {
      prop_name.innerHTML = prop.name;
    }

    var prop_value = document.createElement("div");
    prop_value.setAttribute("class", "property_value");
    prop_div.appendChild(prop_value);

    var prop_elem = null;

    switch(prop.type) {
      case Property.TEXT:
        prop_elem = createTextProperty(id, prop, module_id, false, false);
        break;
      case Property.TEXT_BT:
        prop_elem = createTextProperty(id, prop, module_id, true, false);
        break;
      case Property.TEXT_RO:
        prop_elem = createTextProperty(id, prop, module_id, false, true);
        break;
      case Property.TEXT_AREA:
        prop_elem = createTextAreaProperty(id, prop, module_id, false, false);
        break;
      case Property.TEXT_AREA_BT:
        prop_elem = createTextAreaProperty(id, prop, module_id, true, false);
        break;
      case Property.TEXT_AREA_RO:
      case Property.LOG:
        prop_elem = createTextAreaProperty(id, prop, module_id, false, true);
        break;
      case Property.BUTTON:
        prop_elem = createButtonProperty(id, prop, module_id);
        break;
      case Property.BUTTON_SW:
        prop_elem = createSwitchProperty(id, prop, module_id);
        break;
      default:
        break;
    };

    prop_value.appendChild(prop_elem);
    return prop_div;
  }

  function createApplyBt(module_id) {
    var div = document.createElement("div");
    div.setAttribute("class", "property_apply");

    var div_bt = document.createElement("div");
    div_bt.setAttribute("class", "base_bt apply_bt");
    div_bt.innerHTML = "Apply";
    div_bt.addEventListener("click", function(){applyModuleChanges(module_id)});

    div.appendChild(div_bt);
    return div;
  }

  function addModuleTabBt(module) {
    var mod_bt = document.createElement("div");
    mod_bt.setAttribute("class", "base_bt module_bt");
    mod_bt.innerHTML = module.name;
    mod_bt.addEventListener("click", function(){selectModule(module)});

    document.getElementById("top_bar").appendChild(mod_bt);
    return mod_bt;
  }

  function crateModule(module) {
    var html_bt = addModuleTabBt(module);

    var mod_div = document.createElement("div");
    mod_div.setAttribute("class", "module");

    var mod_prop_list_div = document.createElement("div");
    mod_prop_list_div.setAttribute("class", "property_list");

    var needs_mod_bt = false;
    for (var i = 0; i < module.properties.length; i++) {
       var prop = module.properties[i];
       if(prop.type == Property.SCRIPT) {
         mod_prop_list_div.appendChild(createScript(prop, module.id));
         break;
       }
       if(prop.type == Property.TEXT || prop.type == Property.TEXT_AREA ) { 
         needs_mod_bt = true;
       }
       mod_prop_list_div.appendChild(createProperty(i, module.properties[i], module.id));
    }
    mod_div.appendChild(mod_prop_list_div);
    if(needs_mod_bt == true) {
      mod_div.appendChild(createApplyBt(module.id));
    }
    module.html = mod_div;
    module.html_bt = html_bt;
    document.module_list.push(module);
  }

  function deleteModule(id) {
    var module = null;
    for (var i = 0; i < document.module_list.length; i++) {
      if(document.module_list[i].id == id) {
        module = document.module_list[i];
        if(document.selected_module == module) {
          if (document.module_list[0] == module) {
            clear();
            return;
          }
          else {
            selectModule(document.module_list[0]);
          }
        }
        document.module_list.splice(i, 1);
        break;
      }
    }
    if(module == null)
      return;

    module.html_bt.parentNode.removeChild(module.html_bt);
  }
)""";
