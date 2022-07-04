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

#include <jni.h>

#include "JniGlue.h"

#include "SacsWrapperNoLib.h"
#include "Logger.h"


JavaVM* g_vm;

extern "C" {

JNIEXPORT jboolean JNICALL Java_dbpotato_net_sacs_SacsWrapper_Init(JNIEnv* jenv, jobject obj) {
  log()->info("SacsLib : JniGlue init");
  return (jboolean)JniGlue::Instance().Init(g_vm, jenv);
}

JNIEXPORT jint JNICALL JNICALL Java_dbpotato_net_sacs_SacsWrapper_RegisterModule(JNIEnv* jenv, jobject obj, jstring name, jobjectArray properies) {

  const char* cname = jenv->GetStringUTFChars(name , nullptr);
  std::vector<ModuleProperty> vec;
  JniGlue::Instance().JniPropsToVec(jenv, properies, vec);

  return SacsWrapper::Instance().RegisterModule(cname, vec, &JniGlue::Callback);
}

JNIEXPORT void JNICALL JNICALL Java_dbpotato_net_sacs_SacsWrapper_UnregisterModule(JNIEnv* jenv, jobject obj, jint module_id) {
  SacsWrapper::Instance().UnregisterModule((int)module_id);
}

JNIEXPORT void JNICALL JNICALL Java_dbpotato_net_sacs_SacsWrapper_UpdateProperties(JNIEnv* jenv, jobject obj, jint module_id, jobjectArray properies) {

  std::vector<std::pair<int, std::string> > vec;
  JniGlue::Instance().JniUpdatePropsToVec(jenv, properies, vec);

  SacsWrapper::Instance().UpdateProperties((int)module_id, vec);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  g_vm = vm;
  return JNI_VERSION_1_6;
}

} //extern "C"

