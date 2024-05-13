#pragma once
#include <optional>
#include <string>
#include <utility>
#include <variant>

/// @brief Result 类似 Rust 中的 Result 类型，用于处理函数返回值
/// TODO: and_then, or_else, map, map_err, unwrap_or, unwrap_or_else
/// TODO:如果string作为Err会不会有性能问题，是不是可以考虑使用 string_view,
/// const char *等。

template < typename T > struct Ok {
    Ok( T value )
        : value( value ) {
    }

    T value;
};
template < typename E > struct Err {
    Err( E value )
        : value( value ) {
    }

    E value;
};

template < typename T, typename E > class Result {
  public:
    Result() {
    }

    // 不可以声明为 explicit，确保 Err<E> 可以隐式转换为 Result
    Result( const Ok< T > &ok )
        : value( ok ) {
    }

    // 不可以声明为 explicit，确保 Err<E> 可以隐式转换为 Result
    Result( const Err< E > &err )
        : value( err ) {
    }

    Result &operator=( const Ok< T > &ok ) {
        value = ok;
        return *this;
    }
    Result &operator=( const Err< E > &err ) {
        value = err;
        return *this;
    }

    bool operator==( const Result< T, E > &other ) const {
        return ( is_ok() && other.is_ok() && value == other.value ) ||
               ( is_err() && other.is_err() && value == other.value );
    }
    bool operator==( const Ok< T > &other ) const {
        return is_ok() && value == other.value;
    }
    bool operator==( const Err< E > &other ) const {
        return is_err() && value == other.value;
    }

    bool operator!=( const Result< T, E > &other ) const {
        return !( *this == other );
    }

    // nodiscard C++17标记符，表示不应该舍弃返回值。
    // 如果用户舍弃了返回值，编译器会发生相关warning。
    [[nodiscard]] bool is_ok() const {
        // holds_alaternative 定义与头文件 <variant>
        // 用于判断 value 中是否包含可选项 Ok<T>, 如果包含返回
        // true，如果不包含返回 false
        return std::holds_alternative< Ok< T > >( value );
    }

    [[nodiscard]] bool is_err() const {
        return std::holds_alternative< Err< E > >( value );
    }

    // optional 类似 Rust 中的 Option， 返回值要么是一个值，要么是空
    std::optional< T > ok() const {
        // get< 0 > 获取 value 中的 Ok<T> 的值
        if ( is_ok() ) return std::get< 0 >( value ).value;

        // std::nullopt 是一个空值，用于表示空值
        return std::nullopt;
    }

    std::optional< E > err() const {
        if ( is_err() ) return std::get< 1 >( value ).value;
        return std::nullopt;
    }

    // 解析 Result，如果是 Ok<T>，返回 T，如果是 Err<E>，抛出 E
    T unwrap() {
        if ( is_ok() ) {
            return *ok();
        }

        throw *err();
    }

    T expect( const std::string &msg ) {
        if ( is_ok() ) {
            return unwrap();
        }
        // 如果 E 是 string类型
        if constexpr ( std::is_same_v< E, std::string > )
            throw msg + ": " + unwrap_err();
        else
            throw msg + ": " + std::to_string( unwrap_err() );
    }
    E unwrap_err() {
        if ( is_err() ) {
            return *err();
        }
        throw *ok();
    }

  private:
    // variant 是 union 的高级版本，可以存储多种类型的值，更加安全
    std::variant< Ok< T >, Err< E > > value;
};