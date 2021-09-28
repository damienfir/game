Graphics
- Lights
- Noisy Phong rendering for visual clues
- Materials

Physics

Mesh
- Load simple mesh and display

Engine
- Separate the player and the camera
- Separate the functions that manage the world state. Like adding stuff and deleting.
  - They can be made "pure" in the sense that they affect the world only (no input or display).

Geometry

Game
- Make a small world
  - Even generate automatically, like a floor
- How to teleport on top of things you don't see. (maybe you shouldn't be able to)
- Cancel a teleport

Editor
- Spawn new objects
- Move objects on tiles
- Move around viewing
- More general mouse based UI
  - Fixed screen angle/view, move things with the mouse
  - Turn around the scene using rotate OR FPV
- Raise/lower ground

Debugging
- Visualize bounding boxes
- 3rd person view of the camera
- FPS on screen

Testing
- Start to write tests for pure functions
- Move more functions to be pure so they can be tested
  - Physics
  - Maths
  - Undo/redo
  - Mesh/geometry

Blogging
- Undo/redo system with std::variant and Actions