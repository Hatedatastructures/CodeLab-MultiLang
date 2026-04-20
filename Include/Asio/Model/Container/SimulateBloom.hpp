#pragma once
#include "SimulateAlgorithm.hpp"
#include "SimulateBase.hpp"
#include "SimulateImitate.hpp"
namespace bloom_filter_container
{
    /**
   * @brief 布隆过滤器（Bloom Filter）类实现
   *
   * 布隆过滤器是一种空间效率极高的概率型数据结构，用于快速判断一个元素是否属于某个集合。
   *
   * 特点是：存在假阳性（可能误判元素存在），但无假阴性（若判断不存在则一定不存在），
   *
   * 不支持删除操作，仅支持插入和查询，适用于允许一定误判率的场景（如缓存过滤、垃圾邮件检测等）。
   *
   * 工作原理：通过多个哈希函数将元素映射到一个位集合（bit_set）的多个位置，插入时将这些位置设为1；
   *
   * 查询时检查这些位置是否全为1，全为1则可能存在（假阳性可能），否则一定不存在。
   *
   * 模板参数:
   *
   * * - `bloom_filter_type_value`: 布隆过滤器中存储的元素类型
   *
   * * - `bloom_filter_hash_functor`: 哈希函数对象类型，默认为 `standard_con::algorithm::hash_algorithm::hash_function<bloom_filter_type_value>`
   *
   *   - 需提供三种哈希函数（`hash_sdmmhash`、`hash_djbhash`、`hash_pjwhash`），用于将元素映射到位集合的不同位置
   *
   * 注意事项:
   *
   * - 容量越大、哈希函数越多，假阳性率越低，但空间和时间开销越大
   *
   * - 不支持删除操作（删除会影响其他元素的哈希映射位）
   *
   * - 适用于对误判率不敏感，追求空间效率的场景
   */
    template <typename bloom_filter_type_value, typename bloom_filter_hash_functor = standard_con::hash_function<bloom_filter_type_value>>
    class bloom_filter
    {
        bloom_filter_hash_functor hash_functions_object;
        using bit_set = standard_con::bit_set;
        bit_set instance_bit_set;
        uint64_t _capacity;

    public:
        bloom_filter()
        {
            _capacity = 1000;
            instance_bit_set.resize(_capacity);
        }
        explicit bloom_filter(const uint64_t &temp_capacity)
        {
            _capacity = temp_capacity;
            instance_bit_set.resize(_capacity);
        }
        [[nodiscard]] uint64_t size() const
        {
            return instance_bit_set.size();
        }
        [[nodiscard]] uint64_t capacity() const
        {
            return _capacity;
        }
        bool test(const bloom_filter_type_value &temp_bf_map_value)
        {
            uint64_t primary_mapping_location = hash_functions_object.hash_sdmmhash(temp_bf_map_value) % _capacity;
            uint64_t secondary_mapping_location = hash_functions_object.hash_djbhash(temp_bf_map_value) % _capacity;
            uint64_t tertiary_mapping_location = hash_functions_object.hash_pjwhash(temp_bf_map_value) % _capacity;
            if (instance_bit_set.test(primary_mapping_location) == true && instance_bit_set.test(secondary_mapping_location) == true &&
                    instance_bit_set.test(tertiary_mapping_location) == true)
            {
                return true;
                /* 有一个为0就返回false */
            }
            return false;
        }
        void set(const bloom_filter_type_value &temp_bf_map_value)
        {
            uint64_t primary_mapping_location = hash_functions_object.hash_sdmmhash(temp_bf_map_value) % _capacity;
            uint64_t secondary_mapping_location = hash_functions_object.hash_djbhash(temp_bf_map_value) % _capacity;
            uint64_t tertiary_mapping_location = hash_functions_object.hash_pjwhash(temp_bf_map_value) % _capacity;
            instance_bit_set.set(primary_mapping_location);
            instance_bit_set.set(secondary_mapping_location);
            instance_bit_set.set(tertiary_mapping_location);
        }
        // 布隆过滤器只支持插入和查找，不支持删除
    };
}
namespace standard_con
{
    using bloom_filter_container::bloom_filter;
}