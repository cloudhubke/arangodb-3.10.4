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
/// @author Julia Volmer
/// @author Markus Pfeiffer
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <compare>

#include <velocypack/Value.h>

namespace arangodb::pregel {
struct PregelShard {
  using value_type = uint16_t;

  constexpr PregelShard() : _value(InvalidPregelShardMarker) {}
  // TODO: This is for easier transition from older code
  // which just used to typedef PregelShard to be a uint16_t
  [[deprecated]] explicit PregelShard(uint64_t value)
      : _value(static_cast<value_type>(value)) {}
  explicit PregelShard(value_type value) : _value(value) {}

  static constexpr value_type InvalidPregelShardMarker =
      std::numeric_limits<value_type>::max();

  auto isValid() -> bool { return _value != InvalidPregelShardMarker; }

  auto operator<=>(PregelShard const& other) const = default;

  // TODO: This is for backwards compatibility and easy transition; as soon as
  // all uses of VPackValue conversions on PregelShard are removed, this
  // operator can go, too
  explicit operator arangodb::velocypack::Value() const {
    return VPackValue(_value);
  }

  value_type _value{};
};
constexpr auto InvalidPregelShard = PregelShard();

template<typename Inspector>
auto inspect(Inspector& f, PregelShard& x) {
  return f.object(x).fields(f.field("shardID", x._value));
}

}  // namespace arangodb::pregel

namespace std {
template<>
struct hash<arangodb::pregel::PregelShard> {
  std::size_t operator()(
      const arangodb::pregel::PregelShard& k) const noexcept {
    return std::hash<arangodb::pregel::PregelShard::value_type>()(k._value);
  }
};
}  // namespace std
