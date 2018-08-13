# Colly
Colly is a simple to use and optimized library for checking and handling AABB collision.
It doesnt have any dependencies and it doesnt need to be precompiled into 
a .dll/.lib (or .a and similar extensions). You just need
to drag and drop the header and the source file into your project and start
using it. You can easily modify it and add new CollisionTypes.

**cl\::World** uses GridTree so that the collision is checked with only nearby
elements. It means you can have a pretty large number of elements.

**cl\::GridWorld** can be used for tile based levels.

![](https://i.imgur.com/Df4d0fU.gif)

## How to use
### Start
Download the header and source file and drop it into your project.
Then include the header where you need to use it.

### World
#### Initialize
Define the world which will contain all elements
```c++
cl::World world;
```

#### Adding an object to the World
To add an object, we have to call `AddObject` function:
```c++
world.AddObject(id, bounds, colType, userData);
```

**ID** is additional user data. It can be any number and 
the cl::World doesnt use it anywhere. It is added for user
purposes so that you can easily identify the object.

**Bounds** contains bounds of our object - the area that is
taken by the object.

**colType** is an enum type. It can either be:
   * None **-** any collision checks will be skipped.
   * Solid **-** player cant go through object
   * Cross **-** used with pick ups (for example: coins)

**userData** is also used for user data. It can point to any
class or whatever data you want it to point to. You can have multiple
data types here and see which one is used by checking the ID.

Example which adds a solid object on position (10,10) with 100x100 size
```c++
world.AddObject(0, Rect(10,10,100,100), cl::CollisionType::Solid);
```

**IMPORTANT**: After adding the object to the world
you must build quad tree
```c++
world.UpdateQuadTree();
```

#### Handle collision
To handle collision we use:
```c++
world.Check(steps, bounds, goal, func);
```

This function returns the position of our player after the collision has been handled.

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
GridWorld is better for tile based worlds but it works almost exactly like the cl::World.
```c++
cl::GridWorld world;
```

GridWorld requires additional function call to be initialized. Syntax:
```c++
world.Create(map_width, map_height, cell_width, cell_height);
```

For example, to create a 16x16 grid map with 64x64 cells, it would look like this:
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