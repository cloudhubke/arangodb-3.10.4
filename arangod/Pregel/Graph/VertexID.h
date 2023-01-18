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
/// @author Julia Volmer
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "PregelShard.h"

namespace arangodb::pregel {

struct VertexID {
  VertexID() : _shard(InvalidPregelShard) {}
  VertexID(PregelShard s, std::string k) : _key(std::move(k)), _shard(s) {}

  bool operator==(const VertexID& rhs) const {
    return _shard == rhs._shard && _key == rhs._key;
  }

  bool operator!=(const VertexID& rhs) const {
    return _shard != rhs._shard || _key != rhs._key;
  }

  bool operator<(const VertexID& rhs) const {
    return _shard < rhs._shard || (_shard == rhs._shard && _key < rhs._key);
  }

  [[nodiscard]] bool isValid() const {
    return _shard != InvalidPregelShard && !_key.empty();
  }

  [[nodiscard]] std::string_view key() const { return _key; }
  [[nodiscard]] PregelShard pregelShard() const { return _shard; }
  std::string _key;
  PregelShard _shard;
};

}  // namespace arangodb::pregel
namespace std {
template<>
struct hash<arangodb::pregel::VertexID> {
  std::size_t operator()(const arangodb::pregel::VertexID& k) const noexcept {
    using std::hash;
    using std::string;

    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:
    size_t h1 = std::hash<std::string_view>()(k.key());
    size_t h2 = std::hash<arangodb::pregel::PregelShard>()(k.pregelShard());
    return h2 ^ (h1 << 1);
  }
};
}  // namespace std
