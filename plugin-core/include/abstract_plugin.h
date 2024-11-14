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
#include <tuple>

// x, y, score
using Keypoint = std::tuple<int, int, float>;

struct AlgoObject {
    int target_id;   // 目标 ID
    int64_t track_id;// 目标跟踪 ID

    std::string label;    // 目标类别
    float score;          // 目标置信度
    std::vector<int> rect;// 目标框

    std::vector<Keypoint> keypoints;       // 目标关键点
    std::string ocr_text;                  // OCR 文本
    std::vector<float> feature;            // 目标特征
    std::vector<uint8_t> segment;          // 分割图
    std::vector<std::vector<int>> contours;// 轮廓
};

struct PropertyEntry {
    nlohmann::json local_value;
    std::function<void(nlohmann::json &&value)> setter;
    std::function<nlohmann::json()> getter;
};

class AbstractPlugin {
public:
    AbstractPlugin(const std::string &name) : alias_name_(name) {}

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
     * @brief 设置插件参数
     * 
     * @param key 
     * @param value 
     * @return true 
     * @return false 
     */
    bool try_set_property(const std::string &key, nlohmann::json value);

    /**
     * @brief 获取插件名称
     * 
     * @return std::string 
     */
    std::string get_plugin_name() const;

    /**
     * @brief 获取插件定义
     * 
     * @return nlohmann::json 
     */
    nlohmann::json get_plugin_definition();

protected:
    template<typename T>
    void bind_property(const std::string &name, T &value, const std::string &description) {
        properties_[name] =
            add_property(name, [this, name, &value]() { value = properties_.at(name).local_value.get<T>(); });
    }

    PropertyEntry &add_property(const std::string &key, const std::function<void()> &on_changed);

protected:
    std::string version_{"v3"};// 版本 (默认 v3)
    std::string alias_name_;   // 别名
    std::string description_;  // 描述
    std::map<std::string, PropertyEntry> properties_;
};