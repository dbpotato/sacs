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


#include "HttpServer.h"
#include "Logger.h"
#include "Server.h"
#include "Connection.h"
#include "Message.h"
#include "ModuleManager.h"

#include "index_html.h"
#include "script_html.h"
#include "factory_html.h"
#include "style_html.h"

#include <sstream>
#include <vector>


Request::Request()
  : _type(Type::UNKNOWN)
  , _size(0) {
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    if(item.length())
      elems.push_back(std::move(item));
  }
  return elems;
}

HttpServer::HttpServer() {
}

bool HttpServer::Init(std::shared_ptr<Connection> connection,
                      std::shared_ptr<ModuleManager> module_mgr,
                      int port) {
  _module_mgr = module_mgr;
  _server = connection->CreateServer(port, shared_from_this());
  return (_server != nullptr);
}

void HttpServer::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  ProcessRequest(client, msg);
}

bool HttpServer::IsRaw() {
  return true;
}

void HttpServer::PushEventSourceMsg(const std::string& msg) {
  std::stringstream str_stream;
  str_stream << "data: " << msg << "\n\n";

  NotifyEventListeners(str_stream.str());
}

void HttpServer::ProcessRequest(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  Request req;

  ParseRequest(req, msg->ToString());

  switch(req._type) {
    case Request::Type::UNKNOWN:
      PrepareErrorResponse(req);
      break;
    case Request::Type::FILE:
      PerpareFileResponse(req);
      break;
    case Request::Type::EVENTS:
      {
        AddEventListener(client->Handle(), client);
        PrepareEventResponse(client, req);
      }
      break;
    case Request::Type::XHR:
      PrepareXhrResponse(req);
      break;
    default:
      break;
  }
  client->Send(std::make_shared<Message>(req._size, req._data));
}

void HttpServer::ParseRequest(Request& req, const std::string& str) {
  std::vector<std::string> header = split(str, ' ');
  if(header.size() < 2)
    return;

  if(!header.at(0).compare("GET")) {
    if(!header.at(1).compare("/events")) {
      req._type = Request::Type::EVENTS;
    }
    else {
      req._type = Request::Type::FILE;
      req._value = header.at(1);
    }
  }
  else if(!header.at(0).compare("POST")) {
    if(!header.at(1).compare("/xhr")) {
      std::size_t found = str.find("\r\n\r\n");
      if(found!=std::string::npos) {
        req._value = std::string(str.c_str() + found + 4);
        req._type = Request::Type::XHR;
      }
    }
  }
}

void HttpServer::PerpareFileResponse(Request& req) {
  std::string body;
  std::string type;

  if((!req._value.compare("/index.html")) || (!req._value.compare("/")) ) {
    body = INDEX_HTML;
    type = "text/html";
  }
  else if(!req._value.compare("/script.js")) {
    body = SCRIPT_HTML;
    type = "application/javascript";
  }
  else if(!req._value.compare("/factory.js")) {
    body = FACTORY_HTML;
    type = "application/javascript";
  }
  else if(!req._value.compare("/style.css")) {
    body = STYLE_HTML;
    type = "text/css";
  }
  else {
    PrepareErrorResponse(req);
    return;
  }

  size_t size = body.length();

  std::stringstream str_stream;
  str_stream << "HTTP/1.1 200 OK\r\n"\
                "Cache-Control: no-cache\r\n"\
                "Content-Type: " << type << "\r\n"\
                "Content-Length: " << size << "\r\n\r\n";
  str_stream << body;

  const std::string& msg = str_stream.str();
  req._size = msg.length();
  req._data = std::shared_ptr<unsigned char>(new unsigned char[req._size], std::default_delete<unsigned char[]>());
  std::memcpy(req._data.get(), msg.c_str(), req._size);
}

void HttpServer::PrepareEventResponse(std::shared_ptr<Client> client, Request& req) {

  std::string msg = "HTTP/1.1 200 OK\r\n"\
                    "Cache-Control: no-cache\r\n"\
                    "Content-Type: text/event-stream\r\n\r\n";

  req._size = msg.length();
  req._data = std::shared_ptr<unsigned char>(new unsigned char[req._size], std::default_delete<unsigned char[]>());
  std::memcpy(req._data.get(), msg.c_str(), req._size);
}

void HttpServer::PrepareXhrResponse(Request& req) {
  std::string res = _module_mgr->ProcessXhrRequest(req._value);

  std::stringstream str_stream;
  str_stream << "HTTP/1.1 200 OK\r\n"\
                "Cache-Control: no-cache\r\n"\
                "Content-Type: text/plain\r\n"\
                "Content-Length: " << res.length() << "\r\n\r\n";
  if(res.length())
    str_stream << res;

  const std::string& msg = str_stream.str();
  req._size = msg.length();
  req._data = std::shared_ptr<unsigned char>(new unsigned char[req._size], std::default_delete<unsigned char[]>());
  std::memcpy(req._data.get(), msg.c_str(), req._size);
}

void HttpServer::PrepareErrorResponse(Request& req) {
  std::string msg = "HTTP/1.1 404 Not Found\r\n"\
                    "Content-Length: 0\r\n"
                    "\r\n";

  req._size = msg.length();
  req._data = std::shared_ptr<unsigned char>(new unsigned char[req._size], std::default_delete<unsigned char[]>());
  std::memcpy(req._data.get(), msg.c_str(), req._size);
}

void HttpServer::AddEventListener(int id, std::weak_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_event_listeners_mutex);
  _event_listeners.insert(std::make_pair(id, client));
}

void HttpServer::NotifyEventListeners(const std::string& msg) {
  std::lock_guard<std::mutex> lock(_event_listeners_mutex);

  for(auto it = _event_listeners.begin(); it != _event_listeners.end();) {
    if(auto client = it->second.lock()) {
      client->Send(std::make_shared<Message>(msg));
      it++;
    }
    else{
      it = _event_listeners.erase(it);
    }
  }
}
