#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name ), mutex_is_locked( false ), key_value_pairs(), proposed_pairs() {

    pthread_mutex_init(mutex, NULL);
  }

Table::~Table()
{
  pthread_mutex_destroy(mutex);
}

void Table::lock()
{
  mutex_is_locked = true;
  pthread_mutex_lock(mutex);
}

void Table::unlock()
{
  pthread_mutex_unlock(mutex);
  mutex_is_locked = false;
}

bool Table::trylock()
{
  if (mutex_is_locked) {
    return false;
  }

  mutex_is_locked = true;
  pthread_mutex_lock(mutex);
  return true;
}

void Table::set( const std::string &key, const std::string &value )
{
  proposed_pairs[key] = value;
}

std::string Table::get( const std::string &key )
{
  // If the key is in the current table
  if (key_value_pairs.find(key) != key_value_pairs.end()) {

    return key_value_pairs.at(key);
  } 
  // If the key is in a proposed entry
  else if (proposed_pairs.find(key) != proposed_pairs.end()) {

    return proposed_pairs.at(key);
  } 

  throw OperationException("Key that does not exist requested");
  // Return statement never reached
  return "";
}

bool Table::has_key( const std::string &key )
{
  // If the key is in the commited or proposed entry map
  if (key_value_pairs.find(key) != key_value_pairs.end() || 
      proposed_pairs.find(key) != proposed_pairs.end()) {

    return true;
  }

  return false;
}

void Table::commit_changes()
{
  // Add every entry in the map with new or edited table entries to the commited table
  for (auto it : proposed_pairs) {
    key_value_pairs[it.first] = it.second;
  }
}

void Table::rollback_changes()
{
  proposed_pairs.clear();
}
