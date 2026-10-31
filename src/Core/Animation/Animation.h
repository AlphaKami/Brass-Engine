#ifndef IRONBRASS_ANIMATION_H
#define IRONBRASS_ANIMATION_H
#include <vector>

#include "SFML/Graphics/Rect.hpp"

/**
 * Holds animation data.
 * Frames are stored as a vector of sf::IntRect holding the coords w.r.t the sheet
*/
struct Animation {
    std::vector<sf::IntRect> frames;
    float duration = 0.f; // duration of every frame in seconds
    bool loop = false;
};


#endif //IRONBRASS_ANIMATION_H
