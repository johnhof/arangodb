////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Achim Brandt
////////////////////////////////////////////////////////////////////////////////

#include "ListenTask.h"

#include "Logger/Logger.h"

using namespace arangodb;
using namespace arangodb::rest;

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

ListenTask::ListenTask(EventLoop2 loop, Endpoint* endpoint)
    : Task2(loop, "ListenTask"),
      _endpoint(endpoint),
      _bound(false),
      _ioService(&loop._ioService),
      _acceptor(loop._ioService),
      _peer(loop._ioService) {}

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

void ListenTask::start() {
  try {
    _endpoint->openAcceptor(_ioService, &_acceptor);
    _bound = true;
  } catch (boost::system::system_error const& err) {
    LOG(WARN) << "failed to open endpoint '" << _endpoint->specification()
              << "' with error: " << err.what();
    return;
  }

  _handler = [this](boost::system::error_code const& ec) {
    if (ec) {
      ++_acceptFailures;

      if (_acceptFailures < MAX_ACCEPT_ERRORS) {
        LOG(WARN) << "accept failed: " << ec.message();
      } else if (_acceptFailures == MAX_ACCEPT_ERRORS) {
        LOG(WARN) << "accept failed: " << ec.message();
        LOG(WARN) << "too many accept failures, stopping to report";
      }
    } else {
      ConnectionInfo info;
      // TODO _endpoint->initIncoming(_peer);

      handleConnected(std::move(_peer), std::move(info));
    }

    if (_bound) {
      _acceptor.async_accept(_peer, _peerEndpoint, _handler);
    }
  };

  _acceptor.async_accept(_peer, _peerEndpoint, _handler);
}

void ListenTask::stop() {
  if (!_bound) {
    return;
  }

  _bound = false;
  _acceptor.close();
}
