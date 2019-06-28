# Colly
Colly is a simple to use and optimized library for detecting and handling AABB collision. 
It doesnt have any dependencies and it doesnt need to be precompiled into 
a .dll/.lib (or .a and similar extensions). You just need
to drag and drop the header and the source file into your project and start
using it. You can easily modify this library and add new and custom CollisionTypes.

**cl\::World** is for storing non-tile based objects. It uses
GridTree, which means that the collision is checked only with
nearby objects - it should be pretty optimized.

**cl\::GridWorld** should be used for tile-based worlds.

![](https://i.imgur.com/Df4d0fU.gif)

## How to use
### Start
Download the header and source file and drop it into your project.
Include the header in the file in which you want to use it.

### World
#### Initialize
Define the world which will contain all your game objects:
```c++
cl::World world;
```

#### Adding an object to the World
To add an object to the world, call `AddObject` function:
```c++
world.AddObject(id, bounds, colType, userData);
```

**ID** is additional user data. It can be any number you want and 
the cl::World doesnt use it anywhere. This argument is only used so that you can easily identify the object once the collision occurs.

**Bounds** contains bounds of our object - it's position and size.

**colType** is an enum type. It can either be:
   * None **-** all collision checks will be skipped
   * Solid **-** player cant go through object
   * Cross **-** used with pick ups (for example: coins)

**userData** is used for storing user data. It can point to
any data you want it to point to. You can have multiple data types
and see which one is being used for this object by checking the ID.

Example in which we add a solid object on position (10,10) with size (100,100):
```c++
world.AddObject(0, Rect(10,10,100,100), cl::CollisionType::Solid);
```

**IMPORTANT**: After adding all the objects to the world/changing anything in the world
you must build the quadtree to apply the changes:
```c++
world.UpdateQuadTree();
```

#### Handle collision
To handle the collision, we use:
```c++
world.Check(steps, bounds, goal, func);
```

This function returns the position of our player after the collision has been handled.

Argument **steps** is used when the player is moving with a higher speed. More steps means more checks and higher precision.

When moving at high speeds the player could easily pass through some objects with using only one step/check:

![](https://i.imgur.com/ZzEenRM.png)

But when using more steps we can prevent that:

![](https://i.imgur.com/SkDMkst.png)

**Bounds** contains our player's bounds (X, Y, Width, Height)

**Goal** is (X,Y) coordinate where our player wants to move

**Func** is a function which will be called when and if the collision occurs. This argument is optional.

```c++
Point res = world.Check(1, player.Bounds(), player.NextPosition());
```

### GridWorld
GridWorld is a better option for tile-based worlds but it works almost exactly like the cl::World.
```c++
cl::GridWorld world;
```

GridWorld requires additional function call to be initialized. Syntax:
```c++
world.Create(map_width, map_height, cell_width, cell_height);
```

For example, creating a 16x16 grid map with 64x64 cells would look like this:
```c++
world.Create(16, 16, 64, 64);
```

#### Setting a cell ID
To set an ID to the cell, we use:
```c++
world.SetObject(x, y, id);
```

For example, to set a cell with ID 15 on position (4, 4) in grid, we would use:
```c++
world.SetObject(4, 4, 15);
```

#### Handle collision
Handling collision is identical to handling the collision with World class.
But, as you can notice, we never specified which cell is solid and which 
cell is not solid. To do this, we have to provide a filter.

After calling `Create` function it would be best to define our 'filter'. 
It is just a simple function which specifies `CollisionType` for a cell ID.

In the following example, each cell with ID 15 or higher will be a solid cell.
```c++
world.GetCollisionType = [](int id) -> cl::CollisionType {
    if (id < 15)
        return cl::CollisionType::None;
    return cl::CollisionType::Solid;
} 
```

The **default filter** specifies that all cells with `ID >= 1` are solid cells.

## LICENSE
Colly is licensed under MIT license. See [LICENSE](./LICENSE) for more details.