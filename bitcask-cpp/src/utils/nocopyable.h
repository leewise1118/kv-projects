#pragma once

namespace bitcask {
class Nocopyable {
  public:
    Nocopyable()                                = default;
    ~Nocopyable()                               = default;
    Nocopyable( const Nocopyable & )            = delete;
    Nocopyable &operator=( const Nocopyable & ) = delete;
};
} // namespace bitcask