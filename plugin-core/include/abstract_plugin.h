/**
 * @file abstract_plugin.h
 * @author zhdotcai (caizhehong@gddi.com.cn)
 * @brief 
 * @version 0.1
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024 by GDDI
 * 
 */

#pragma once

#include "json.hpp"
#include <functional>

struct AlgoObject {
    int target_id;
    std::string label;
    float score;
    std::vector<int> rect;
    int64_t track_id;
};

struct PropertyEntry {
    nlohmann::json local_value;
    std::function<void(nlohmann::json &&value)> setter;
    std::function<nlohmann::json()> getter;
};

class AbstractPlugin {
public:
    AbstractPlugin(std::string name) : plugin_name_(std::move(name)) {}

    /**
     * @brief 推理结果处理
     * 
     * @param objects 
     * @return std::vector<AlgoObject> 
     */
    virtual std::vector<AlgoObject> infer_result_process(const std::vector<AlgoObject> &objects) {
        throw std::runtime_error("infer_result_process not implemented");
    }

    /**
     * @brief 跟踪结果处理
     * 
     * @param objects 
     * @return std::vector<AlgoObject> 
     */
    virtual std::vector<AlgoObject> tracked_result_process(const std::vector<AlgoObject> &objects) {
        throw std::runtime_error("tracked_result_process not implemented");
    }

    /**
     * @brief 获取插件名称
     * 
     * @return std::string 
     */
    std::string get_plugin_name() const { return plugin_name_; }

    /**
     * @brief 获取插件参数
     * 
     * @return nlohmann::json 
     */
    nlohmann::json get_properties() const;

    /**
     * @brief 设置插件参数
     * 
     * @param key 
     * @param value 
     * @return true 
     * @return false 
     */
    bool try_set_property(const std::string &key, nlohmann::json value);

protected:
    template<typename T>
    void bind_property(const std::string &name, T &value, const std::string &description) {
        properties_[name] =
            add_property(name, [this, name, &value]() { value = properties_.at(name).local_value.get<T>(); });
    }

    PropertyEntry &add_property(const std::string &key, const std::function<void()> &on_changed);

protected:
    std::string plugin_name_;
    std::map<std::string, PropertyEntry> properties_;
};