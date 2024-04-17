#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name ), locked(false), key_value_pairs(), proposed_pairs(){}

Table::~Table()
{
  // TODO: implement
}

void Table::lock()
{
  // TODO: implement
}

void Table::unlock()
{
  // TODO: implement
}

bool Table::trylock()
{
  // TODO: implement
}

void Table::set( const std::string &key, const std::string &value )
{
  this->proposed_pairs[key] = value;
}

std::string Table::get( const std::string &key )
{
  // If the key is in the current table
  if (this->key_value_pairs.find(key) != this->key_value_pairs.end()) {

    return this->key_value_pairs.at(key);
  } 
  // If the key is in a proposed entry
  else if (this->proposed_pairs.find(key) != this->proposed_pairs.end()) {

    return this->proposed_pairs.at(key);
  } 

}

bool Table::has_key( const std::string &key )
{
  // If the key is in the commited or proposed entry map
  if (this->key_value_pairs.find(key) != this->key_value_pairs.end() || 
      this->proposed_pairs.find(key) != this->proposed_pairs.end()) {

    return true;
  }

  return false;
}

void Table::commit_changes()
{
  // Add every entry in the map with new or edited table entries to the commited table
  for (auto it : this->proposed_pairs) {
    this->key_value_pairs[it.first] = it.second;
  }
}

void Table::rollback_changes()
{
  this->proposed_pairs.clear();
}
