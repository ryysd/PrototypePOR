#ifndef POR_MODULE_UTIL_PROFILER_H_
#define POR_MODULE_UTIL_PROFILER_H_

#include <chrono> // NOLINT
#include <sstream>
#include <map>
#include <queue>
#include <vector>
#include <string>
#include "../common.h"

namespace profiler {
  class Profiler {
   public:
    ~Profiler() { for (auto kv : timers_) { delete kv.second; } }

    static Profiler& GetInstance() {
      static Profiler instance;
      return instance;
    }

    void Start(const std::string& name) {
      if (timers_.find(name) == timers_.end()) timers_.insert(std::make_pair(name, new ProfilerTimer()));
      timers_.at(name)->Start();
    }

    void Stop(const std::string& name) {
      timers_.at(name)->Stop();
    }

    void Dump() {
      unsigned column_width = 15;
      unsigned fix_width = 2;
      std::string separator = " | ";
      std::vector<std::string> labels{"name", "incl_ratio [%]", "incl_total [ms]", "incl_ave [ms]", "call"};
      float total_time = timers_.at("total")->total();
      std::stringstream sstream;

      sstream << MakeLabels(labels, column_width, separator) << std::endl;

      auto comp = [](DumpObject a, DumpObject b) { return a.inclusive_time_ratio() > b.inclusive_time_ratio(); };
      std::priority_queue<DumpObject, std::vector<DumpObject>, decltype(comp)> dump_objects(comp);
      float real_total_time = 0;
      for (auto kv : timers_) {
        std::string name = kv.first;
        dump_objects.push(DumpObject(
              kv.first,
              kv.second->average(),
              kv.second->total(),
              ((kv.second->total() * 100) / total_time),
              kv.second->call_count()));

        real_total_time += kv.second->total();
      }
      float other_total_time = total_time - (real_total_time - total_time);
      dump_objects.push(DumpObject("other", 0, other_total_time, ((other_total_time * 100) / total_time), 0));

      while (!dump_objects.empty()) {
        sstream << dump_objects.top().ToString(column_width, fix_width, separator) << std::endl;
        dump_objects.pop();
      }

      std::cout << sstream.str();
    }

   private:
    Profiler() {}

    std::string MakeLabels(const std::vector<std::string>& labels, unsigned column_width, const std::string& separator) const {
      std::stringstream ss;
      for (auto label : labels) { ss << std::setw(column_width) << label << separator; }

      return ss.str();
    }

    class ProfilerTimer {
     public:
      ProfilerTimer() : total_(0), call_count_(0) {}
      void Start() { start_ = std::chrono::high_resolution_clock::now(); }
      void Stop() { total_ += std::chrono::high_resolution_clock::now() - start_; call_count_++; }

      float total() const { return (static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(total_).count())) / 1000; }
      unsigned call_count() const { return call_count_; }
      float average() const { return total() / call_count_; }

     private:
      std::chrono::high_resolution_clock::time_point start_;
      std::chrono::high_resolution_clock::duration total_;
      unsigned call_count_;
    };

    class DumpObject {
     public:
      DumpObject(std::string name, float inclusive_average_time, float inclusive_total_time, float inclusive_time_ratio, unsigned call_count)
        : name_(name), inclusive_averagel_time_(inclusive_average_time),
        inclusive_total_time_(inclusive_total_time), inclusive_time_ratio_(inclusive_time_ratio), call_count_(call_count) {}

      const std::string& name() const { return name_; }
      float inclusive_averate_time() const { return inclusive_averagel_time_; }
      float inclusive_total_time() const { return inclusive_total_time_; }
      float inclusive_time_ratio() const { return inclusive_time_ratio_; }
      unsigned call_count() const { return call_count_; }

      std::string ToString(unsigned column_width, unsigned fix_width, std::string separator) const {
        std::stringstream ss;

        ss << std::fixed << std::setprecision(fix_width) <<
          std::setw(column_width) << name_ << separator <<
          std::setw(column_width) << inclusive_time_ratio_ << separator <<
          std::setw(column_width) << inclusive_total_time_ << separator <<
          std::setw(column_width) << inclusive_averagel_time_ << separator <<
          std::setw(column_width) << call_count_ << separator;

        return ss.str();
      }

     private:
      std::string name_;
      float inclusive_averagel_time_;
      float inclusive_total_time_;
      float inclusive_time_ratio_;
      unsigned call_count_;
    };

    std::map<std::string, ProfilerTimer*> timers_;

    DISALLOW_COPY_AND_ASSIGN(Profiler);
  };

  inline void start(const std::string& name) { Profiler::GetInstance().Start(name); }
  inline void stop(const std::string& name) { Profiler::GetInstance().Stop(name); }
  inline void start_scope() { Profiler::GetInstance().Start("total"); }
  inline void end_scope() { Profiler::GetInstance().Stop("total"); }
  inline void dump() { Profiler::GetInstance().Dump(); }
}  // namespace profiler

#endif  // POR_MODULE_UTIL_PROFILER_H_
