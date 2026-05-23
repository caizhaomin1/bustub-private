//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"
#include <chrono>
namespace bustub {

LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool { 
    bool founded = false;

    latch_.lock();
    size_t min_k_access = SIZE_MAX; 
    auto it = node_store_.begin();
    while (it != node_store_.end()) {
        LRUKNode node = it->second;
        size_t k_access = SIZE_MAX;
        if (node.history_.size() >= k_) {
            k_access = *(std::next(node.history_.begin(), k_ - 1));
        }
         if (node.history_.size() > 0) {
            k_access = node.history_.front();
        }
        if (k_access < min_k_access) {
            min_k_access = k_access;
            *frame_id = node.fid_;
            founded = true;
        }
    }

    if (founded) {
        node_store_.erase(*frame_id);
        replacer_size_--;
    }

    latch_.unlock();
    
    return founded;
 }

void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
    latch_.lock();
    LRUKNode node = node_store_.find(frame_id)->second;
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    size_t timestamp_ms = static_cast<size_t>(ms.count());
    node.history_.push_front(timestamp_ms);
    latch_.unlock();
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
    if (node_store_.find(frame_id) == node_store_.end()) {
        exit(1);
    }
    
    latch_.lock();
    LRUKNode node = node_store_.find(frame_id)->second;
    if (set_evictable && !node.is_evictable_) {
        replacer_size_++;
    } 
    if (!set_evictable && node.is_evictable_) {
        replacer_size_--;
    }
    latch_.unlock();
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
    if (node_store_.find(frame_id) == node_store_.end()) {
        return;
    }
    
    latch_.lock();
    LRUKNode node = node_store_.find(frame_id)->second;
    if (node.is_evictable_) {
        replacer_size_--;
    } else {
        exit(1);
    }
    node_store_.erase(frame_id);
    latch_.unlock();
}

auto LRUKReplacer::Size() -> size_t { return replacer_size_; }

}  // namespace bustub
