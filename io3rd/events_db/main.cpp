#include <iostream>
#include <unordered_map>
#include <deque>
#include <time.h>

class RobotStatistics {
  
  void OnEvent(time_t now, int userId) {
      for (auto& [_, events] : user2events) {
          clean(now, events);
      }
      
      user2events[userId].push_back(now);
  }

  int GetRobotCount(time_t now) {
      int usersCount = 0;
      for (const auto& pair : user2events) {
          if (get_last_five_minutes(now, pair.second) >= 1000) {
              ++usersCount;
          }
      }
      return usersCount;
  }

  using events_list_t = std::deque<time_t>;

  void clean(time_t now, const events_list_t& events) {
    while (now  - events.front() > 60*5) {
          events.pop_front();
    }
  }

  private:
    std::unordered_map<int, events_list_t> user2events;
  
};

int main() {
  
    RobotStatistics r;
}