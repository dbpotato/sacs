/*
Copyright (c) 2020 - 2022 Adam Kaniewski

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

#include "JniGlue.h"
#include "MsgThread.h"
#include "Logger.h"

JniGlue::JniGlue() : _java_env(nullptr) {
}

JniGlue& JniGlue::Instance() {
  static JniGlue instance;
  return instance;
}

bool JniGlue::Init(JavaVM* vm, JNIEnv* env) {
  _java_env = env;

  jclass property_class = _java_env->FindClass("dbpotato/net/sacs/SacsWrapper$ModuleProperty");
  if(!property_class) {
    log()->error("JniGlue::Init failed : ModuleProperty class not found");
    return false;
  }

  _property_desc.class_id = (jclass) _java_env->NewGlobalRef(property_class);
  _property_desc.type = _java_env->GetFieldID(_property_desc.class_id, "type", "I");
  _property_desc.name = _java_env->GetFieldID(_property_desc.class_id, "name", "Ljava/lang/String;");
  _property_desc.value = _java_env->GetFieldID(_property_desc.class_id, "value", "Ljava/lang/String;");

  if(!_property_desc.type || !_property_desc.name || !_property_desc.value) {
    log()->error("JniGlue::Init failed : ModuleProperty's field not found");
    return false;
  }

  jclass update_class = _java_env->FindClass("dbpotato/net/sacs/SacsWrapper$ModulePropertyUpdate");
  if(!property_class) {
    log()->error("JniGlue::Init failed : ModulePropertyUpdate class not found");
    return false;
  }

  _update_desc.class_id = (jclass) _java_env->NewGlobalRef(update_class);
  _update_desc.constructor = _java_env->GetMethodID(_update_desc.class_id, "<init>", "(ILjava/lang/String;)V");
  _update_desc.id = _java_env->GetFieldID(_update_desc.class_id, "id", "I");
  _update_desc.value = _java_env->GetFieldID(_update_desc.class_id, "value", "Ljava/lang/String;");

  if(!_update_desc.id || !_update_desc.value) {
    log()->error("JniGlue::Init failed : ModulePropertyUpdate's field not found");
    return false;
  }

  jclass sacs_wrapper_class = _java_env->FindClass("dbpotato/net/sacs/SacsWrapper");
  if(!sacs_wrapper_class) {
    log()->error("JniGlue::Init failed : SacsWrapper class not found");
    return false;
  }

  _sacs_desc.class_id = (jclass) _java_env->NewGlobalRef(sacs_wrapper_class);
  _sacs_desc.on_props_updated = _java_env->GetStaticMethodID(_sacs_desc.class_id, "onPropertiesUpdated", "(I[Ldbpotato/net/sacs/SacsWrapper$ModulePropertyUpdate;)V");
  if(!_sacs_desc.on_props_updated) {
    log()->error("JniGlue::Init failed : SacsWrapper's field not found");
    return false;
  }

  _msg_thread = std::make_shared<MsgThread>();
  _msg_thread->Init(vm);

  return true;
}

void JniGlue::Callback(int module_id, int count, const int* property_no, const char* const* property_val) {
  Instance().InternalCallback(module_id, count, property_no, property_val);
}

void JniGlue::InternalCallback(int module_id, int count, const int* property_no, const char* const* property_val) {
  std::vector<std::pair<int, std::string> > update_vec;
  for(int i = 0; i < count; i++) {
    update_vec.emplace_back(*(property_no + i), *(property_val + i));
  }

  QueuedCallback callback = {module_id, update_vec};

  _collector.Add(callback);
  _msg_thread->Notify();
}

void JniGlue::CallJavaPropertiesUpdate(JNIEnv* env) {

  std::vector<QueuedCallback> callbacks;
  _collector.Collect(callbacks);

  for(auto& callback : callbacks) {

    auto update_array = env->NewObjectArray(callback.update_vec.size(), _update_desc.class_id, nullptr);

    for(size_t i = 0 ; i < callback.update_vec.size(); ++i) {
      jobject obj = env->NewObject(_update_desc.class_id,
                                   _update_desc.constructor,
                                   (jint)callback.update_vec.at(i).first,
                                   env->NewStringUTF(callback.update_vec.at(i).second.c_str()));

      env->SetObjectArrayElement(update_array, i, obj);
    }

    env->CallStaticVoidMethod(_sacs_desc.class_id,
                              _sacs_desc.on_props_updated,
                              (jint)callback.module_id,
                              update_array);
    env->DeleteLocalRef(update_array);
  }
}

void JniGlue::JniPropsToVec(JNIEnv* env, jobjectArray& jni_array, std::vector<ModuleProperty>& out_vec) {
  if(!jni_array) {
    return;
  }

  jsize props_len = env->GetArrayLength(jni_array);

  for(jsize i = 0; i < props_len; ++i ) {
    jobject jprop = env->GetObjectArrayElement(jni_array, i);
    if(jprop == nullptr)
      continue;

    jint jprop_type = env->GetIntField(jprop, _property_desc.type);
    jstring jprop_name = (jstring)env->GetObjectField(jprop, _property_desc.name);
    jstring jprop_value = (jstring)env->GetObjectField(jprop, _property_desc.value);

    const char* cname = env->GetStringUTFChars(jprop_name, nullptr);
    const char* cvalue = env->GetStringUTFChars(jprop_value, nullptr);

    out_vec.emplace_back((ModuleProperty::Type)jprop_type, cname, cvalue);
  }
}

void JniGlue::JniUpdatePropsToVec(JNIEnv* env, jobjectArray& jni_array, std::vector<std::pair<int, std::string> >& out_vec) {
  jsize props_len = env->GetArrayLength(jni_array);

  for(jsize i = 0; i < props_len; ++i ) {
    jobject jprop = env->GetObjectArrayElement(jni_array, i);
    jint jprop_id = env->GetIntField(jprop, _update_desc.id);
    jstring jprop_value = (jstring)env->GetObjectField(jprop, _update_desc.value);
    const char* cvalue = env->GetStringUTFChars(jprop_value, nullptr);

    out_vec.emplace_back((int)jprop_id, cvalue);
  }
}
