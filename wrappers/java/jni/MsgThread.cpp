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

#include "MsgThread.h"
#include "JniGlue.h"

#include <unistd.h>

MsgThread::MsgThread()
    : _java_vm(nullptr)
    , _is_ready(false) {
}

void MsgThread::Init(JavaVM* java_vm) {
  _java_vm = java_vm;
  _run_thread.Run(shared_from_this(), 0);
}

void MsgThread::Notify(){
  {
    std::lock_guard<std::mutex> lock(_condition_mutex);
    _is_ready = true;
  }
  _condition.notify_one();
}

void MsgThread::OnThreadStarted(int thread_id) {
  JNIEnv* java_env = nullptr;
  _java_vm->GetEnv((void **)&java_env, JNI_VERSION_1_6);
  int attach_err = _java_vm->AttachCurrentThread(&java_env, NULL);
  if (attach_err)
    return;

  while(_run_thread.ShouldRun()) {
    std::unique_lock<std::mutex> lock(_condition_mutex);
    _condition.wait(lock, [this]{return _is_ready;});
    _is_ready = false;
    JniGlue::Instance().CallJavaPropertiesUpdate(java_env);
    lock.unlock();
  }

  _java_vm->DetachCurrentThread();
}
