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


#include "ServerImpl.h"
#include "Message.h"
#include "MimeTypeFinder.h"
#include "WebsocketMessage.h"
#include "ModuleManager.h"
#include "HttpHeader.h"
#include "HttpHeaderDecl.h"
#include "HttpMessage.h"
#include "Logger.h"

#include <sstream>
#include <vector>


ServerImpl::ServerImpl(std::weak_ptr<ModuleManager> module_mgr)
    : _module_mgr(module_mgr) {
}

void ServerImpl::Handle(HttpRequest& request) {
  auto req_header = request._request_msg->GetHeader();
  if(req_header->GetMethod() == HttpHeaderMethod::GET) {
    PerpareHTTPGetResponse(request);
  } else {
    request._response_msg = std::make_shared<HttpMessage>(405);
  }
}

bool ServerImpl::OnWsClientConnected(std::shared_ptr<Client> client, const std::string& request_arg) {
  AddEventListener(client);
  return true;
}

void ServerImpl::OnWsClientMessage(std::shared_ptr<Client> client, std::shared_ptr<WebsocketMessage> message) {
  if(auto module_mgr = _module_mgr.lock()) {
    std::string msg_str = message->ToString();
    std::string response = module_mgr->ProcessJSRequest(msg_str);
    auto ws_msg = std::make_shared<WebsocketMessage>(response);
    client->Send(ws_msg);
  }
}

void ServerImpl::OnWsClientClosed(std::shared_ptr<Client> client) {
  RemoveEventListener(client);
}

void ServerImpl::PushMsgForJS(const std::string& msg) {
  std::lock_guard<std::mutex> lock(_event_listeners_mutex);

  for(auto it = _event_listeners.begin(); it != _event_listeners.end();) {
    auto msg_obj = (std::make_shared<WebsocketMessage>(msg))->ConvertToBaseMessage();
    if(auto client = it->second.lock()) {
      client->Send(msg_obj);
      it++;
    }
    else{
      it = _event_listeners.erase(it);
    }
  }
}

void ServerImpl::PerpareHTTPGetResponse(HttpRequest& request) {
  std::string name = request._request_msg->GetHeader()->GetRequestTarget();
  if(name.at(0) == '/') {
    name = name.substr(1);
  }
  if(!name.length()) {
    name = "index.html";
  }

  std::string body = _web_data.GetResource(name);
  if(!body.length()) {
    request._response_msg = std::make_shared<HttpMessage>(404);
    return;
  }
  request._response_msg = std::make_shared<HttpMessage>(200, body);
  request._response_msg->GetHeader()->AddField(HttpHeaderField::CONTENT_TYPE, MimeTypeFinder::Find(name));
}

void ServerImpl::AddEventListener(std::shared_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_event_listeners_mutex);
  _event_listeners.insert(std::make_pair(client->GetId(), client));
}

void ServerImpl::RemoveEventListener(std::shared_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_event_listeners_mutex);
  _event_listeners.erase(client->GetId());
}
