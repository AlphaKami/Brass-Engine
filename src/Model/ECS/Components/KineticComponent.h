#ifndef IRONBRASS_KINETICCOMPONENT_H
#define IRONBRASS_KINETICCOMPONENT_H

#include <entt/entt.hpp>
#include <vector>
#include <cstdint>
#include "../../Behaviours/Kinetic/KineticDef.h"

enum class KineticStatus : std::uint8_t { Idle, Running, Jammed };

/**
 * Marks an entity as part of a kinetic network.
 * Non-owning link to the static data in KineticDatabase, so systems don't have to go through the catalog.
 */
struct KineticNode {
    const KineticDef* def = nullptr;
    std::uint64_t serial = 0; // Creation order of nodes
};

/**
 * How a kinetic node follows its parent in the network: its speed is the parent's times ratio,
 * and its angle is `ratio * parent angle + offset`, where offset is the alignment of the link:
 * rigid parts share the angle, meshed gears have their teeth interleaved.
 * The reference node of a network, and every node of a jammed one, has a null parent.
 */
struct KineticDrive {
    entt::entity parent = entt::null;
    double ratio = 1.0;
    double offset = 0.0;
};

/** An edge of the kinetic graph, seen from one of its two nodes. */
struct KineticConnection {
    entt::entity other = entt::null;
    KineticLinkKind kind = KineticLinkKind::Coupled;
    float phase = 0.f;
};

/**
 * Neighbors of a kinetic node, discovered from the occupancy grid by rebuildKineticLinks.
 * Connections are symmetric: A lists B, B lists A (with the same kind).
 */
struct KineticLinks {
    std::vector<KineticConnection> connections;
};

/**
 * Marks a kinetic node as a source of rotation.
 * Plain data on purpose: speed and direction can be changed from code on the instance.
 * To change them while the game runs use registry.patch<KineticSource>(entity, [](KineticSource& s) { ... }),
 * so that the network gets recomputed (a direct write is not noticed).
 */
struct KineticSource {
    float speed = 0.f;
};

/**
 * Runtime rotation state of a kinetic node.
 * Written by the kinetic systems, read by the animation system.
 */
struct KineticState {
    double angle = 0.f; // Current angle in turns. Convention: positive speed = direction in which the animation frames advance.
    float speed = 0.f; // Signed angular speed in turns/second.
    KineticStatus status = KineticStatus::Idle;
};

/**
 * Global state of the kinetic behavior. Lives in registry.ctx(), not on an entity:
 * it is created by KineticModule::onAttach, so its presence also tells whether the kinetic behavior is enabled.
 */
struct KineticNetworkState {
    bool topologyDirty = true; // A kinetic node has been added or removed: links and speeds must be recomputed.
    bool speedsDirty = true; // A source changed speed or direction: speeds must be recomputed (links are still valid).
    std::uint64_t nextSerial = 0; // Next KineticNode::serial to hand out.
    /*
     * Running nodes in visit order: for every network the source first, and every node after its parent.
     * Lets later systems derive each angle from its parent's in a single linear pass.
     */
    std::vector<entt::entity> order;
};

#endif //IRONBRASS_KINETICCOMPONENT_H
