#pragma once

#include <iostream>
#include <shared_mutex>
#include <thread>
namespace bitcask {

template < class T > class WriteGuard;
template < class T > class ReadGuard;

/// @brief Read-write lock like Rust's RwLock
/// Usage:
template < typename T > class RwLock {
  public:
    RwLock( T &&data )
        : m_data( std::move( data ) ) {
    }
    ~RwLock() = default;

    ReadGuard< T > read() {
        return ReadGuard< T >( m_data, mutex );
    }
    WriteGuard< T > write() {
        return WriteGuard< T >( m_data, mutex );
    }

  private:
    std::shared_mutex mutex;
    T                 m_data;
};

template < class T > class ReadGuard {
  public:
    ReadGuard( T &data, std::shared_mutex &mutex )
        : m_data( data )
        , m_mutex( mutex ) {
        lock();
    }
    ~ReadGuard() {
        unlock();
    }

    void lock() {
        m_mutex.lock_shared();
    }
    void unlock() {
        m_mutex.unlock_shared();
    }
    T &data() {
        return m_data;
    }

  private:
    T                 &m_data;
    std::shared_mutex &m_mutex;
};

template < class T > class WriteGuard {
  public:
    WriteGuard( T &data, std::shared_mutex &mutex )
        : m_data( data )
        , m_mutex( mutex ) {
        lock();
    }
    ~WriteGuard() {
        unlock();
    }
    void lock() {
        m_mutex.lock_shared();
    }
    void unlock() {
        m_mutex.unlock_shared();
    }
    T &data() {
        return m_data;
    }

  private:
    T                 &m_data;
    std::shared_mutex &m_mutex;
};

} // namespace bitcask