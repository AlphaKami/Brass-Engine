#ifndef IRONBRASS_TAGREGISTRY_H
#define IRONBRASS_TAGREGISTRY_H

#include <unordered_map>
#include <string>
#include <vector>
#include "MarkerId.h"

/**
 * Maps marker names to integer ids.
 * Names are only used at load time: at runtime everything works on MarkerId.
 */
class MarkerRegistry {
public:
    static constexpr const char* DefaultMarkName = "default_mark";

    MarkerRegistry();

    /** Declares every marker listed in the file. Already known names are ignored. */
    void loadFromFile(const std::string& path);

    /** Get-or-create: returns the id of `name`, registering it if it is new. */
    MarkerId resolve(const std::string& name);
    /**
     * Strict lookup, used when reading buildable definitions.
     * @throws std::runtime_error if the marker was never declared (catches typos in the data files)
     */
    [[nodiscard]] MarkerId get(const std::string& name) const;
    [[nodiscard]] const std::string& nameOf(MarkerId id) const;

    [[nodiscard]] MarkerId defaultMark() const { return defaultMarkId; }

private:
    std::unordered_map<std::string, MarkerId> nameToId;
    std::vector<std::string> idToName;
    MarkerId defaultMarkId;
};
#endif //IRONBRASS_TAGREGISTRY_H
