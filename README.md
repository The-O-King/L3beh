# L3beh - Entity Component System Game Engine

## Code Structure
The core folder contains all the code and assets that are included with the engine. Additional systems/components/config files should be stored outside of this folder.

To create a new game you create a subclass of World.cpp, register any additional components/systems that you have created, add the parser to the additional components you created by overriding the CustomWorldGen() function in World.cpp, and then create a main.cpp in which you #include runner.h and call run the run() function with a new instance of your World subclass and the corresponding config file for the level you want to load up!

Look at the example that I have provided for details.

## Overall Architecture
**Entities**
These are the individual "things" that are in the game itself, and are a composition of discrete components. The entities within the code itself are represented simply as Integer IDs, and these integer IDs are used as handles to the ComponentManager to get components that pertain to the specified entity

**Components**
Components are meant to be very simple structs that encapsulate some state data (for example the Transform component stores the position, rotation, and scale of the entity). An entity that is specified to have a component will have an instance of said component be created and stored/kept track of by the engine.

**Systems**
Systems are where game logic is to be stored. Systems are to be provided a list of required components (represented as a bitfield) that allow the System to decide which entities it is allowed to act upon. A System can have multiple acceptable required component bitsets (For example, the CollisionSystem can accept entities with SphereCollider, Transform, and Physics OR BoxCollider, Transform, and Physics). 

 - Each System has an addEntity() function which you can override to modify how the list of entities it accepts is organized. 
 - There is also a removeEntity() function that you can override to cleanup entities that no longer fit within the systems component requirements.
 - The init() function is called once at the beginning of the game and can be override to do initial setup of entities

# Core Engine Details
**runner.h**
Contains the main game loop and initial setup for openGL/GLFW for window creation
Also computes the frame times

**input.h/.cpp**
This is the code that provides Input services for the programmer. It uses the GLFW input API to get key/mouse data from the currently active window. For checking generating keyDown, keyUp, and keyHold events, a boolean array (keyState) is used to store the last state of the key (which is updated every frame using a callback provided by GLFW)

**component_utils.h**
A file that contains utilities used to create metadata and functions for components. This includes:

 - **componentSignature**: a typedef for the bitset used by systems to verify if an entity belongs to a system
 - **invoker, make_invoker, and erase_at_index**: Functions used to create type-erased function signatures for deletion functions needed to remove an entity and it's corresponding components from the game world. More info can be found here: https://stackoverflow.com/questions/52397451/call-a-vectors-function-given-the-vectors-pointer-as-a-void
 - **generate_type_id()** and type_id(): Functions that are used to make it easy for the programmer to get the ID of a registered component in the game engine. This uses templated functions with static variables that generate a new ID every time a new component type is registered and will always return that value when provided the same component type.

**componentManager.h/.cpp**
This code has all the logic that is used to manage component data. Before being used, component types need to be registered with the component manager (this is done using the world object though). Once registered, the component type gets an entry in a vector of vectors which stores the actual component instances (see the visualization below)

| ComponentManager componentHolder |         |         |         |         |
|----------------------------------|---------|---------|---------|---------|
| TransformComponent               | data... | data... | data... | data... |
| PhysicsComponent                 | data... | data... |         |         |
| RenderComponent                  | data... |         |         |         |
| BoxColliderComponent             | data... | data... | data... |         |

The component instances themselves will as a result be stored linearly in memory, which should help a lot with caching! In order to access components for a specified entity, we keep another vector of dictionaries that map entityID -> index_in_componentHolder.

Special care is taken when deleting entities (and deleting their specified component instances as a result) to update subsequent entities whose component instances have indexes larger than the one deleted; since everything is contiguous in memory a deletion will shift all subsequent components down, thus invalidating the handle we currently stored for entities that correspond to those component instances.

A single world object has a single ComponentManager instance which is used to register/store the components of entities within that world. Functions that provide this functionality are: 

 - **registerComponent()**: Gives a new component type a specific ID as well as creating a vector in the componentHolder to store component instances
 - **addComponent()**: A templated function that allows the programmer to add a specified component type to a given entity. This function can be called by providing the entity ID and an already initialized instance of the component type you want to add, or it can be called without a component instance, and will instead will add a component instance with default parameters.
 - **setComponent()**: Allows you to replace an already existing component that an entity has with a new instance of the component that you pass in.
 - **removeComponent()**: Removes a component from an entity entirely and removes it from corresponding systems as well. 
 - **destroyEntity()**: A function which removes an entity entirely from the world; deleting all of it's corresponding components and removing it from all systems. This will also remove the entity ID from the living_entities set in the world. The actual destruction of the entity occurs at the end of the frame this function is called in.
 - **getComponent()**: A templated function that returns a reference to a specified component for an entity given it's entity ID. This allows the programmer to read or update that component's information

**components.h**
*TODO: Add more detail once core engine components are finalized*
This file includes the definitions for all the component types that the core engine include. These component types are defined as structs of just data (the exception being the Transform component which will be explained below)

**system.h**
This code provides the interface that a programmer needs to implement in order to create new systems for the engine. By default, a System has these member variables:

 - **mWorld**: Pointer to the World object that this system belongs to
 - **neededComponentSignatures**: A vector of all the componentSignatures that could allow an entity to fall within this system's control
 - **entities**: A set that is provided by default to store all the entity IDs that fall within this system

These function prototypes are provided and need to be overridden:

 - **init()**: Called just before the first frame of the game starts. Is a good place to do some initialization of values for entities
 - **update()**: Called every frame of the game, this is where most game logic will sit. The deltaTime between the last frame and the current frame is also provided to allow for framerate independent logic to be implemented.

Along with these functions, additional functions that can be overridden if needed include:

 - **addEntity()**: Function which is called whenever a new entity fits the needed componentSignature of the system. By default, the function adds the entity to a set of all entities that fit within the system. Overriding this function can allow you to better organize components that fit within the system once they are added.
 - **removeEntity()**: Function that is called when an entity needs to be removed from a system because it no longer has the required componentSignature. This will most likely need to be overridden if you overrode the default addEntity() function in order to remove the entityID from a custom organization of entities that you implemented.

Lastly, one function provided by default that does not need to be changed:
 - **getNeededComponents()**: Returns a vector of componentSignatures that this system can accept.

When implementing a new System, the workflow looks something like this:

 1. Create a new class (for example "DeathFlowerSystem") that inherits from the System class
 2. Implement a constructor which takes in a World pointer as an argument; set the mWorld member variable of the system equal to that world and then define all the required componentSignatures that you need entities to have, adding them to the neededComponentSignatures vector
 3. Override the addEntity(), removeEntity() functions if you have a specific organization for systems in mind
 4. Implement the init() and update() functions with the necessary game logic you want entities in this system to exercise
