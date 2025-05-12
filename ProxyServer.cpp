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

#include "ProxyServer.h"
#include "Connection.h"
#include "DataResource.h"
#include "Server.h"
#include "SimpleMessage.h"
#include "ModuleManager.h"
#include "Message.h"
#include "Logger.h"


void ProxyServer::Init(std::shared_ptr<Connection> connection,
                       std::shared_ptr<ModuleManager> mgr,
                       int port) {
  _module_mgr = mgr;
  _server = connection->CreateServer(port, shared_from_this()); //TODO error handle 
}

void ProxyServer::AddModule(int local_id, int remote_id, uint32_t client_id) {
  _remote_modules.insert(std::make_pair(local_id, std::make_pair(remote_id, client_id)));
}

void ProxyServer::RemoveModule(int local_id) {
  _remote_modules.erase(local_id);
}

void ProxyServer::RemoveClientsModules(uint32_t client_id) {
  for(auto it = _remote_modules.begin(); it != _remote_modules.end(); ) {
    if(it->second.second == client_id) {
      _module_mgr->UnregisterModule(it->first);
      it = _remote_modules.erase(it);
    }
    else {
      ++it;
    }
  }
}

int ProxyServer::LocalIdFromRemote(int remote_id, uint32_t client_id) {
  for(auto& entry : _remote_modules) {
    if(entry.second.first == remote_id && entry.second.second == client_id)
      return entry.first;
  }
  return -1;
}

std::pair<int, uint32_t> ProxyServer::RemoteIdFromLocal(int local_id) {
  auto it = _remote_modules.find(local_id);
  if(it != _remote_modules.end())
    return it->second;
  return std::make_pair(-1,-1);
}

bool ProxyServer::OnClientConnecting(std::shared_ptr<Client> client, NetError err) {
  if(err == NetError::OK) {
    auto msg_builder = std::unique_ptr<SimpleMessageBuilder>(new SimpleMessageBuilder());
    client->SetMsgBuilder(std::move(msg_builder));
    return true;
  }
  return false;
}

void ProxyServer::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  std::shared_ptr<SimpleMessage> simple_msg = std::static_pointer_cast<SimpleMessage>(msg);
  auto msg_content = simple_msg->GetContent();
  auto msg_data = msg_content->GetMemCache();
  std::string str = msg_data->ToString();

  JsonMsg json;
  if(!json.Parse(str))
    return;

  switch(json.GetType()) {
    case JsonMsg::Type::MODULE_ADD :
      {
        std::shared_ptr<Module> module = json.ToModule();
        int remote_id = module->_id;
        int local_id = _module_mgr->RegisterModule(module);
        AddModule(local_id, remote_id, client->GetId());
      }
      break;
    case JsonMsg::Type::MODULE_DEL :
      {
        int id = LocalIdFromRemote(json.FindId(), client->GetId());
        _module_mgr->UnregisterModule(id);
        RemoveModule(id);
      }
      break;
    case JsonMsg::Type::PROPERTY_UPDATE :
      {
        int id = LocalIdFromRemote(json.FindId(), client->GetId());
        json.SetId(id);
        _module_mgr->UpdateProperties(id, json);
      }
      break;
    default:
      break;
  }
}

void ProxyServer::OnClientClosed(std::shared_ptr<Client> client) {
  RemoveClientsModules(client->GetId());
}

void ProxyServer::HandlePropertyUpdate(JsonMsg& json) {
  int local_id = json.FindId();
  std::pair<int, uint32_t> remote = RemoteIdFromLocal(local_id);

  std::shared_ptr<Client> client = _server->GetClient(remote.second);
  if(client) {
    json.SetId(remote.first);
    client->Send(std::make_shared<SimpleMessage>(0,json.ToString()));
  }
}
