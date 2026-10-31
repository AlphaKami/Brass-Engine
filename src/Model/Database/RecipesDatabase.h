#ifndef IRONBRASS_RECIPESDATABASE_H
#define IRONBRASS_RECIPESDATABASE_H

#include "ResourcesDatabase.h"
#include "MachineryDatabase.h"
#include "Recipe.h"

#include <optional>


class RecipesDatabase {
public:
    void loadFromFile(const std::string& path, const ResourcesDatabase& resourceDb, const MachineryDatabase& machineDb);

    [[nodiscard]] const Recipe& getRecipe(const RecipeId id) const { return recipes.at(id); }
    [[nodiscard]] RecipeId getRecipeId(const std::string& name) const;

    [[nodiscard]] size_t recipeCount() const { return recipes.size(); }

private:
    std::vector<Recipe> recipes;
    std::unordered_map<std::string, RecipeId> recipeNameToId;

    static ResourceRef resolveResource(const nlohmann::json& entry, const ResourcesDatabase& resourceDb);

    static std::vector<ResourceAmount> parseAmounts(const nlohmann::json& array, const ResourcesDatabase& resourceDb);
    static std::vector<ByproductAmount> parseByproducts(const nlohmann::json& array, const std::vector<ResourceAmount>& outputs, const ResourcesDatabase& resourceDb);

    static size_t findOutputIndexByName(const std::string& name, const std::vector<ResourceAmount>& outputs, const ResourcesDatabase& resourceDb);
    static size_t countPortsByType(const std::vector<MachinePort>& ports, ResourceType type);
    static size_t countAmountsByType(const std::vector<ResourceAmount>& amounts, ResourceType type);
    static void validateRecipeAgainstMachine(const Recipe& recipe, const Machine& machine);
};


#endif //IRONBRASS_RECIPESDATABASE_H
