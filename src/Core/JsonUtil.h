#ifndef IRONBRASS_JSONUTIL_H
#define IRONBRASS_JSONUTIL_H

#include <nlohmann/json.hpp>
#include <algorithm>
#include <array>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>

/** Helpers shared by the data-file loaders. */
namespace jsonutil {
    using json = nlohmann::json;

    /** Rejects keys not in allowed, so that typos in the data files can be spotted in log. */
    inline void requireKnownKeys(const json& obj, const std::initializer_list<std::string_view> allowed, const std::string& where) {
        if (!obj.is_object()) throw std::runtime_error(where + " must be a JSON object");
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (std::ranges::find(allowed, std::string_view(it.key())) == allowed.end())
                throw std::runtime_error("unknown key '" + it.key() + "' in " + where);
        }
    }

    template<size_t N, typename T>
    std::array<T, N> readArray(const json& j, const std::string& where) {
        if (!j.is_array() || j.size() != N)
            throw std::runtime_error(where + " must be an array of " + std::to_string(N) + " numbers");
        std::array<T, N> out{};
        for (size_t i = 0; i < N; ++i) out[i] = j[i].get<T>();
        return out;
    }
}


#endif //IRONBRASS_JSONUTIL_H
