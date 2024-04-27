#ifndef TABLE_H
#define TABLE_H

#include <unordered_map>
#include <string>
#include <pthread.h>

class Table {
private:
  std::string m_name;

  pthread_mutex_t* mutex;

  bool mutex_is_locked;

  /* String keys are mapped to string values. */
  std::unordered_map<std::string, std::string> key_value_pairs;

  /* Map of a) proposed new table entries and b) entries with committed keys and proposed new values. */
  std::unordered_map<std::string, std::string> proposed_pairs;

  // copy constructor and assignment operator are prohibited
  Table( const Table & );
  Table &operator=( const Table & );

public:
  Table( const std::string &name );
  ~Table();

  std::string get_name() const { return m_name; }
  bool get_is_locked() const { return mutex_is_locked; }

  void lock();
  void unlock();
  bool trylock();

  // Note: these functions should only be called while the
  // table's lock is held!
  void set( const std::string &key, const std::string &value );
  void suggest_set( const std::string &key, const std::string &value );
  bool has_key( const std::string &key );
  std::string get( const std::string &key );
  void commit_changes();
  void rollback_changes();
};

#endif // TABLE_H
