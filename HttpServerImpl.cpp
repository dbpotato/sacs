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


#include "HttpServerImpl.h"
#include "Message.h"
#include "ModuleManager.h"

#include <sstream>
#include <vector>

void HttpServerImpl::GetResource(HttpRequest& request) {
  if(request._type == HttpRequest::Type::GET) {
    if(!request._resource_name.compare("/events")) {
      AddEventListener(request._client);
      PrepareEventResponse(request);
    }
    else{
      PerpareFileResponse(request);
    }
  }
  else if(request._type == HttpRequest::Type::POST){
    if(!request._resource_name.compare("/xhr")) {
      PrepareXhrResponse(request);
    }
  }
}

bool HttpServerImpl::Init(std::shared_ptr<Connection> connection,
                      std::shared_ptr<ModuleManager> module_mgr,
                      int port) {
  _module_mgr = module_mgr;
  _server = std::make_shared<HttpServer>();
  return (_server ->Init(connection, shared_from_this(), port));
}


void HttpServerImpl::PushEventSourceMsg(const std::string& msg) {
  std::stringstream str_stream;
  str_stream << "data: " << msg << "\n\n";
  NotifyEventListeners(str_stream.str());
}

void HttpServerImpl::PerpareFileResponse(HttpRequest& request) {
  std::string name = request._resource_name;
  if(name.at(0) == '/') {
    name = name.substr(1);
  }
  std::string body =  _web_data.GetResource(name);
  if(!body.length()) {
    request._valid = false;
    return;
  }
  request.SetResponse(body);
}

void HttpServerImpl::PrepareEventResponse(HttpRequest& request) {
  request._handled = true;

  auto client_sptr = request._client.lock();
  if(!client_sptr)
    return;

  std::string msg = "HTTP/1.1 200 OK\r\n"\
                    "Cache-Control: no-cache\r\n"\
                    "Content-Type: text/event-stream\r\n\r\n";

  client_sptr->Send(std::make_shared<Message>(msg));
}

void HttpServerImpl::PrepareXhrResponse(HttpRequest& request) {
  std::string response = _module_mgr->ProcessXhrRequest(request._post_msg);
  request._response_mime_type = "text/plain";
  request.SetResponse(response);
}

void HttpServerImpl::AddEventListener(std::weak_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_event_listeners_mutex);
  _event_listeners.push_back(client);
}

void HttpServerImpl::NotifyEventListeners(const std::string& msg) {
  std::lock_guard<std::mutex> lock(_event_listeners_mutex);

  for(auto it = _event_listeners.begin(); it != _event_listeners.end();) {
    if(auto client = it->lock()) {
      client->Send(std::make_shared<Message>(msg));
      it++;
    }
    else{
      it = _event_listeners.erase(it);
    }
  }
}
