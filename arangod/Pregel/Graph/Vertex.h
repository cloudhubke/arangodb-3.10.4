////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2022 ArangoDB GmbH, Cologne, Germany
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
/// @author Markus Pfeiffer
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Assertions/Assert.h"

#include "Edge.h"
#include "VertexID.h"

#include <vector>

namespace arangodb::pregel {

template<typename V, typename E>
// cppcheck-suppress noConstructor
class Vertex {
  std::string _key;
  PregelShard _shard;
  std::vector<Edge<E>> _edges;

  bool _active;

  V _data;  // variable byte size

 public:
  Vertex() noexcept
      : _key(),
        _edges(),
        _active(true),
        _shard() {
    TRI_ASSERT(keyLength() == 0);
    TRI_ASSERT(active());
  }

  // TODO: this is probably not true (anymore/soon)
  // note: the destructor for this type is never called,
  // so it must not allocate any memory or take ownership
  // of anything

  std::vector<Edge<E>>& getEdges() noexcept { return _edges; }
  std::vector<Edge<E>>& getEdges() const noexcept { return _edges; }

  // adds an edge for the vertex. returns the number of edges
  // after the addition. note that the caller must make sure that
  // we don't end up with more than 4GB edges per verte.
  size_t addEdge(Edge<E>&& edge) noexcept {
    // must only be called during initial vertex creation
    TRI_ASSERT(active());
    _edges.emplace(edge);
    return _edges.size();
  }

  // returns the number of associated edges
  [[nodiscard]] size_t getEdgeCount() const noexcept {
    return _edges.size();
  }

  void setActive(bool bb) noexcept {
    _active = bb;
    TRI_ASSERT((bb && active()) || (!bb && !active()));
  }

  [[nodiscard]] bool active() const noexcept { return _active; }

  void setShard(PregelShard shard) noexcept { _shard = shard; }
  [[nodiscard]] PregelShard shard() const noexcept { return _shard; }

  void setKey(char const* key, uint16_t keyLength) noexcept {
    // must only be called during initial vertex creation
    TRI_ASSERT(active());
    TRI_ASSERT(this->keyLength() == 0);
    _key = std::string(key, keyLength);
    TRI_ASSERT(active());
    TRI_ASSERT(this->keyLength() == keyLength);
  }

  [[nodiscard]] uint16_t keyLength() const noexcept { return _key.length(); }

  [[nodiscard]] std::string_view key() const {
    return std::string_view(_key);
  }
  V const& data() const& { return _data; }
  V& data() & { return _data; }

  [[nodiscard]] VertexID pregelId() const {
    return VertexID{_shard, _key};
  }
};

}  // namespace arangodb::pregel
