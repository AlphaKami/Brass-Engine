#include "KineticModule.h"

#include "../../../View/ECS/Components/AnimationComponent.h"
#include "../../ECS/Components/KineticComponent.h"

namespace {
    void markTopologyDirty(entt::registry& registry) {
        if (auto* network = registry.ctx().find<KineticNetworkState>()) network->topologyDirty = true;
    }

    // Listener for registry.patch<KineticSource>: speed or direction of a source changed.
    void onSourceChanged(entt::registry& registry, entt::entity) {
        if (auto* network = registry.ctx().find<KineticNetworkState>()) network->speedsDirty = true;
    }
}

void KineticModule::onAttach(entt::registry &registry) {
    registry.ctx().emplace<KineticNetworkState>();
    registry.on_update<KineticSource>().connect<&onSourceChanged>();
}

void KineticModule::onSpawn(entt::registry& registry, const entt::entity entity, const Buildable& buildable) {
    const KineticDef* def = database.find(buildable.id, buildable.orientation);
    if (def == nullptr) return;

    auto& network = registry.ctx().get<KineticNetworkState>();
    registry.emplace<KineticNode>(entity, KineticNode{def, network.nextSerial++});
    registry.emplace<KineticState>(entity);
    if (def->sourceSpeed) {
        registry.emplace<KineticSource>(entity, KineticSource{*def->sourceSpeed});
    }

    markTopologyDirty(registry);
}

void KineticModule::onRemove(entt::registry &registry, entt::entity entity, const Buildable &buildable) {
    IBehaviourModule::onRemove(registry, entity, buildable);
}
