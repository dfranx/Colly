# Colly
A simple library for handling simple collision.

![](https://i.imgur.com/IHHGtOf.png)

## How to use
### Start
First, include Colly main header:
```c++
#include <Colly.h>
```

### World
#### Initialize
Define the world:
```c++
cl::World world;
```

#### Adding an object to the World
To add an object, we have to call `AddObject` function:
```c++
world.AddObject(id, bounds, colType);
```

**ID** argument is user data. It can be any number

**Bounds** contains bounds of our object

**colType** is an enum type. It can either be:
   * None **-** any collision checks will be skipped.
   * Solid **-** player cant go through object
   * Cross **-** used with pick ups (for example: coins)

Example which adds a solid object on position (10,10) with 100x100 size
```c++
world.AddObject(0, Rect(10,10,100,100), cl::CollisionType::Solid);
```

#### Handle collision
To handle collision we use:
```c++
world.Check(steps, bounds, goal, func);
```

This function returns where our player ended up.

**Steps** argument is used when player is moving with a higher speed. More steps means more precision.

Using high speeds and one step would mean player could easily pass through object:

![](https://i.imgur.com/ZzEenRM.png)

But using more steps we could prevent that:

![](https://i.imgur.com/SkDMkst.png)

**Bounds** contains our players bounds (X, Y, Width, Height)

**Goal** is where our player wants to move

**Func** is a function which will be called when collision occurs. You dont have to use this argument.

```c++
Point res = world.Check(1, player.Bounds(), player.NextPosition());
```

### GridWorld
GridWorld is more optimized than World, so if we are working with a tilemap, it is best to use GridWorld:
```c++
cl::GridWorld world;
```

GridWorld requires additional function call to be initialized. Syntax:
```c++
world.Create(map_width, map_height, cell_width, cell_height);
```

For example, to create a 16x16 grid with 64x64 cells, it would look like this:
```c++
world.Create(16, 16, 64, 64);
```

#### Setting a cell ID
To set an ID to a cell, we use:
```c++
world.SetObject(x, y, id);
```

For example, to set a cell with ID 15 on position (4, 4) in grid, we would use:
```c++
world.SetObject(4, 4, 15);
```

#### Handle collision
Handling collision is identical to handling a collision with World class.
But, as you can notice, we never specified which cell is solid and which 
cell is not solid. To do this, we have to specify a filter.

After calling `Create` function it would be best to define our 'filter'. 
It is just a simple function which specifies `CollisionType` for a given ID.

In the following example, each cell with ID 15 or higher will be a solid cell.
```c++
world.GetCollisionType = [](int id) -> cl::CollisionType {
    if (id < 15)
        return cl::CollisionType::None;
    return cl::CollisionType::Solid;
} 
```

The default filter specifies that all cells with `ID >= 1` are solid cells.

## Building
This library requires no dependencies. But if you are going to build examples,
you need to link SFML.