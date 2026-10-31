# Build System Details

The build system allows placing and/or removing objects from the map.

Objects are divided into two main categories:
- **Tiles**: Static objects that perform no special functions.
- **Entities**: Functional objects (defined using the Entity Component System / ECS architecture).

>Both object types indicate their presence using "markers", a tagging system 
> distinct from the item tags. Each type can be animated using one of two 
> AnimationPlayer implementations: AnimatedSprite for tiles, and AnimationSystem
> for entities.


While tiles exist strictly within their active world (being non-functional
components), entities persist and remain active even when their parent world 
is inactive allowing their work wile being "passive".