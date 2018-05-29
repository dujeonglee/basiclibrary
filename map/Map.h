#ifndef _MAP_H_
#define _MAP_H_

#include <cstdint>    // uint8_t
#include <functional> // std::hash
#include <vector>     // std::vector
namespace dujeonglee
{
namespace basiclibrary
{
namespace map
{
template <typename TYPE>
class GenericObject
{
private:
  alignas(TYPE) uint8_t obj[sizeof(TYPE)] = {0};

public:
  /* Constructors*/
  GenericObject<TYPE>() = delete;
  GenericObject<TYPE>(const TYPE &value)
  {
    new (obj) TYPE(value);
  }
  GenericObject<TYPE>(TYPE &&value)
  {
    new (obj) TYPE(std::move(value));
  }
  GenericObject<TYPE>(const GenericObject<TYPE> &copyobj)
  {
    new (obj) TYPE(copyobj.ConstGet());
  }
  GenericObject<TYPE>(GenericObject<TYPE> &&moveobj)
  {
    new (obj) TYPE(std::move(moveobj.Get()));
  }

  /* Assign operators */
  GenericObject<TYPE> &operator=(const GenericObject<TYPE> &o)
  {
    Get() = o.ConstGet();
    return (*this);
  }
  GenericObject<TYPE> &operator=(GenericObject<TYPE> &&o)
  {
    Get() = std::move(o.Get());
    return (*this);
  }
  /* Getters */
  TYPE &Get()
  {
    return (*reinterpret_cast<TYPE *>(obj));
  }
  TYPE ConstGet() const
  {
    return (*reinterpret_cast<const TYPE *>(obj));
  }
};

namespace plain
{
template <typename KEY, typename DATA, unsigned int MINSIZE = (0x1 << 16)>
class UnorderedMap
{
private:
  std::vector<std::pair<GenericObject<KEY>, GenericObject<DATA>>> m_Table;
  uint32_t m_TableSize;
  uint32_t m_Elements;
  const KEY c_EmptyKey;
  const DATA c_EmptyData;
  DATA m_EmptyData;

private:
  void ReHash(const uint32_t &newSize)
  {
    std::pair<GenericObject<KEY>, GenericObject<DATA>> tmp = std::pair<GenericObject<KEY>, GenericObject<DATA>>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData));
    uint32_t max_probes = 0;
    uint32_t probes = 0;
    const uint32_t oldSize = m_TableSize;
    if (oldSize == newSize)
    {
      return;
    }
    try
    {
      m_Table.resize(newSize, std::pair<GenericObject<KEY>, GenericObject<DATA>>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData)));
    }
    catch (const std::exception &ex)
    {
      return;
    }
    for (uint32_t old_position = 0; old_position < oldSize + max_probes; old_position++)
    {
      // Note that tmp.first.Get() = c_EmptyKey.
      std::swap(tmp, m_Table[old_position]);
      uint32_t new_position = (std::hash<KEY>{}(tmp.first.Get()) % newSize);
      probes = 0;
      do
      {
        if (m_Table[new_position].first.Get() == c_EmptyKey)
        {
          // Note that tmp.first.Get() = c_EmptyKey.
          std::swap(tmp, m_Table[new_position]);
          if (oldSize < newSize && max_probes < probes)
          {
            max_probes = probes;
          }
          break;
        }
        probes++;
        new_position++;
        if (new_position == newSize)
        {
          new_position = 0;
        }
      } while (true);
    }
    m_TableSize = newSize;
  }

public:
  UnorderedMap<KEY, DATA, MINSIZE>() = delete;
  UnorderedMap<KEY, DATA, MINSIZE>(const KEY &emptykey, const DATA &emptydata) : c_EmptyKey(emptykey), c_EmptyData(emptydata)
  {
    m_Elements = 0;
    m_TableSize = MINSIZE;
    m_Table.resize(m_TableSize, std::pair<GenericObject<KEY>, GenericObject<DATA>>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData)));
  }
  UnorderedMap<KEY, DATA, MINSIZE>(const UnorderedMap<KEY, DATA, MINSIZE> &other) : c_EmptyKey(other.c_EmptyKey), c_EmptyData(other.c_EmptyData)
  {
    m_TableSize = other.m_TableSize;
    m_Elements = other.m_Elements;
    m_Table = other.m_Table;
  }

  UnorderedMap<KEY, DATA, MINSIZE> &operator=(const UnorderedMap<KEY, DATA, MINSIZE> &other)
  {
    return UnorderedMap<KEY, DATA, MINSIZE>(other);
  }

  void Clear()
  {
    m_Table.clear();
    m_Elements = 0;
    m_TableSize = MINSIZE;
    m_Table.resize(m_TableSize, std::pair<GenericObject<KEY>, GenericObject<DATA>>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData)));
  }

  bool Insert(const std::pair<KEY, DATA> &data)
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
      position++;
      if (position == m_TableSize)
      {
        position = 0;
      }
    } while (true);
    if (m_Elements > m_TableSize * 2 / 3)
    {
      ReHash(m_TableSize * 2);
    }
    return true;
  }

  bool Insert(std::pair<KEY, DATA> &&data)
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
      position++;
      if (position == m_TableSize)
      {
        position = 0;
      }
    } while (true);
    if (m_Elements > m_TableSize * 2 / 3)
    {
      ReHash(m_TableSize * 2);
    }
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

  DATA &FindRef(const KEY &key)
  {
    uint32_t position = std::hash<KEY>{}(key) % m_TableSize;
    do
    {
      if (m_Table[position].first.Get() == key)
      {
        return (m_Table[position].second.Get());
      }
      else if (m_Table[position].first.Get() == c_EmptyKey)
      {
        m_EmptyData = c_EmptyData;
        return m_EmptyData;
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
  }

  bool Erase(const KEY &key)
  {
    uint32_t position = std::hash<KEY>{}(key) % m_TableSize;
    do
    {
      if (m_Table[position].first.Get() == key)
      {
        // remove entry.
        m_Table[position].first.Get() = c_EmptyKey;

        // rehash entries
        for (uint32_t old_position = (position + 1 == m_TableSize ? 0 : position + 1);
             m_Table[old_position].first.Get() != c_EmptyKey;
             old_position = (old_position + 1 == m_TableSize ? 0 : old_position + 1))
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
        m_Elements--;
        if (m_Elements <= (m_TableSize / 4))
        {
          ReHash((m_TableSize / 2 < MINSIZE ? MINSIZE : m_TableSize / 2));
        }
        return true;
      }
      else if (m_Table[position].first.Get() == c_EmptyKey)
      {
        return false;
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
  }

  class Iterator
  {
  private:
    UnorderedMap<KEY, DATA, MINSIZE> *m_List;
    uint32_t m_Position;

  public:
    Iterator(UnorderedMap<KEY, DATA, MINSIZE> *list, uint32_t position) : m_List(list), m_Position(position) {}
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
}

namespace robinhood
{
template <typename KEY, typename DATA, unsigned int MINSIZE = (0x1 << 16)>
class UnorderedMap
{
private:
  std::vector<std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t>> m_Table;
  uint32_t m_TableSize;
  uint32_t m_Elements;
  const KEY c_EmptyKey;
  const DATA c_EmptyData;
  DATA m_EmptyData;

private:
  void ReHash(const uint32_t &newSize)
  {
    std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t> tmp =
        std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData), 0);
    uint32_t max_probes = 0;
    uint32_t probes = 0;
    const uint32_t oldSize = m_TableSize;
    if (oldSize == newSize)
    {
      return;
    }
    try
    {
      m_Table.resize(newSize, std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData), 0));
    }
    catch (const std::exception &ex)
    {
      return;
    }
    // ReHash
    for (uint32_t old_position = 0; old_position < oldSize + max_probes; old_position++)
    {
      std::swap(tmp, m_Table[old_position]);
      std::get<2>(tmp) = 0;
      uint32_t position = std::hash<KEY>{}(std::get<0>(tmp).Get()) % newSize;
      probes = 0;
      do
      {
        // Item with new key.
        if (std::get<0>(m_Table[position]).Get() == c_EmptyKey)
        {
          std::swap(tmp, m_Table[position]);
          if (oldSize < newSize && max_probes < probes)
          {
            max_probes = probes;
          }
          break;
        }
        // Robinhood
        else if (std::get<2>(m_Table[position]) < std::get<2>(tmp))
        {
          std::swap(tmp, m_Table[position]);
        }
        probes++;
        position++;
        if (position == newSize)
        {
          position = 0;
        }
        std::get<2>(tmp)++;
      } while (true);
    }
    m_TableSize = newSize;
  }

public:
  UnorderedMap<KEY, DATA, MINSIZE>() = delete;
  UnorderedMap<KEY, DATA, MINSIZE>(const KEY &emptykey, const DATA &emptydata) : c_EmptyKey(emptykey), c_EmptyData(emptydata)
  {
    m_Elements = 0;
    m_TableSize = MINSIZE;
    m_Table.resize(m_TableSize, std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData), 0));
  }
  UnorderedMap<KEY, DATA, MINSIZE>(const UnorderedMap<KEY, DATA, MINSIZE> &other) : c_EmptyKey(other.c_EmptyKey), c_EmptyData(other.c_EmptyData)
  {
    m_TableSize = other.m_TableSize;
    m_Elements = other.m_Elements;
    m_Table = other.m_Table;
  }

  UnorderedMap<KEY, DATA, MINSIZE> &operator=(const UnorderedMap<KEY, DATA, MINSIZE> &other)
  {
    return UnorderedMap<KEY, DATA, MINSIZE>(other);
  }

  void Clear()
  {
    m_Table.clear();
    m_Elements = 0;
    m_TableSize = MINSIZE;
    m_Table.resize(m_TableSize, std::pair<GenericObject<KEY>, GenericObject<DATA>>(GenericObject<KEY>(c_EmptyKey), GenericObject<DATA>(c_EmptyData)));
  }

  bool Insert(const std::pair<KEY, DATA> &data)
  {
    // Table is full.
    if (m_Elements == m_TableSize)
    {
      return false;
    }
    // Get insert position.
    uint32_t position = std::hash<KEY>{}(data.first) % m_TableSize;
    std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t> item =
        std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t>(GenericObject<KEY>(data.first), GenericObject<KEY>(data.second), 0);
    do
    {
      // Item with new key.
      if (std::get<0>(m_Table[position]).Get() == c_EmptyKey)
      {
        std::get<0>(m_Table[position]).Get() = std::get<0>(item).Get();
        std::get<1>(m_Table[position]).Get() = std::get<1>(item).Get();
        std::get<2>(m_Table[position]) = std::get<2>(item);
        m_Elements++;
        break;
      }
      // Existing item.
      else if (std::get<0>(m_Table[position]).Get() == std::get<0>(item).Get())
      {
        std::get<0>(m_Table[position]).Get() = std::get<0>(item).Get();
        std::get<1>(m_Table[position]).Get() = std::get<1>(item).Get();
        std::get<2>(m_Table[position]) = std::get<2>(item);
        break;
      }
      // Robinhood
      else if (std::get<2>(m_Table[position]) < std::get<2>(item))
      {
        std::swap(std::get<0>(m_Table[position]).Get(), std::get<0>(item).Get());
        std::swap(std::get<1>(m_Table[position]).Get(), std::get<1>(item).Get());
        std::swap(std::get<2>(m_Table[position]), std::get<2>(item));
      }
      position++;
      if (position == m_TableSize)
      {
        position = 0;
      }
      std::get<2>(item)++;
    } while (true);
    if (m_Elements > m_TableSize * 2 / 3)
    {
      ReHash(m_TableSize * 2);
    }
    return true;
  }

  bool Insert(std::pair<KEY, DATA> &&data)
  {
    // Table is full.
    if (m_Elements == m_TableSize)
    {
      return false;
    }
    // Get insert position.
    uint32_t position = std::hash<KEY>{}(data.first) % m_TableSize;
    std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t> item =
        std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t>(GenericObject<KEY>(std::move(data.first)), GenericObject<DATA>(std::move(data.second)), 0);
    do
    {
      // Item with new key.
      if (std::get<0>(m_Table[position]).Get() == c_EmptyKey)
      {
        std::get<0>(m_Table[position]).Get() = std::get<0>(item).Get();
        std::get<1>(m_Table[position]).Get() = std::get<1>(item).Get();
        std::get<2>(m_Table[position]) = std::get<2>(item);
        m_Elements++;
        break;
      }
      // Existing item.
      else if (std::get<0>(m_Table[position]).Get() == std::get<0>(item).Get())
      {
        std::get<0>(m_Table[position]).Get() = std::get<0>(item).Get();
        std::get<1>(m_Table[position]).Get() = std::get<1>(item).Get();
        std::get<2>(m_Table[position]) = std::get<2>(item);
        break;
      }
      // Robinhood
      else if (std::get<2>(m_Table[position]) < std::get<2>(item))
      {
        std::swap(std::get<0>(m_Table[position]).Get(), std::get<0>(item).Get());
        std::swap(std::get<1>(m_Table[position]).Get(), std::get<1>(item).Get());
        std::swap(std::get<2>(m_Table[position]), std::get<2>(item));
      }
      position++;
      if (position == m_TableSize)
      {
        position = 0;
      }
      std::get<2>(item)++;
    } while (true);
    if (m_Elements > m_TableSize * 2 / 3)
    {
      ReHash(m_TableSize * 2);
    }
    return true;
  }

  DATA *FindPtr(const KEY &key)
  {
    uint32_t position = std::hash<KEY>{}(key) % m_TableSize;
    DATA *ret = nullptr;
    do
    {
      if (std::get<0>(m_Table[position]).Get() == key)
      {
        ret = &(std::get<1>(m_Table[position]).Get());
        break;
      }
      else if (std::get<0>(m_Table[position]).Get() == c_EmptyKey)
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

  DATA &FindRef(const KEY &key)
  {
    uint32_t position = std::hash<KEY>{}(key) % m_TableSize;
    do
    {
      if (std::get<0>(m_Table[position]).Get() == key)
      {
        return (std::get<1>(m_Table[position]).Get());
      }
      else if (std::get<0>(m_Table[position]).Get() == c_EmptyKey)
      {
        m_EmptyData = c_EmptyData;
        return m_EmptyData;
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
  }

  bool Erase(const KEY &key)
  {
    uint32_t position = std::hash<KEY>{}(key) % m_TableSize;
    do
    {
      if (std::get<0>(m_Table[position]).Get() == key)
      {
        // remove entry.
        std::get<0>(m_Table[position]).Get() = c_EmptyKey;

        // rehash entries
        for (uint32_t old_position = (position + 1 == m_TableSize ? 0 : position + 1);
             std::get<0>(m_Table[old_position]).Get() != c_EmptyKey;
             old_position = (old_position + 1 == m_TableSize ? 0 : old_position + 1))
        {
          uint32_t new_position = std::hash<KEY>{}(std::get<0>(m_Table[old_position]).Get()) % m_TableSize;
          std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t> item =
              std::tuple<GenericObject<KEY>, GenericObject<DATA>, uint32_t>(GenericObject<KEY>(std::get<0>(m_Table[old_position]).Get()), GenericObject<DATA>(std::get<1>(m_Table[old_position]).Get()), 0);
          std::get<0>(m_Table[old_position]).Get() = c_EmptyKey;
          do
          {
            // Item with new key.
            if (std::get<0>(m_Table[new_position]).Get() == c_EmptyKey)
            {
              std::get<0>(m_Table[new_position]).Get() = std::get<0>(item).Get();
              std::get<1>(m_Table[new_position]).Get() = std::get<1>(item).Get();
              std::get<2>(m_Table[new_position]) = std::get<2>(item);
              break;
            }
            // Existing item.
            else if (std::get<0>(m_Table[new_position]).Get() == std::get<0>(item).Get())
            {
              std::get<0>(m_Table[new_position]).Get() = std::get<0>(item).Get();
              std::get<1>(m_Table[new_position]).Get() = std::get<1>(item).Get();
              std::get<2>(m_Table[new_position]) = std::get<2>(item);
              break;
            }
            // Robinhood
            else if (std::get<2>(m_Table[new_position]) < std::get<2>(item))
            {
              std::swap(std::get<0>(m_Table[new_position]).Get(), std::get<0>(item).Get());
              std::swap(std::get<1>(m_Table[new_position]).Get(), std::get<1>(item).Get());
              std::swap(std::get<2>(m_Table[new_position]), std::get<2>(item));
            }
            new_position++;
            if (new_position == m_TableSize)
            {
              new_position = 0;
            }
            std::get<2>(item)++;
          } while (true);
        }
        m_Elements--;
        if (m_Elements <= (m_TableSize / 4))
        {
          ReHash((m_TableSize / 2 < MINSIZE ? MINSIZE : m_TableSize / 2));
        }
        return true;
      }
      else if (std::get<0>(m_Table[position]).Get() == c_EmptyKey)
      {
        return false;
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
  }

  class Iterator
  {
  private:
    UnorderedMap<KEY, DATA, MINSIZE> *m_List;
    uint32_t m_Position;

  public:
    Iterator(UnorderedMap<KEY, DATA, MINSIZE> *list, uint32_t position) : m_List(list), m_Position(position) {}
    Iterator(const Iterator &it) : m_List(it.m_List), m_Position(it.m_Position) {}
    Iterator &operator++()
    {
      do
      {
        m_Position++;
        if (std::get<0>(m_List->m_Table[m_Position]).Get() != m_List->c_EmptyKey)
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
      return std::get<1>(m_List->m_Table[m_Position]).Get();
    }
  };

  Iterator begin()
  {
    uint32_t start = 0;
    while (std::get<0>(m_Table[start]).Get() == c_EmptyKey)
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
}
}
}
}

#endif