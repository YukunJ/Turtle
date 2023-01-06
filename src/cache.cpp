/**
 * @file cache.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 05 2023
 *
 * This is an implementation file implementing LRU Cache
 */

#include "cache.h"

#include <chrono>  // NOLINT
namespace TURTLE_SERVER {

auto GetTimeUtc() noexcept -> uint64_t {
  auto mill_since_epoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();
  return mill_since_epoch;
}

Cache::CacheNode::CacheNode() noexcept {}

Cache::CacheNode::CacheNode(const std::string &identifier,
                            const std::vector<unsigned char> &data) {}

void Cache::CacheNode::SetIdentifier(const std::string &identifier) {}

void Cache::CacheNode::SetData(const std::vector<unsigned char> &data) {}

void Cache::CacheNode::Serialize(std::vector<unsigned char> &destination) {}

auto Cache::CacheNode::Size() const noexcept -> size_t { return 0; }

void Cache::CacheNode::UpdateTimestamp() noexcept {}

auto Cache::CacheNode::GetTimestamp() const noexcept -> uint64_t { return 0; }

Cache::Cache(size_t capacity) noexcept : capacity_(capacity) {}

auto Cache::GetOccupancy() const noexcept -> size_t { return 0; }

auto Cache::GetCapacity() const noexcept -> size_t { return 0; }

auto Cache::TryLoad(const std::string &resource_url,
                    std::vector<unsigned char> &destination) -> bool {
  return false;
}

auto Cache::TryInsert(const std::string &resource_url,
                      const std::vector<unsigned char> &source) -> bool {
  return false;
}

void Cache::RemoveFromList(
    std::shared_ptr<TURTLE_SERVER::Cache::CacheNode> node) noexcept {}

void Cache::AppendToListTail(
    std::shared_ptr<TURTLE_SERVER::Cache::CacheNode> node) noexcept {}
}  // namespace TURTLE_SERVER
