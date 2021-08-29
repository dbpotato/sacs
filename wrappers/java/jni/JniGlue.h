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

#include <jni.h>

#include "SacsWrapperNoLib.h"
#include "Collector.h"

#include <memory>
#include <mutex>

class MsgThread;

struct QueuedCallback {
  int module_id;
  std::vector<std::pair<int, std::string> > update_vec;
};

struct JniPropertyDesc {
  jclass class_id;
  jfieldID type;
  jfieldID name;
  jfieldID value;
};

struct JniPropertyUpdateDesc {
  jclass class_id;
  jmethodID constructor;
  jfieldID id;
  jfieldID value;
};

struct JniSacsWrapperDesc {
  jclass class_id;
  jmethodID on_props_updated;
};

class JniGlue {
public:
  static JniGlue& Instance();
  bool Init(JavaVM* vm, JNIEnv* env);
  void JniPropsToVec(JNIEnv* env, jobjectArray& jni_array, std::vector<ModuleProperty>& out_vec);
  void JniUpdatePropsToVec(JNIEnv* env, jobjectArray& jni_array, std::vector<std::pair<int, std::string> >& out_vec);
  void static Callback(int module_id, int count, const int* property_no, const char* const* property_val);
  void CallJavaPropertiesUpdate(JNIEnv* env);

protected :
  JniGlue();
  void InternalCallback(int module_id, int count, const int* property_no, const char* const* property_val);
  JNIEnv* _java_env;
  JniPropertyDesc _property_desc;
  JniPropertyUpdateDesc _update_desc;
  JniSacsWrapperDesc _sacs_desc;
  Collector<QueuedCallback> _collector;
  std::mutex _callbacks_mutex;
  std::shared_ptr<MsgThread> _msg_thread;
};
