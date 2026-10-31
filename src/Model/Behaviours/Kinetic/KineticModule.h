#ifndef IRONBRASS_KINETICMODULE_H
#define IRONBRASS_KINETICMODULE_H

#include "../IBehaviourModule.h"
#include "../../Database/KineticDatabase.h"

/** Turns the buildables listed in the KineticDatabase into kinetic nodes. */
class KineticModule : public IBehaviourModule {
public:
    explicit KineticModule(const KineticDatabase& database) : database(database) {}

    void onAttach(entt::registry& registry) override;
    void onSpawn(entt::registry& registry, entt::entity entity, const Buildable& buildable) override;
    void onRemove(entt::registry& registry, entt::entity entity, const Buildable& buildable) override;

private:
    const KineticDatabase& database;
};

#endif //IRONBRASS_KINETICMODULE_H
