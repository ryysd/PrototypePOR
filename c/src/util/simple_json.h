#ifndef UTIL_SIMPLE_JSON_H_
#define UTIL_SIMPLE_JSON_H_

#include <map>
#include <string>
#include <vector>

namespace simplejson {
  std::string make_json_document(const std::string& string) {
    return "{" + string + "}";
  }

  std::string make_json_key_value(const std::string& key, const std::string& value) {
    return "\"" + key + "\":" + value;
  }

  std::string make_json_array(const std::vector<std::string>& string_array) {
    std::string array_string;
    for (auto string : string_array) array_string += "\"" + string + "\"" + ",";
    if (!array_string.empty()) array_string.erase( --array_string.end() );
    return "[" + array_string + "]";
  }

  std::string make_json_hash(const std::string& key, const std::string& value) {
    return "\"" + key + "\":{" + value + "}";
  }

  std::string make_json_hash(const std::map<std::string, std::string>& hash) {
    std::string hash_string;
    for (auto kv : hash) hash_string += make_json_hash(kv.first, kv.second) + ",";
    if (!hash_string.empty()) hash_string.erase( --hash_string.end() );
    return hash_string;
  }

  std::string make_json_key_values(const std::map<std::string, std::string>& hash) {
    std::string hash_string;
    for (auto kv : hash) hash_string += make_json_key_value(kv.first, kv.second) + ",";
    if (!hash_string.empty()) hash_string.erase( --hash_string.end() );
    return hash_string;
  }
};  // namespace simplejson

#endif  // UTIL_SIMPLE_JSON_H_
