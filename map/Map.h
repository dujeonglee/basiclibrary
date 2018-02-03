#include <cstdint>
#include <utility>
#include <functional>
#include <vector>
#include <iostream>

template <typename KEY, typename DATA>
class Map
{
private:
  std::vector<std::pair<KEY, DATA>> m_Table;
  uint32_t m_TableSize;
  uint32_t m_Elements;
  const KEY c_EmptyKey;

private:
  void ReHash()
  {
    if (m_Elements <= m_TableSize * 2 / 3)
    {
      return;
    }
    if (m_TableSize == 0xffffffff)
    {
      return;
    }
    m_TableSize *= 2;
    try
    {
      m_Table.resize(m_TableSize, std::pair<KEY, DATA>(c_EmptyKey, DATA()));
    }
    catch (const std::exception &ex)
    {
      m_TableSize /= 2;
      return;
    }
    for (uint32_t old_position = 0; old_position < m_TableSize; old_position++)
    {
      uint32_t new_position = (std::hash<KEY>{}(m_Table[old_position].first) % m_TableSize);
      do
      {
        if (m_Table[new_position].first == c_EmptyKey)
        {
          m_Table[new_position].first = m_Table[old_position].first;
          m_Table[new_position].second = m_Table[old_position].second;
          m_Table[old_position].first = c_EmptyKey;
          break;
        }
        else if (new_position == old_position)
        {
          break;
        }
        else
        {
          new_position++;
          if (new_position == m_TableSize)
          {
            new_position = 0;
          }
        }
      } while (true);
    }
  }

public:
  Map<KEY, DATA>() = delete;
  Map<KEY, DATA>(const KEY &emptykey) : c_EmptyKey(emptykey)
  {
    m_Elements = 0;
    m_TableSize = 0x1 << 16;
    m_Table.resize(m_TableSize, std::pair<KEY, DATA>(c_EmptyKey, DATA()));
  }
  Map<KEY, DATA>(const Map<KEY, DATA> &other)
  {
    c_EmptyKey = other.c_EmptyKey;
    m_TableSize = other.m_TableSize;
    m_Elements = other.m_Elements;
    m_Table = other.m_Table;
  }

  Map<KEY, DATA> &operator=(const Map<KEY, DATA> &other)
  {
    return Map<KEY, DATA>(other);
  }

  bool Push(const std::pair<KEY, DATA> &data)
  {
    // Table is full.
    if (m_Elements == m_TableSize)
    {
      return false;
    }
    // Get insert position.
    uint32_t position = std::hash<KEY>{}(data.first) % m_TableSize;
    do
    {
      // Item with new key.
      if (m_Table[position].first == c_EmptyKey)
      {
        m_Table[position] = data;
        m_Elements++;
        break;
      }
      // Existing item.
      else if (m_Table[position].first == data.first)
      {
        m_Table[position] = data;
        break;
      }
      else
      {
        position++;
        if (position == m_TableSize)
        {
          position = 0;
        }
      }
    } while (true);
    ReHash();
    return true;
  }

  bool Push(std::pair<KEY, DATA> &&data)
  {
    // Table is full.
    if (m_Elements == m_TableSize)
    {
      return false;
    }
    // Get insert position.
    uint32_t position = std::hash<KEY>{}(data.first) % m_TableSize;
    do
    {
      // Item with new key.
      if (m_Table[position].first == c_EmptyKey)
      {
        m_Table[position].first = data.first;
        m_Table[position].second = data.second;
        m_Elements++;
        break;
      }
      // Existing item.
      else if (m_Table[position].first == data.first)
      {
        m_Table[position].first = data.first;
        m_Table[position].second = data.second;
        break;
      }
      else
      {
        position++;
        if (position == m_TableSize)
        {
          position = 0;
        }
      }
    } while (true);
    ReHash();
    return true;
  }

  DATA *FindPtr(const KEY &key)
  {
    uint32_t position = std::hash<KEY>{}(key) % m_TableSize;
    DATA *ret = nullptr;
    do
    {
      if (m_Table[position].first == key)
      {
        ret = &(m_Table[position].second);
        break;
      }
      else if (m_Table[position].first == c_EmptyKey)
      {
        break;
      }
      else
      {
        position++;
        if (position == m_TableSize)
        {
          position = 0;
        }
      }
    } while (true);
    return ret;
  }

  class Iterator
  {
  private:
    Map<KEY, DATA> *m_List;
    uint32_t m_Position;

  public:
    Iterator(Map<KEY, DATA> *list, uint32_t position) : m_List(list), m_Position(position) {}
    Iterator(const Iterator &it) : m_List(it.m_List), m_Position(it.m_Position) {}
    Iterator &operator++()
    {
      do
      {
        m_Position++;
        if (m_List->m_Table[m_Position].first != m_List->c_EmptyKey)
        {
          break;
        }
      } while (m_Position < m_List->m_Table.size());
      return *this;
    }
    Iterator operator++(int)
    {
      Iterator tmp(*this);
      operator++();
      return tmp;
    }
    bool operator==(const Iterator &rhs) const
    {
      return (m_List == rhs.m_List) && (m_Position == rhs.m_Position);
    }
    bool operator!=(const Iterator &rhs) const
    {
      return (m_List != rhs.m_List) || (m_Position != rhs.m_Position);
    }
    DATA &operator*()
    {
      return m_List->m_Table[m_Position].second;
    }
  };

  Iterator begin()
  {
    uint32_t start = 0;
    while (m_Table[start].first == c_EmptyKey)
    {
      start++;
    }
    return Iterator(this, start);
  }

  Iterator end()
  {
    return Iterator(this, m_TableSize);
  }

  const uint32_t Size()
  {
    return m_Elements;
  }
};