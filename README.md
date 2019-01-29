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

 - componentSignature: a typedef for the bitset used by systems to verify if an entity belongs to a system
 - invoker, make_invoker, and erase_at_index: Functions used to create type-erased function signatures for deletion functions needed to remove an entity and it's corresponding components from the game world. More info can be found here: https://stackoverflow.com/questions/52397451/call-a-vectors-function-given-the-vectors-pointer-as-a-void
 - generate_type_id() and type_id(): Functions that are used to make it easy for the programmer to get the ID of a registered component in the game engine. This uses templated functions with static variables that generate a new ID every time a new component type is registered and will always return that value when provided the same component type.

**componentManager.h/.cpp**
This code has all the logic that is used to manage component data. Before being used, component types need to be registered with the component manager (this is done using the world object though). Once registered, the component type gets an entry in a vector of vectors which stores the actual component instances (see the visualization below)
|ComponentManager componentHolder| | | |
|--|--|--|--
|TransformComponent| data... | data...
|BoxColliderComponent| data... |
|PhysicsComponent| data... | data...|
|PlayerMovementComponent| data... | data... | data... 

The component instances themselves will as a result be stored linearly in memory, which should help a lot with caching! In order to access components for a specified entity, we keep another vector of dictionaries that map entityID -> index_in_componentHolder.

Special care is taken when deleting entities (and deleting their specified component instances as a result) to update subsequent entities whose component instances have indexes larger than the one deleted; since everything is contiguous in memory a deletion will shift all subsequent components down, thus invalidating the handle we currently stored for entities that correspond to those component instances.