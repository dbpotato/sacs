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

const char* STYLE_HTML = R"""(
:focus {outline:none;}
::-moz-focus-inner {border:0;}

body {
 background-color: #fafafa;
 font-size : 15px;
 font-family : Arial;
 display: flex;
 flex-direction : column;
 height: 100%;
 margin: 0;
 align-items: center;
}

#script_holder {
  display: hidden;
}

#top_bar {
  background-color: #eeeeee;
  border-radius: 8px;
  width : 90%;
  height : 50px;
  flex-direction : row;
  justify-content: center;
  align-items: center;
  margin-top: 30px;
}

#main_content {
  width : 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
  margin: 0;
}

#reconnect_info {
  flex-direction : column;
  justify-content: center;
  align-items: center;
}

.module {
  display: flex;
  margin-top: 100px;
  height : 100%;
  width : 70%;
  flex-direction : column;
}

.base_bt {
  color : black;
  border-radius: 10px;
  border: solid #8c8c8c 1px;
  display: flex;
  justify-content: center;
  align-items: center;
  padding-left: 8px;
  padding-right: 8px;
  text-decoration: none;
  cursor: pointer;
  height : 35px;
}
.base_bt:hover {
  border: solid #8c8c8c 2px;
  height : 33px;
}

.send_bt {
  border-radius: 5px;
  margin-left: 4px;
  height : 22px;
  width : 30px;
  font-size: 14px;
}

.send_bt:hover {
  border: solid #8c8c8c 2px;
  height : 20px;
}

.module_bt {
  float: left;
  margin-left: 5px;
  margin-right: 5px;
  height : 48px;
}

.module_bt_selected {
  background-color : #4d6791;
  color : white;
}
.module_bt:hover {
  height : 46px;
}

.horizontal {
  display: flex;
  flex-direction : row;
}

.apply_bt {
  width: 65px;
}

#reconnect_bt {
   width: 45px;
}

.property_list {
  display: flex;
  flex-direction : column;
}
.property {
  display: flex;
  flex-direction : row;
  margin-bottom : 25px;
}
.property_name {
  width : 20%;
}
.property_val {
  width : 80%;
  margin-left : 10px;
}
.property_apply {
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100%;
}
.property_input {
  height: 23px;
  background-color : white;
}
.property_textarea {
  height: 15em;
  width: 50em;
}
.property_ro {
  background-color : #efefef;
}

.toggle {
  -webkit-appearance: none;
  -moz-appearance: none;
  appearance: none;
  -webkit-tap-highlight-color: transparent;
  box-sizing: border-box;
  cursor: pointer;

  height: 32px;
  width: 65px;
  border-radius: 10px;
  display: inline-block;
  position: relative;
  margin: 0;
  border: 2px solid #474755;
  background: #f5fdff;
  transition: all 0.2s ease;
}

.toggle :focus {
  outline: 0;
}

.toggle:after {
  content: "";
  position: absolute;
  top: 2px;
  left: 2px;
  width: 24px;
  height: 24px;
  border-radius: 50%;
  background: #4d6791;
  box-shadow: 0 1px 2px rgba(44, 44, 44, 0.2);
  transition: all 0.2s cubic-bezier(0.5, 0.1, 0.75, 1.35);
}
.toggle:checked {
  background: #96baeb;
  transition: background-color 0.2s linear;
  border-color: #4d6791;
}
.toggle:checked:after {
  transform: translatex(33px);
}
)""";
