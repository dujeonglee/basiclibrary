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
  const KEY c_NULLKEY;
  DATA c_NULLDATA;
  uint32_t m_StartIndex;

private:
  void Put(uint32_t hint, const std::pair<KEY, DATA> &data)
  {
    /**
      * Open addressing.
      * Linear probing.
      */
    do
    {
      if (m_Table[hint].first == c_NULLKEY)
      {
        m_Table[hint] = data;
        if(m_StartIndex > hint)
        {
          m_StartIndex = hint;
        }
        break;
      }
      else
      {
        hint++;
      }
      if (hint == m_TableSize)
      {
        hint = 0;
      }
    } while (true);
  }

  void Put(const std::pair<KEY, DATA> &data)
  {
    /**
      * Open addressing.
      * Linear probing.
      */
    uint32_t index = std::hash<KEY>{}(data.first) % m_TableSize;
    do
    {
      if (m_Table[index].first == c_NULLKEY)
      {
        m_Table[index] = data;
        if(m_StartIndex > index)
        {
          m_StartIndex = index;
        }
        break;
      }
      else
      {
        index++;
      }
      if (index == m_TableSize)
      {
        index = 0;
      }
    } while (true);
  }

  void Put(std::pair<KEY, DATA> &&data)
  {
    /**
      * Open addressing.
      * Linear probing.
      */
    uint32_t index = std::hash<KEY>{}(data.first) % m_TableSize;
    do
    {
      if (m_Table[index].first == c_NULLKEY)
      {
        std::swap(m_Table[index], data);
        if(m_StartIndex > index)
        {
          m_StartIndex = index;
        }
        break;
      }
      else
      {
        index++;
      }
      if (index == m_TableSize)
      {
        index = 0;
      }
    } while (true);
  }

  void Put(uint32_t hint, std::pair<KEY, DATA> &&data)
  {
    /**
      * Open addressing.
      * Linear probing.
      */
    do
    {
      if (m_Table[hint].first == c_NULLKEY)
      {
        std::swap(m_Table[hint], data);
        if(m_StartIndex > hint)
        {
          m_StartIndex = hint;
        }
        break;
      }
      else
      {
        hint++;
      }
      if (hint == m_TableSize)
      {
        hint = 0;
      }
    } while (true);
  }

  bool ReHash()
  {
    /**
     * Double the size of hash table.
     * Perform rehashing on the old elements.
     */
    if (m_TableSize == 0xffffffff)
    {
      return false;
    }
    m_TableSize *= 2;
    try
    {
      m_Table.resize(m_TableSize, std::pair<KEY, DATA>(c_NULLKEY, (DATA)0));
    }
    catch (const std::exception &ex)
    {
      m_TableSize /= 2;
      return false;
    }
    m_StartIndex = m_TableSize;
    for (uint32_t i = 0; i < m_TableSize / 2; i++)
    {
      const uint32_t new_index = (std::hash<KEY>{}(m_Table[i].first) % m_TableSize);
      if (new_index == i)
      {
        continue;
      }
      Put(new_index, std::move(m_Table[i]));
      m_Table[i].first = c_NULLKEY;
    }
  }

public:
  Map<KEY, DATA>() = delete;
  Map<KEY, DATA>(const KEY &nullkey) : c_NULLKEY(nullkey)
  {
    m_Elements = 0;
    m_TableSize = 0x1 << 20;
    m_Table.resize(m_TableSize, std::pair<KEY, DATA>(c_NULLKEY, (DATA)0));
    m_StartIndex = m_TableSize;
  }
  Map<KEY, DATA>(const KEY &nullkey, uint32_t size) : c_NULLKEY(nullkey)
  {
    m_Elements = 0;
    m_TableSize = size;
    m_Table.resize(m_TableSize, std::pair<KEY, DATA>(c_NULLKEY, (DATA)0));
    m_StartIndex = m_TableSize;
  }
  Map<KEY, DATA>(const Map<KEY, DATA> &other)
  {
    c_NULLKEY = other.c_NULLKEY;
    m_TableSize = other.m_TableSize;
    m_Elements = other.m_Elements;
    m_Table = other.m_Table;
    m_StartIndex = m_TableSize;
  }

  Map<KEY, DATA> &operator=(const Map<KEY, DATA> &other)
  {
    return Map<KEY, DATA>(other);
  }

  bool Push(const std::pair<KEY, DATA> &data)
  {
    if (m_Elements == m_TableSize)
    {
      return false;
    }
    Put(data);
    m_Elements++;
    if (m_Elements > m_TableSize / 2)
    {
      ReHash();
    }
    return true;
  }

  bool Push(std::pair<KEY, DATA> &&data)
  {
    if (m_Elements == m_TableSize)
    {
      return false;
    }
    Put(data);
    m_Elements++;
    if (m_Elements > m_TableSize / 2)
    {
      ReHash();
    }
    return true;
  }

  DATA &FindRef(const KEY &key)
  {
    uint32_t index = std::hash<KEY>{}(key) % m_TableSize;
    do
    {
      if (m_Table[index].first == key)
      {
        return m_Table[index].second;
        break;
      }
      else if (m_Table[index].first == c_NULLKEY)
      {
        return c_NULLDATA;
      }
      else
      {
        index++;
        if (index == m_TableSize)
        {
          index = 0;
        }
      }
    } while (true);
  }

  DATA *FindPtr(const KEY &key)
  {
    uint32_t index = std::hash<KEY>{}(key) % m_TableSize;
    do
    {
      if (m_Table[index].first == key)
      {
        return &m_Table[index].second;
        break;
      }
      else if (m_Table[index].first == c_NULLKEY)
      {
        return nullptr;
      }
      else
      {
        index++;
        if (index == m_TableSize)
        {
          index = 0;
        }
      }
    } while (true);
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
        if (m_List->m_Table[m_Position].first != m_List->c_NULLKEY)
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
    return std::move(Iterator(this, m_StartIndex));
  }

  Iterator end()
  {
    return std::move(Iterator(this, m_TableSize));
  }
};
