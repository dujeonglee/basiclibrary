#include <cstdint>
#include <utility>
#include <functional>
#include <vector>
#include <iostream>

template <typename TYPE>
class GenericObject
{
private:
  alignas(TYPE) uint8_t obj[sizeof(TYPE)] = {0};

public:
  /* Constructors*/
  GenericObject<TYPE>() {}
  GenericObject<TYPE>(const TYPE &value) { Get() = value; }
  GenericObject<TYPE>(TYPE &&value) { Get() = std::move(value); }
  GenericObject<TYPE>(const GenericObject<TYPE> &copyobj) { Get() = copyobj.ConstGet(); }
  GenericObject<TYPE>(GenericObject<TYPE> &&moveobj) { Get() = std::move(moveobj.Get()); }

  /* Assign operators */
  GenericObject<TYPE> &operator=(const GenericObject<TYPE> &obj){Get() = obj.ConstGet();return (*this);}
  GenericObject<TYPE> &operator=(GenericObject<TYPE> &&obj){Get() = std::move(obj.Get());return (*this);}
  /* Getters */
  inline TYPE &Get()
  {
    return (*reinterpret_cast<TYPE *>(obj));
  }
  inline TYPE ConstGet() const { return (*reinterpret_cast<const TYPE *>(obj)); }
};

template <typename KEY, typename DATA>
class Map
{
private:
  std::vector<std::pair<GenericObject<KEY>, GenericObject<DATA>>> m_Table;
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
      m_Table.resize(m_TableSize, std::pair<GenericObject<KEY>, GenericObject<DATA>>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>()));
    }
    catch (const std::exception &ex)
    {
      m_TableSize /= 2;
      return;
    }
    for (uint32_t old_position = 0; old_position < m_TableSize; old_position++)
    {
      uint32_t new_position = (std::hash<KEY>{}(m_Table[old_position].first.Get()) % m_TableSize);
      do
      {
        if (m_Table[new_position].first.Get() == c_EmptyKey)
        {
          m_Table[new_position].first.Get() = m_Table[old_position].first.Get();
          m_Table[new_position].second.Get() = m_Table[old_position].second.Get();
          m_Table[old_position].first.Get() = c_EmptyKey;
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
    m_Table.resize(m_TableSize, std::pair<GenericObject<KEY>, GenericObject<DATA>>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>()));
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
      if (m_Table[position].first.Get() == c_EmptyKey)
      {
        m_Table[position].first.Get() = data.first;
        m_Table[position].second.Get() = data.second;
        m_Elements++;
        break;
      }
      // Existing item.
      else if (m_Table[position].first.Get() == data.first)
      {
        m_Table[position].first.Get() = data.first;
        m_Table[position].second.Get() = data.second;
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
      if (m_Table[position].first.Get() == c_EmptyKey)
      {
        m_Table[position].first.Get() = std::move(data.first);
        m_Table[position].second.Get() = std::move(data.second);
        m_Elements++;
        break;
      }
      // Existing item.
      else if (m_Table[position].first.Get() == data.first)
      {
        m_Table[position].first.Get() = std::move(data.first);
        m_Table[position].second.Get() = std::move(data.second);
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
      if (m_Table[position].first.Get() == key)
      {
        ret = &(m_Table[position].second.Get());
        break;
      }
      else if (m_Table[position].first.Get() == c_EmptyKey)
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
        if (m_List->m_Table[m_Position].first.Get() != m_List->c_EmptyKey)
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
      return m_List->m_Table[m_Position].second.Get();
    }
  };

  Iterator begin()
  {
    uint32_t start = 0;
    while (m_Table[start].first.Get() == c_EmptyKey)
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
