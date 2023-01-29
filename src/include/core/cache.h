/**
 * @file cache.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 05 2023
 *
 * This is a header file implementing the LRU Cache
 */

#ifndef SRC_INCLUDE_CORE_CACHE_H_
#define SRC_INCLUDE_CORE_CACHE_H_

#include <core/utils.h>

#include <memory>
#include <mutex>         // NOLINT
#include <shared_mutex>  // NOLINT
#include <string>
#include <unordered_map>
#include <vector>

namespace TURTLE_SERVER {

/* default cache size 10 MB */
static constexpr size_t DEFAULT_CACHE_CAPACITY = 10 * 1024 * 1024;

/* get the current UTC time in milliseconds */
auto GetTimeUtc() noexcept -> uint64_t;

/**
 * An concurrent LRU cache to reduce load on server disk I/O and improve the
 * responsiveness It uses doubly-linked list and hashmap to achieve O(1) time
 * seek, insert, update nodes that are closer to the header are to be victims to
 * be evicted next time, i.e with older timestamp nodes newly-added or accessed
 * are closer to the tail, i.e. with newer timestamp
 */
class Cache {
 public:
  /**
   * Helper class inside the Cache
   * It represents a single file cached in the form of an unsigned char vector
   * and serves as a node in the doubly-linked list data structure
   */
  class CacheNode {
    friend class Cache;

   public:
    CacheNode() noexcept;
    CacheNode(const std::string& identifier,
              const std::vector<unsigned char>& data);
    void SetIdentifier(const std::string& identifier);
    void SetData(const std::vector<unsigned char>& data);
    void Serialize(std::vector<unsigned char>& destination);  // NOLINT
    auto Size() const noexcept -> size_t;
    void UpdateTimestamp() noexcept;
    auto GetTimestamp() const noexcept -> uint64_t;

   private:
    /* the resource identifier for this node */
    std::string identifier_;
    /* may contain binary data */
    std::vector<unsigned char> data_;
    /* the timestamp of last access in milliseconds */
    uint64_t last_access_{0};
    CacheNode* prev_{nullptr};
    CacheNode* next_{nullptr};
  };

  explicit Cache(size_t capacity = DEFAULT_CACHE_CAPACITY) noexcept;

  NON_COPYABLE_AND_MOVEABLE(Cache);

  auto GetOccupancy() const noexcept -> size_t;

  auto GetCapacity() const noexcept -> size_t;

  /**
   * Given the resource url to search, if found
   * populate the destination buffer and return true
   * if not exists, return false
   */
  auto TryLoad(const std::string& resource_url,
               std::vector<unsigned char>& destination) -> bool;  // NOLINT

  /**
   * Given the resource_url and content, try to insert it into the cache
   * return true if success, false otherwise
   * failure reason could be that the content is too big or identical
   * resource_url already cached
   */
  auto TryInsert(const std::string& resource_url,
                 const std::vector<unsigned char>& source) -> bool;

  /**
   * Remove everything in the cache
   */
  void Clear();

 private:
  /**
   * Evict out the head cache node to save space
   */
  void EvictOne() noexcept;

  /**
   * Helper function to remove a node from the doubly-linked list
   * essentially re-wire the prev and next pointers to each other
   */
  void RemoveFromList(std::shared_ptr<CacheNode> node) noexcept;

  /**
   * Append a node to the tail of the doubly-linked list
   */
  void AppendToListTail(std::shared_ptr<CacheNode> node) noexcept;

  /* concurrency */
  std::shared_mutex mtx_;
  /* map a key (resource name) to the corresponding cache node if exists */
  std::unordered_map<std::string, std::shared_ptr<CacheNode>> mapping_;
  /* the upper limit of cache storage capacity in bytes */
  const size_t capacity_;
  /* current occupancyin bytes */
  size_t occupancy_{0};
  /* the dummy sentinel header in doubly-linked list */
  const std::shared_ptr<CacheNode> header_;
  /* the dummy sentinel tailer in doubly-linked list */
  const std::shared_ptr<CacheNode> tailer_;
};

}  // namespace TURTLE_SERVER

#endif  // SRC_INCLUDE_CORE_CACHE_H_
