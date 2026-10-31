#ifndef IRONBRASS_RESOURCESDATABASE_H
#define IRONBRASS_RESOURCESDATABASE_H

#include "ResourceRef.h"
#include "Fluid.h"
#include "Item.h"
#include "Gas.h"

class ResourcesDatabase {
public:
    void loadFromFile(const std::string& path);

    [[nodiscard]] const Item& getItem(const ItemId id) const { return items.at(id);; }
    [[nodiscard]] const Fluid& getFluid(const FluidId id) const { return fluids.at(id);; }
    [[nodiscard]] const Gas& getGas(const GasId id) const { return gases.at(id);; }

    [[nodiscard]] ItemId getItemId(const std::string& name) const;
    [[nodiscard]] FluidId getFluidId(const std::string& name) const;
    [[nodiscard]] GasId getGasId(const std::string& name) const;

    [[nodiscard]] std::optional<ItemId>  tryGetItemId(const std::string& name)  const;
    [[nodiscard]] std::optional<FluidId> tryGetFluidId(const std::string& name) const;
    [[nodiscard]] std::optional<GasId>   tryGetGasId(const std::string& name)   const;

    [[nodiscard]] size_t itemCount()  const { return items.size(); }
    [[nodiscard]] size_t fluidCount() const { return fluids.size(); }
    [[nodiscard]] size_t gasCount()   const { return gases.size(); }

private:
    std::vector<Item> items;
    std::vector<Fluid> fluids;
    std::vector<Gas> gases;

    std::unordered_map<std::string, ItemId> itemNameToId;
    std::unordered_map<std::string, FluidId> fluidNameToId;
    std::unordered_map<std::string, GasId> gasNameToId;

};


#endif //IRONBRASS_RESOURCESDATABASE_H
