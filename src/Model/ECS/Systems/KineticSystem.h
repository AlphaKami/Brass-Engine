#ifndef IRONBRASS_KINETICSYSTEM_H
#define IRONBRASS_KINETICSYSTEM_H

#include <entt/entt.hpp>
#include "../../BuildSystem/CellContents.h"
#include "../../Database/BuildableCatalog.h"

struct KineticResolveReport {
    size_t networks = 0; // connected components that contain at least one source
    size_t runningNodes = 0;
    size_t idleNodes = 0;
    size_t jammedNodes = 0;
};

/**
 * Rediscovers the neighbors of every kinetic node.
 *
 * For each link declared in the node's KineticDef it looks at the target cell in the occupancy grid and connects the entities that deposited the requested marker there.
 * A connection is created only if both nodes have the same axis and, for Mesh links, both have teeth.
 * It is symmetric even if only one side declares the link.
 * If the two sides declare different kinds for the same pair, the first one found wins.
 *
 * Replaces the KineticLinks of every node, so it also cleans up after removed entities.
 */
void rebuildKineticLinks(entt::registry& registry, const OccupancyGrid& occupancy);

/**
 * Computes how every kinetic node follows the others: speed, direction, status and alignment.
 *
 * A network is a connected component of the kinetic graph.
 * Each one is visited breadth-first starting from its reference node, the oldest one, and every node gets a KineticDrive:
 * its parent, the transmission ratio of the link and the alignment offset.
 * Multiplying the ratios along the way gives the gain of each node over the reference node:
 *   speed(node) = gain(node) * speed(reference node)
 * The speed of the reference node is the one that makes the sources of the network turn at their speed.
 * A network without sources is Idle: it doesn't move, but it is still aligned.
 *
 * A network jams, when it contains contradictory constraints: a loop whose gains don't close or two sources that disagree on the speed of the network.
 * The angles of a jammed network are left as they are.
 *
 * Because the phase of every node comes from the reference node and the rules of the links, and not from
 * the history of the node, a part added later is born in phase with the rest, and removing parts doesn't move
 * what is left. Merging two networks aligns the newer one to the older one.
 */
KineticResolveReport resolveKineticNetworks(entt::registry& registry);

/**
 * Places every node of the networks that are not jammed at the angle that its link to the parent dictates, starting from the reference node of each network,
 * whose angle is left as it is.
 * This aligns meshed teeth and rigid parts, also when nothing is moving.
 *
 * @requires KineticNetworkState::order up to date
 */
void deriveKineticAngles(entt::registry& registry);

/**
 * Advances the angle of every running kinetic node.
 * Only the root of each network integrates its own angle; every other node derives its angle from its parent's, so meshed parts can't drift apart whatever the ratios are.
 * Idle and jammed nodes keep their last angle.
 *
 * @requires KineticNetworkState::order up to date.
 */
void integrateKineticAngles(entt::registry& registry, float dt);

#endif //IRONBRASS_KINETICSYSTEM_H
