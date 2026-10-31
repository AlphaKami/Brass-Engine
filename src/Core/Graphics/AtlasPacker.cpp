#include "AtlasPacker.h"
#include <algorithm>
#include <numeric>

namespace AtlasPacker {
    namespace {
        struct Page {
            unsigned cursorX = 0;
            unsigned shelfTop = 0;
            unsigned shelfHeight = 0;
            unsigned usedWidth = 0;
            unsigned usedHeight = 0;
        };
    }

    Result pack(const std::vector<sf::Vector2u>& sizes, const unsigned maxPageSize, const unsigned padding) {
        Result result;
        result.placements.resize(sizes.size());

        // A page is as wide as the widest rectangle that can fit in one
        unsigned pageWidth = 0;
        for (const sf::Vector2u& size : sizes) {
            if (size.x <= maxPageSize && size.y <= maxPageSize) pageWidth = std::max(pageWidth, size.x);
        }

        std::vector<std::size_t> order(sizes.size());
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::ranges::stable_sort(order, [&sizes](const std::size_t a, const std::size_t b) {
            return sizes[a].y > sizes[b].y;
        });

        std::vector<Page> pages;
        for (const std::size_t index : order) {
            const sf::Vector2u size = sizes[index];
            if (size.x == 0 || size.y == 0 || size.x > maxPageSize || size.y > maxPageSize) continue; // left unpacked

            if (pages.empty()) pages.emplace_back();

            Page* page = &pages.back();
            if (page->cursorX + size.x > pageWidth) { // shelf full, start a new one
                page->shelfTop += page->shelfHeight;
                page->shelfHeight = 0;
                page->cursorX = 0;
            }
            if (page->shelfTop + size.y > maxPageSize) { // page full, start a new one
                pages.emplace_back();
                page = &pages.back();
            }

            result.placements[index] = {true, pages.size() - 1, {page->cursorX, page->shelfTop}};
            page->cursorX += size.x + padding;
            page->shelfHeight = std::max(page->shelfHeight, size.y + padding);
            page->usedWidth = std::max(page->usedWidth, page->cursorX - padding);
            page->usedHeight = std::max(page->usedHeight, page->shelfTop + size.y);
        }

        for (const Page& page : pages) result.pageSizes.emplace_back(page.usedWidth, page.usedHeight);
        return result;
    }
}