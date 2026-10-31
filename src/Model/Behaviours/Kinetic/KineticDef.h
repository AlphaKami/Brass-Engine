#ifndef IRONBRASS_KINETICDEF_H
#define IRONBRASS_KINETICDEF_H

#include <cstdint>
#include <optional>
#include <vector>
#include "../../BuildSystem/MarkerId.h"

/** Rotation axis of a kinetic node, in grid space. */
enum class Axis : std::uint8_t { X, Y, Z };

/**
 * How two connected nodes transmit rotation to each other.
 * - Coupled: rigid link, same speed and same direction (shaft <-> gear on it, shaft <-> shaft).
 * - Mesh: toothed contact, direction is inverted and speed scales with the teeth ratio.
 */
enum class KineticLinkKind : std::uint8_t { Coupled, Mesh };

/**
 * Look at the cell [origin + (dx, dy, dz)] and find who deposited marker there.
 * Links are undirected: it is enough that one of the two nodes declares it.
 */
struct KineticLink {
    KineticLinkKind kind = KineticLinkKind::Coupled;
    std::int32_t dx = 0, dy = 0, dz = 0;
    MarkerId marker{};
    float phase = 0.f;
};

/**
 * Static kinetic description of one orientation of a buildable.
 * Owned by KineticDatabase; entities only keep a pointer to it (KineticNode).
 */
struct KineticDef {
    Axis axis = Axis::X;

    float teeth = 0.f; // an object with 0 teeth cannot use a Meshed link (like shaft)
    float frameSpan = 1.f;

    std::vector<KineticLink> links;
    std::optional<float> sourceSpeed; // Set only for kinetic sources (motors)
};

/** Whether a link of this kind can connect nodes `a` and `b`. Used when discovering neighbors. */
inline bool canConnect(const KineticLinkKind kind, const KineticDef& a, const KineticDef& b) {
    if (a.axis != b.axis) return false;

    switch (kind) {
        case KineticLinkKind::Coupled:
            return true;
        case KineticLinkKind::Mesh:
            return a.teeth > 0.f && b.teeth > 0.f;
    }
    return false;
}

/** Factor to apply to the speed and angle of `from` to get those of `to`, sign included. */
inline double transmissionRatio(const KineticLinkKind kind, const KineticDef& from, const KineticDef& to) {
    switch (kind) {
        case KineticLinkKind::Coupled:
            return 1.0;
        case KineticLinkKind::Mesh:
            return -static_cast<double>(from.teeth) / static_cast<double>(to.teeth);
    }
    return 0.0;
}

/**
 * Constant to add to `ratio * angle(from)` to get the angle of `to`, in turns, so that the two are aligned.
 * Coupled: 0 (rigid body).
 * Mesh: `phase` tooth pitches of `to`.
 * Since the pitch of a node is 1 / teeth, `phase` is the same number seen from either side of the link, so the alignment is consistent whichever node is the parent.
 */
inline double alignmentOffset(const KineticLinkKind kind, const float phase, const KineticDef& from, const KineticDef& to) {
    switch (kind) {
        case KineticLinkKind::Coupled:
            return 0.0;
        case KineticLinkKind::Mesh:
            return static_cast<double>(phase) / static_cast<double>(to.teeth);
    }
    return 0.0;
}

#endif //IRONBRASS_KINETICDEF_H
